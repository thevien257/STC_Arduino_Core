# STC8G Arduino Core

Arduino support for STC8G microcontrollers.

## Installation

### Via Arduino Board Manager (Recommended)

1. Open Arduino IDE
2. Go to **File > Preferences**
3. Add this URL to **Additional Boards Manager URLs**:
```
   https://raw.githubusercontent.com/thevien257/STC_Arduino_Core/refs/heads/main/package_stc8051_index.json
```
4. Go to **Tools > Board > Boards Manager**
5. Search for "STC8051"
6. Click **Install**

### Requirements

- Python 3.6 or later (for stcgal programmer)
- Arduino IDE 1.8.x or Arduino IDE 2.x

### Supported Boards

- STC8G1K08A

## Usage

1. Select **Tools > Board > STC8G Boards > STC8G1K08A**
2. Select your clock frequency
3. Select your serial port
4. Upload your sketch

## License
