/***************************************************************************
                              cec/interrupt.c
                             -------------------

    begin                : Dec 2001
    copyright            : (C) 2001, 2002 by Frank Mori Hess
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

#include "cec.h"
#include <asm/bitops.h>
#include <asm/dma.h>

/*
 * GPIB interrupt service routines
 */

void cec_interrupt(int irq, void *arg, struct pt_regs *registerp)
{
	gpib_board_t *board = arg;
	cec_private_t *priv = board->private_data;
static int i = 0;
i++;
printk("plx intcsr 0x%x\n", inl(priv->plx_iobase + PLX_INTCSR_REG));
if(i > 100) outl(0, priv->plx_iobase + PLX_INTCSR_REG);

	nec7210_interrupt(board, &priv->nec7210_priv);
}

