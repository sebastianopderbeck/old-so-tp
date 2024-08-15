#include "deadlocks.h"

void init_resources()
{
  all_resources = list_create();
  all_resources_instances = list_create();
  list_of_queue_blocked = list_create();
  for (int resource_index = 0; resource_index < string_array_size(kernel_config->resources); resource_index++)
  {
    list_add(list_of_queue_blocked, queue_create());
    list_add(all_resources, kernel_config->resources[resource_index]);
    list_add(all_resources_instances, kernel_config->resources_instances[resource_index]);
  }
}

void destroy_resources()
{
  list_clean(all_resources);
  list_clean(list_of_queue_blocked);
  list_clean(all_resources_instances);

  list_destroy(all_resources);
  list_destroy(list_of_queue_blocked);
  list_destroy(all_resources_instances);
}

void check_deadlocks(t_pcb *process_to_check)
{
  log_info(logger, "Checking Deadlock...");
  char *required_resource_to_check = get_required_resource(process_to_check);

  for (int list_index = 0; list_index < list_size(list_of_queue_blocked); list_index += 1)
  {
    char *required_resource = list_get(all_resources, list_index);
    int quantity_of_process = queue_size(list_get(list_of_queue_blocked, list_index));
    for (int process_index = 0; process_index < quantity_of_process; process_index++)
    {
      t_queue *queue_blocked = list_get(list_of_queue_blocked, list_index);
      t_pcb *process_to_evaluate = list_get(queue_blocked->elements, process_index);
      if (find_resource(process_to_evaluate->resources, required_resource_to_check))
      {
        log_deadlock(process_to_check);
        log_deadlock(process_to_evaluate);
      }
      //free(queue_blocked); dont use, free() is bait
      //free(process_to_evaluate);
    }
    //free(required_resource);
  }
}

void log_deadlock(t_pcb *process_to_check)
{
  char *retained_resources = string_new();
  for (int resource_index = 0; resource_index < list_size(process_to_check->resources); resource_index++)
  {
    if (resource_index > 0)
    {
      string_append(&retained_resources, " , ");
    }
    string_append(&retained_resources, list_get(process_to_check->resources, resource_index));
  }
  log_error(logger, "DEADLOCK DETECTED PID: %d RETAINED RESOURCES: %s - REQUIRED RESOURCES: %s ", process_to_check->pid, retained_resources, get_required_resource(process_to_check));
}

/* I am dying inside */
char *get_required_resource(t_pcb *process_to_check)
{
  t_queue *queue_blocked;
  for (int list_index = 0; list_index < list_size(list_of_queue_blocked); list_index += 1)
  {
    queue_blocked = list_get(list_of_queue_blocked, list_index);
    if (find_process_from_queue(queue_blocked, process_to_check->pid))
      return list_get(all_resources, list_index);
  }
}

void add_resource_instance(char *resource_to_add)
{
  _modify_resource_instance(resource_to_add, 1);
}

void remove_resource_instance(char *resource_to_remove)
{
  _modify_resource_instance(resource_to_remove, -1);
}

/*TODO: duplicate resources in array or +1/-1 to atoi then string_itoa */
void _modify_resource_instance(char *resource_to_modify, int value_to_modify)
{
  for (int list_index = 0; list_index < list_size(all_resources_instances); list_index += 1)
  {
    char *resource_to_compare = list_get(all_resources, list_index);
    if (strcmp(resource_to_compare, resource_to_modify) == 0)
    {
      char *initial_instance = list_get(all_resources_instances, list_index);
      int value = atoi(initial_instance) + value_to_modify;
      list_replace(all_resources_instances, list_index, string_itoa(value));
      break;
    }
  }
}

char *find_resource(t_list *list_to_find, char *resource_requested)
{
  bool compare_resource(char *element)
  {
    return strcmp(element, resource_requested) == 0;
  }
  return list_find(list_to_find, (void *)*(compare_resource));
}

int quantity_resource_instance_available(char *resource)
{
  for (int resource_index = 0; resource_index < list_size(all_resources); resource_index++)
  {
    char *resource_to_compare = list_get(all_resources, resource_index);
    if (strcmp(resource_to_compare, resource) == 0)
      return atoi(list_get(all_resources_instances, resource_index));
    //free(resource_to_compare);
  }
}

int quantity_resource_instance_retained(t_pcb *process, char *resource)
{
  int resources_retained = 0;
  for (int resource_index = 0; resource_index < list_size(process->resources); resource_index++)
  {
    char *resource_to_compare = list_get(process->resources, resource_index);
    char *justCheck = list_get(process->resources, 0);
    if (strcmp(resource_to_compare, resource) == 0)
      resources_retained += 1;
  }
  return resources_retained;
}

t_list *char_array_to_list(char **resource)
{
  t_list *result_list = list_create();

  for (int i = 0; resource[i] != NULL; ++i)
  {
    list_add(result_list, resource[i]);
  }

  return result_list;
}

void free_resources_retained(t_pcb *process)
{
  for (int resource_index = 0; resource_index < list_size(process->resources); resource_index += 1)
  {
    add_resource_instance(list_get(process->resources, resource_index));
  }
}

void handle_signal_interruption(t_pcb *process, char *resource_requested)
{
  bool is_not_valid = finish_process_if_is_not_valid(process, resource_requested);
  if (is_not_valid)
    return;
  char *resource_found = find_resource(process->resources, resource_requested);
  if (resource_found)
  {
    add_resource_instance(resource_requested);
    bool removed = list_remove_element(process->resources, resource_found);
    process->program_counter += 1;
  }

  log_trace(logger, "PID:%d - SIGNAL:%s - INSTANCES:%d - AVAILABLE:%d", process->pid, resource_requested, quantity_resource_instance_retained(process, resource_requested), quantity_resource_instance_available(resource_requested));
  
  if (resource_found)
    send_pcb(cpu_dispatch_connection, SIGNAL_RESPONSE, process);
  else
    finish_process(process->pid);
  free(resource_found);
}

void handle_wait_interruption(t_pcb *process, char *resource_requested)
{
  bool is_not_valid = finish_process_if_is_not_valid(process, resource_requested);
  if (is_not_valid)
    return;
  bool has_resouses_available = quantity_resource_instance_available(resource_requested) > 0;
  if (has_resouses_available)
  {
    process->program_counter += 1;
    log_info(logger, "Assigning requested resource...");
    list_add(process->resources, resource_requested);
    remove_resource_instance(resource_requested);
  }
  else
  {
    log_warning(logger, "No resource available for the requested resource");
  }

  log_trace(logger, "PID:%d - WAIT:%s - INSTANCES:%d - AVAILABLE:%d", process->pid, resource_requested, quantity_resource_instance_retained(process, resource_requested), quantity_resource_instance_available(resource_requested));

  if (has_resouses_available)
    send_pcb(cpu_dispatch_connection, WAIT_RESPONSE, process);
  else
    move_process_from_exec_to_blocked(get_queue_blocked(resource_requested));
}

bool finish_process_if_is_not_valid(t_pcb *process, char *resource_requested)
{
  t_list *all = all_resources;
  t_list *instances = all_resources_instances;
  char *value = list_get(all, 0);
  if (!find_resource(all_resources, resource_requested))
  {
    log_warning(logger, "Process with ID: %d request Resource: %s that is not valid", process->pid, resource_requested);
    finish_process(process->pid);
    return true;
  }
  return false;
}