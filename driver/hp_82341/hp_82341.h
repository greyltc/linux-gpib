/***************************************************************************
                              hp_82341/hp_82341.h
                             -------------------

    copyright            : (C) 2002 by Frank Mori Hess
    email                : fmhess@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _HP82335_H
#define _HP82335_H

#include "tms9914.h"
#include "gpibP.h"


// struct which defines private_data for board
typedef struct
{
	tms9914_private_t tms9914_priv;
	unsigned int irq;
	unsigned long raw_iobase;
	unsigned short mode_control_bits;
} hp_82341_private_t;

// interfaces
extern gpib_interface_t hp_82341_interface;

// interface functions
ssize_t hp_82341_read( gpib_board_t *board, uint8_t *buffer, size_t length, int *end, int *nbytes);
ssize_t hp_82341_write( gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi );
ssize_t hp_82341_command( gpib_board_t *board, uint8_t *buffer, size_t length );
int hp_82341_take_control( gpib_board_t *board, int synchronous );
int hp_82341_go_to_standby( gpib_board_t *board );
void hp_82341_request_system_control( gpib_board_t *board, int request_control );
void hp_82341_interface_clear( gpib_board_t *board, int assert );
void hp_82341_remote_enable( gpib_board_t *board, int enable );
void hp_82341_enable_eos( gpib_board_t *board, uint8_t eos_byte, int
 compare_8_bits );
void hp_82341_disable_eos( gpib_board_t *board );
unsigned int hp_82341_update_status( gpib_board_t *board, unsigned int clear_mask );
void hp_82341_primary_address( gpib_board_t *board, unsigned int address );
void hp_82341_secondary_address( gpib_board_t *board, unsigned int address, int
 enable );
int hp_82341_parallel_poll( gpib_board_t *board, uint8_t *result );
void hp_82341_parallel_poll_configure( gpib_board_t *board, uint8_t config );
void hp_82341_parallel_poll_response( gpib_board_t *board, int ist );
void hp_82341_serial_poll_response( gpib_board_t *board, uint8_t status );
void hp_82341_return_to_local( gpib_board_t *board );

// interrupt service routines
irqreturn_t hp_82341_interrupt(int irq, void *arg, struct pt_regs *registerp);

// utility functions
int hp_82341_allocate_private(gpib_board_t *board);
void hp_82341_free_private(gpib_board_t *board);

// size of io memory region used
static const int hp_82341_iomem_size = 0x1000;

// hp 82341 register offsets
enum hp_82341_registers
{
	CONFIG_CONTROL_STATUS_REG = 0x0,
	MODE_CONTROL_STATUS_REG = 0x1,
	MONITOR_REG = 0x2,	// after initialization
	XILINX_DATA_REG = 0x2,	// before initialization, write only
	INTERRUPT_ENABLE_REG = 0x3,
	EVENT_STATUS_REG = 0x4,
	EVENT_ENABLE_REG = 0x5,
	RESTART_REG = 0x7,
	ID0_READ_REG = 0xa,
	ID1_READ_REG = 0xb,
	TRANSFER_COUNT_LOW_READ_REG = 0xc,
	TRANSFER_COUNT_MID_READ_REG = 0xd,
	TRANSFER_COUNT_HIGH_READ_REG = 0xe,
	BUFFER_PORT_LOW_READ_REG = 0x18,
	BUFFER_PORT_HIGH_READ_REG = 0x19,
	ID2_READ_REG = 0x1a,
	ID3_READ_REG = 0x1b,
	ID0_WRITE_REG = 0x402,
	ID1_WRITE_REG = 0x403,
	TRANSFER_COUNT_LOW_WRITE_REG = 0x404,
	TRANSFER_COUNT_MID_WRITE_REG = 0x405,
	TRANSFER_COUNT_HIGH_WRITE_REG = 0x406,
	BUFFER_PORT_LOW_WRITE_REG = 0xc00,
	BUFFER_PORT_HIGH_WRITE_REG = 0xc01,
	ID2_WRITE_REG = 0xc02,
	ID3_WRITE_REG = 0xc03,
	BUFFER_FLUSH_REG = 0xc04,
};

enum config_control_status_bits
{
	IRQ_SELECT_MASK = 0x7,
	DMA_CONFIG_MASK = 0x18,
	ENABLE_DMA_CONFIG_BIT = 0x20,
	XILINX_READY_BIT = 0x40,	//read only
	DONE_PGL_BIT = 0x80	
};
static inline unsigned IRQ_SELECT_BITS(int irq)
{
	return irq & IRQ_SELECT_MASK;
};
static inline unsigned DMA_CONFIG_BITS(int dma_channel)
{
	return (dma_channel << 3) & DMA_CONFIG_MASK;
};

enum mode_control_status_bits
{
	SLOT8_BIT = 0x1,	// read only
	ACTIVE_CONTROLLER_BIT = 0x2,	// read only
	ENABLE_DMA_BIT = 0x4,
	SYSTEM_CONTROLLER_BIT = 0x8,
	MONITOR_BIT = 0x10,
	ENABLE_IRQ_CONFIG_BIT = 0x20,
	ENABLE_TI_STR_BIT = 0x40
};

enum monitor_bits
{
	MONITOR_INTERRUPT_PENDING_BIT = 0x1,	// read only
	MONITOR_CLEAR_HOLDOFF_BIT = 0x2,	// write only
	MONITOR_PPOLL_BIT = 0x4,	// write clear
	MONITOR_SRQ_BIT = 0x8,	// write clear
	MONITOR_IFC_BIT = 0x10,	// write clear
	MONITOR_REN_BIT = 0x20,	// write clear
	MONITOR_END_BIT = 0x40,	// write clear
	MONITOR_DAV_BIT = 0x80	// write clear
};

enum interrupt_enable_bits
{
	ENABLE_TI_INTERRUPT_BIT = 0x1,
	ENABLE_POINTERS_EQUAL_INTERRUPT_BIT = 0x4,
	ENABLE_BUFFER_END_INTERRUPT_BIT = 0x10,
	ENABLE_TERMINAL_COUNT_INTERRUPT_BIT = 0x20,
	ENABLE_DMA_TERMINAL_COUNT_INTERRUPT_BIT = 0x80,
};

enum event_status_bits
{
	TI_INTERRUPT_EVENT_BIT = 0x1,	//write clear
	INTERRUPT_PENDING_EVENT_BIT = 0x2,	// read only
	POINTERS_EQUAL_EVENT_BIT = 0x4,	//write clear
	BUFFER_END_EVENT_BIT = 0x10,	//write clear
	TERMINAL_COUNT_EVENT_BIT = 0x20,	// write clear
	DMA_TERMINAL_COUNT_EVENT_BIT = 0x80,	// write clear
};

enum event_enable_bits
{
	ENABLE_TI_INTERRUPT_EVENT_BIT = 0x1,	//write clear
	ENABLE_POINTERS_EQUAL_EVENT_BIT = 0x4,	//write clear
	ENABLE_BUFFER_END_EVENT_BIT = 0x10,	//write clear
	ENABLE_TERMINAL_COUNT_EVENT_BIT = 0x20,	// write clear
	ENABLE_DMA_TERMINAL_COUNT_EVENT_BIT = 0x80,	// write clear
};

enum restart_bits
{
	HALTED_BIT = 0x1,	//read
	RESTART_BIT = 0x1	//write
};

#endif	// _HP82335_H
