#ifndef CPU_HANDLE_CONNECTION_H_
#define CPU_HANDLE_CONNECTION_H_

#include "cpu.h"
#include "semaphores.h"

void handler_connection(void *args);

void handle_kernel_connection(int socket);

void execute_process(int socket);

#endif