BUILD_DIR=.build
OUT=$(BUILD_DIR)/litter-controller
DEPS=-lwiringPi -lwiringPiDev -lpthread -lcrypt -lm -lrt -largparse -Largparse/
WARNINGS=-Wall
FLAGS=$(DEPS) $(WARNINGS)
COMPILER=gcc
PREFIX?=/usr/local
INSTALL_DIR?=$(PREFIX)/bin
SYSTEMD_UNIT_DESTINATION?=/etc/systemd/system/

.PHONY: test install uninstall clean

$(OUT): $(wildcard ./*.c)
	mkdir -p $(BUILD_DIR)
	set -ex && $(COMPILER) $< $(FLAGS) -o $@

deploy: $(out) install service

test: $(OUT)
	./$(OUT) -d9

deps:
	git submodule update --init
	cd wiringPi; ./build; cd ../
	$(MAKE) -Cargparse/
	install argparse/libargparse.so $(PREFIX)/lib/
	install argparse/argparse.h $(PREFIX)/include/
	ldconfig

install: $(OUT)
	install -d $(INSTALL_DIR)
	install $(OUT) $(INSTALL_DIR)

service:
	install litter-controller.service $(SYSTEMD_UNIT_DESTINATION)
	systemctl enable litter-controller.service
	systemctl restart litter-controller.service

rpi-config:
	install config.txt /boot/
	echo i2c-dev >> /etc/modules-load.d/modules.conf
	usermod -a -G i2c pi
	modprobe i2c-dev

uninstall:
	$(RM) $(INSTALL_DIR)/$(OUT)

clean:
	$(RM) -r $(BUILD_DIR)

GITROOT=$(shell git rev-parse --show-toplevel)
include $(shell test -d $(GITROOT)/include.mk/ || git clone git@github.com:smaslennikov/include.mk.git && echo $(GITROOT))/include.mk/*.mk
