#include <errno.h>
#include <string.h>

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>

#include "i2s_mic.h"

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

#define STRIP_NODE		DT_ALIAS(led_strip)

#if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)
#else
#error Unable to determine length of LED strip
#endif

#define DELAY_TIME K_MSEC(500)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
	RGB(0xf0, 0x00, 0x00), /* red */
	RGB(0x00, 0xf0, 0x00), /* green */
	RGB(0x00, 0x00, 0xf0), /* blue */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

LOG_MODULE_REGISTER(main);

K_MSGQ_DEFINE(btn_press_msgq, sizeof(uint32_t), 10, 1);

int main(void)
{
	LOG_INF("Starting music wand firmware");

	enum states {
		STATE_SLEEP,
		STATE_MUSIC,
		STATE_FLICK,
		NUM_STATES
	} state = STATE_SLEEP;
	enum states prev_state = NUM_STATES;

	uint32_t btn_press;

	while (1) {
		while(k_msgq_get(&btn_press_msgq, &btn_press, K_MSEC(100)) == 0) {
			state++;
			if(state >= NUM_STATES) {
				state = 0;
			}
		}
		switch(state) {
			case STATE_SLEEP:
				/* Handle sleep state */
				break;
			case STATE_MUSIC:
				/* Handle music state */
				if(state != prev_state) {
					// first state entry, suspend mic thread
				}
				break;
			case STATE_FLICK:
				/* Handle flick state */
				if(state != prev_state) {
					// first state entry, suspend mic thread
				}
				break;
			default:
				LOG_ERR("Invalid state: %d", state);
				state = STATE_SLEEP;
				break;
		}
		if(state != prev_state) {
			LOG_INF("State changed to %d", state);
			prev_state = state;
		}
	}
}

static void led_thread(void)
{
	size_t color = 0;
	int rc;

	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return;
	}

	LOG_INF("Displaying pattern on strip");
	for(;;) {
		for (size_t cursor = 0; cursor < ARRAY_SIZE(pixels); cursor++) {
			memset(&pixels, 0x00, sizeof(pixels));
			memcpy(&pixels[cursor], &colors[color], sizeof(struct led_rgb));

			rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
			if (rc) {
				LOG_ERR("couldn't update strip: %d", rc);
			}

			k_sleep(DELAY_TIME);
		}

		color = (color + 1) % ARRAY_SIZE(colors);
	}
}

K_THREAD_DEFINE(led_thread_id, STACKSIZE, led_thread, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(i2s_mic_thread_id, STACKSIZE, i2s_mic_thread, NULL, NULL, NULL, PRIORITY, 0, 0);