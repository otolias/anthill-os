#ifndef _MOD_PL011
#define _MOD_PL011

#include <drivers/gpio.h>

#define DR    (volatile uint32_t *) (MMIO_BASE + 0x00201000)
#define FR    (volatile uint32_t *) (MMIO_BASE + 0x00201018)
#define IBRD  (volatile uint32_t *) (MMIO_BASE + 0x00201024)
#define FBRD  (volatile uint32_t *) (MMIO_BASE + 0x00201028)
#define LCRH  (volatile uint32_t *) (MMIO_BASE + 0x0020102c)
#define CR    (volatile uint32_t *) (MMIO_BASE + 0x00201030)
#define IFLS  (volatile uint32_t *) (MMIO_BASE + 0x00201034)
#define IMSC  (volatile uint32_t *) (MMIO_BASE + 0x00201038)
#define RIS   (volatile uint32_t *) (MMIO_BASE + 0x0020103c)
#define MIS   (volatile uint32_t *) (MMIO_BASE + 0x00201040)
#define ICR   (volatile uint32_t *) (MMIO_BASE + 0x00201044)
#define DMACR (volatile uint32_t *) (MMIO_BASE + 0x00201048)
#define ITCR  (volatile uint32_t *) (MMIO_BASE + 0x00201080)
#define ITIP  (volatile uint32_t *) (MMIO_BASE + 0x00201084)
#define ITOP  (volatile uint32_t *) (MMIO_BASE + 0x00201088)
#define TDR   (volatile uint32_t *) (MMIO_BASE + 0x0020108c)

#define DR_OE (1 << 1)

enum pl011_error {
    PL011_OK,
};

enum pl011_error pl011_init(void);

unsigned pl011_write(const unsigned char *data, unsigned n);

#endif /* _MOD_PL011 */
