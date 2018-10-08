/*
 * (c) 2018 Apollo Project Developers
 * adt/ringbuf.c - Ring Buffer implementation
 * History:
 * 10/8/18 - First implementation - Char ringbuffer only
 */

#include <adt/ringbuf.h>

char_ringbuf_t make_char_ringbuf(char *buf, int len)
{
    char_ringbuf_t temp;
    temp.buffer = temp.buffer_start = temp.buffer_end = buf;
    temp.buffer_length = len;
    return temp;
}


int char_ringbuf_read(char_ringbuf_t *state, char *buf, int len) {
    if (state->buffer_start == state->buffer_end) {
        return 0;
    }

    int n = 0;
    while (state->buffer_start != state->buffer_end && n < len) {
        buf[n++] = *state->buffer_start++;

        if (state->buffer_start >= (state->buffer+state->buffer_length)) {
            state->buffer_start -= state->buffer_length;
        }
    }
    return n;
}

void char_ringbuf_write(char_ringbuf_t *state, const char *buf, int len) {
    for (int i = 0; i < len; ++i) {
        *state->buffer_end++ = buf[i];

        // Reset to the start if we're approaching the end
        if(state->buffer_end >= (state->buffer+state->buffer_length)) {
            state->buffer_end -= state->buffer_length;
        }
    }
}


