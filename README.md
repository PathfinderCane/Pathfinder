# Pathfinder

Pathfinder is a smart assistive navigation system designed for visually impaired users. It combines LiDAR sensors, an ESP32 microcontroller, voice guidance, and GSM-based emergency support to safely guide users, detect obstacles, and provide real-time location updates. The system is modular, lightweight, and optimized for real-world usability.

---

## Features

- **Obstacle Detection:** 5 VL53L0X LiDAR sensors detect obstacles in real-time.
- **Voice Guidance:** DFPlayer Mini provides audio feedback to alert the user.
- **Emergency Support:** EC200U GSM module allows sending emergency calls/messages.
- **Smart Sunglasses Integration:** Outward-facing sensors provide additional environmental awareness.
- **Caster Wheel Design:** Smooth navigation without traditional tapping tips.
- **Wireless Communication:** ESP32 modules communicate via ESP-NOW for seamless data transfer.
- **Optimized Angles:** Sensors and cane handle designed for maximal obstacle detection efficiency.

---

## Hardware Components

- ESP32 microcontroller (cane and sunglasses)
- 5x VL53L0X LiDAR sensors
- DFPlayer Mini audio module
- EC200U GSM module
- Custom sunglasses and cane handle
- Caster wheel for mobility

---

## Usage

1. Assemble the hardware as per the provided schematics.
2. Upload the firmware to the respective ESP32 modules.
3. Power on the system. Voice guidance will provide feedback on detected obstacles.
4. The cane communicates wirelessly with the sunglasses to provide enhanced environmental awareness.

---

## Restrictions

This code and project are released for **personal, educational, or research use only**.  

- **Prohibited:**
  - Commercial use or sale of hardware/software.
  - Redistribution of this project for profit.
  - Use in products or services intended for mass sale or profit.

- **Allowed:**
  - Learning and experimentation.
  - Personal assistive projects.
  - Educational demonstrations and research purposes.

By using this project, you agree to respect these restrictions.

---

## License

This project is licensed under the **GNU General Public License**. See `LICENSE` file for details.

---

## Contribution

Contributions are welcome for **bug fixes, documentation improvements, or educational enhancements**, but must **respect the non-commercial nature**

