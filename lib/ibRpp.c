
#include "ib_internal.h"
#include <ibP.h>

int internal_ibrpp( ibConf_t *conf, char *result )
{
	uint8_t poll_byte;
	ibBoard_t *board;
	int retval;
	
	board = interfaceBoard( conf );

	if( board->is_system_controller == 0 )
	{
		setIberr( ENEB );
		return -1;
	}

	set_timeout( board, conf->ppoll_usec_timeout );

	retval = ioctl( board->fileno, IBRPP, &poll_byte );
	if( retval < 0 )
	{
		switch( errno )
		{
			case ETIMEDOUT:
				conf->timed_out = 1;
				break;
			default:
				setIberr( EDVR );
				setIbcnt( errno );
				break;
		}
		return -1;
	}

	*result = poll_byte;

	return 0;
}

int ibrpp( int ud, char *ppr )
{
	ibConf_t *conf;
	int retval;

	conf = enter_library( ud );
	if( conf == NULL )
		return exit_library( ud, 1 );

	retval = internal_ibrpp( conf, ppr );
	if( retval < 0 )
	{
		return exit_library( ud, 1 );
	}

	return exit_library( ud, 0 );
}

void PPoll( int boardID, short *result )
{
	char byte_result;
	ibConf_t *conf;
	int retval;

	conf = enter_library( boardID );
	if( conf == NULL )
	{
		exit_library( boardID, 1 );
		return;
	}

	if( conf->is_interface == 0 )
	{
		setIberr( EDVR );
		exit_library( boardID, 1 );
		return;
	}

	retval = internal_ibrpp( conf, &byte_result );
	if( retval < 0 )
	{
		exit_library( boardID, 1 );
		return;
	}

	*result = byte_result & 0xff;

	exit_library( boardID, 0 );
}

