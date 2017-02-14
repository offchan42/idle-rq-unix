#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 5104
#define BUFSIZE 10000

void invertcase(char *str) {
  size_t len = strlen(str);
  char ch;
  size_t i;
  for (i = 0; i < len; i++) {
    ch = str[i];
    if (ch >= 'A' && ch <= 'Z')
      str[i] += 32;
    else if (ch >= 'a' && ch <= 'z')
      str[i] -= 32;
    // 32 is the absolute distance between 'A' and 'a', we can just do 'a' - 'A'
    // but that is not the way cool kids get stuff done
  }
}

int main(void) {
  // socket - create an endpoint for communication
  int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) {
    fprintf(stderr, "Error: Cannot create a socket\n");
    return EXIT_FAILURE;
  }
  // try reusing the socket with the same port when it says that address
  // is already in use when bind
  int yes = 1;
  if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    perror("setsockopt");
    exit(1);
  }

  // bind - bind a name to a socket
  struct sockaddr_in addrport;
  addrport.sin_family = AF_INET;
  // htons - hostshort to network (little endian to big endian)
  addrport.sin_port = htons(PORT);
  addrport.sin_addr.s_addr = htonl(INADDR_ANY); // htonl = hostlong to network
  // assigning a name to a socket
  int bind_status = bind(
    socket_desc,
    (struct sockaddr *) &addrport, // casting 'sockaddr_in' to 'sockaddr'
    sizeof(addrport) // stupidity of C, it can't figure out the length on its own
  );
  if (bind_status == -1) {
    fprintf(stderr, "Error: Cannot bind a socket\n");
    return EXIT_FAILURE;
  }

  // listen - listen for connections on a socket
  int qLimit = 10;
  int listen_status = listen(socket_desc, qLimit);
  if (listen_status == -1) {
    fprintf(stderr, "Error: Cannot listen for a socket\n");
    return EXIT_FAILURE;
  }

  // accept - accept a connection on a socket
  struct sockaddr_in clientAddr;
  int clientLen;
  // for (;;) {
  clientLen = sizeof(clientAddr);
  printf("Accepting a connection ...\n");
  int client_socket = accept(
    socket_desc, // input
    (struct sockaddr *) &clientAddr, // output
    &clientLen // input & output
  );
  if (client_socket == -1) {
    fprintf(stderr, "Error: Cannot accept a socket\n");
    return EXIT_FAILURE;
  }
  printf("A client from port %d is connected.\n", clientAddr.sin_port);
  // }

  // communicate
  int bufsize = BUFSIZE;
  char msgbuffer[bufsize];
  int msgsize = recv(client_socket, msgbuffer, bufsize, 0);
  if (msgsize == -1 ) {
    fprintf(stderr, "Error: Cannot receive using recv()\n");
    return EXIT_FAILURE;
  }
  // ensure null-terminated string, otherwise it will print garbage below
  msgbuffer[msgsize] = 0;
  printf("Message received, bytes: %d\n", msgsize);
  printf("The message is \n\"%s\"\n", msgbuffer);

  // invert case the entire buffer
  invertcase(msgbuffer);
  printf("The message after case inversion: \n\"%s\"\n", msgbuffer);

  // send back to client
  int sent_bytes = send(client_socket, msgbuffer, msgsize, 0);
  if (sent_bytes != msgsize) {
    fprintf(stderr, "Error: send() sent a different number of bytes than\
        expected\n");
    return EXIT_FAILURE;
  }
  printf("The message is sent back to the client.\n");

  // close the socket and free the port
  int close_status = close(socket_desc);
  if (close_status == -1) {
    fprintf(stderr, "Error: Cannot close a socket\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
