/*
 * (c) 2022 Apollo Developers
 * For terms, see LICENSE
 * hosted_console.c - console that interacts with a host operating system's TTY
 */

#include <sys/hal.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

static struct termios orig;


int c_read(console_t *obj, char *buf, int len) {
  fd_set set;
  FD_ZERO(&set);
  FD_SET(0, &set);
  struct timeval tv;
  memset(&tv, 0, sizeof(struct timeval));

  if (select(1, &set, NULL, NULL, &tv) > 0) {
    return (int)read(0, buf, len);
  }
  return 0;
}

int c_write(console_t *obj, const char *buf, int len) {
  return (int)write(1, buf, len);
}

console_t c = {
  .open = NULL,
  .close = NULL,
  .read = &c_read,
  .write = &c_write,
  .flush = NULL,
  .data = NULL
};

int init_console() {
  struct termios t;
  tcgetattr(1, &t);
  tcgetattr(1, &orig);

  t.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL|ICANON);
  tcsetattr(1, TCSANOW, &t);

  register_console(&c);
  return 0;
}

// Console cleanup.
int fini_console() {
  tcsetattr(1, TCSANOW, &orig);
  return 0;
}

static prereq_t prereqs[] = { {"console",NULL}, {NULL,NULL} };
MODULE = {
  .name = "hosted/console",
  .required = prereqs,
  .load_after = NULL,
  .init = &init_console,
  .fini = NULL
};
