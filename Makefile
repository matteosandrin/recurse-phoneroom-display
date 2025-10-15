SRC_DIR := ./main
CLI := arduino-cli
FQBN := esp32:esp32:heltec_vision_master_e290
PORT := $(shell ls -1 /dev/cu.usbmodem* | head -n 1)

upload: build
	cd ${SRC_DIR} && ${CLI} upload -b ${FQBN} -p ${PORT} -v

build: icons
	cd ${SRC_DIR} && ${CLI} compile -b ${FQBN} -v

icons:
	cd ${SRC_DIR}/icons && mogrify -format xbm *.png
	cd ${SRC_DIR}/icons && cat *.xbm | sed s/static/const/ | sed s/=/PROGMEM=/ | sed s/char/uint8_t/ > icons.h
	cd ${SRC_DIR}/icons && rm *.xbm

monitor:
	cd ${SRC_DIR} && ${CLI} monitor -p ${PORT}

.PHONY: build upload monitor
