xsneak
---
Simple utility for dumping linux processes using process_vm_readv

Usage
./xsneak <PID>

will result in files with memory contents

-rw------- 1 root root   4096 Feb 10 07:57 map-5588ebc74000-5588ebc75000-0.bin
-rw------- 1 root root   8192 Feb 10 07:57 map-7fb65bb11000-7fb65bb13000-0.bin
-rw------- 1 root root   4096 Feb 10 07:57 map-7fb65bd3f000-7fb65bd40000-0.bin
-rw------- 1 root root 135168 Feb 10 07:57 mem-5588ecc3d000-5588ecc5e000-0.bin
-rw------- 1 root root  16384 Feb 10 07:57 mem-7fb65bb13000-7fb65bb17000-0.bin
-rw------- 1 root root   8192 Feb 10 07:57 mem-7fb65bd24000-7fb65bd26000-0.bin
-rw------- 1 root root   4096 Feb 10 07:57 mem-7fb65bd40000-7fb65bd41000-0.bin
-rw------- 1 root root 139264 Feb 10 07:57 mem-7fff319f7000-7fff31a19000-0.bin
Limitations
Requires root

License
MIT
