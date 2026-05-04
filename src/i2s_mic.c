#include <errno.h>
#include <string.h>

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/audio/codec.h>

#include "i2s_mic.h"
#include "beat_detector.h"

#define SAMPLE_FREQUENCY    44100
#define SAMPLE_BIT_WIDTH    24
#define BYTES_PER_SAMPLE    sizeof(int32_t) /* 24-bit samples are stored in 32-bit integers */
#define NUMBER_OF_CHANNELS  1
#define SAMPLES_PER_BLOCK    1024

#define BLOCK_SIZE (SAMPLES_PER_BLOCK * NUMBER_OF_CHANNELS * BYTES_PER_SAMPLE)
#define INITIAL_BLOCKS 2
#define BLOCK_COUNT (INITIAL_BLOCKS + 2)
K_MEM_SLAB_DEFINE_STATIC(mem_slab, BLOCK_SIZE, BLOCK_COUNT, 4);

LOG_MODULE_REGISTER(i2s_mic);

#define I2S_RX_NODE  DT_NODELABEL(i2s_rx)

static const struct device *i2s_dev;

void i2s_mic_thread(void)
{
    int ret;

    const struct device *i2s_dev = DEVICE_DT_GET(I2S_RX_NODE);
    const BeatDetector *beat_detector = NULL;

    LOG_INF("Initializing I2S microphone...");

    if (!device_is_ready(i2s_dev)) {
		LOG_ERR("%s is not ready", i2s_dev->name);
		return 0;
	}

    struct i2s_config i2s_cfg = {
        .word_size = SAMPLE_BIT_WIDTH,
        .channels = NUMBER_OF_CHANNELS,
        .format = I2S_FMT_DATA_FORMAT_I2S,
        .options = I2S_OPT_FRAME_CLK_MASTER | I2S_OPT_BIT_CLK_MASTER,
        .frame_clk_freq = SAMPLE_FREQUENCY,
        .block_size = BLOCK_SIZE,
        .timeout = 1000,
    };

    beat_detector = (BeatDetector *)k_malloc(sizeof(BeatDetector));
    if (beat_detector == NULL) {
        LOG_ERR("Failed to allocate memory for BeatDetector");
        return;
    }
    beat_detector_init(beat_detector);

    ret = i2s_configure(i2s_dev, I2S_DIR_RX, &i2s_cfg);
    if (ret < 0) {
        LOG_ERR("Failed to configure I2S RX stream: %d", ret);
        return;
    }

    ret = i2s_trigger(i2s_dev, I2S_DIR_RX, I2S_TRIGGER_START);
    if (ret < 0) {
        LOG_ERR("Failed to start I2S RX stream: %d", ret);
        return;
    }

    for(;;) {
        void * mem_block;
        uint32_t block_size;

        ret = i2s_read(i2s_dev, &mem_block, &block_size);
        if (ret < 0) {
            LOG_ERR("Failed to read from I2S RX stream: %d", ret);
            break;
        }

        /* Process the audio data in mem_block of size block_size (should be SAMPLES_PER_BLOCK number of samples) */
        int32_t *samples = (int32_t *)mem_block;
        int num_samples = block_size / BYTES_PER_SAMPLE;
        

    }
}