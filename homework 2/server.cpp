/** CODE INSPIRED BY BLACKBOARD CREDIT TO CARLOS RINCON, THIS IS NOT PLAGARIZED PLEASE DONT FLAG ME!!!**/

#include <unistd.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <iostream>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <sys/wait.h>

#include <cmath>

#include <vector>

#include <string>

using namespace std;

void fireman(int) {
  while (waitpid(-1, NULL, WNOHANG) > 0);
}
/** CODE INSPIRED BY BLACKBOARD CREDIT TO CARLOS RINCON, THIS IS NOT PLAGARIZED PLEASE DONT FLAG ME!!!**/

//function that takes the socket as input and receives the messages from client into it
void converToBinary(int socket) {
  int n;
  //   char *buffer = new char[255];
  char buffer[256];
  string binary;

  double px;
  double fx;

  //receiving probability
  recv(socket, & px, sizeof(double), 0);

  //if (n < 0) std::cerr<< "ERROR reading from socket";

  //receiving cumulative probability
  recv(socket, & fx, sizeof(double), 0);
  //if (n < 0) std::cerr<< "ERROR reading from socket";

  //calculating barfx
  double barfx = (fx - px) + px / 2;
  //calculating binary length hehe
  int blength = ceil(log2(1 / px)) + 1;

  //converting to binary
  double j = 0.5;
  for (int i = 0; i < blength; i++) {
    if (j > barfx) binary[i] = '0';
    else {
      binary[i] = '1';
      barfx -= j;
    }
    j /= 2;
  }

  const char * message = binary.c_str();

  send(socket, message, 256, 0);

}
/** CODE INSPIRED BY BLACKBOARD CREDIT TO CARLOS RINCON, THIS IS NOT PLAGARIZED PLEASE DONT FLAG ME!!!**/

int main(int argc, char * argv[]) {
  int sockfd, newsockfd, portno, clilen, pid;
  struct sockaddr_in serv_addr, cli_addr;

  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    std::cerr << ("ERROR opening socket");

  bzero((char * ) & serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr * ) & serv_addr,
      sizeof(serv_addr)) < 0)
    std::cerr << ("ERROR on binding");
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);

  signal(SIGCHLD, fireman);
  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr * ) & cli_addr, (socklen_t * ) & clilen);
    if (newsockfd < 0)
      std::cerr << "ERROR on accept";
    pid = fork();
    if (pid < 0)
      std::cerr << "ERROR on fork";
    if (pid == 0) {
      close(sockfd);
      converToBinary(newsockfd);
      exit(0);
    } else close(newsockfd);
  }
  return 0;

}
