### ESP32 Board

- **Board:** Denky32
- **Platform:** espressif32
- **Framework:** arduino

### Rotary Encoder

- **CLK_PIN:** GPIO 19
- **DT_PIN:** GPIO 18
- **SW_PIN:** GPIO 5

### OLED Display

- **SDA:** GPIO 21
- **SCL:** GPIO 22


### HX711 Load Cell Amplifier
  - **DT:** GPIO 4
  - **SCK:** GPIO 16

## Setup Instructions

1. **Install PlatformIO:**
   - Follow the instructions on the [PlatformIO website](https://platformio.org/install) to install PlatformIO in your preferred environment.

2. **Clone the Repository:**
   - Clone this repository to your local machine.

3. **Open the Project:**
   - Open the project folder in your PlatformIO-compatible IDE (e.g., Visual Studio Code).

4. **Configure the Project:**
   - Ensure the `platformio.ini` file is configured correctly for your board.

5. **Upload the Code:**
   - Connect your ESP32 board to your computer.
   - Use PlatformIO to upload the code to the board.

## Acknowledgements

- [PlatformIO](https://platformio.org/)
- [Arduino Framework](https://www.arduino.cc/)
- [Adafruit Libraries](https://github.com/adafruit)
