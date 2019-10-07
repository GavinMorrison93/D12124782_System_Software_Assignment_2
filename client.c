#include<stdio.h>
#include<string.h> 
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h> 
#include<stdlib.h>
#include<pwd.h>
#include<grp.h>
#include<pthread.h>

#include "permissions_handler.h"

int main(int argc, char * argv[]) {

    permissions_set();

    int client_socket;
    char local_file_name[100];
    char menu_option[2];
    char file_directory[500];
    char copy_message[2];
    char * dest_path;
    char * final_dest_path;
    char local[30];
    char dest_path_buffer[500];
    pthread_mutex_t xfer_lock;
    struct sockaddr_in server;

    // Mutex is initiated 
    pthread_mutex_init( & xfer_lock, NULL);

    //create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
      printf("\nError creating socket");
    } else {
      printf("\nSocket created");
    }
    //set sockaddr_in variables
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;

    //connect to server
    if (connect(client_socket, (struct sockaddr * ) & server, sizeof(server)) < 0) {
      printf("\nConnect failed. error");
      return 1;
    }
    printf("\nConnected to server ok!!\n");
    printf("Menu Options:\n\n1: Transfer file \n2: Logout\n\nEnter choice: ");
    scanf("%s", menu_option);

    if (strcmp(menu_option, "1") == 0) {

      // Initiate transfer
      printf("\nInitiating transfer\n");
      printf("\nEnter the name of the file: ");
      scanf("%s", local_file_name);
      strcpy(file_directory, "/tmp/file_storage/");
      strcat(file_directory, local_file_name);

      printf("\nLocal File Path: %s\n", file_directory);

      // Choose a file destination    
      printf("\nChoose a destination location:\n\n1: Root\n2: Sales\n3: Promotions\n4: Offers\n5: Marketing\n\nEnter choice: ");
      scanf("%s", menu_option);

      if (strcmp(menu_option, "1") == 0) {
        strcpy(dest_path, "/var/intranet/");
      } else if (strcmp(menu_option, "2") == 0) {
        strcpy(dest_path, "/var/intranet/sales/");
      } else if (strcmp(menu_option, "3") == 0) {
        strcpy(dest_path, "/var/intranet/promotions/");
      } else if (strcmp(menu_option, "4") == 0) {
        strcpy(dest_path, "/var/intranet/offers/");
      } else if (strcmp(menu_option, "5") == 0) {
        strcpy(dest_path, "/var/intranet/marketing");
      } else {
        printf("Incorrect entry");
        return 1;
      }

      final_dest_path = strcat(dest_path, local_file_name);

      // This is a replicated function which I  tried to remove but a segementation fault was thrown that I didn't have time to debug
      strcpy(local, "/tmp/file_storage/");
      strcat(local, local_file_name);

      strcpy(dest_path_buffer, final_dest_path);

      while (1) {
        if (send(client_socket, dest_path_buffer, strlen(dest_path_buffer), 0) > 1); {
          break;
        }
      }

      while (1) {
        printf("Do you want to proceed ?\n\n1: Yes\n2: No\n\nEnter choice: ");
        scanf("%s", copy_message);
        if (strcmp(copy_message, "1") == 0) {

          // Mutex is locked before transfer begins
          pthread_mutex_lock( & xfer_lock);

          char * file_name = local;
          char file_buffer[512];
          printf("\n[Client] Sending %s to the Server... \n", local_file_name);
          FILE * file_open = fopen(file_name, "r");
          bzero(file_buffer, 512);
          int block_size, i = 0;
          while ((block_size = fread(file_buffer, sizeof(char), 512, file_open)) > 0) {
            printf("\nData Sent %d = %d\n", i, block_size);
            if (send(client_socket, file_buffer, block_size, 0) < 0) {
              exit(1);
            }
            bzero(file_buffer, 512);
            i++;
          }
        }
        // Receive confirmation of transfer from Server
        char server_response[500];
        while (1) {
          if (recv(client_socket, server_response, 50, 0) > 0);
          printf(server_response);

          // Mutex unlocked now that transfer is complete
          pthread_mutex_unlock( & xfer_lock);

          close(client_socket);

          return 0;
        }
      }
      return 0;
    }
    permissions_reset();

  }




