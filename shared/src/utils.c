#include "utils.h"

t_config *config;

int close_program(t_log *logger)
{
	log_info(logger, "Program close");
	exit(0);
}

int get_minimun(int first, int second)
{
	if (first < second)
		return first;
	return second;
}