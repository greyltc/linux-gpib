
#include "ib_internal.h"
#include <ibP.h>
#include <stdlib.h>

int ibonl( int ud, int onl )
{
	int oflags=0;
	ibConf_t *conf = ibConfigs[ud];
	int retval;
	ibBoard_t *board;
	int status = ibsta & CMPL;
	online_ioctl_t online_cmd;

	if( ibCheckDescriptor( ud ) < 0 )
	{
		status |= ERR;
		ibsta = status;
		return status;
	}

	board = &ibBoard[ conf->board ];

	// XXX
	if( conf->flags & CN_EXCLUSIVE )
		oflags |= O_EXCL;

	if( ( board->fileno < 0 )
		&& ( ibBoardOpen( conf->board, oflags ) & ERR ) )
	{
		status |= ERR;
		iberr = EDVR;
		ibcnt = errno;
		ibsta = status;
		return status;
	}

	online_cmd.master = board->is_system_controller;
	online_cmd.online = onl;
	retval = ioctl( board->fileno, IBONL, &online_cmd );
	if( retval < 0 )
	{
		switch( errno )
		{
			default:
				iberr = EDVR;
				break;
		}
		status |= ERR;
		ibsta = status;
		return status;
	}

	if(onl == 0)
	{
		if( conf->is_interface )
			ibBoardClose( conf->board );
		if( conf )
		{
			free( ibConfigs[ ud ] );
			ibConfigs[ ud ] = NULL;
		}
	}

	return status;
}


