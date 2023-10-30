/*
* External Mass Media Controller (EMMC) driver.
*
* Qemu emulates Host Controller version 3, so we only deal with that
*/

#include <stdint.h>
#include <stdio.h>

#include <drivers/gpio.h>

#include <drivers/emmc.h>

#define EMMC_ARG2               (volatile uint32_t*) (MMIO_BASE + 0x00300000)
#define EMMC_BLKSIZECNT         (volatile uint32_t*) (MMIO_BASE + 0x00300004)
#define EMMC_ARG1               (volatile uint32_t*) (MMIO_BASE + 0x00300008)
#define EMMC_CMDTM              (volatile uint32_t*) (MMIO_BASE + 0x0030000C)
#define EMMC_RESP0              (volatile uint32_t*) (MMIO_BASE + 0x00300010)
#define EMMC_RESP1              (volatile uint32_t*) (MMIO_BASE + 0x00300014)
#define EMMC_RESP2              (volatile uint32_t*) (MMIO_BASE + 0x00300018)
#define EMMC_RESP3              (volatile uint32_t*) (MMIO_BASE + 0x0030001C)
#define EMMC_DATA               (volatile uint32_t*) (MMIO_BASE + 0x00300020)
#define EMMC_STATUS             (volatile uint32_t*) (MMIO_BASE + 0x00300024)
#define EMMC_CONTROL0           (volatile uint32_t*) (MMIO_BASE + 0x00300028)
#define EMMC_CONTROL1           (volatile uint32_t*) (MMIO_BASE + 0x0030002C)
#define EMMC_INTERRUPT          (volatile uint32_t*) (MMIO_BASE + 0x00300030)
#define EMMC_IRPT_MASK          (volatile uint32_t*) (MMIO_BASE + 0x00300034)
#define EMMC_IRPT_EN            (volatile uint32_t*) (MMIO_BASE + 0x00300038)
#define EMMC_CONTROL2           (volatile uint32_t*) (MMIO_BASE + 0x0030003C)
#define EMMC_SLOTISR_VER        (volatile uint32_t*) (MMIO_BASE + 0x003000FC)

// Status register values
#define STAT_DAT_INHIBIT        (1 << 1) // Line still used by previous transfer
#define STAT_CMD_INHIBIT        (1 << 0) // Line still used by previous command

// Control 1 register settings
#define C1_CLK_INTLEN           (1 << 0) // Enable clock for power saving
#define C1_CLK_STABLE           (1 << 1) // Is clock stable
#define C1_CLK_EN               (1 << 2) // Is clock enabled
#define C1_CLK_FREQ8            (0x2 << 8) // Clock frequency
#define C1_DATA_TOUNIT          (0xe << 16) // Max timeout
#define C1_SRST_HC              (1 << 24) // Reset circuit

// Interrupt register values
#define INT_CMD_DONE            (1 << 0) // Command has finished
#define INT_DATA_DONE           (1 << 1) // Data transfer has finished
#define INT_ERR                 (1 << 15) // General interrupt error
#define INT_CTO_ERR             (1 << 16) // Command timeout
#define INT_CCRC_ERR            (1 << 17) // Incorrect command CRC
#define INT_CEND_ERR            (1 << 18) // Incorrect command end bit
#define INT_CBAD_ERR            (1 << 19) // Wrong command index
#define INT_DTO_ERR             (1 << 20) // Data timeout
#define INT_DCRC_ERR            (1 << 21) // Incorrect data CRC
#define INT_DEND_ERR            (1 << 22) // Incorrect data end bit
#define INT_ACMD_ERR            (1 << 24) // Auto command error

// Command Flags
#define CMD_RSPNS_TYPE_136      (1 << 16) // Expect 136 bit response
#define CMD_RSPNS_TYPE_48       (2 << 16) // Expect 48 bit response
#define CMD_RSPNS_TYPE_48B      (3 << 16) // Expect 48 bit response with busy bit
#define CMD_CRCCHK_EN           (1 << 19) // Check CRC
#define CMD_IXCHK_EN            (1 << 20) // Response has same index

// Commands
enum commands {
    GO_IDLE_STATE           = 0,
    ALL_SEND_CID            = 2,
    SEND_RELATIVE_ADDR      = 3,
    SELECT_DESELECT_CARD    = 7,
    SEND_IF_COND            = 8,
    SD_SEND_OP_COND         = 41,
    APP_CMD                 = 55,
};

uint32_t rca;

static int emmc_wait_for_interrupt(uint32_t mask) {
    int counter = 150;

    while (counter--) {
        if (*EMMC_INTERRUPT & INT_ERR) {
            printf("EMMC Interrupt error -> %x\n", *EMMC_INTERRUPT);
            *EMMC_INTERRUPT &= *EMMC_INTERRUPT;
            return SD_ERROR;
        }

        if (*EMMC_INTERRUPT & mask ) {
            *EMMC_INTERRUPT &= mask;
            return SD_OK;
        }

        /* timer_wait(20); */
    }

    return SD_TIMEOUT;
}

