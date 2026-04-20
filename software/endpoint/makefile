build:
	arduino-cli compile --fqbn esp32:esp32:esp32

run:
	arduino-cli compile --fqbn esp32:esp32:esp32:CDCOnBoot=cdc
	arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32:CDCOnBoot=cdc

upload:
	arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32

watch:
	arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=115200
