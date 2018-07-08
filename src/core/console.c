/*
 * (c) 2017 Apollo Project Developers
 * core/console.c - Console interface for early kernel debugging
 *
 * TODO: Implement spinlocks and uncomment the lines for them!
 */

#include <sys/hal.h>

static console_t *consoles = NULL;

// A lock for all console operations.
static spinlock_t lock = SPINLOCK_RELEASED;

int register_console(console_t *c)
{
   // spinlock_acquire(&lock);

    if (consoles) {
        consoles->prev = c;
    }
    c->next = consoles;
    c->prev = NULL;

    consoles = c;

    // If an open() was provided, call it.
    if (c->open) {
        c->open(c);
    }
    // There's some sort of weird thing going on with console. If you
    // Don't write something to it during init, it wont work at all.
    write_console("\n", 1);
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

MODULE = {
    .name = "console",
    .required = NULL,
    .load_after = NULL,
    .init = NULL,
    .fini = &shutdown_console,
};
