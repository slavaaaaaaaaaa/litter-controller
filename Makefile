test:
	SOURCE=playground.c $(MAKE) compile
	sudo ./a.out

compile:
	gcc $(SOURCE) -lwiringPi -lwiringPiDev
