#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef ENCRYPT_H
#define ENCRYPT_H


/*********************************************************************
** Description: 
** Return a modulus
w/ help: http://stackoverflow.com/questions/4003232/how-to-code-a-modulo-operator-in-c-c-obj-c-that-handles-negative-numbers#4003287
*********************************************************************/
int mod (int x, int y);

void encrypt(char * instring, char * keystring, char * outstring);

#endif