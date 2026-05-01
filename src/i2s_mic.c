#include <errno.h>
#include <string.h>

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/audio/codec.h>

#define SAMPLE_FREQUENCY    44100
#define SAMPLE_BIT_WIDTH    24
#define BYTES_PER_SAMPLE    sizeof(int32_t) /* 24-bit samples are stored in 32-bit integers */
#define NUMBER_OF_CHANNELS  2
#define BUFFER_SIZE         (SAMPLE_FREQUENCY * BYTES_PER_SAMPLE * NUMBER_OF_CHANNELS)

LOG_MODULE_REGISTER(i2s_mic);

#define I2S_RX_NODE  DT_NODELABEL(i2s_rx)