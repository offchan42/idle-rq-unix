#include <sys/types.h>
#include <sys/socket.h>

// out frame will be a byte long

ssize_t mysend(int sockfile, const void *buf, size_t len, int flags);
ssize_t recv(int sockfile, void *buf, size_t len, int flags);
