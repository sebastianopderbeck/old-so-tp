#include "instruction_execution.h"

int execute_instruction_cycle(int socket, t_pcb *pcb, char *instruction)
{
	int logical_address, sleep_time;

	char **instruction_with_params = string_split(instruction, " ");

	char *instruction_type = instruction_with_params[0];

	int param;

	t_list *params_order;

	if (translate_instruction(instruction_type) == NO_OP)
	{
		log_trace(logger, "NO_OP - PID:%d", pcb->pid);
	}
	else if (translate_instruction(instruction_type) == SET)
	{
		log_trace(logger, "EXECUTING SET - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 1);
		uint32_t param_index_set = list_get(params_order, 0);
		list_remove(pcb->cpu_register, param_index_set);
		list_add_in_index(pcb->cpu_register, param_index_set, atoi(instruction_with_params[2]));
	}
	else if (translate_instruction(instruction_type) == SUM)
	{
		log_trace(logger, "EXECUTING SUM - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 2);
		uint32_t destiny_param_index_sum = list_get(params_order, 0);
		uint32_t origin_param_index_sum = list_get(params_order, 1);
		int sum_result = (uint32_t)list_get(pcb->cpu_register, destiny_param_index_sum) + (uint32_t)list_get(pcb->cpu_register, origin_param_index_sum);
		list_remove(pcb->cpu_register, destiny_param_index_sum);
		list_add_in_index(pcb->cpu_register, destiny_param_index_sum, sum_result);
	}
	else if (translate_instruction(instruction_type) == SUB)
	{
		log_trace(logger, "EXECUTING SUB - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 2);
		uint32_t destiny_param_index_sub = list_get(params_order, 0);
		uint32_t origin_param_index_sub = list_get(params_order, 1);
		int sub_result = list_get(pcb->cpu_register, destiny_param_index_sub) - list_get(pcb->cpu_register, origin_param_index_sub);
		list_remove(pcb->cpu_register, destiny_param_index_sub);
		list_add_in_index(pcb->cpu_register, destiny_param_index_sub, sub_result);
	}
	else if (translate_instruction(instruction_type) == SLEEP)
	{
		log_trace(logger, "EXECUTING SLEEP - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 1);
		int sleep_time = atoi(list_get(params_order, 0));
		sleep_to_kernel(pcb, sleep_time, SLEEP_REQUEST, socket);
		return 0;
	}
	else if (translate_instruction(instruction_type) == WAIT)
	{
		log_trace(logger, "EXECUTING WAIT - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 1);
		char *wait_resource = list_get(params_order, 0);
		send_pcb_and_resource(socket, WAIT_REQUEST, pcb, wait_resource);
		return 0;
	}
	else if (translate_instruction(instruction_type) == SIGNAL)
	{
		log_trace(logger, "EXECUTING SIGNAL - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 1);
		char *signal_resource = list_get(params_order, 0);
		send_pcb_and_resource(socket, SIGNAL_REQUEST, pcb, signal_resource);
		// TODO: check if after throw signal the process can be continue executing until receive a response
		return 0;
	}
	else if (translate_instruction(instruction_type) == JNZ)
	{
		log_trace(logger, "EXECUTING JNZ - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 1);
		uint32_t param_index_jnz = list_get(params_order, 0);
		if (list_get(pcb->cpu_register, param_index_jnz) != 0)
		{
			list_add_in_index(pcb->cpu_register, param_index_jnz, atoi(instruction_with_params[2]));
		}
	}
	else if (translate_instruction(instruction_type) == MOV_IN)
	{
		log_trace(logger, "EXECUTING MOV_IN - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 2);
		uint32_t param_index_mov_in = list_get(params_order, 0);
		uint32_t logical_address_mov_in = list_get(params_order, 1);
		// int displacement_mov_in = mmu(logical_address_mov_in, page_size);

		// TODO: RECEIVE FRAME
		// list_get(pcb->cpu_register, param_index_mov_in)[read_param_index] = read_memory((frame*page_size) + displacement_mov_in);
	}
	else if (translate_instruction(instruction_type) == MOV_OUT)
	{
		log_trace(logger, "EXECUTING MOV_OUT - PID:%d", pcb->pid);
		params_order = parse_register_param(instruction_with_params, 2);
		uint32_t param_index_mov_out = list_get(params_order, 1);
		uint32_t logical_address_mov_out = list_get(params_order, 0);
		// int displacement_move_out = mmu(logical_address_mov_out, page_size);

		// TODO: RECEIVE FRAME
		// write_memory((frame*page_size) + displacement_move_out, list_get(pcb->cpu_register, indice)[param_index_mov_out]);
	}
	else if (translate_instruction(instruction_type) == F_CLOSE)
	{
		log_trace(logger, "EXECUTING F_CLOSE - PID:%d", pcb->pid);
	}
	else if (translate_instruction(instruction_type) == F_SEEK)
	{
		log_trace(logger, "EXECUTING F_SEEK - PID:%d", pcb->pid);
	}
	else if (translate_instruction(instruction_type) == F_READ)
	{
		log_trace(logger, "EXECUTING F_READ - PID:%d", pcb->pid);
	}
	else if (translate_instruction(instruction_type) == F_WRITE)
	{
		log_trace(logger, "EXECUTING F_WRITE - PID:%d", pcb->pid);
	}
	else if (translate_instruction(instruction_type) == F_TRUNCATE)
	{
		log_trace(logger, "EXECUTING F_TRUNCATE - PID:%d", pcb->pid);
	}
	else if (translate_instruction(instruction_type) == EXIT)
	{
		log_trace(logger, "EXECUTING EXIT - PID:%d", pcb->pid);
		sem_wait(mutex_dispatch_connection);
		send_pcb(socket, FINISH_PROCESS, pcb);
		sem_post(mutex_dispatch_connection);
		log_info(logger, "Process with ID:%d was send to finish", pcb->pid);
		return 0;
	}

	return 1;
}

uint32_t read_memory(int physical_direction)
{
	// uint32_t value;
	// memcpy(&value, user_memory + physical_direction, sizeof(uint32_t));
	return 0;
}

uint32_t write_memory(int physical_direction, int value)
{
	// uint32_t value;
	// memcpy(&user_memory + physical_direction, &value, sizeof(uint32_t));
	return 0;
}

int get_param_instruction(t_list *params_instruction, int index)
{
	return (int)list_get(params_instruction, index);
}

void sleep_to_kernel(t_pcb *pcb, int sleep_time, t_operation_code header, int socket)
{
	// TODO FIX EXECUTION TIME
	// end_time = time(NULL);
	// pcb -> execution_time +=  end_time - start_time;

	log_info(logger, "Returning PCB  from process (%d) with sleep (%d)", pcb->pid, sleep_time);
	send_sleep(socket, header, pcb, sleep_time);
}

t_list *parse_register_param(char **params, int params_quantity)
{
	t_list *lista_aux = list_create();
	for (int i = 0; i < params_quantity; i++)
	{
		char *identifier = params[i + 1];
		if (strcmp(identifier, "AX") == 0)
		{
			list_add(lista_aux, 0);
		}
		else if (strcmp(identifier, "BX") == 0)
		{
			list_add(lista_aux, 1);
		}
		else if (strcmp(identifier, "CX") == 0)
		{
			list_add(lista_aux, 2);
		}
		else if (strcmp(identifier, "DX") == 0)
		{
			list_add(lista_aux, 3);
		}
		else
		{
			list_add(lista_aux, identifier);
		}
	}
	return lista_aux;
}