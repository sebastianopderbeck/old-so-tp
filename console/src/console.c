#include "console.h"

int main(int argc, char **argv)
{
	console_config = create_console_config("configuration/console.config");
	logger = log_create("logs/console.log", "CONSOLE", 1, LOG_LEVEL_TRACE);
	log_info(logger, "Starting console");

	kernel_connection = create_connection(logger, "CONSOLE - KERNEL", console_config->kernel_ip, console_config->kernel_port, TYPE_SOCKET_CLIENT);
	// send_header(kernel_connection, CONSOLE);
	if (kernel_connection != 1)
	{
		// If need a menu, uncomment the next line
		interactive_menu();

		// If need a manual input, uncomment the next line
		// start_manual_console();
	}
	free_console(kernel_connection, console_config);
	return EXIT_SUCCESS;
}

void read_file(char *file_path)
{
	char line_instruction[100];
	FILE *file_to_read = fopen(file_path, "rt");

	if (file_to_read == NULL)
	{
		log_error(logger, "File (%s) could not be opened", file_path);
		return;
	}

	while (fgets(line_instruction, sizeof(line_instruction), file_to_read) != NULL)
	{
		char **arguments = string_split(line_instruction, " ");
		start_console(arguments);
	}
	fclose(file_to_read);
}

void start_manual_console()
{
	while (1)
	{
		start_console(enter_command());
	}
}

char **enter_command()
{
	char *command = string_new();
	string_append(&command, readline(">"));
	string_trim(&command);
	char **arguments = string_split(command, " ");
	return arguments;
}

void start_console(char **arguments)
{
	t_command command = translate_command(arguments[0]);

	switch (command)
	{
	case INICIAR_PROCESO:
		start_process(arguments);
		break;
	case FINALIZAR_PROCESO:
		send_process_id(kernel_connection, FINISH_PROCESS, atoi(arguments[1]));
		log_trace(logger, "Process was sent to the kernel for elimination");
		break;
	case DETENER_PLANIFICACION:
		send_header(kernel_connection, STOP_PLANNING);
		break;
	case INICIAR_PLANIFICACION:
		send_header(kernel_connection, START_PLANNING);
		break;
	case PROCESO_ESTADO:
		send_header(kernel_connection, SHOW_ALL_PROCESSES);
		break;
	case SALIR:
		close_program(logger);
		break;
	default:
		printf("\n Wrong choice. Enter Again");
		break;
	}
}

t_command translate_command(char *command)
{
	if (strcmp(command, "INICIAR_PROCESO") == 0)
		return INICIAR_PROCESO;
	else if (strcmp(command, "FINALIZAR_PROCESO") == 0)
		return FINALIZAR_PROCESO;
	else if (strcmp(command, "DETENER_PLANIFICACION") == 0)
		return DETENER_PLANIFICACION;
	else if (strcmp(command, "INICIAR_PLANIFICACION") == 0)
		return INICIAR_PLANIFICACION;
	else if (strcmp(command, "EXECUTE_SCRIPT") == 0)
		return EXECUTE_SCRIPT;
	else if (strcmp(command, "PROCESO_ESTADO") == 0)
		return PROCESO_ESTADO;
	else if (strcmp(command, "SALIR") == 0)
		return SALIR;
	else
		return ERROR;
}

void interactive_menu()
{
	int choice = 0;
	do
	{
		// clear();
		//  TODO: We must set up a marquee if time is enough
		//  marquee();
		printf("\n");
		printf("==================================== \n");
		printf("Please select an action: \n\n");
		printf("1. Create process \n");
		printf("2. Finish process \n");
		printf("3. Start planification \n");
		printf("4. Stop planification \n");
		printf("5. Execute script \n");
		printf("6. Show process list \n");
		printf("7. Exit\n");
		printf("==================================== \n");

		char *arguments = readline(">");
		int choice = atoi(arguments);
		switch (choice)
		{
		case 1:
			start_process_by_interactive_console();
			break;
		case 2:
			finish_process_by_interactive_console();
			break;
		case 3:
			send_header(kernel_connection, START_PLANNING);
			break;
		case 4:
			send_header(kernel_connection, STOP_PLANNING);
			break;
		case 5:
			// TODO: Execute script
			execute_script();
			break;
		case 6:
			send_header(kernel_connection, SHOW_ALL_PROCESSES);
			break;
		case 7:
			close_program(logger);
			break;
		default:
			printf("Wrong choice. Enter Again");
			break;
		}
	} while (choice != 7);
}

void execute_script()
{
	printf("Execute script [PATH]:");
	char *path = readline("\n> [PATH]:");
	read_file(path);
}

void start_process_by_interactive_console()
{
	printf("Start process needs [PATH]");
	char *path = readline("\n> [PATH]:");
	char *arguments[2] = {"INICIAR_PROCESO", path};
	start_process(arguments);
}

void start_process(char **arguments)
{
	char *path = arguments[1];
	t_process *process_to_create = create_process(path);
	send_process(kernel_connection, process_to_create);
	log_trace(logger, "Process was sent to the kernel for creation");
	free(path);
	destroy_process(process_to_create);
}

void finish_process_by_interactive_console()
{
	printf("Finish process needs [ID]:");
	char *id = readline("\n> [ID]:");
	send_process_id(kernel_connection, FINISH_PROCESS, atoi(id));
	log_trace(logger, "Process was sent to the kernel for elimination");
}

void clear()
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	system("clear");
#endif

#if defined(_WIN32) || defined(_WIN64)
	system("cls");
#endif
}

void free_console(int kernel_connection, t_console_config *console_config)
{
	log_info(logger, "Closing console");
	destroy_socket(kernel_connection);
	destroy_console_config(console_config);
}
