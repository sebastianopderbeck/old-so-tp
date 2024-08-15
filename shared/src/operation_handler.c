
#include "operation_handler.h"

const char *translate_header(uint8_t operation_code)
{
  switch (operation_code)
  {
  case DEFAULT_OPERATION:
    return "DEFAULT_OPERATION";
    break;
  case INSTRUCTION:
    return "INSTRUCTION";
    break;
  case INSTRUCTIONS:
    return "INSTRUCTIONS";
    break;
  case INTERRUPT:
    return "INTERRUPT";
    break;
  case REJECTED_INTERRUPTION:
    return "REJECTED_INTERRUPTION";
    break;
  case INTERRUPT_QUANTUM:
    return "INTERRUPT_QUANTUM";
    break;
  case INTERRUPT_PRIORITY:
    return "INTERRUPT_PRIORITY";
    break;
  case DISPATCH_PROCESS:
    return "DISPATCH_PROCESS";
    break;
  case CREATE_PROCESS:
    return "CREATE_PROCESS";
    break;
  case PROCESS_DENIED:
    return "PROCESS_DENIED";
    break;
  case FINISH_PROCESS:
    return "FINISH_PROCESS";
    break;
  case BLOCK_PROCESS:
    return "BLOCK_PROCESS";
    break;
  case EXECUTE_PROCESS:
    return "EXECUTE_PROCESS";
    break;
  case STOP_PLANNING:
    return "STOP_PLANNING";
    break;
  case START_PLANNING:
    return "START_PLANNING";
    break;
  case MODIFY_MULTIPROGRAMMING_DEGREE:
    return "MODIFY_MULTIPROGRAMMING_DEGREE";
    break;
  case SHOW_ALL_PROCESSES:
    return "SHOW_ALL_PROCESSES";
    break;
  case WRITE_IN_MEMORY:
    return "WRITE_IN_MEMORY";
    break;
  case MEMORY_OK:
    return "MEMORY_OK";
    break;
  case KERNEL:
    return "KERNEL";
    break;
  case CPU:
    return "CPU";
    break;
  case FILESYSTEM:
    return "FILESYSTEM";
    break;
  case MEMORY:
    return "MEMORY";
    break;
  case CONSOLE:
    return "CONSOLE";
    break;
  case LOAD_PAGE:
    return "LOAD_PAGE";
    break;
  case PAGE_FAULT:
    return "PAGE_FAULT";
    break;
  case PAGE_FAULT_REQUEST:
    return "PAGE_FAULT_REQUEST";
    break;
  case CONNECTION_CPU_MEMORY:
    return "CONNECTION_CPU_MEMORY";
    break;
  case PROCESS_CREATED:
    return "PROCESS_CREATED";
    break;
  case GET_FRAME:
    return "PROCESS_CREATED";
    break;
  case RESERVE_SWAP:
    return "PROCESS_CREATED";
    break;
  case WAIT_REQUEST:
    return "WAIT_REQUEST";
    break;
  case SLEEP_REQUEST:
    return "SLEEP_REQUEST";
    break;
  case SIGNAL_REQUEST:
    return "SIGNAL_REQUEST";
    break;
  case WAIT_RESPONSE:
    return "WAIT_RESPONSE";
    break;
  case SIGNAL_RESPONSE:
    return "SIGNAL_RESPONSE";
    break;
  default:
    return "KNOW_OPEARATION";
    break;
  }
}