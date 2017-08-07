all: otp_enc_d otp_enc otp_dec_d otp_dec keygen

otp_enc_d: utils.o otp_enc_d.o 
	gcc -g -Wall -o otp_enc_d  utils.o otp_enc_d.o -lm

otp_enc: otp_enc.c utils.o
	gcc -g -Wall -o otp_enc otp_enc.c utils.o -lm

otp_enc_d.o: otp_enc_d.c
	gcc -g -Wall -c otp_enc_d.c

otp_dec_d: utils.o otp_dec_d.o 
	gcc -g -Wall -o otp_dec_d  utils.o otp_dec_d.o -lm

otp_dec: otp_dec.c utils.o
	gcc -g -Wall -o otp_dec otp_dec.c utils.o -lm

otp_dec_d.o: otp_dec_d.c
	gcc -g -Wall -c otp_dec_d.c

keygen: keygen.c utils.o
	gcc -g -Wall -o keygen keygen.c utils.o -lm

utils.o: utils.c utils.h 
	gcc -g -Wall -c utils.c 

# encrypt.o: encrypt.c encrypt.h
# 	gcc -g -Wall -c encrypt.c

# newtypes.o: newtypes.c newtypes.h 
# 	gcc -g -Wall -c newtypes.c 

clean:
	rm *.o

cleanall:
	rm keygen otp_enc otp_enc_d otp_dec_d otp_dec
	make clean