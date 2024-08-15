#ifndef MEMORY_MAIN_H_
#define MEMORY_MAIN_H_

#include "utils.h"
#include "config.h"
#include "instructions.h"
#include "process.h"
#include "handle_connection.h"

extern t_log *logger;

extern int page_time;

extern t_memory_config *memory_config;

extern void *main_memory;

extern pthread_t thread_memory, thread_filesystem;
extern int memory_connection, filesystem_connection;

extern t_list *page_table_list;
extern t_list *frames_used_list;

void reserve_memory();

void init_connections();

void *handle_memory_connection();

char *get_instruction_from_file(char *instruction_path, int program_counter);

#endif