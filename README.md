# ADS7830 Arduino Sketch with LCD Display and Serial Monitor Control

This Arduino sketch enables interaction with the ADS7830 Analog-to-Digital Converter (ADC) and an LCD display. Users can issue commands via the Serial Monitor or visualize data on the Serial Plotter for dynamic control and visualization.

---

## Features

- Interact with the ADS7830 ADC via I2C.
- Display data on an LCD.
- Control via Serial Monitor commands.
- Real-time visualization on the Serial Plotter.

---

## Commands

### 1. Set Frequency Range
- **Command:** `setRange <minFreq>,<maxFreq>`  
- **Description:** Sets the frequency range for measurements.  
- **Example:** `setRange 50,1400`

### 2. Toggle LCD Display
- **Command:** `toggleLCD`  
- **Description:** Toggles the LCD display on or off.

### 3. Toggle Tone Generation
- **Command:** `toggleTone`  
- **Description:** Enables or disables tone generation.

### 4. Toggle Lowpass Filter
- **Command:** `toggleLowpass`  
- **Description:** Activates or deactivates the software-based Lowpass Filter.

### 5. ADC Calibration
- **Command:** `calibrate`  
- **Description:** Calibrates the ADC using a known reference voltage (in millivolts).

### 6. Auto Cycle Displays
- **Command:** `autoCycle`  
- **Description:** Toggles automatic cycling through different display views.

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
