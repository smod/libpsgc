#include "psgc.h"

#include <stdio.h>

int main(void) {
    psgc_t * psgc;

    psgc_init(&psgc, "/dev/ttyS3");
    psgc_clear(psgc);
    psgc_set_orientation(psgc,  PSGC_ORIENTATION_90);
    psgc_set_background(psgc, PSGC_RGB555(0, 0, 0));

    psgc_draw_text(
        psgc, 16, 16,
        PSGC_FONT_12X16, PSGC_RGB555(31, 31, 31),
        1, 1, "ECOLLECT @ ECOBOX"
    );

    psgc_draw_text(
        psgc, 16, 64,
        PSGC_FONT_12X16, PSGC_RGB555(31, 31, 31),
        1, 1, "System is up and ready."
    );

    psgc_draw_text(
        psgc, 16, 96,
        PSGC_FONT_12X16, PSGC_RGB555(31, 31, 31),
        1, 1, "Plug a FAT32 formatted"
    );

    psgc_draw_text(
        psgc, 16, 128,
        PSGC_FONT_12X16, PSGC_RGB555(31, 31, 31),
        1, 1, "USB key and hit INIT!"
    );

    psgc_draw_button(
        psgc, 0, 16, 192,
        PSGC_RGB555(0, 0, 31), PSGC_FONT_12X16, PSGC_RGB555(31, 31, 31),
        2, 2, "INIT"
    );

    psgc_set_touchscreen(psgc, PSGC_TOUCHSCREEN_ON);

    while (1) {
        u_int16_t event;
        u_int16_t x, y;

        psgc_read_touchscreen(psgc, &event, &x, &y);

        if (event == PSGC_EVENT_RELEASE) {
            printf("RELEASE @ %d %d\n", x, y);
        }
    }

    psgc_exit(psgc);

    return 0;
}
