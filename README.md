# DevAccess

ESP32-based embedded device access system using RFID authentication, PIR presence detection, ultrasonic proximity sensing, and an OLED status display. The system enables authorized device activation, tracks active sessions, and enforces automatic shutdown when access conditions are no longer met.

## Key Features
- RFID-based authorization for access control
- PIR motion confirmation to reduce accidental activations
- Ultrasonic distance gate for proximity verification
- OLED status display for user feedback and session state
- Session tracking with idle timeout and auto shutdown
- Safe default: device remains off until all conditions are valid

## System Overview
1. User presents an authorized RFID card.
2. PIR confirms presence.
3. Ultrasonic distance verifies proximity within threshold.
4. Device is activated and a session begins.
5. Session ends on timeout, exit conditions, or explicit deauth.

## Hardware
- ESP32 development board
- RFID reader (MFRC522)
- PIR motion sensor
- Ultrasonic sensor (HC-SR04 or equivalent)
- OLED display (I2C, SSD1306)
- Device activation output (relay or MOSFET driver)
- 3.3V/5V power as required by modules

## Wiring Summary (Default Pin Map)
| Module | Signal | ESP32 GPIO |
|---|---|---|
| RFID (MFRC522, SPI) | SCK | 18 |
|  | MISO | 19 |
|  | MOSI | 23 |
|  | SS/SDA | 5 |
|  | RST | 27 |
| OLED (I2C) | SDA | 21 |
|  | SCL | 22 |
| PIR | OUT | 13 |
| Ultrasonic | TRIG | 25 |
|  | ECHO | 26* |
| Device Control | RELAY/MOSFET | 32 |
| Status LED (optional) | LED | 2 |

\* Use a level shifter or divider on ECHO if the sensor outputs 5V.

## Firmware
The final firmware is in the single file below:
- [firmware/DevAccess.ino](firmware/DevAccess.ino)

Library dependencies (Arduino IDE):
- MFRC522
- Adafruit SSD1306
- Adafruit GFX

## Debug and Run
1. Open [firmware/DevAccess.ino](firmware/DevAccess.ino) in Arduino IDE.
2. Install the libraries listed above.
3. Select your ESP32 board and the correct port.
4. Upload to the device.
5. Open Serial Monitor at 115200 baud for live status.

## Images (Placeholders)
The images in [images](images) are placeholders and should be replaced with real photos/serial captures:
- serial_output.png (serial session output)
- setup_photo.png (device setup photo)

## Documentation
- Concept and state flow: [docs/concept.md](docs/concept.md)
- Pin mapping details: [docs/pinout.md](docs/pinout.md)

## Repo Structure
- firmware/ (final .ino only)
- wiring/ (breadboard diagram)
- docs/ (concept + pinout)
- images/ (serial output + setup photo)

## Notes
- Adjust GPIOs to fit your board layout.
- Ensure the relay/MOSFET is rated for your load.
- Use proper level shifting for 5V sensors.
