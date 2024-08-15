#ifndef IO_MAIN_H_
#define IO_MAIN_H_

#include "utils.h"
#include "config.h"
#include "sockets.h"

#include "handle_connection.h"

extern t_log *logger;

extern t_io_config *io_config;

extern int kernel_connection, memory_connection;

#endif