# ambientika_MQTT2ir_esp32
MQTT-to-IR bridge to controll Südwind Ambientika with an ESP32

## Hardware
This software uses the builtin RMT of the ESP32, configured to GPIO 16. IR LED must be driven over a transistor. For me, multiple hundreds of mA were required to bridge a distance of about 3m.

## Flash
Copy `src/BaseConfig.h.example` to `src/BaseConfig.h` and fill out fields. Flash with PlatformIO

## Control
Supported modes in topic MQTT_TOPIC_BASE/ambientika/mode/set:
- "Manual"
- "In"
- "Out"
- "MaSl"
- "SlMa"
- "Auto"
- "Watch"
- "Night"
- "Boost"

Supported levels in topic MQTT_TOPIC_BASE/ambientika/level/set:
- "1"
- "2"
- "3"

Level is automatically interpreted as fan speed or humidity, depending on mode. All changes are are send with both mode and level to ensure consistency. Boost is not reported back as a state.
