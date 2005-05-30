/***************************************************************************
                          hp_82341/hp_82341_write.c  -  description
                             -------------------

    copyright            : (C) 2002, 2004, 2005 by Frank Mori Hess
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

#include "hp_82341.h"
#include <linux/delay.h>

int restart_write_fifo(gpib_board_t *board, hp_82341_private_t *hp_priv)
{	
	tms9914_private_t *tms_priv = &hp_priv->tms9914_priv;
	if((inb(hp_priv->iobase[0] + STREAM_STATUS_REG) & HALTED_STATUS_BIT) == 0)
		return 0; 
	int count = 0;
	while(1) 
	{
		int status;
++count;
		//restart doesn't work if data holdoff is in effect
		status = tms9914_line_status(board, tms_priv);
		if((status & BusNRFD) == 0)
		{
			outb(RESTART_STREAM_BIT, hp_priv->iobase[0] + STREAM_STATUS_REG); 
printk("restarted after count %i\n", count);
			return 0;
		}
		if(test_bit(DEV_CLEAR_BN, &tms_priv->state))
			return -EINTR;
		if(test_bit(TIMO_NUM, &board->status))
			return -ETIMEDOUT;
		if(msleep_interruptible(1)) return -EINTR;
	}
	return 0;
}

ssize_t hp_82341_accel_write( gpib_board_t *board, uint8_t *buffer, size_t length, int send_eoi )
{
	hp_82341_private_t *hp_priv = board->private_data;
	tms9914_private_t *tms_priv = &hp_priv->tms9914_priv;
	int i, j;
	unsigned short event_status;
	ssize_t retval = 0;
	int fifoTransferLength = length;
	if(send_eoi)
	{
		--fifoTransferLength;
	}
	clear_bit(DEV_CLEAR_BN, &tms_priv->state);
	read_and_clear_event_status(board);
	outb(0, hp_priv->iobase[3] + BUFFER_CONTROL_REG); 
	outb(0x00, hp_priv->iobase[3] + BUFFER_FLUSH_REG);
	for(i = 0; i < fifoTransferLength;)
	{
		int block_size;
		if(fifoTransferLength - i < hp_82341_fifo_size)
			block_size = fifoTransferLength - i;
		else
			block_size = hp_82341_fifo_size;
		set_transfer_counter(hp_priv, block_size);
		// load data into board's fifo
		for(j = 0; j < block_size;)
		{
			unsigned short data_word = buffer[i++];
			++j;
			if(j < block_size)
			{
				data_word |= buffer[i++] << 8;
				++j;
			}
			outw(data_word, hp_priv->iobase[3] + BUFFER_PORT_LOW_REG); 
		}
		printk("restarting write stream\n");
		clear_bit(WRITE_READY_BN, &tms_priv->state);
		outb(ENABLE_TI_BUFFER_BIT, hp_priv->iobase[3] + BUFFER_CONTROL_REG); 
		retval = restart_write_fifo(board, hp_priv);
		if(retval < 0)
		{
			printk("hp82341: failed to restart write stream\n");			
			break;
		}else
			printk("hp82341: restarted write stream\n");			
		if(wait_event_interruptible(board->wait, 
			((event_status = read_and_clear_event_status(board)) & TERMINAL_COUNT_EVENT_BIT) ||
			test_bit(DEV_CLEAR_BN, &tms_priv->state) ||
			test_bit(TIMO_NUM, &board->status)))
		{
			printk("%s: write wait interrupted\n", __FILE__);
			retval = -ERESTARTSYS;
			break;
		}
		if(test_bit(TIMO_NUM, &board->status))
		{
			printk("%s: minor %i: write timed out\n", __FILE__, board->minor);
			retval = -ETIMEDOUT;
			break;
		}
		if(test_bit(DEV_CLEAR_BN, &tms_priv->state))
		{
			printk("%s: device clear interrupted write\n", __FILE__);
			retval = -EINTR;
			break;
		}
	}
printk("turning off output fifo\n");	
	outb(0, hp_priv->iobase[3] + BUFFER_CONTROL_REG); 
	if(retval) return retval;
	if(send_eoi)
	{
printk("sending eoi byte\n");	
		retval = hp_82341_write(board, buffer + fifoTransferLength, 1, 1);
		if(retval < 0) return retval;
printk("done\n");	
printk("read ready=%i\n", test_bit(READ_READY_BN, &tms_priv->state));	
	}
	return length;
}



