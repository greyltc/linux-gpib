#include <autoconf.h>

#include "ib.h"
#include <gpib_types.h>
#include <gpib_registers.h>
#include <gpib_ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "ibConf.h"

#define NOADDR -1
#define IbcAUTOPOLL 0

extern int ibCheckDescriptor(int ud);
extern  int ibBdChrConfig(int ud);
extern  void ibBoardDefaultValues(void);
extern  int ibBoardOpen(int bd,int flags);
extern  int ibBoardClose(int bd);
extern  int ibGetNrBoards(void);
extern  void yyerror(char *s);
extern  int ibeos(int ud, int v);
extern  int iblcleos( const ibConf_t *conf );
extern  char *ibVerbCode(int code);
extern  void ibPutMsg (char *format,...);
extern  void ibPutErrlog(int ud,char *routine);
extern  int ibParseConfigFile(char *filename);
extern  int ibGetDescriptor(ibConf_t conf);
extern  int ibFindDevIndex(char *name);
extern ssize_t my_ibcmd( const ibBoard_t *board, const ibConf_t *conf, uint8_t *buffer, size_t length);
extern int config_parsed;
extern int send_setup( const ibBoard_t *board, const ibConf_t *conf );
extern void init_ibconf( ibConf_t *conf );
extern int my_ibdev( int minor, int pad, int sad, unsigned int usec_timeout,
	int send_eoi, int eos, int eosflags);
extern unsigned int timeout_to_usec( enum gpib_timeout timeout );
extern int set_timeout( const ibBoard_t *board, unsigned int usec_timeout );
extern int ib_lock_mutex( ibBoard_t *board );
extern int ib_unlock_mutex( ibBoard_t *board );
extern int close_gpib_device( ibBoard_t *board, ibConf_t *conf );
extern int open_gpib_device( ibBoard_t *board, ibConf_t *conf );
extern int gpibi_change_address( ibBoard_t *board, ibConf_t *conf,
	unsigned int pad, int sad );
extern int lock_board_mutex( ibBoard_t *board );
extern int unlock_board_mutex( ibBoard_t *board );

#include <stdio.h>
int gpib_yyparse(void);
void gpib_yyrestart(FILE*);
int gpib_yylex(void);

