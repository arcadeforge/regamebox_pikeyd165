/* Raspberry Pi 5 GPIO backend (libgpiod 2.x).
 * GPIO offsets are BCM-numbered header lines on the RP1 GPIO chip.
 * Select the chip using PIKEYD_GPIOCHIP if the default is incorrect.
 */
#define _POSIX_C_SOURCE 200809L
#include "gpio_backend.h"
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static struct gpiod_chip *chip;
static struct gpiod_line_request *request;

int pi_gpio_init(void)
{
    const char *path = getenv("PIKEYD_GPIOCHIP");
    struct gpiod_line_config *cfg = NULL;
    struct gpiod_line_settings *out = NULL, *in = NULL;
    struct gpiod_request_config *reqcfg = NULL;
    const unsigned int outputs[] = { CLK, PL };
    const unsigned int inputs[] = { DIN };
    if (!path || !*path) path = "/dev/gpiochip0";
    chip = gpiod_chip_open(path);
    if (!chip) { perror("gpiod_chip_open"); goto fail; }
    cfg = gpiod_line_config_new();
    out = gpiod_line_settings_new();
    in = gpiod_line_settings_new();
    reqcfg = gpiod_request_config_new();
    if (!cfg || !out || !in || !reqcfg) goto fail;
    gpiod_line_settings_set_direction(out, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(out, GPIOD_LINE_VALUE_ACTIVE);
    gpiod_line_settings_set_direction(in, GPIOD_LINE_DIRECTION_INPUT);
    if (gpiod_line_config_add_line_settings(cfg, outputs, 2, out) < 0 ||
        gpiod_line_config_add_line_settings(cfg, inputs, 1, in) < 0) goto fail;
    gpiod_request_config_set_consumer(reqcfg, "pikeyd165");
    request = gpiod_chip_request_lines(chip, reqcfg, cfg);
    if (!request) { perror("gpiod_chip_request_lines"); goto fail; }
    gpiod_request_config_free(reqcfg);
    gpiod_line_settings_free(out);
    gpiod_line_settings_free(in);
    gpiod_line_config_free(cfg);
    return 0;
fail:
    if (reqcfg) gpiod_request_config_free(reqcfg);
    if (out) gpiod_line_settings_free(out);
    if (in) gpiod_line_settings_free(in);
    if (cfg) gpiod_line_config_free(cfg);
    pi_gpio_close();
    return -1;
}

void pi_gpio_close(void)
{
    if (request) { gpiod_line_request_release(request); request = NULL; }
    if (chip) { gpiod_chip_close(chip); chip = NULL; }
}
int pi_gpio_write(unsigned int offset, int value)
{
    return gpiod_line_request_set_value(request, offset,
        value ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
}
int pi_gpio_read(unsigned int offset)
{
    enum gpiod_line_value value = gpiod_line_request_get_value(request, offset);
    if (value == GPIOD_LINE_VALUE_ERROR) return -1;
    return value == GPIOD_LINE_VALUE_ACTIVE ? 1 : 0;
}
void pi_gpio_delay_us(unsigned int us)
{
    struct timespec start, now;
    uint64_t elapsed;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
        elapsed = (uint64_t)(now.tv_sec - start.tv_sec) * 1000000000ULL +
                  (uint64_t)(now.tv_nsec - start.tv_nsec);
    } while (elapsed < (uint64_t)us * 1000ULL);
}
