/*********************************************************************
** Author: Collin James
** Date: 11/9/16
** Description: Custom types used in smallsh; implementation (see header)
** for more detail
*********************************************************************/

#include "newtypes.h"

#ifndef __ST
#define __ST
#define ST struct Statuskeeper
#define ST_SIZE sizeof(struct Statuskeeper)
#endif

struct Pidkeeper new_PK(pid_t pid, int status)
{
	struct Pidkeeper pk;
	pk.pid = pid;
	pk.status = status;
	return pk;
}

struct Statuskeeper * new_SK(us_int type, int sk_sig){
	struct Statuskeeper * st = (ST *) malloc(ST_SIZE);
	st->type = type;
	st->sk_sig = sk_sig;

	return st;
}

struct Commandkeeper new_CK(char * cmd, struct argArray * args, int num_args){
	struct Commandkeeper ck;
	ck.bg = 0;
	ck.bltin = 0;
	ck.red_in = 0;
	ck.red_out = 0;
	ck.red_error = 0;
	ck.no_cmd=0;
	ck.num_args = num_args;
	ck.cmd = cmd;
	ck.args = args;
	ck.infile = NULL;
	ck.outfile = NULL;
	return ck;
}

void free_sk(struct Statuskeeper * sk){
	free(sk);
}