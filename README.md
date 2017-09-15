## mach_portal_memctl

mach_portal_memctl is a partial reimplementation of [mach_portal] over [libmemctl]. I developed
mach_portal_memctl primarily to test that libmemctl is working correctly.

[mach_portal]: https://bugs.chromium.org/p/project-zero/issues/detail?id=965
[libmemctl]: https://github.com/bazad/memctl

### Setup

Set up mach_portal_memctl by cloning the project, creating a link to the memctl directory, and
downloading iosbinpack64:

	$ git clone https://github.com/bazad/memctl
	$ cd memctl
	$ make lib/libmemctl.a IPHONEOS_DEPLOYMENT_TARGET=10.1
	$ cd ..
	$ git clone https://github.com/bazad/mach_portal_memctl
	$ cd mach_portal_memctl
	$ ln -s ../memctl memctl
	$ mkdir mach_portal/iosbinpack64
	$ cd mach_portal/iosbinpack64
	$ curl http://newosxbook.com/tools/iosbinpack64.tgz | tar -xf-
	$ cd ../..

After this, you can open the project in Xcode, change the group ID in jailbreak.c, set the target
to your iOS device, and run.

### License

My modifications to the original mach_portal code are released into the public domain. The original
mach_portal code is released under its original license.
