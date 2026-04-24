# System Architecture Analysis

The NavBot EG01 firmware utilizes a multi-threaded architecture leveraging the ESP32's dual-core Xtensa processor.

## Task Allocation
### Core 1: Real-Time Control (High Priority)
- **`ControlLoopTask`**: This is the heart of the robot. It runs as a high-priority FreeRTOS task with a strict 10ms period.
- **Execution Pipeline:** 
  1. **Sensor Acquisition:** Fetch IMU raw data via I2C (Bus 2).
  2. **Orientation Estimation:** Update Euler angles (Pitch/Roll).
  3. **Control Logic:** Compute gait phase, apply attitude compensation, and calculate IK.
  4. **Actuator Output:** Update PCA9685 PWM channels via I2C (Bus 1).

### Core 0: Asynchronous Communication (Low Priority)
- **`ESPAsyncWebServer`**: Handles incoming HTTP requests for remote control and calibration.
- **Mechanism**: The web server updates global state variables (`spd`, `L`, `R`, `H_goal`). The `ControlLoopTask` reads these variables at the start of every tick, ensuring a decoupled but responsive interface.

## System Block Diagram
```text
[ WiFi AP / Web UI ] <--- (Core 0: Asynchrous) ---> [ Global State ]
                                                        |
                                                        V
[ MPU6050 IMU ] <--- (I2C Bus 2) --- [ Control Loop (Core 1: 100Hz) ] --- (I2C Bus 1) ---> [ PCA9685 Servos ]
```

## Concurrency and Safety
- **Non-blocking I/O**: The use of `ESPAsyncWebServer` prevents network latency from stalling the locomotion engine.
- **Memory Safety**: Static memory allocation is prioritized over dynamic allocation (Heap) to prevent fragmentation during long-term operation.
