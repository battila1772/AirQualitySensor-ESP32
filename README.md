# AirQualitySensor-ESP32

ESP32-based air quality monitor using an ENS160 air quality sensor, an AHT21/AHT20 temperature and humidity sensor, and an SH1106 OLED display.

The project measures air quality values, displays them locally on an OLED screen, and also provides the measured data through a simple Wi-Fi web interface and JSON endpoint. A Python script can read the ESP32 data endpoint and plot the values in real time.

## Features

- ESP32 microcontroller
- ENS160 air quality sensor
- AHT21/AHT20 temperature and humidity sensor
- SH1106 128x64 OLED display
- I2C communication
- AQI, TVOC, eCO2, temperature and humidity measurement
- OLED display with simple icons and AQI face indicator
- Wi-Fi connection in STA mode
- Automatic fallback to Access Point mode
- Web page output
- JSON API endpoint
- Python real-time plotting script

---

## Hardware Used

| Component | Description |
|---|---|
| ESP32 | Main microcontroller |
| ENS160 | Air quality sensor |
| AHT21 / AHT20 | Temperature and humidity sensor |
| SH1106 OLED | 128x64 I2C display |
| Breadboard / wires | Prototype wiring |

---

## Pinout

| Signal | ESP32 Pin |
|---|---|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| VCC | 3.3V |
| GND | GND |

---

## Measured Values

| Value | Description |
|---|---|
| AQI | Air Quality Index |
| TVOC | Total Volatile Organic Compounds in ppb |
| eCO2 | Estimated CO2 in ppm |
| Temperature | Temperature in °C |
| Humidity | Relative humidity in % |

---

## Software Structure

The ESP32 firmware is written in Arduino/C++.

Main functions:

- `initSensors()` - initializes the AHT and ENS160 sensors
- `initDisplay()` - initializes the OLED display
- `connectWiFi()` - connects to Wi-Fi or starts Access Point mode
- `readTemperaturHumidity()` - reads temperature and humidity
- `readAirQuality()` - reads AQI, TVOC and eCO2 values
- `updateDisplay()` - updates the OLED screen
- `handleData()` - provides JSON data at `/data`
- `handleRoot()` - provides a simple web page

---

## Web Interface

After the ESP32 connects to Wi-Fi, the IP address is printed to the Serial Monitor.

Open the IP address in a browser:

```text
http://ESP32_IP_ADDRESS/
```

JSON data endpoint:

```text
http://ESP32_IP_ADDRESS/data
```

Example JSON output:

```json
{
  "aqi": 1,
  "tvoc": 120,
  "eco2": 450,
  "temp": 23.4,
  "hum": 48
}
```

---

## Python Plotter

The `PythonPlotter.py` script reads the ESP32 JSON endpoint and displays live charts using Matplotlib.

Required Python packages:

```bash
pip install requests matplotlib
```

Before running the script, update the ESP32 IP address inside `PythonPlotter.py`:

```python
URL = "http://ESP32_IP_ADDRESS/data"
```

Run:

```bash
python PythonPlotter.py
```

---

## Arduino Libraries

Install these libraries in the Arduino IDE:

- SparkFun ENS160
- Adafruit AHTX0
- Adafruit GFX
- Adafruit SH110X
- ESP32 board package

---

## Notes

The ENS160 sensor may need a short warm-up time before stable values are available. Breadboard wiring can also affect I2C stability, so short and clean wiring is recommended.

---

## Future Improvements

- Save Wi-Fi credentials outside the source code
- Add configuration page for Wi-Fi setup
- Add CSV logging
- Add better error handling for sensor read failures
- Add moving average filtering
- Add a desktop app with Start/Stop buttons
- Add enclosure design for a finished prototype

---

## Author

Attila Balogh
