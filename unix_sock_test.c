#include "unix_sock_ancil.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void child_process(const char *sock_path) {
  int fds[2];
  printf("start recv\n");
  unsigned int nfds = sizeof(fds) / sizeof(fds[0]);
  if (unix_sock_ancil_recv_fds(sock_path, fds, &nfds)) {
    exit(1);
  }

  fprintf(stderr, "%d/2 file descriptors received: %d, %d\n",
          nfds, fds[0], fds[1]);
}

void parent_process(const char *sock_path) {
  sleep(1);

  int fds[2] = {1, 2};
  printf("start send\n");
  unsigned int nfds = sizeof(fds) / sizeof(fds[0]);
  if (unix_sock_ancil_send_fds(sock_path, fds, &nfds)) {
    exit(1);
  }

  fprintf(stderr, "%d/2 file descriptors sent: %d, %d\n",
          nfds, fds[0], fds[1]);
}

int main(void) {
  const char *sock_path = "ancil_sock_path";

  switch(fork()) {
    case 0:
      child_process(sock_path);
      fprintf(stderr, "child quit\n");
      break;
    case -1:
      perror("fork");
      exit(1);
    default:
      parent_process(sock_path);
      wait(NULL);
      fprintf(stderr, "parent quit\n");
      break;
  }
  return(0);
}
