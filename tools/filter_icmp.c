#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/filter.h>

#define err_and_die(retcode, fmt, ...) \
  do { \
    fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
    exit(retcode); \
  } while (0)

int
main(int argc, char *argv[])
{
  int fd, ret;

  struct sock_filter insns[] = {
    /* tcpdump -dd -nn icmp */
    { 0x28, 0, 0, 0x0000000c },
    { 0x15, 0, 3, 0x00000800 },
    { 0x30, 0, 0, 0x00000017 },
    { 0x15, 0, 1, 0x00000001 },
    { 0x6, 0, 0, 0x00040000 },
    { 0x6, 0, 0, 0x00000000 },
  };
  struct sock_fprog fprog = {
    .len = sizeof(insns)/sizeof(insns[0]),
    .filter = insns,
  };

  if (argc != 2)
    err_and_die(-1, " Usage: filter_icmp <proc file>");

  if ((fd = open(argv[1], O_WRONLY)) == -1)
    err_and_die(errno, "Failed on open() - (%s)", strerror(errno));

  if ((ret = write(fd, &fprog, sizeof(fprog))) == -1)
    err_and_die(errno, "Failed on write() - (%s)", strerror(errno));

  close(fd);

  return 0;
}

