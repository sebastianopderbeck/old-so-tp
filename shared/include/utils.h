#ifndef SHARED_UTILS_H_
#define SHARED_UTILS_H_

#include <commons/log.h>
#include <sys/socket.h>
#include <commons/string.h>
#include <semaphore.h>
#include <stdlib.h>
#include <commons/config.h>

extern t_log *logger;
extern t_config *config;

int close_program(t_log *logger);
int get_minimun(int first, int second);
#endif