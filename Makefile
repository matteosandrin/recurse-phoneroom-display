FQBN = esp32:esp32:heltec_vision_master_e290
PORT = /dev/cu.usbmodem101

build:
	arduino-cli compile -b ${FQBN} -v

upload: build
	arduino-cli upload -b ${FQBN} -p ${PORT} -v

monitor:
	arduino-cli monitor -p ${PORT}

.PHONY: build upload
