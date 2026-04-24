# Implementation Details: Locomotion & Kinematics

## 1. Inverse Kinematics (IK) Engine
The system supports two mechanical leg configurations selectable via `Config.h`:
- **Series Topology:** Standard 2-DOF serial linkage calculation.
- **Parallel Topology:** Accounts for the parallel linkage geometry where the shank servo affects the thigh angle through mechanical coupling. The implementation uses a coordinate shift to normalize the input for the core trigonometric solver.
- **Safety Clamping:** All inputs to `acos()` are clamped to `[-1.0, 1.0]` using `fmaxf` and `fminf` to prevent `NaN` results from reaching the servo drivers.

## 2. Attitude Control Strategy
The robot maintains its body orientation through a 3D Rotation Matrix implementation in `AttitudeControl.cpp`.
- **Coordinate Frame:** 
  - +X: Forward
  - +Y: Left
  - +Z: Upward (Vertical)
- **Mirroring:** Rear legs (Leg 3 and 4) use mirrored rotation logic to ensure that a positive Roll command tilts the body correctly while maintaining foot contact points.

## 3. Stabilization Algorithm
The `Stabilizer` class implements a PD-like feedback loop:
- **Inputs:** Current Euler angles (Pitch/Roll) vs. Target angles.
- **Filtering:** Raw IMU data is processed through a moving average filter (Moving Window) to remove high-frequency noise from servo vibration.
- **Axis Decoupling:** Due to physical IMU orientation, the Pitch correction is driven by Roll error and vice versa to align with the robot's primary axes.

## 4. Servo Mapping & Calibration
- **PCA9685 Interface:** Servos are mapped to channels 4 through 11.
- **Calibration (init_case):** A special mode in the web UI allows locking all servos at 90.0 degrees for mechanical alignment and center-point calibration.
