#ifndef SLEEP_H
#define SLEEP_H

/**
* Inserts a task into the sleep list using insertion sort. Ensures that
* the task is inserted into the correct position based on its wake-up time.
*
* @param	taskToInsert	Void pointer to the task that needs to be 
*                     	inserted into the sleep list.
*												Pointer is typecasted in the function.
*/
void i_sort_sleep_list(void * taskToInsert);

#endif /* SLEEP_H */
