#include <sys/irqchip.h>
#include <types.h>

static irqchip_t *irq_chips = NULL;

static irqchip_t *find_irq_line(uint32_t n) {
    irqchip_t *ret = irq_chips;

    while(ret != NULL)
    {
        if ((n >= ret->isr_start) && (n < (ret->isr_start + ret->isr_len))) {
            return ret;
        }
        ret = ret->next;
    }
    return ret;
}

int irqchip_register(irqchip_t *c) {
    if(c == NULL)
    {
        return -1;  // Null pointer!!
    }
    c->next = irq_chips;
    irq_chips = c;
    return 0;
}

int irqchip_unregister(irqchip_t *c) {
    irqchip_t *current = irq_chips;
    irqchip_t *previous = NULL;
    while(current != c)
    {
        if(current->next == NULL)
        {
            return -1; // Not found in the list!
        }
        previous = current;
        current = current->next;
    }
    if(previous == NULL)
    {
        previous = current->next;
    } else {
        previous->next = current->next;
    }
    return 0;
}

void irqchip_ack(uint32_t n) {
    irqchip_t *this = find_irq_line(n);
    if(this) {
        if(this->ack) {
            this->ack(this, n - this->isr_start);
        }
    }
}

void irqchip_mask(uint32_t n) {
    irqchip_t *this = find_irq_line(n);
    if(this) {
        if(this->mask) {
            this->mask(this, n - this->isr_start);
        }
    }
}

void irqchip_unmask(uint32_t n) {
    irqchip_t *this = find_irq_line(n);
    if(this) {
        if(this->unmask) {
            this->unmask(this, n - this->isr_start);
        }
    }
}
