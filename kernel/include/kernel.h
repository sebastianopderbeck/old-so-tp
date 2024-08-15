#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#include "sockets.h"
#include "process.h"

#include "handle_connection.h"
#include "config.h"
#include "semaphores.h"
#include "planners.h"
#include "utils.h"

extern t_log *logger;

extern t_kernel_config *kernel_config;

extern int memory_connection, filesystem_connection, cpu_dispatch_connection, cpu_interrupt_connection, console_connection;

// TODO: check if necessary
// extern int multiprogramming_degree;

extern pthread_t thread_cpu_dispatch, thread_console, thread_cpu_interrupt;

void *handle_console_connection();

void init_connections();

#endif