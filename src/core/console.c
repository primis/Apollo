/*
 * (c) 2017 Apollo Project Developers
 * core/console.c - Console interface for early kernel debugging
 *
 * TODO: Implement spinlocks and uncomment the lines for them!
 */

#include <sys/hal.h>
#include <adt/ringbuf.h>
#include <string.h>
#include <stdio.h>

static console_t *consoles = NULL;

// A lock for all console operations.
static spinlock_t lock = SPINLOCK_RELEASED;

char_ringbuf_t console_buf;    // Console buffer, used for pre-screen.
char console_char_buf[1920];   // 80*24

int register_console(console_t *c)
{
    // spinlock_acquire(&lock);
    int first_console = 0; // Used to detect if we dump the buffer.
    if (consoles) {
        consoles->prev = c;
    } else {
        first_console = 1;
    }
    c->next = consoles;
    c->prev = NULL;

    consoles = c;

    // If an open() was provided, call it.
    if (c->open) {
        c->open(c);
    }

    if (first_console) {
        char buf[1920]; // Magic number = 80*24
        char_ringbuf_read(&console_buf, buf, console_buf.buffer_length);
        // Printf works here. if we use write_console we get garbage.
        // Honestly I have no idea why.
        printf("%s",buf);
    }

    //spinlock_release(&lock);
    return 0;
}

void unregister_console(console_t *c)
{
   // spinlock_acquire(&lock);
    console_t *prev = NULL;
    console_t *this = consoles;

    while(this) {
        if(this == c) {
            if(this->next) {
                this->next->prev = prev;
            }
            if(this->prev) {
                this->prev->next = this->next;
            }
            if(!prev) {
                consoles = c;
            }

            if(this->flush) {
                this->flush(this);
            }
            if(this->close) {
                this->close(this);
            }
            break;
        }
        prev = this;
        this = this->next;
    }
    //spinlock_release(&lock);
}

// write to all consoles
void write_console(const char *buf, int len)
{
   // spinlock_acquire(&lock);
    console_t *this = consoles;
    if(consoles == NULL) {
        char_ringbuf_write(&console_buf, buf, len);
    }
    while (this) {
        if(this->write) {
            this->write(this, buf, len);
        }
        this = this->next;
    }
    //spinlock_release(&lock);
}

int read_console(char *buf, int len)
{
    if (len == 0) {
        return 0;
    }
   // spinlock_acquire(&lock);
    console_t *this = consoles;
    while (this) {
        if (this->read) {
            int n = this->read(this, buf, len);
            if (n > 0) {
                spinlock_release(&lock);
                return n;
            }
        }
        this = this->next;
        if (!this) {
            this = consoles;
        }
    }
    //spinlock_release(&lock);
    return -1;
}

static int shutdown_console()
{
    console_t *this = consoles;
    while (this) {
        if (this->flush) {
            this->flush(this);
        }
        if (this->close) {
            this->close(this);
        }
        this = this->next;
    }
    return 0;
}

int console_init()
{
    memset(console_char_buf, 0, 1920);                  // Zero out the buffer.
    console_buf = make_char_ringbuf(console_char_buf, 1920);
    return 0;
}

MODULE = {
    .name = "console",
    .required = NULL,
    .load_after = NULL,
    .init = &console_init,
    .fini = &shutdown_console,
};
