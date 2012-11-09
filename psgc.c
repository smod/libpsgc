#include "psgc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <arpa/inet.h>

/* structures =============================================================== */
struct psgc_t {
    int fd;
    char * pathname;
    struct termios termios;
    struct termios otermios;
};

/* private functions ======================================================== */
static void reset(psgc_t * psgc) {
    close(psgc->fd);

    psgc->fd = open(psgc->pathname, O_RDWR);
}

/* public functions ========================================================= */
int psgc_init(
    psgc_t ** psgc,
    const char * pathname
) {
    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) command;

    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) response;

    if ((*psgc = malloc(sizeof **psgc)) == NULL) {
        goto err_malloc;
    }

    if (((*psgc)->fd = open(pathname, O_RDWR)) == -1) {
        goto err_open;
    }

    if (((*psgc)->pathname = strdup(pathname)) == NULL) {
        goto err_strdup;
    }

    tcgetattr((*psgc)->fd, &(*psgc)->otermios);
    (*psgc)->termios = (*psgc)->otermios;
    cfmakeraw(&(*psgc)->termios);
    cfsetspeed(&(*psgc)->termios, B9600);
    tcsetattr((*psgc)->fd, TCSANOW, &(*psgc)->termios);

    command.code = 0x55;

    write((*psgc)->fd, &command, sizeof command);
    read((*psgc)->fd, &response, sizeof response);

    (void) response.code;

    return 0;

err_strdup:
    close((*psgc)->fd);

err_open:
    free(*psgc);

err_malloc:
    return -1;
}

int psgc_exit(
    psgc_t * psgc
) {
    tcsetattr(psgc->fd, TCSANOW, &psgc->otermios);

    free(psgc->pathname);

    close(psgc->fd);

    free(psgc);

    return 0;
}

int psgc_set_background(
    psgc_t * psgc,
    u_int16_t color
) {
    struct {
        u_int8_t code;
        u_int16_t color;
    } __attribute__ ((
        packed
    )) command;

    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) response;

    command.code = 0x42;
    command.color = htons(color);

    write(psgc->fd, &command, sizeof command);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    return 0;

nack:
    reset(psgc);

    return -1;
}

int psgc_clear(
    psgc_t * psgc
) {
    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) command;

    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) response;

    command.code = 0x45;

    write(psgc->fd, &command, sizeof command);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    return 0;

nack:
    reset(psgc);

    return -1;
}

int psgc_set_orientation(
    psgc_t * psgc,
    u_int8_t state
) {
    struct {
        u_int8_t code;
        u_int8_t mode;
        u_int8_t state;
    } __attribute__ ((
        packed
    )) command;

    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) response;

    command.code = 0x59;
    command.mode = 0x04;
    command.state = state;

    write(psgc->fd, &command, sizeof command);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    return 0;

nack:
    reset(psgc);

    return -1;
}

int psgc_set_touchscreen(
    psgc_t * psgc,
    u_int8_t state
) {
    struct {
        u_int8_t code;
        u_int8_t mode;
        u_int8_t state;
    } __attribute__ ((
        packed
    )) command;

    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) response;

    command.code = 0x59;
    command.mode = 0x05;
    command.state = state;

    write(psgc->fd, &command, sizeof command);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    return 0;

nack:
    reset(psgc);

    return -1;
}

int psgc_set_opaque(
    psgc_t * psgc,
    u_int8_t state
) {
    struct {
        u_int8_t code;
        u_int8_t state;
    } __attribute__ ((
        packed
    )) command;

    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) response;

    command.code = 0x4f;
    command.state = state;

    write(psgc->fd, &command, sizeof command);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    return 0;

nack:
    reset(psgc);

    return -1;
}

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
) {
    struct {
        u_int8_t code;
        u_int16_t x;
        u_int16_t y;
        u_int8_t font;
        u_int16_t color;
        u_int8_t xscale;
        u_int8_t yscale;
    } __attribute__ ((
        packed
    )) command;

    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) response;

    char s[256];
    va_list ap;

    command.code = 0x53;
    command.x = htons(x);
    command.y = htons(y);
    command.font = font;
    command.color = htons(color);
    command.xscale = xscale;
    command.yscale = yscale;

    va_start(ap, format);
    vsnprintf(s, sizeof s, format, ap);
    va_end(ap);

    write(psgc->fd, &command, sizeof command);
    write(psgc->fd, s, strlen(s) + 1);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    return 0;

nack:
    reset(psgc);

    return -1;
}

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
) {
    struct {
        u_int8_t code;
        u_int8_t pressed;
        u_int16_t x;
        u_int16_t y;
        u_int16_t bgcolor;
        u_int8_t font;
        u_int16_t fgcolor;
        u_int8_t xscale;
        u_int8_t yscale;
    } __attribute__ ((
        packed
    )) command;

    struct {
        u_int8_t code;
    } __attribute__ ((
        packed
    )) response;

    char s[512];
    va_list ap;

    command.code = 0x62;
    command.pressed = pressed ? 0x00 : 0x01;
    command.x = htons(x);
    command.y = htons(y);
    command.bgcolor = htons(bgcolor);
    command.font = font;
    command.fgcolor = htons(fgcolor);
    command.xscale = xscale;
    command.yscale = yscale;

    va_start(ap, format);
    vsnprintf(s, sizeof s, format, ap);
    va_end(ap);

    write(psgc->fd, &command, sizeof command);
    write(psgc->fd, s, strlen(s) + 1);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    return 0;

nack:
    reset(psgc);

    return -1;
}

int psgc_read_touchscreen(
    psgc_t * psgc,
    u_int16_t * event,
    u_int16_t * x,
    u_int16_t * y
) {
    struct {
        u_int8_t code;
        u_int8_t mode;
    } __attribute__ ((
        packed
    )) command;

    union {
        u_int8_t code;
        u_int16_t event;

        struct {
            u_int16_t x;
            u_int16_t y;
        } __attribute__ ((
            packed
        )) position;
    } response;

    command.code = 0x6f;
    command.mode = 0x04;

    write(psgc->fd, &command, sizeof command);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    *event = ntohs(response.event);

    command.code = 0x6f;
    command.mode = 0x05;

    write(psgc->fd, &command, sizeof command);
    read(psgc->fd, &response, sizeof response);

    if (response.code == 0x15) {
        goto nack;
    }

    *x = ntohs(response.position.x);
    *y = ntohs(response.position.y);

    return 0;

nack:
    reset(psgc);

    return -1;
}
