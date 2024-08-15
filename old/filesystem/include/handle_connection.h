#ifndef FILESYSTEM_HANDLE_CONNECTION_H_
#define FILESYSTEM_HANDLE_CONNECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>

#include "sockets.h"
#include "operation_handler.h"

#include "filesystem.h"

void handler_connection(void *args);

void handle_memory_connection(int socket);

void handle_kernel_connection(int socket);

void handle_create_process(int socket);

void handle_finish_process(int socket);

void handle_write_swap();

void handle_read_swap(int socket);

t_list *reserve_swap(int reserve_quantity);

t_list *free_swap(char *blocks_path, t_list *free_blocks, int block_size, int swap_blocks);

void handle_swap(t_list *swap_blocks);

#endif