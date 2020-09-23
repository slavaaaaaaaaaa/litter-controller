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
	$(MAKE) -Cargparse/
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$$(pwd)/argparse/ ./$(OUT) -d1

install_argparse:
	install argparse/libargparse.so $(PREFIX)/lib/
	install argparse/argparse.h $(PREFIX)/include/

install: $(OUT)
	install -d $(INSTALL_DIR)
	install $(OUT) $(INSTALL_DIR)

service:
	install litter-controller.service $(SYSTEMD_UNIT_DESTINATION)
	systemctl enable litter-controller.service
	systemctl restart litter-controller.service

rpi-config:
	install config.txt /boot/

uninstall:
	$(RM) $(INSTALL_DIR)/$(OUT)

clean:
	$(RM) -r $(BUILD_DIR)

GITROOT=$(shell git rev-parse --show-toplevel)
include $(shell test -d $(GITROOT)/include.mk/ || git clone git@github.com:smaslennikov/include.mk.git && echo $(GITROOT))/include.mk/*.mk
