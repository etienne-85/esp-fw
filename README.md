# esp32-remote-control
`esp32-remote-control`, `esp32-controls-ota`, `esp32-gpio-ota`, `esp32-ws-controls`, `esp32-remote-gpio`, `esp32-full-remote`, `esp32-ws-rc-gpio`, `esp32-universal-controls`, `esp32-gpio-over-ws`
Gpio over websockets project is an ESP32 firmware aiming to access and control GPIO pins remotely through websockets.
It aims to be as generic as possible by externalizing hardware drivers implementation and significantly reduce device's firmware updates.


## Design and motivations
It is designed to:
- heavily relies upon websockets to provide instant communication with device
- to allow web app hosted separately to communicate with device by providing web secured connection
- modular architecture making easy implementation of new services 


What? 
allow controlling pins remotely

Why?
- universal firmware reusable across projects: avoid building specific firmare version each time new hardware support is required 
- move all project's logic to high level language like javascript

How?
 Drivers implemented outside device's firmware

Requirement:
- GPIO remote control capabilities
    - requires websockets
    - requires webservice
    - requires network connection

Extra requirements  : 
- network settings load from external json configuration
    - requires Filesystem access
- OTA firmware updater

## Limitations

- if you need device to work offline or perform independant tasks
- 


Built-in support for:
- external config loader
- Filesystem
- wifi (both AP and STA mode)
- OTA firmware deployment 
- web-app hosting through static web server 
- realtime communications through websockets
- GPIO over websockets: remotely control GPIOs => support new hardware without changing any firmware code

Designed for
- minimizing firmware coding.
- up and running ESP32 dev workspace out of the box
- modular architecture to easily extend firmware's core features 

## Usage

`.pio.env` script provides access to following commands:
- `pio-install`: install platform io (should only be run once)
- `pio-build-firmware`: build firmware 
- `pio-build-fs`: build image to embedd config and static web content into esp32's filesystem
- `pio-upload-firmware`
- `pio-upload-fs`

## Setup

- download workspace artifact and extract it
- after sourcing `.pio.env` install `plaftormIO` devenv with `pio-install` command
- init workspace by running `pio-init-project` at project's root to create required symlinks 
- connect your esp32 board to USB serial port in download mode
- build and upload default firmware with `pio-upload-firmware`
- customize `data/config.json` with your network credentials as follow:
```
{
    wifiSSID: "changeme",
    wifiPWD: "changeme"
}
```
- build and upload FS image with `pio-upload-fs`
- watch boot logs with `screen /dev/ttyUSB0` or any other way, and restart esp32 board in normal mode
(providing `/dev/ttyUSB0` is the serial USB device)
- board's IP address on local network should be shown in serial console if connection was successfull

Check that:
- OTA service is available at board's local adress followed by `/update`
- default welcome page is reachable at `/index` url

If all preceding worked, firmware was successfully uploaded to the board, the config was loaded correctly, the board managed to connect to local network and its services started correctly.

Note that serial flashing won't be required anymore to update the board as `OTA flash` can be used instead.

## Advanced tips

### Alternative serial flashing

Particularly convenient if firmware is developed on remote server which isn't connected to esp32 device directly.

In that case built files can be transferred from dev server to a local machine having access to esp32 and uploaded using chrome browser serial capabilities: 

Firmware bin
- location: `.pio/build/esp32-latest/firmware.bin`
- flash address: `0x10000`

FS image:
- location: `.pio/build/esp32-latest/`
- flash address: `0x`

### Architecture

Firmware inner feats:
- `JSON` support
- `LittleFS` as internal FS

The firmware architecture is made of different modules and services.
A module inherits the `FirmwareModule` class and can overrides `setup`, `loop` methods to provide its own implementation.
In a similar way, a service provides a custom implementation of another service such as `WebSocketService`.

On top of core modules, firmware can be easily extended through custom modules to provide additional features (such as specific sensor support, ...).
To get included in the main update loop, a custom module inherits the `FirmwareModule` class.
As C++ allows multiple inheritance, it can also inherit a service to make use of it.

For instance a module requiring acceess to websockets, will inherit both `FirmwareModule` and `WebSocketService` which will forward received message to process them.