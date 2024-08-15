#ifndef FILESYSTEM_MAIN_H_
#define FILESYSTEM_MAIN_H_

#include "config.h"
#include "string.h"
#include "utils.h"
#include <stdint.h>
#include "process.h"
#include "handle_connection.h"

typedef struct
{
    char *file_name;
    u_int file_size;
    u_int start_block;
} t_FCB;

extern pthread_t thread_filesystem;

extern t_filesystem_config* filesystem_config;

extern t_log *logger;

extern t_list* free_swap_blocks;

extern t_list* free_fat_blocks;

extern int filesystem_connection;

extern int memory_connection;

extern t_filesystem_config *filesystem_config;
//t_config *fcb;

void* handle_filesystem_connection();

void receive_operation(int client_socket);
// void send_instruction(int client_socket);
// t_FCB* read_fcb(char* fcb_path, char* fcb_name);

void destroy_fcb(t_FCB *fcb);

#endif