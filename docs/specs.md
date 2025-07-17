# High level requirements
## Overview
`client` <-- WS --> `gateway device` <-- LORA --> `remote device` 

## Features
- real time communication over ws
- long range communications over lora
- full device control over ws interface 

## Usecases
- update device firmware remotely
- monitor logs from web interface
- send/receive lora messages from web interface 
- direct control of local device

## Applications
*** Alarm system ***
- remote alarm system to be notified of any intrusions
- `remote device` connected to PIR sensor to detect motion
- `gateway device` connected to `client` to interact with `remote device` through `WS` and `LORA` messages
- `client` can access and monitor `remote device` through `gateway device`
- `client`can be notified of alert triggered from `remote device`

*** Device remote control ***
- ROV remote control from mobile phone
- real time remote control of device from client web interface
- trigger actions from web interface and see device instant feedback

# Technical specifications

## Client web interface
- monitor GPIO pins: see current state
- control GPIO pins: can trigger pin action
- watch logs
- send LORA message


# Proposal
## Data model
### Paquet
### Event

## Link communication interfaces
### Common
- serialize and send data over `packets`
- receive and parse data from `packets`
- ACK

### Websockets
- handle multiple remote clients
- push notification, broadcast

### Lora
- `sender`, `receiver` indentification
- incoming messages filtering
- message delivery confirmation: `ACK`
- message integrity checksum

## Events system
- supported events: `PIN`, `LOG`, `MSG`

## API modules
performing logic, handling specific events

For instance:
***`LOG` event*** 
- write logs to fs
- send log over ws to remote client 

***`PIN` event***
- pin trigger signal -> send message 
- pin control action 
- pin watch/monitor

