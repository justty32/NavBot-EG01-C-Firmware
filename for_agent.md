# Technical Manifest for AI Agents (System Maintenance)

This document defines the strict operational boundaries for any AI Agent tasked with modifying or extending this firmware.

## 1. Architectural Mandates
- **Core Affinity:** The `ControlLoopTask` MUST remain pinned to Core 1. It must never contain blocking calls (`delay`, `while(1)`) that lack an escape or task yield.
- **I2C Separation:** 
  - Bus 1 (Pins 19/23): Dedicated to high-speed PCA9685 output.
  - Bus 2 (Pins 32/33): Dedicated to MPU6050 IMU input.
  Do not merge these buses without validating the timing impact on the 100Hz loop.

## 2. Locomotion Logic
- **Coordinate System:** Legs are numbered 1 (FL), 2 (FR), 3 (RL), 4 (RR).
- **Leg 3/4 Mirroring:** In `AttitudeControl`, Leg 3 uses AB4 logic and Leg 4 uses AB3 logic. This is an intentional physical mapping to accommodate the robot's rear-end symmetry. DO NOT CHANGE this mapping without verifying mechanical layout.

## 3. Web Service
- **PROGMEM Assets:** All HTML content is stored in `WebResources.h`. To update the UI, modify the raw string literals in this header. Do not implement file-system based serving (LittleFS) as it increases task latency.

## 4. Safety Guardrails
- **IK Clamping:** Every calculation in `Kinematics::calculateIK` must have boundary protection. Quadruped movements frequently reach the limits of leg extension; `NaN` propagation will cause the ESP32 to trigger a crash or produce erratic servo behavior.

## Hardware Reference
- **SDA1/SCL1:** 19 / 23 (Servos)
- **SDA2/SCL2:** 33 / 32 (IMU)
- **PCA9685 Address:** 0x40
- **Baud Rate:** 115200 (Serial), 921600 (Upload)
