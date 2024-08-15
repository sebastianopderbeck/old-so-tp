#ifndef CONSOLE_MAIN_H_
#define CONSOLE_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <readline/readline.h>

#include "utils.h"
#include "config.h"
#include "process.h"
#include "sockets.h"
#include "instructions.h"
typedef enum
{
  INICIAR_PROCESO,
  FINALIZAR_PROCESO,
  DETENER_PLANIFICACION,
  INICIAR_PLANIFICACION,
  EXECUTE_SCRIPT,
  PROCESO_ESTADO,
  SALIR,
  ERROR
} t_command;

t_console_config *console_config;
int kernel_connection;
t_log *logger;

void clear();
void free_console(int kernel_connection, t_console_config *console_config);
void read_file(char *file_path);
char **enter_command();
void start_console(char **arguments);
t_command translate_command(char *command);
void interactive_menu();
void start_process(char **arguments);
void start_process_by_interactive_console();
void finish_process_by_interactive_console();
void execute_script();

#endif