static int emmc_command(uint8_t code, uint32_t arg, uint32_t flags) {
    int ret;

    *EMMC_ARG1 = arg;
    *EMMC_CMDTM = (code << 24) | flags;

    if ((ret = emmc_wait_for_interrupt(INT_CMD_DONE | INT_DATA_DONE))) {
        printf("EMMC command %d failed", code);
        return ret;
    }

    return SD_OK;
}

int emmc_init() {
    int ret;
    volatile uint32_t reg;

    reg = *GPFSEL4;
    reg &= ~(0b111 << 21); // Set gpio 47 (CARD DETECT) to input
    reg |= (0b111 << 24) | (0b111 << 27); // Set gpio 48 and 49 to alt 3
    *GPFSEL4 = reg;

    // Enable High Detect on gpio 47
    reg = *GPHEN1;
    reg |= 1 << 15;
    *GPHEN1 = reg;

    reg = *GPFSEL5;
    reg |= (0b111) | (0b111 << 3) | (0b111 << 6) | (0b111 << 9); // Set gpio 50, 51, 52 and 53 to alt 3
    *GPFSEL5 = reg;

    // Enable Pull-up control
    *GPPUD = 2;
    reg = 150;
    while (reg--) { __asm__ volatile ("nop"); }
    *GPPUDCLK1 = (1 << 15) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19) | (1 << 20) | (1 << 21);
    reg = 150;
    while (reg--) { __asm__ volatile ("nop"); }
    *GPPUD = 0;
    *GPPUDCLK1 = 0;

    // Reset card
    // On real hardware it should wait for some time after each action
    *EMMC_CONTROL0 = 0;
    *EMMC_CONTROL1 |= C1_SRST_HC;

    if (*EMMC_CONTROL1 & C1_SRST_HC) {
        printf("Failed to reset EMMC\n");
        return SD_ERROR;
    }

    // Configure card
    *EMMC_CONTROL1 |= C1_CLK_INTLEN | C1_DATA_TOUNIT | C1_CLK_FREQ8;
    reg = 100;
    while (reg--) { __asm__ volatile ("nop"); }
    *EMMC_CONTROL1 |= C1_CLK_EN;

    *EMMC_IRPT_MASK = INT_CMD_DONE | INT_DATA_DONE | INT_ERR | INT_CTO_ERR |
        INT_CCRC_ERR| INT_CEND_ERR | INT_CBAD_ERR | INT_DTO_ERR | INT_DCRC_ERR |
        INT_DEND_ERR | INT_ACMD_ERR;

    *EMMC_IRPT_EN = *EMMC_IRPT_MASK;

    // Set clock
    if (*EMMC_STATUS & (STAT_DAT_INHIBIT | STAT_CMD_INHIBIT)) {
        printf("Inhibit flags are set");
        return SD_ERROR;
    }

    if (!(*EMMC_CONTROL1 & C1_CLK_STABLE)) {
        printf("Failed to get stable clock");
        return SD_ERROR;
    }

    // SD Initialisation process
    if ((ret = emmc_command(GO_IDLE_STATE, 0, 0)))
        return ret;
    if ((ret = emmc_command(SEND_IF_COND, 0x1AA, CMD_RSPNS_TYPE_48 | CMD_CRCCHK_EN | CMD_IXCHK_EN )))
        return ret;

    if (*EMMC_RESP0 != *EMMC_ARG1) {
        printf("Failed to establish voltage\n");
        return SD_ERROR;
    }

    if ((ret = emmc_command(APP_CMD, 0, CMD_RSPNS_TYPE_48 | CMD_CRCCHK_EN | CMD_IXCHK_EN)))
        return ret;
    if ((ret = emmc_command(SD_SEND_OP_COND, 0x51ff8000, CMD_RSPNS_TYPE_48B)))
        return ret;
    if ((ret = emmc_command(ALL_SEND_CID, 0, CMD_RSPNS_TYPE_136)))
        return ret;
    if ((ret = emmc_command(SEND_RELATIVE_ADDR, 0, CMD_RSPNS_TYPE_48 | CMD_IXCHK_EN | CMD_CRCCHK_EN)))
        return ret;

    rca = *EMMC_RESP0 & 0xffff0000;

    // Select card
    if ((ret = emmc_command(SELECT_DESELECT_CARD, rca, CMD_RSPNS_TYPE_48B | CMD_IXCHK_EN | CMD_CRCCHK_EN)))
        return ret;

    /* debug_log("%x\n", *EMMC_RESP0); */

    return SD_OK;
}
