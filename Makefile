CLI := arduino-cli
FQBN := esp32:esp32:heltec_vision_master_e290
PORT := $(shell ls -1 /dev/cu.usbmodem* | head -n 1)

build:
	${CLI} compile -b ${FQBN} -v

upload: build
	${CLI} upload -b ${FQBN} -p ${PORT} -v

monitor:
	${CLI} monitor -p ${PORT}

.PHONY: build upload monitor
