#ifndef SHARED_SOCKETS_H_
#define SHARED_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <assert.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <errno.h>

#include "serialization.h"
#include "operation_handler.h"
typedef enum
{
  TYPE_SOCKET_SERVER,
  TYPE_SOCKET_CLIENT
} t_socket_type;

typedef struct
{
  t_log *logger;
  int client_socket;
  char *socket_name;
} t_handler_connection_args;

typedef struct
{
  t_operation_code operation_code;
  int program_counter;
} t_instruction_request;

int waiting_client(int server_socket);

int server_listen(t_log *logger, char *identifier, int server_socket, void (*handle_connection)(void));

int create_connection(t_log *logger, const char *identifier, char *ip, char *port, t_socket_type socket_type);

void destroy_socket(int socket_to_destroy);

int send_all(int socket_to, void *buffer, size_t size);

int recv_all(int socket_from, void *destination, size_t size);

void socket_send(int socket, void *source, size_t size);

bool socket_get(int socket, void *dest, size_t size);

void send_package(int socket, t_package *package);

t_buffer *receive_payload(int socket);

t_package *receive_package(int socket_from);

uint8_t receive_header(int socket);

void send_header(int socket, uint8_t header);

void omit_header(int client_socket);
#endif