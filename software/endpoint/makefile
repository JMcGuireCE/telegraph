build:
	arduino-cli compile --fqbn esp32:esp32:esp32

run:
	arduino-cli compile --fqbn esp32:esp32:esp32:CDCOnBoot=cdc
	arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32:CDCOnBoot=cdc

upload:
	arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32

watch:
	arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=115200


builds3:
	arduino-cli compile --fqbn esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc

runs3:
	arduino-cli compile --fqbn esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc
	arduino-cli upload -p /dev/ttyACM0 --fqbn esp32:esp32:esp32s3:CDCOnBoot=cdc

uploads3:
	arduino-cli upload -p /dev/ttyACM0 --fqbn esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc

watchs3:
	arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200

