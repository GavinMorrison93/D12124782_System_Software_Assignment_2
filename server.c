#include<stdio.h>
#include<string.h> 
#include<sys/socket.h> 
#include<arpa/inet.h> 
#include<unistd.h> 
#include<stdlib.h>

int main(int argc, char * argv[]) {

    int socket_descriptor;
    int client_socket;
    int conn_size;
    int READSIZE;
    char client_response[500];
    pid_t child_pid;
    struct sockaddr_in server, client;

    //-create socket
    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor == -1) {
      printf("\nCould not create socket");
    } else {
      printf("\nSocket successfully created!!!");
    }

    // Set sockaddr_in variables
    server.sin_port = htons(8081);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(socket_descriptor, (struct sockaddr * ) & server, sizeof(server)) < 0) {
      perror("\nBind issue!!\n");
      return 1;
    } else {
      printf("\nBind complete!!\n");
    }

    // Listen for a connection
    listen(socket_descriptor, 3);

    // Accept an incoming connection
    printf("\nWaiting for incoming connection from client...\n");
    conn_size = sizeof(struct sockaddr_in);

    // Added for multithread
    while (1) {

      // Accept connection from incoming client
      client_socket = accept(socket_descriptor, (struct sockaddr * ) & client, (socklen_t * ) & conn_size);
      if (client_socket < 0) {
        perror("\nCan't establish connection\n");
        return 1;
      } else {
        printf("\nConnection from client accepted\n");
      }

      // Added for multithread - forking point
      if ((child_pid = fork()) == 0) {
        close(socket_descriptor);

        while (1) {
          memset(client_response, 0, 500);
          if (recv(client_socket, client_response, 50, 0) > 1) {
            printf("\nDestination path: %s\n", client_response);
            break;
          }
        }

        while (1) {
          char file_buffer[512]; // Receiver buffer
          char * file_name = client_response;
          FILE * file_open = fopen(file_name, "w");

          if (file_open == NULL) {
            printf("\nFile %s cannot be opened on server.\n", file_name);
          } else {
            bzero(file_buffer, 512);
            int block_size = 0;
            int i = 0;

            while ((block_size = recv(client_socket, file_buffer, 512, 0)) > 0) {
              printf("\n[Server] Data Received %d = %d\n", i, block_size);
              int write_sz = fwrite(file_buffer, sizeof(char), block_size, file_open);
              bzero(file_buffer, 512);
              i++;

            }
            printf("\nTransfer Complete!\n");
            fclose(file_open);
            exit(1);
          }
          // Send confirmation to CLient that transfer was successful
          char confirmation_buffer[500];
          strcpy(confirmation_buffer, "Transfer completed successfully!");
          while (1) {
            if (send(client_socket, confirmation_buffer, strlen(confirmation_buffer), 0) > 0);

          }
          close(client_socket);

          return 0;
        }
    }
  }
  return 0;
}
