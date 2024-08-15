#ifndef IO_HANDLE_CONNECTION_H_
#define IO_HANDLE_CONNECTION_H_

#include "io.h"

void handler_connection(void *args);

void handle_kernel_connection(int socket);
void handle_memory_connection(int socket);

#endif