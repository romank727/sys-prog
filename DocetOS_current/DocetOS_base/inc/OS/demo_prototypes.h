#ifndef DEMO_PROTOTYPES_H
#define DEMO_PROTOTYPES_H

void fixedSchedulerAndSleep_demoTask1(void const *args);
void fixedSchedulerAndSleep_demoTask2(void const *args);
void fixedSchedulerAndSleep_demoTask3(void const *args);

void mutex_demoTask1(void const *args);
void mutex_demoTask2(void const *args);
void mutex_demoTask3(void const *args);

void binarySemaphore_demoTask1(void const *args);
void binarySemaphore_demoTask2(void const *args);
void binarySemaphore_demoTask3(void const *args);

void memPoolAlloc_demoTask(void const *args);
void memPoolDealloc_demoTask(void const *args);

#endif /* DEMO_PROTOTYPES_H */
