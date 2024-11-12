#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "messages.h"

char* sum(char* path_one, char* path_two) {
    if (!path_one || !path_two) {
        return NULL;
    }

    int first_num = atoi(path_one);
    int second_num = atoi(path_two);
    int sum = first_num + second_num;

    int sum_size = snprintf(NULL, 0, "a = %d\nb = %d\nsum = %d\n\n", first_num, second_num, sum) + 1;
    char* result = malloc(sum_size);
    
    if (result != NULL) {
        snprintf(result, sum_size, "a = %d\nb = %d\nsum = %d\n\n", first_num, second_num, sum);
    }

    return result;
}

bool check_finished_message(char* buffer) {
    if (strlen(buffer) < 10) {
        return false;
    } else if (strncmp(buffer, "GET ", 4) != 0) {
        return false;
    }
        // } else if (strcmp(buffer + strlen(buffer) - 4, "\r\n\r\n") != 0) {
        //     return false;
        // }
    return true;
}

void read_given_client_message(int a_client, client_http_message** message_structure, results_http* result) {
    *message_structure = malloc(sizeof(client_http_message));
    if (*message_structure == NULL) {
        *result = BAD_REQUEST;
        return;
    }

    char buffer[1024];
    strcpy(buffer, "");

    while (!check_finished_message(buffer)) {
        int read_bytes = read(a_client, buffer + strlen(buffer), sizeof(buffer) - strlen(buffer));

        if (read_bytes == 0) {
            *result = CLOSED_CONNECTION;
            return;
        } else if (read_bytes < 0) {
            *result = BAD_REQUEST;
            return;
        }
    }

    // (*message_structure) -> method = strdup(buffer + 4);
    // char* start_path = buffer + 4;
    // char* end_path = strchr(buffer + 4, ' ');
    // char* end_http = strchr(end_path, ' ');
    
    char *method = strtok(buffer, " ");
    char *path = strtok(NULL, " ");
    char *version = strtok(NULL, "\r\n");

    if (!method || !path || !version) {
        *result = BAD_REQUEST;
        free(*message_structure);
        return;
    }

    (*message_structure)-> method = strdup(method);
    (*message_structure)-> path = strdup(path);
    (*message_structure)-> http_vnum = strdup(version);

    char *start_path = strtok(path, "/");
    char *first_sub_p = NULL;
    char *second_sub_p = NULL;

    // if (end_path != NULL) {
    //     (*message_structure) -> path = strndup(start_path, end_path - start_path);
    // }

    if (strcmp(start_path, "calc") == 0) {
        printf("Path to calc\n");

        first_sub_p = strtok(NULL, "/");
        printf("a = %s\n", first_sub_p);

        second_sub_p = strtok(NULL, "/");
        printf("b = %s\n", second_sub_p);

        (*message_structure) -> body = sum(first_sub_p, second_sub_p);

    } else if (strcmp((*message_structure) -> path, "/stats") == 0) {
        printf("Path to stats\n");
    } else if (strcmp((*message_structure) -> path, "/static") == 0) {
        printf("Path to static\n");
    }
    

    *result = MESSAGE;
}

void free_client_message(client_http_message* message_structure) {
    free(message_structure);
}