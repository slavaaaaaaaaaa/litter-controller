lcd:
	SOURCE=LCD_1602_IIC.c $(MAKE) compile
	sudo ./a.out

blink:
	SOURCE=blink.c $(MAKE) compile
	sudo ./a.out

button:
	SOURCE=button.c $(MAKE) compile
	sudo ./a.out

ultrasonic:
	SOURCE=HCsr04.c $(MAKE) compile
	sudo ./a.out

compile:
	gcc $(SOURCE) -lwiringPi -lwiringPiDev
