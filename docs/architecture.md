# System Architecture and Usecases

This document provides architectural diagrams and illustrates common usecases for the `esp32-toolkit-lib`.

## 1. Usecases

### 1.1. Use Case 1: Alarm Signal (Remote Device -> Local Device)

This diagram shows the sequence of events when the remote device's PIR sensor detects motion and sends an alarm to the local device, which then forwards it to a connected computer.

```mermaid
sequenceDiagram
    participant Computer
    participant LocalDevice as Local Device
    participant RemoteDevice as Remote Device
    participant PIRSensor as PIR Sensor

    PIRSensor->>+RemoteDevice: Motion Detected
    RemoteDevice->>+LocalDevice: Send Alarm Signal (LoRa)
    LocalDevice->>-RemoteDevice: Acknowledge Signal (LoRa)
    LocalDevice->>+Computer: Forward Alarm (WebSocket)
    Computer-->>-LocalDevice: (Display Alarm)
```

### 1.2. Use Case 2: Test/Ping Signal (Local Device -> Remote Device)

This diagram illustrates the user-initiated test signal from the local device to confirm the remote device is operational.

```mermaid
sequenceDiagram
    participant User
    participant LocalDevice as Local Device
    participant RemoteDevice as Remote Device

    User->>+LocalDevice: Press Test Button
    LocalDevice->>+RemoteDevice: Send Ping Signal (LoRa)
    RemoteDevice-->>-LocalDevice: Acknowledge Ping (LoRa)
    LocalDevice->>User: Sound Buzzer (Confirmation)
```

---

## 2. Detailed Interaction Diagrams

### 2.1. Detailed Alarm Signal Flow

This diagram provides a more detailed view of the internal components and events involved in the alarm signal use case.

```mermaid
sequenceDiagram
    participant Computer
    box Local Device
        participant WsInterface_L as WsInterface
        participant TestModule as TestModule
        participant EventQueue_L as EventQueue
        participant LoraInterface_L as LoraInterface
    end
    box Remote Device
        participant LoraInterface_R as LoraInterface
        participant AlarmSystem as AlarmSystem
        participant EventQueue_R as EventQueue
        participant TriggerPin as TriggerPin
    end
    participant PIRSensor as PIR Sensor

    PIRSensor->>+TriggerPin: Motion triggers hardware interrupt
    TriggerPin->>+EventQueue_R: Push "PIN_TRIGGER" Event
    EventQueue_R-->>-AlarmSystem: Dispatch Event
    AlarmSystem->>+LoraInterface_R: Send Alarm Packet (API: "Alarm", Cmd: "onAlert")
    LoraInterface_R->>+LoraInterface_L: Transmit LoRa Signal
    LoraInterface_L->>+TestModule: Dispatch Packet
    Note right of TestModule: 1. Acknowledge Remote
    TestModule->>+LoraInterface_L: Send ACK Packet
    LoraInterface_L-->>-LoraInterface_R: Transmit ACK
    Note right of TestModule: 2. Forward to Client
    TestModule->>+WsInterface_L: Forward Alarm Packet
    WsInterface_L-->>-Computer: Send WebSocket Message
```

### 2.2. Detailed Test/Ping Signal Flow

This diagram details the internal interactions for the user-initiated test signal.

```mermaid
sequenceDiagram
    participant User
    participant PushButton as Push Button
    box Local Device
        participant TriggerPin_L as TriggerPin
        participant EventQueue_L as EventQueue
        participant TestModule as TestModule
        participant SndNotif_L as SndNotif
        participant LoraInterface_L as LoraInterface
    end
    box Remote Device
        participant LoraInterface_R as LoraInterface
        participant AlarmSystem as AlarmSystem
    end

    User->>+PushButton: Press Button
    PushButton->>+TriggerPin_L: Hardware interrupt
    TriggerPin_L->>+EventQueue_L: Push "PIN_TRIGGER" Event
    EventQueue_L-->>-TestModule: Dispatch Event
    TestModule->>+LoraInterface_L: Send Ping Packet (API: "Alarm", Cmd: "ping", ack: true)
    LoraInterface_L->>+LoraInterface_R: Transmit LoRa Signal
    LoraInterface_R->>+AlarmSystem: Dispatch Packet
    AlarmSystem-->>LoraInterface_R: (Processing ping...)
    LoraInterface_R-->>-LoraInterface_L: Transmit ACK (Auto-reply from LinkInterface)
    LoraInterface_L->>+TestModule: Dispatch ACK Packet
    TestModule->>+SndNotif_L: Trigger Confirmation Sound
    SndNotif_L-->>-User: Sound Buzzer
```

---

## 3. Core System Interactions

These diagrams illustrate the fundamental interactions between core components of the `esp32-toolkit-lib`.

### 3.1. API Module Interaction with LinkInterface

*** TODO ***

### 3.2. Event System

This diagram illustrates the event handling mechanism, where events from sources like `GPIO` or `LOGS` are queued and then processed by registered event handlers.

```mermaid
graph TD
    subgraph "Event Sources"
        GpioInterface[GPIO Interface]
        LogsModule[LOGS Module]
    end

    subgraph "Event Handling"
        EventQueue[Event Queue]
        EventHandler[Registered EventHandler]
    end

    GpioInterface -- "PIN Event" --> EventQueue
    LogsModule -- "LOG Event" --> EventQueue
    EventQueue -- "Dispatched Event" --> EventHandler
