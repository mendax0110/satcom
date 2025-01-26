# Radio Telescope Control Code with ADS7830 ADC and LCD Display

This Arduino sketch is designed for controlling a small-scale radio telescope. It leverages the ADS7830 Analog-to-Digital Converter (ADC) and an LCD display to measure, visualize, and analyze signals. The system is controlled via commands entered in the Serial Monitor and offers real-time data visualization through the Serial Plotter.

---

## Overview

This code is the backbone of a radio telescope project. The telescope captures signals and processes them with the ADS7830 ADC. The data can be displayed on an LCD for local monitoring or visualized in real-time using the Serial Plotter. Key features include frequency range adjustment, signal gain control, toggling of various display and processing modes, and settings persistence using EEPROM.

---

## Features

- Interaction with the ADS7830 ADC via I2C for signal processing.
- Display data on an LCD for on-site monitoring.
- Control and customization via Serial Monitor commands.
- Real-time visualization on the Serial Plotter.
- Tools for signal calibration and filtering.
- Persistent settings storage in EEPROM.
- Flexible voltage measurement ranges (0-5V or 0-3.3V).

---

## Commands

### 1. Set Frequency Range
- **Command:** `setRange <minFreq>,<maxFreq>`  
- **Description:** Sets the frequency range for analyzing signals.  
- **Example:** `setRange 50,1400`

### 2. Toggle LCD Display
- **Command:** `toggleLCD`  
- **Description:** Toggles the LCD display on or off.

### 3. Toggle Tone Generation
- **Command:** `toggleTone`  
- **Description:** Enables or disables tone generation for debugging or signal output.

### 4. Toggle Lowpass Filter
- **Command:** `toggleLowpass`  
- **Description:** Activates or deactivates the software-based Lowpass Filter for noise reduction.

### 5. ADC Calibration
- **Command:** `calibrate`  
- **Description:** Calibrates the ADC using a known reference voltage (in millivolts).

### 6. Auto Cycle Displays
- **Command:** `autoCycle`  
- **Description:** Toggles automatic cycling through different display views on the LCD.

### 7. Set Display View
- **Command:** `display <index>`  
- **Description:** Sets the LCD to a specific view.  
- **Views:**
  - **0:** ADC Value
  - **1:** Voltage in Volts
  - **2:** Voltage in Millivolts
  - **3:** Frequency in Hz
  - **4:** Voltage Bar Graph
  - **5:** Frequency Bar Graph

### 8. Set Gain Factor
- **Command:** `setGain <gainFactor>`  
- **Description:** Sets the software-based gain factor for voltage measurements.  
- **Example Values:**
  - `1.0` (default)  
  - `2.0` (double gain)  
  - `0.5` (reduce gain)

### 9. Save Settings
- **Command:** `saveSettings`  
- **Description:** Saves the current settings (e.g., frequency range, gain factor) to EEPROM for persistence across power cycles.

### 10. Load Settings
- **Command:** `loadSettings`  
- **Description:** Loads the saved settings from EEPROM.

### 11. Live Calibration
- **Command:** `calibrateLive`  
- **Description:** Performs dynamic calibration, allowing real-time adjustment for more precise measurements.

### 12. Set Voltage Range
- **Command:** `setVoltageRange <range>`  
- **Description:** Sets the voltage measurement range.  
- **Supported Ranges:** `5.0` (default) or `3.3`.  
- **Example:** `setVoltageRange 3.3`

---

## Serial Plotter Outputs

The following data streams are visualized on the Serial Plotter:

- **ADC:** The raw ADC value (0-255).  
- **Voltage:** The measured voltage (in Volts).  
- **Voltage_mV:** The measured voltage (in millivolts).  
- **Frequency:** The calculated frequency (in Hz).

---

## Setup Notes

- Ensure the ADS7830 ADC is connected to the I2C bus.  
- Verify proper wiring of the LCD display.  
- This system is designed to measure and process radio frequencies received by the telescope's antenna, providing a hands-on tool for exploring the radio spectrum.

---

## Applications

This code forms the foundation of a radio telescope, enabling hobbyists, educators, and researchers to:

- Analyze and monitor radio frequency signals.
- Study celestial phenomena emitting in the radio spectrum.
- Experiment with signal processing techniques.
- Persist and restore custom settings for consistent results.

---
