#ifndef SHARED_PROCESS_H
#define SHARED_PROCESS_H

#include <stdio.h>
#include <string.h>
#include <commons/collections/list.h>
#include "sockets.h"
#include "serialization.h"
#include "instructions.h"
#include "operation_handler.h"
#include "process.h"

typedef enum
{
  NEW,
  READY,
  EXECUTION,
  BLOCKED,
} t_process_state;

typedef struct
{
  int pid;
  int program_counter;
  int size;
  int quantum;
  t_list *cpu_register;
  t_list *resources;
  // t_instruction_list *instruction_list;
  int32_t table_pages;
  char *path;
  t_process_state state;
  int client_socket;
  // int execution_time;
} t_pcb;

typedef struct
{
  int sleep_time;
  t_pcb *socketpcb;
} t_sleep;

const char *translate_process_state(t_process_state state);

t_process *create_process(char *path);

void destroy_process(t_process *process);

void send_process(int socket, t_process *process);

void add_process_to_buffer(t_buffer *buffer, t_process *process);

t_process *take_process_from_buffer(t_buffer *buffer);

void send_pcb(int socket, uint8_t header, t_pcb *pcb);

void send_process_id(int socket, uint8_t header, int id);

void send_page_to_load(int socket, uint8_t header, int page);

int get_page_from_socket(int socket);

int get_int_from_socket(int socket);

t_pcb *take_pcb_from_buffer(t_buffer *buffer);

t_pcb *get_pcb_from_socket(int socket);

void destroy_pcb(t_pcb *pcb);

void send_multiprogramming_degree(int socket, uint8_t header, int new_degree);

void send_int(int socket, uint8_t header, int integer);

int32_t get_new_degree_from_socket(int socket);

uint32_t get_process_id_from_socket(int socket);

void add_pcb_to_buffer(t_package *package, t_pcb *pcb);

void send_sleep(int socket, uint8_t header, t_pcb *pcb, int sleep_time);

bool sort_by_priority(t_pcb *first_process, t_pcb *second_process);

void send_pcb_and_resource(int socket, uint8_t header, t_pcb *pcb, char *resource);

void send_swap(int socket, uint8_t header, int pid, t_list *list);
#endif