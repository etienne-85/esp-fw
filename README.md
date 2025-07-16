# esp32-remote-control
Keywords: `esp32-remote-control`, `esp32-remote-gpio`, `esp32-remote-first`, `esp32-gpio-over-ws`,
`esp-IO`, `esp-arch`

`esp-fw` is an ESP32 firmware exposing low level feat remotely.

Altough `esp32-fw` firmware can work standalone, it is specifically designed to work
with `esp32-app` companion 

**Strength**

- `HTTPS` server allowing use of `WSS` (secured web-sockets)
- `WSS`  (besides real time communication ) allows use of externaly web hosted web-app `PWA` (progressive web application)
- `PWA` hosted separately allows updating UI and logic without changing device's firmware 

## Concept
**Remote first approach**

Device is primarily controlled by client which can access and control GPIO pins remotely
This allows hardware drivers externalization so firmware focuses purely on low-level generic actions, while client handles projects' specific logic and drivers implementation.

Benefits:
- genericity: reduces need for frequent firmware rebuild
- confort: delegating implementation of project's specific logic and drivers on client side using more friendly scripting languages

**WYSIWYG**
 what you do/see on webapp reflects what happens on device.

## Features
- **GPIO-over-ws** provide real time remote access and control of GPIOs over websockets.
- **Network**  real-time (`WS`) and long range communications (`LORA`)




Extra requirements  : 
- load settings from external configuration file => requires `FS` (filesystem)
- OTA firmware updater


Built-in support for:
- external config loader
- Filesystem
- wifi (both AP and STA mode)
- OTA firmware deployment 
- realtime communications through websockets
- GPIO over websockets: remotely control GPIOs => support new hardware without changing any firmware code

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

### Recovery flash using `esptool-js` browser app
Bootloader bin
- location: `.pio/build/esp32s3/bootloader.bin`
- flash address: `0x0`

Partition bin
- location: `.pio/build/esp32s3/partitions.bin`
- flash address: `0x8000`

Boot bin
- location: `.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin`
- flash address: `0xe000`

Firmware bin
- location: `.pio/build/esp32s3/firmware.bin`
- flash address: `0x10000`

FS image:
- location: `.pio/build/esp32s3/littlefs.bin`
- flash address: `0x290000`

##  Design & architecture
ESP device is considered as mostly used to
- react to events (GPIO, ...)
- talk with external source
### Interfaces
Interfaces can be seen as doors to interact with
- outside world through `GPIO interface` 
- remote clients through `MSG interface`

```
+---------------+
|     ESP       |
|               |
|G             G|
|P --> EVT <-- P|
|I             I|
|O             O|
|               |
|---------------|
| MSG INTERFACE |
|   WS  |  LORA |  <--->  MSG
+---------------+
```

This introduces 2 types of entities: MSG and EVT 
### Messages
ESP device exchange message through `LinkInterfaces` with remote clients supporting
- WS client like browsers
- LORA clients such as other ESP devices

Received message will be dispatched to specific `APIService` to perform action based on input data, and optionaly sends reply to client.

```
                   +---------------+
OUTGOING MSG  <--  |               | <---- OUT -------- (optional)
                   | MSG INTERFACE |                  |
INCOMING MSG  -->  |   WS  |  LORA | ---- IN ----     |
                   |               |            |     |
                   +---------------+            v     |
                                            +-------------+
                                            | API SERVICE |
                                            +-------------+
```
API service examples :
- GPIO interface: called remotely to perform actions on pins
- FS: for filesystem operation
### Events
Events are triggered and handled from inside device through `EventHandler`.
Depending on event types, EventHandlers will catch event.
example: `PIN` event is triggered by `GPIOInterface` when pin state changes. 
It can be catched then by EventHandler to perform an action.


```

+----------------+                +---------+
| GPIO INTERFACE | -- pin evt --> |         |
+----------------+                |         |              +-------------+
                                  |  EVENT  | --- evt -->  | EVT HANDLER |
                                  |  QUEUE  |              +-------------+
+--------------+                  |         |
|     LOGS     |  --- log evt --> |         |
+--------------+                  +---------+
```



DEPRECATED:

By default MsgEvt are caught by MessageInEventHandler which will dispatch to corresponding 
MessageHandler depending on MsgType. 

Any EventTrigger, willing to send message to remote client will trigger OutgoingMsgEvent which will be
caught by MessageOutEvtHandler and sent through corresponding interface WS or LORA
- `IncomingMsgEvt` is triggered by LinkInterfaces when messages is received. MessagesInterfaces wraps message in IncomingMsgEvt object, with additional data like `MsgSrc` (e.g. interface on which it was received: WS or LORA), `ClientKey` (client identifier from which it was received)

### Event forwarding
Event forwarding is used to share events between different devices.
Any remote client or device can subscribe and receive notification from 
another device's events through messages.


```
            DEVICE #1                     |                 DEVICE #2
                                          |
+-----------+                    +-----------------+     
|   GPIO    |___             --> |  MSG INTERFACE  | ----
| INTERFACE |   | (push)    |    +-----------------+     | 
+-----------+   | PinEvt    |             |              | Msg
                v           |             |              v
            +--------+      |             |          +---------+
            | EVENT  |.     |             |          | MSG     |
            | HANDLER|      |             |          | HANDLER |
            +--------+      |             |          +---------+
      (dispatch) |          |  Msg        |               | (dispatch)
            +------------+  |             |          +------------------+
            | EVENT MSG  |__|             |          | MSG NOTIFICATION |--- NotEvt -->
            | NOTIFIER   | (forward)      |          | HANDLER          |   (push)
            +------------+                |          +------------------+
                                          |
                           
```
`MessageEventNotifier` will catch specific events like `LogEvt`, `PinEvt` and notify
client subscribers through `WS`/`LORA` message

When regular event is triggered on local device, it can be caught by EventRemoteForwarder which will wrap EVT into MSG and sent over interface to another device.
On remote device side , message will be handled in `MessageNotificationHandler` which will trigger
 event in local event queue

An example: `LOG` is triggered when log is printed and sent to browser client for display purpose

Another example can be alarm system which is split on 2 devices separated by long distance by which only communication mean is through LORA messages .
When event occurs on remote device, pin state changes triggering an `AlarmPinEvt` which is caught by `AlarmRemoteSystem` event handler and immediately remotely forwarded through LoraMsg.
Once received on other device, notification event will be triggered, caught by 
`AlarmSystem` which react with sound alert.