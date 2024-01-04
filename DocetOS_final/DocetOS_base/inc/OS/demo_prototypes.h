#ifndef DEMO_PROTOTYPES_H
#define DEMO_PROTOTYPES_H

/**
*	User configurable tasks used for the demo.
*	Simply add a prototype function and then also
*	configure said function in 'main.c'. 
*/

void sleep_demoTask1(void const *args);
void sleep_demoTask2(void const *args);
void sleep_demoTask3(void const *args);

void mutex_demoTask1(void const *args);
void mutex_demoTask2(void const *args);
void mutex_demoTask3(void const *args);

void binarySemaphore_demoTask1(void const *args);
void binarySemaphore_demoTask2(void const *args);
void binarySemaphore_demoTask3(void const *args);

void memPool_demoTask(void const *args);

#endif /* DEMO_PROTOTYPES_H */
