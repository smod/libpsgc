#ifndef PSGC_H
#define PSGC_H

/*
 * Version 1.0 (April 2012)
 *
 * Etienne Doms <etienne.doms@gmail.com>
 *
 * This library abstracts a PICASO SGC device.
 *
 * Reference: PICASO-SGC-COMMANDS-SIS-rev11.pdf
 *
 * Any function returns 0 in case of success or -1 in case of error. You must
 * first declare and then init a pointer to a psgc_t object with psgc_init().
 * psgc_init() requires the full path to the device file associated to the
 * PICASO SGC, for instance "/dev/ttyS0". At the end, psgc_exit() must be used
 * to release allocated memory and restore device file configuration.
 *
 * PSGC_RGB555(r, g, b) can be used to generate a u_int16_t RGB555 value to
 * pass to functions that require color parameters.
 *
 * PSGC_RGB565(r, g, b) can be used to generate a u_int16_t RGB565 value to
 * pass to functions that require color parameters.
 */

#include <sys/types.h>

/* types ==================================================================== */
typedef struct psgc_t psgc_t;

/* constants ================================================================ */
#define PSGC_FONT_5X7           0x00
#define PSGC_FONT_8X8           0x01
#define PSGC_FONT_8X12          0x02
#define PSGC_FONT_12X16         0x03

#define PSGC_ORIENTATION_90     0x01
#define PSGC_ORIENTATION_270    0x02
#define PSGC_ORIENTATION_0      0x03
#define PSGC_ORIENTATION_180    0x04

#define PSGC_EVENT_NONE         0x00
#define PSGC_EVENT_PRESS        0x01
#define PSGC_EVENT_RELEASE      0x02
#define PSGC_EVENT_MOVE         0x04

#define PSGC_TOUCHSCREEN_ON     0x00
#define PSGC_TOUCHSCREEN_OFF    0x01

#define PSGC_OPAQUE_OFF         0x00
#define PSGC_OPAQUE_ON          0x01

/* macros =================================================================== */
#define PSGC_RGB555(r, g, b)    (((r) << 11) | ((g) << 6) | (b))
#define PSGC_RGB565(r, g, b)    (((r) << 11) | ((g) << 5) | (b))

/* public functions ========================================================= */
int psgc_init(
    psgc_t ** psgc,
    const char * pathname
);

int psgc_exit(
    psgc_t * psgc
);

int psgc_set_background(
    psgc_t * psgc,
    u_int16_t color
);

int psgc_clear(
    psgc_t * psgc
);

int psgc_set_orientation(
    psgc_t * psgc,
    u_int8_t state
);

int psgc_set_touchscreen(
    psgc_t * psgc,
    u_int8_t state
);

int psgc_set_opaque(
    psgc_t * psgc,
    u_int8_t state
);

int psgc_draw_text(
    psgc_t * psgc,
    u_int16_t x,
    u_int16_t y,
    u_int8_t font,
    u_int16_t color,
    u_int8_t xscale,
    u_int8_t yscale,
    const char * format,
    ...
) __attribute__ ((
    format(printf, 8, 9)
));

int psgc_draw_button(
    psgc_t * psgc,
    u_int8_t pressed,
    u_int16_t x,
    u_int16_t y,
    u_int16_t bgcolor,
    u_int8_t font,
    u_int16_t fgcolor,
    u_int8_t xscale,
    u_int8_t yscale,
    const char * format,
    ...
) __attribute__ ((
    format(printf, 10, 11)
));

int psgc_read_touchscreen(
    psgc_t * psgc,
    u_int16_t * event,
    u_int16_t * x,
    u_int16_t * y
);

#endif
