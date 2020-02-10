/*
 * Brendon Tiszka
 * xsneak - simple tool for reading memory without ptrace using
 * process_vm_readv
 * MIT
 */

#define _GNU_SOURCE
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/* User defined variables */

#define MAXBUF    8192
#define DEBUG	  0

/* User defined values */
#define MAPPREFIX "map-"
#define MEMPREFIX "mem-"
#define BINSUFFIX ".bin"

#define FILTER      1
#define RWX_FILTER  "rw-"
#define INTERVALS   1
#define SLEEP_MICRO 10000

/* End user defined variables */

#define debug(x...) { \
fprintf(stderr, x); \
fflush(stderr); \
}\

#define fatal(x...) { \
fprintf(stderr, "[-] ERROR: " x); \
exit(1); \
}\


void usage(char *myname) {
  debug("Usage: %s PID\n", myname);
  exit(3);
}

void dump_memory(FILE *mapfile, pid_t child, int count) {
  char *filepath, tmp[MAXBUF], procmem[MAXBUF];

  while (fgets(tmp, MAXBUF, mapfile)) {
    char line[MAXBUF], perms[MAXBUF], r, w, x;
    long unsigned int st, en, len;
    char *writeptr;
    int dumpfile, retval;

    // Read the memory ranges for each segment along with read/write/execute
    if (sscanf(tmp, "%lx-%lx %c%c%cp", &st, &en, &r, &w, &x) != 5) {
      debug("[!] Parse error in /proc/%d/maps: %s", child, tmp);
      continue;
    }

    // Calculate size of segment
    len = en - st;

    if (DEBUG) debug("%lx %lx\n", st, en);

    if ((filepath=strchr(tmp,'/'))) {
      *(filepath-1)=0;
      sprintf(line, MAPPREFIX "%lx-%lx-%d" BINSUFFIX, st, en, count);
    } else {
      if (strchr(tmp, '\n')) *strchr(tmp,'\n')=0;
      sprintf(line, MEMPREFIX "%lx-%lx-%d" BINSUFFIX, st, en, count);
    }

    // Filter by memory segment permissions
    if (FILTER && (r != RWX_FILTER[0] || w != RWX_FILTER[1] || x != RWX_FILTER[2])) {
      if (DEBUG)  debug("[!] line %s %c%c%c\n", line, r, w, x);
      continue;
    }

    struct iovec local[1];
    local[0].iov_base = calloc(len, sizeof(char));
    local[0].iov_len = len;

    struct iovec remote[1];
    remote[0].iov_base = (void *) st;
    remote[0].iov_len = len;

    retval = process_vm_readv(child, local, 2, remote, 1, 0);

    if (retval < 0) {
      debug("[!] failed to process_vm_readv(%d) == %d\n", retval, errno);
      continue;
    }

    dumpfile = open(line, O_WRONLY | O_TRUNC | O_CREAT | O_EXCL, 0600);

    if (dumpfile == -1) {
      debug("[!] failed to open(%s)\n", line);
      continue;
    }

    if (write(dumpfile, local[0].iov_base, len) != len)
      debug("[!] short write to %s.\n", line);

    close(dumpfile);
  }

  fseek(mapfile, 0, SEEK_SET);
}

void dump_wrapper(pid_t child) {
  FILE *mapfile;
  int memfd;
  char tmp[MAXBUF];
  int count = 0;

  sprintf(tmp, "/proc/%d/maps", child);
  mapfile = fopen(tmp, "r");

  if (!mapfile) {
    fatal("/proc/%d/maps could not be open\n", child);
  }

  while (count < INTERVALS) {
    dump_memory(mapfile, child, count++);
    usleep(SLEEP_MICRO);
  }
}

int main(int argc, char *argv[]) {
  pid_t child;

  if (DEBUG) debug("version 0.1 brendon tiszka\n\n");
  if (argc < 2) usage(argv[0]);

  child = atoi(argv[1]);

  dump_wrapper(child);

  return 0;
}
