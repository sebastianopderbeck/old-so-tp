#ifndef SHARED_OPERATION_HANDLER_H_
#define SHARED_OPERATION_HANDLER_H_

#include <stdint.h>
typedef enum
{
  DEFAULT_OPERATION,
  INSTRUCTION,
  INSTRUCTIONS,
  INTERRUPT,
  REJECTED_INTERRUPTION,
  INTERRUPT_QUANTUM,
  INTERRUPT_PRIORITY,
  DISPATCH_PROCESS,
  CREATE_PROCESS,
  PROCESS_DENIED,
  FINISH_PROCESS,
  BLOCK_PROCESS,
  EXECUTE_PROCESS,
  STOP_PLANNING,
  START_PLANNING,
  MODIFY_MULTIPROGRAMMING_DEGREE,
  SHOW_ALL_PROCESSES,
  WRITE_IN_MEMORY,
  MEMORY_OK,
  KERNEL,
  CPU,
  FILESYSTEM,
  MEMORY,
  CONSOLE,
  LOAD_PAGE,
  PAGE_FAULT_REQUEST,
  PAGE_FAULT,
  CONNECTION_CPU_MEMORY,
  PROCESS_CREATED,
  GET_FRAME,
  WRITE_MEMORY,
  READ_MEMORY,
  RESERVE_SWAP,
  WAIT_RESPONSE,
  WAIT_REQUEST,
  SLEEP_REQUEST,
  SIGNAL_REQUEST,
  SIGNAL_RESPONSE
} t_operation_code;

const char *translate_header(uint8_t operation_code);

// deprecated
// void send_instruction(int client_socket, t_operation_code *instruction);
// void receive_message(int client_socket, char *message);
// void handler_operation(int client_socket, t_operation_code *operation);
#endif