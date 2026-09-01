#include <errno.h>
#include <string.h>

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>

#include <zephyr/drivers/sensor.h>
#include "imu_manager.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

/* Register logging module */
LOG_MODULE_REGISTER(imu_thread, LOG_LEVEL_INF);

#define SAMPLING_PERIOD_MS 500

void imu_thread(void *p1, void *p2, void *p3)
{
    /* Fetch device from the devicetree label defined in the board overlay */
    const struct device *const imu_dev = DEVICE_DT_GET_ONE(st_ism330dhcx);

    if (imu_dev == NULL) {
        LOG_ERR("No st,ism330dhcx device found in devicetree!");
        return;
    }

    if (!device_is_ready(imu_dev)) {
        LOG_ERR("IMU device %s is not ready!", imu_dev->name);
        return;
    }

    LOG_INF("IMU device %s initialized successfully", imu_dev->name);

    struct sensor_value accel[3];
    struct sensor_value gyro[3];

    while (1) {
        /* Trigger a sensor sample fetch from the hardware */
        int rc = sensor_sample_fetch(imu_dev);
        if (rc < 0) {
            LOG_WRN("Failed to fetch sample from IMU (err: %d)", rc);
            k_msleep(SAMPLING_PERIOD_MS);
            continue;
        }

        /* Read the specific channels into Zephyr's standard struct */
        sensor_channel_get(imu_dev, SENSOR_CHAN_ACCEL_XYZ, accel);
        sensor_channel_get(imu_dev, SENSOR_CHAN_GYRO_XYZ, gyro);

        /* Log data using built-in float conversion helper */
        LOG_INF("Accel [m/s²]: X=%.2f, Y=%.2f, Z=%.2f",
                sensor_value_to_double(&accel[0]),
                sensor_value_to_double(&accel[1]),
                sensor_value_to_double(&accel[2]));

        LOG_INF("Gyro [rad/s]: X=%.2f, Y=%.2f, Z=%.2f",
                sensor_value_to_double(&gyro[0]),
                sensor_value_to_double(&gyro[1]),
                sensor_value_to_double(&gyro[2]));

        /* Yield the CPU until the next sampling period */
        k_msleep(SAMPLING_PERIOD_MS);
    }
}
