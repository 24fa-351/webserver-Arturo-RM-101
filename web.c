#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include "messages.h"

// This will be worked on as PORT 46645 connect correctly
#define STARTING_PORT 80
#define LISTEN_BACKLOG 5

int respond_client_message(int a_client, client_http_message *message_http) {
    char* message_response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    write(a_client, message_response, strlen(message_response));
    return 0;
}

void* handleConnection(void* ptr)
{
    int* a_client_ptr = (int*)ptr;
    int a_client = *a_client_ptr;
    free(a_client_ptr);

    char buffer[1024];

    while(1) {
        client_http_message *message_http;
        results_http result;

        read_given_client_message(a_client, &message_http, &result);

        if(result == BAD_REQUEST) {
            printf("Bad Request Made\n");
            close(a_client);
            return NULL;
        } else if (result == CLOSED_CONNECTION) {
            printf("Connection terminated\n");
            close(a_client);
            return NULL;
        }
        respond_client_message (a_client, message_http);
        free_client_message(message_http);
    }
    printf("Client %d terminated\n", a_client);
    return NULL;
}

int main(int argc, char* argv[])
{
    int port_num;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (argc != 3 || strcmp(argv[1], "-p") != 0) {
        // If the user doesn't want to change ports we keep
        // Port as 80 unless it's not changed
        port_num = STARTING_PORT;
    } else {
        port_num = atoi(argv[2]);
    }
    

    if (socket_fd < 0) {
        perror("Socket failed to be created\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port_num);

    int returnval;

    returnval = bind(
        socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address));

    if (returnval != 0) {
        perror("socket wasn't binded\n");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    returnval = listen(socket_fd, LISTEN_BACKLOG);

    if (returnval == -1) {
        perror ("Socket listen failed\n");
        close (socket_fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    int client_fd;

    while (1) {

        client_fd = accept(socket_fd, (struct sockaddr*)&client_address, &client_address_len);
        if (client_fd == -1) {
            break;
        }

        int* client_fd_location = malloc(sizeof(int));
        *client_fd_location = client_fd;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handleConnection, client_fd_location);
        pthread_detach(thread_id);
        //handleConnection(client_fd);
        
    }

    close (client_fd);
    return 0;
}