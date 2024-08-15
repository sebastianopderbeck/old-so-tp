#ifndef MEMORY_HANDLE_CONNECTION_H_
#define MEMORY_HANDLE_CONNECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <semaphore.h>
#include <pthread.h>

#include "sockets.h"
#include "serialization.h"
#include "operation_handler.h"
#include "process.h"

#include "memory.h"
#include "page_table.h"

void handler_connection(void *args);

void handle_kernel_connection(int socket);

void handle_cpu_connection(int socket);

void handle_create_process(int socket);

void handle_filesystem_connection(int socket);

void handle_finish_process();

void handle_write_memory(socket);

void reserve_swap_memory(int socket);
// void reserve_swap_memory(int pid, t_list* swap_blocks);

void get_frame(int socket);

void handle_page_fault(int socket);

void handle_read_memory(int socket);

void handle_write_memory(int socket);

uint32_t read_memory(int physical_direction);

void write_memory(int physical_direction, uint32_t value);

void handle_instruction(int socket);

#endif