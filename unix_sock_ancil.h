#ifndef UNIX_SOCK_ANCIL_H_
#define UNIX_SOCK_ANCIL_H_
#include "ancillary.h"

#ifdef __cplusplus
extern "C" {
#endif

// return 0 on success
int unix_sock_ancil_send_fds(const char *sock_path, const int *fds, unsigned int *nfd);
int unix_sock_ancil_recv_fds(const char *sock_path, int *fds, unsigned int *nfd);
int unix_sock_ancil_send_fd(const char *sock_path, int fd);
int unix_sock_ancil_recv_fd(const char *sock_path, int *fd);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: UNIX_SOCK_ANCIL_H_ */
