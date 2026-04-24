# Project Documentation: NavBot-EG01-C-Firmware

## Technical Objective
This project implements a high-performance control system for the NavBot EG01 quadruped robot using C++/Arduino on the ESP32 platform. The primary goal is to provide a deterministically timed control environment suitable for complex legged locomotion and active stabilization.

## Performance Requirements
- **Control Loop Period:** 10ms (100Hz) deterministic execution.
- **Jitter Tolerance:** < 500µs (achieved via FreeRTOS task prioritization).
- **Communication Latency:** < 50ms via Asynchronous HTTP server.
- **IK Computation Time:** < 1ms per leg.

## Key Subsystems
- **Locomotion Engine:** Implements cycloid-based Trot trajectories and state-machine based Walking gaits.
- **Inverse Kinematics (IK):** Supports both Series (Standard) and Parallel (Mechanical Linkage) leg topologies with coordinate space transformations.
- **Attitude & Heading Reference System (AHRS):** 6-axis data fusion via Mahony Filter to estimate robot orientation (Pitch/Roll).
- **Active Stabilizer:** Closed-loop feedback system using IMU data to compensate for terrain unevenness and center-of-mass shifts.

## Deployment Strategy
The firmware is designed as a standalone binary. All web assets (HTML/JS/CSS) are embedded as `PROGMEM` constants, eliminating dependencies on external filesystem partitions (SPIFFS/LittleFS) and ensuring robust startup behavior.
