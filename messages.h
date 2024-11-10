#ifndef MESSAGES_H
#define MESSAGES_H

typedef struct message_structure {
    char* method;
    char* path;
    char* http_vnum;
    char* body;
    int body_length;
    char* header;
} client_http_message;

typedef enum {
    BAD_REQUEST,
    CLOSED_CONNECTION,
    MESSAGE
} results_http;

void read_given_client_message(int a_client, client_http_message** message_structure, results_http* result);
void free_client_message(client_http_message* message_structure);

#endif