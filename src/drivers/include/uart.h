#include "gpio.h"

#define AUX_ENABLES     ((volatile uint32_t*) (MMIO_BASE + 0x00215004))
#define AUX_MU_IO_REG   ((volatile uint32_t*) (MMIO_BASE + 0x00215040))
#define AUX_MU_IER_REG  ((volatile uint32_t*) (MMIO_BASE + 0x00215044))
#define AUX_MU_IIR_REG  ((volatile uint32_t*) (MMIO_BASE + 0x00215048))
#define AUX_MU_LCR_REG  ((volatile uint32_t*) (MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR_REG  ((volatile uint32_t*) (MMIO_BASE + 0x00215050))
#define AUX_MU_LSR_REG  ((volatile uint32_t*) (MMIO_BASE + 0x00215054))
#define AUX_MU_MSR_REG  ((volatile uint32_t*) (MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH  ((volatile uint32_t*) (MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL_REG ((volatile uint32_t*) (MMIO_BASE + 0x00215060))
#define AUX_MU_STAT_REG ((volatile uint32_t*) (MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD_REG ((volatile uint32_t*) (MMIO_BASE + 0x00215068))

/*
* Initialise uart
*/
void uart_init();
/*
* Send character via uart
*/
void uart_send_char(char c);
/*
* Receive character via uart
*/
char uart_get_char();
/*
* Send string via uart
*/
void uart_puts(char *string);
