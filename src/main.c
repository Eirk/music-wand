#include <errno.h>
#include <string.h>

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <inttypes.h>

#include "i2s_mic.h"

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

#define STRIP_NODE		DT_ALIAS(led_strip)
#define BUTTON_NODE		DT_ALIAS(sw0)

#if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)
#else
#error Unable to determine length of LED strip
#endif

#define DELAY_TIME K_MSEC(500)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static struct gpio_callback button_cb_data;

LOG_MODULE_REGISTER(main);

K_MSGQ_DEFINE(btn_press_msgq, sizeof(uint32_t), 10, 1);
K_MSGQ_DEFINE(beat_msgq, sizeof(struct beat_msg), 10, 1);

static void button_input_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_msgq_put(&beat_msgq, &(struct beat_msg){.energy = 1.0}, K_NO_WAIT);
}

int main(void)
{
	int ret;

	LOG_INF("Starting music wand firmware");

	if (!gpio_is_ready_dt(&button)) {
		LOG_ERR("Error: button device %s is not ready\n",
		       button.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure %s pin %d\n",
		       ret, button.port->name, button.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		LOG_ERR("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button.port->name, button.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_input_cb, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);
	LOG_INF("Set up button at %s pin %d\n", button.port->name, button.pin);

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
	int rc;
	const uint8_t attack = 0xf0;
	const uint8_t release = 0x0a;
	uint8_t brightness = 0;

	static struct led_rgb color = {
		.r = 0,
		.g = 0,
		.b = 0,
	};

	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return;
	}

	k_msleep(1000); /* Wait for other threads to initialize */

	LOG_INF("Displaying pattern on strip");
	for(;;) {
		struct beat_msg msg;
		while(k_msgq_get(&beat_msgq, &msg, K_NO_WAIT) == 0) {
			/* Handle beat message */
			LOG_INF("Received beat message with energy: %.6f", (double)msg.energy);
			brightness = (uint8_t)(msg.energy * attack);
		}
		color.r = brightness;
		color.g = brightness;
		color.b = brightness;
		// write the color to the LED strip
		for (size_t cursor = 0; cursor < ARRAY_SIZE(pixels); cursor++) {
			memset(&pixels, 0x00, sizeof(pixels));
			memcpy(&pixels[cursor], &color, sizeof(struct led_rgb));

			rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
			if (rc) {
				LOG_ERR("couldn't update strip: %d", rc);
			}
		}

		if (brightness < release) {
			brightness = 0;
		} else {
			brightness -= release;
		}
		k_msleep(25);
	}
}

K_THREAD_DEFINE(led_thread_id, STACKSIZE * 2, led_thread, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(i2s_mic_thread_id, STACKSIZE * 8, i2s_mic_thread, NULL, NULL, NULL, PRIORITY, 0, 0);