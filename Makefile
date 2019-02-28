BUILD_DIR=.build
OUT=$(BUILD_DIR)/litter-controller
DEPS=-lwiringPi -lwiringPiDev -lpthread -lcrypt -lm -lrt -largparse -Largparse/
WARNINGS=-Wall
FLAGS=$(DEPS) $(WARNINGS)
COMPILER=gcc
PREFIX?=/usr/local
INSTALL_DIR?=$(PREFIX)/bin

.PHONY: test install uninstall clean

$(OUT): $(wildcard ./*.c)
	mkdir -p $(BUILD_DIR)
	set -ex && $(COMPILER) $< $(FLAGS) -o $@

test: $(OUT)
	$(MAKE) -Cargparse/
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$$(pwd)/argparse/ ./$(OUT) -d1

install_argparse:
	install argparse/libargparse.so $(PREFIX)/lib/
	install argparse/argparse.h $(PREFIX)/include/

install: $(OUT)
	install -d $(INSTALL_DIR)
	install $(OUT) $(INSTALL_DIR)

uninstall:
	$(RM) $(INSTALL_DIR)/$(OUT)

clean:
	$(RM) -r $(BUILD_DIR)
