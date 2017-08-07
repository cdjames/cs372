#include "encrypt.h"

/*********************************************************************
** Description: 
** Return a modulus
w/ help: http://stackoverflow.com/questions/4003232/how-to-code-a-modulo-operator-in-c-c-obj-c-that-handles-negative-numbers#4003287
*********************************************************************/

int mod (int x, int y)
{
   int themod = x % y;
   if(themod < 0)
     themod += y;
   return themod;
}

void encrypt(char * instring, char * keystring, char * outstring){
	const int ASCII_DIF = 65;
	const int SPACE_VAL = 32;
	const int TOP_VAL = 27;
	/* vars for plaintext char, key char, intermediate char, encrypted char */
	int pc, kc, ic, ec, 
		i = 0;

	/* until end of string */
	while (instring[i] != '\0') {
		/* convert characters A-Z \s into 0-26 for in and key */
		pc = instring[i] - ASCII_DIF;
		if(pc < 0) // must be space since it is 32 - 65
			pc = TOP_VAL-1;
		// printf("pc = %d\n", pc);
		kc = keystring[i] - ASCII_DIF;
		if(kc < 0)
			kc = TOP_VAL-1;
		/* get an intermediate value for clarity */
		ic = pc + kc;
		/* your encrypted int will be modulo of ic if less than top*/
		ec = (ic >= TOP_VAL) ? ic-TOP_VAL : mod(ic, TOP_VAL);
		if(ec == TOP_VAL-1)
			ec = SPACE_VAL - ASCII_DIF;

		outstring[i] = ec + ASCII_DIF;
		i += 1;
	}
}

// int main(int argc, char const *argv[])
// {
// 	/* code */
// 	char msg[5];
// 	char key[5];
// 	char out[5];

// 	memset(msg, '\0', sizeof(msg));
// 	memset(key, '\0', sizeof(key));
// 	memset(out, '\0', sizeof(out));

// 	int n = snprintf(msg, 5, "%s", "ZELA");

// 	n = hasValidChars(msg);

// 	printf("hasValidChars = %d\n", n);	
// 	// n = snprintf(key, 5, "%s", "ZAKZ");

// 	// encrypt(msg, key, out);

// 	// printf("out = %s\n", out);

// 	return 0;
// }