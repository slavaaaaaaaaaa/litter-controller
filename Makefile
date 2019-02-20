litter:
	SOURCE=litter-controller.c $(MAKE) compile
	sudo ./a.out

compile:
	gcc $(SOURCE) -lwiringPi -lwiringPiDev -lpthread -lcrypt -lm -lrt -Wall
