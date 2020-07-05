(CC) = gcc

comport: comport.c ComportOpen.o ComportInit.o ComportSwap.o ComportSwap.o
	$(CC) comport.c ComportOpen.o ComportInit.o ComportSwap.o -o comport -Werror -Wall

ComportOpen.o: ComportOpen.c
	$(CC) -c ComportOpen.c

ComportInit.o: ComportInit.c
	$(CC) -c ComportInit.c

ComportSwap.o: ComportSwap.h
	$(CC) -c ComportSwap.c


clear:
	rm *.o comport
