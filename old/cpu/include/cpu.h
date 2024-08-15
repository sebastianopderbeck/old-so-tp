#ifndef CPU_MAIN_H_
#define CPU_MAIN_H_

#include "utils.h"
#include "config.h"
#include "instruction_execution.h"
#include "handle_connection.h"

typedef struct
{
  uint32_t program_counter;
  uint32_t ax;
  uint32_t bx;
  uint32_t cx;
  uint32_t dx;
} t_execution_context;

extern t_log *logger;

extern t_cpu_config *cpu_config;

extern pthread_t thread_dispatch, thread_interrupt, thread_cpu_memory;

extern int dispatch_connection, interrupt_connection, memory_connection;

extern uint32_t page_size;

extern bool has_interrupt;

void init_connections();

void *handle_dispatch_connection();

void *handle_interrupt_connection();

void get_memory_config(void);

void *handle_memory_connection();

#endif