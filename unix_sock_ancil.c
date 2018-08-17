#include "unix_sock_ancil.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>

int unix_sock_ancil_send_fds(const char *sock_path, const int *fds, unsigned int *nfd) {
  int sock;
  if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "socket() failed: %s\n", strerror(errno));
    return -1;
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

  if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    fprintf(stderr, "connect() failed with sock_path (%s): %s\n",
            sock_path, strerror(errno));
    goto fail;
  }

  if (ancil_send_fds(sock, fds, *nfd)) {
    fprintf(stderr, "ancil_send_fd() failed: %s\n", strerror(errno));
    goto fail;
  }
  close(sock);
  return 0;

fail:
  if (sock != -1) {
    close(sock);
  }
  return -1;
}

int unix_sock_ancil_recv_fds(const char *sock_path, int *fds, unsigned int *nfd) {
  int sock;
  if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "socket() failed: %s\n", strerror(errno));
    return -1;
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

  unlink(sock_path);

  if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    fprintf(stderr, "bind() failed with sock_path (%s): %s\n",
            sock_path, strerror(errno));
    goto fail;
  }

  if (listen(sock, 1) == -1) {
    fprintf(stderr, "listen() failed with sock_path (%s): %s\n",
            sock_path, strerror(errno));
    goto fail;
  }

  for (;;) {
    int c_sock;
    struct sockaddr_un c_addr;
    socklen_t len = sizeof(c_addr);
    if ((c_sock = accept(sock, (struct sockaddr *)&c_addr, &len)) == -1) {
      fprintf(stderr, "accept() failed: %s\n", strerror(errno));
      continue;
    }

    *nfd = ancil_recv_fds(c_sock, fds, *nfd);
    close(c_sock);

    if (*nfd <= 0) {
      fprintf(stderr, "ancil_recv_fd() failed with sock_path (%s): %s\n",
              sock_path, strerror(errno));
    } else {
      break;
    }
  }

  close(sock);
  return 0;

fail:
  if (sock != -1) {
    close(sock);
  }
  return -1;
}

int unix_sock_ancil_send_fd(const char *sock_path, int fd) {
  unsigned int nfd = 1;
  return unix_sock_ancil_send_fds(sock_path, &fd, &nfd);
}

int unix_sock_ancil_recv_fd(const char *sock_path, int *fd) {
  unsigned int nfd = 1;
  return unix_sock_ancil_recv_fds(sock_path, fd, &nfd);
}
