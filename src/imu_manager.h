#ifndef IMU_MANAGER_H
#define IMU_MANAGER_H

/* Thread configuration */
#define IMU_STACK_SIZE 2048
#define IMU_PRIORITY 7

/* Define and initialize the thread */
void imu_thread(void *p1, void *p2, void *p3);

#endif // IMU_MANAGER_H