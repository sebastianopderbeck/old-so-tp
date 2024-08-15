#ifndef CPU_INSTRUCTION_EXECUTION_H_
#define CPU_INSTRUCTION_EXECUTION_H_

#include "utils.h"
#include "process.h"
#include "cpu.h"
#include "operation_handler.h"

// time_t start_time, end_time;

int execute_instruction_cycle(int socket, t_pcb *pcb, char *instruction);

void write_in_memory(int logical_address, uint32_t intruction_param, uint32_t pid);

uint32_t search_operand(int logical_address, uint32_t pid);

t_list *parse_register_param(char **params_instruction, int index);

void update_pcb_and_give_back_to_kernel(t_pcb *pcb, t_operation_code header, int socket);

void sleep_to_kernel(t_pcb *pcb, int sleep_time, t_operation_code header, int socket);

#endif