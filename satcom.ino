/*
 * This Arduino sketch allows interaction with the ADS7830 ADC and an LCD display
 * using a set of commands entered via the Serial Monitor or plotted on the Serial Plotter.
 *
 * Available commands:
 * 1. setRange <minFreq>,<maxFreq> - Set the frequency range (e.g., "setRange 50,1400").
 * 2. toggleLCD - Toggle the LCD display on/off.
 * 3. toggleTone - Toggle tone generation on/off.
 * 4. toggleLowpass - Toggle the software LowpassFilter on/off
 * 5. calibrate - Calibrate the ADC using a known reference voltage in millivolts.
 * 6. autoCycle - Toggle auto-cycling through different displays.
 * 7. display <index> - Set the display to a specific view (0-5):
 *    - 0: ADC Value
 *    - 1: Voltage in Volts
 *    - 2: Voltage in Millivolts
 *    - 3: Frequency in Hz
 *    - 4: Voltage Bar Graph
 *    - 5: Frequency Bar Graph
 * 8. setGain <gainFactor> - Set the software-based gain factor for voltage measurements.
 *    - Gain factor values: 1.0 (default), 2.0 (double gain), 0.5 (reduce gain), etc.
 *
 * The Serial Plotter outputs the following data for visualization:
 * - ADC: The raw ADC value (0-255).
 * - Voltage: The measured voltage (in Volts).
 * - Voltage_mV: The measured voltage (in millivolts).
 * - Frequency: The calculated frequency (in Hz).
 *
 * Make sure the ADS7830 ADC is connected to the I2C bus and the LCD is properly wired.
 */

#include <Wire.h>
#include <LiquidCrystal.h>

// ADS7830 Class Declaration
class ADCDevice
{
protected:
    int address;
    int cmd;
public:
    ADCDevice() : address(0), cmd(0) {}
    virtual ~ADCDevice() {}
    virtual bool detectI2C(int addr);
    virtual int analogRead(int chn) = 0;
};

class ADS7830 : public ADCDevice
{
public:
    ADS7830(int addr = 0x4B);
    int analogRead(int chn) override;
};

bool ADCDevice::detectI2C(int addr)
{
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0)
    {
        address = addr;
        Serial.println("Device found");
        return true;
    }
    else
    {
        Serial.print("Device not found at address 0x");
        Serial.println(addr, HEX);
        return false;
    }
}

ADS7830::ADS7830(int addr)
{
    address = addr;
    cmd = 0x84; // default command for ADS7830
}

int ADS7830::analogRead(int chn)
{
    Wire.beginTransmission(address);
    Wire.write(cmd | (((chn << 2 | chn >> 1) & 0x07) << 4));
    Wire.endTransmission();
    Wire.requestFrom(address, 1);
    if (Wire.available())
    {
        return Wire.read();
    }
    else
    {
        Serial.println("Error reading ADC!");
        return -1;
    }
}

// LCD pins
const int rs = 12; // Register Select pin
const int en = 11; // Enable pin
const int d4 = 5;  // Dp 4
const int d5 = 4;  // Dp 5
const int d6 = 3;  // Dp 6
const int d7 = 2;  // Dp 7

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// init ADS7830 I2C address 0x4B
ADS7830 adc(0x4B);

int adcValue;
float voltage;
float voltage_mV;
int frequency;
float scaleFactor = 1.0;
bool lcdEnabled = true;
bool toneEnabled = true;
bool autoCycle = true;
bool lowPassEnabled = false;
int frequencyMin = 50;
int frequencyMax = 1400;
int currentDisplay = 0;

float gainFactor = 1.0;
const float GAIN_TWOTHIRDS = 1.5;
const float GAIN_ONE = 1.0;
const float GAIN_TWO = 0.5;
const float GAIN_FOUR = 0.25;

const int tonePin = 6;

int adcHistory[10] = {0};
int historyIndex = 0;

// Display queue
enum DisplayType { DISPLAY_ADC, DISPLAY_VOLTAGE, DISPLAY_MV, DISPLAY_FREQUENCY, DISPLAY_VOLTAGE_BAR, DISPLAY_FREQUENCY_BAR };
const int numDisplays = 6;

void displayMessageOnLCD(String message)
{
    lcd.clear();
    if (message.length() > 16)
    {
        String firstLine = message.substring(0, 16);
        String secondLine = message.substring(16);
        lcd.setCursor(0, 0);
        lcd.print(firstLine);
        lcd.setCursor(0, 1);
        lcd.print(secondLine);
    }
    else
    {
        lcd.setCursor(0, 0);
        lcd.print(message);
    }
}

float applyLowPassFilter(int adcValue)
{
    adcHistory[historyIndex] = adcValue;
    historyIndex = (historyIndex + 1) % 10;

    int sum = 0;
    for (int i = 0; i < 10; i++)
    {
        sum += adcHistory[i];
    }

    return sum / 10.0;
}

void setGain(float newGain)
{
    if (newGain > 0.0)
    {
        gainFactor = newGain;
        displayMessageOnLCD("[INFO] Gain factor updated to: " + String(gainFactor, 4));
    }
    else
    {
        displayMessageOnLCD("[ERROR] Invalid gain factor.");
    }
}

void displayVoltageBarGraph(float voltage)
{
    int bars = map(voltage * 1000, 0, 3300, 0, 16);
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++)
    {
        lcd.print(i < bars ? '|' : ' ');
    }
}

void displayFrequencyBarGraph(int frequency)
{
    int bars = map(frequency, frequencyMin, frequencyMax, 0, 16);
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++)
    {
        lcd.print(i < bars ? '#' : ' ');
    }
}

void sendPlotterData(int adc, float voltage, int freq)
{
    Serial.print("ADC:");
    Serial.print(adc);
    Serial.print(",Voltage:");
    Serial.print(voltage, 4);
    Serial.print(",Voltage_mV:");
    Serial.print(voltage * 1000, 2);
    Serial.print(",Frequency:");
    Serial.println(freq);
}

void processSerialInput(String input)
{
    if (input.startsWith("setRange"))
    {
        int minFreq = input.substring(9, input.indexOf(",")).toInt();
        int maxFreq = input.substring(input.indexOf(",") + 1).toInt();
        if (minFreq > 0 && maxFreq > minFreq)
        {
            frequencyMin = minFreq;
            frequencyMax = maxFreq;
            Serial.println("[INFO] Frequency range updated.");
        }
        else
        {
            Serial.println("[ERROR] Invalid range.");
        }
    }
    else if (input.startsWith("setGain"))
    {
        float newGain = input.substring(8).toFloat();
        setGain(newGain);
    }
    else if (input == "toggleLCD")
    {
        lcdEnabled = !lcdEnabled;
        Serial.println("[INFO] LCD display toggled.");
    }
    else if (input == "toggleLowpass")
    {
        lowPassEnabled = !lowPassEnabled;
        Serial.println("[INFO] Lowpass toggled");
    }
    else if (input == "toggleTone")
    {
        toneEnabled = !toneEnabled;
        if (toneEnabled)
        {
            tone(tonePin, frequency);
            Serial.println("[INFO] Tone generation toggled.");
        }
        else
        {
            noTone(tonePin);
            Serial.println("[INFO] Tone generation disabled.");
        }
    }
    else if (input == "calibrate")
    {
        Serial.println("[INFO] Enter reference voltage in mV:");
        while (!Serial.available());
        float referenceVoltage = Serial.parseFloat();
        scaleFactor = referenceVoltage / ((adcValue / 255.0) * 3.3);
        Serial.println("[INFO] Calibration complete.");
    }
    else if (input == "autoCycle")
    {
        autoCycle = !autoCycle;
        Serial.println(autoCycle ? "[INFO] Auto-cycle enabled." : "[INFO] Auto-cycle disabled.");
    }
    else if (input.startsWith("display"))
    {
        int index = input.substring(7).toInt();
        if (index >= 0 && index < numDisplays)
        {
            autoCycle = false;
            currentDisplay = index;
            Serial.println("[INFO] Display updated to " + String(index));
        }
        else
        {
            Serial.println("[ERROR] Invalid display index.");
        }
    }
    else
    {
        Serial.println("[ERROR] Unknown command.");
    }
}

void checkSerial()
{
    if (Serial.available() > 0)
    {
        String input = Serial.readStringUntil('\n');
        processSerialInput(input);
    }
}

void updateDisplay()
{
    if (!lcdEnabled) return;

    lcd.clear();
    switch (currentDisplay)
    {
    case DISPLAY_ADC:
        lcd.setCursor(0, 0);
        lcd.print("ADC Value:");
        lcd.setCursor(0, 1);
        lcd.print(adcValue);
        break;
    case DISPLAY_VOLTAGE:
        lcd.setCursor(0, 0);
        lcd.print("Voltage:");
        lcd.setCursor(0, 1);
        lcd.print(voltage, 2);
        lcd.print(" V");
        break;
    case DISPLAY_MV:
        lcd.setCursor(0, 0);
        lcd.print("Voltage:");
        lcd.setCursor(0, 1);
        lcd.print(voltage_mV, 2);
        lcd.print(" mV");
        break;
    case DISPLAY_FREQUENCY:
        lcd.setCursor(0, 0);
        lcd.print("Frequency:");
        lcd.setCursor(0, 1);
        lcd.print(frequency);
        lcd.print(" Hz");
        break;
    case DISPLAY_VOLTAGE_BAR:
        lcd.setCursor(0, 0);
        lcd.print("Voltage Graph:");
        displayVoltageBarGraph(voltage);
        break;
    case DISPLAY_FREQUENCY_BAR:
        lcd.setCursor(0, 0);
        lcd.print("Frequency Graph:");
        displayFrequencyBarGraph(frequency);
        break;
    }
}

void setup()
{
    Serial.begin(9600);
    while (!Serial);
    Serial.println("[SUCCESS] Serial Init.");

    Wire.begin();

    lcd.begin(16, 2);
    lcd.clear();
    displayMessageOnLCD("[INFO] Radio-Astronomy");
    delay(3000);

    if (!adc.detectI2C(0x4B))
    {
        displayMessageOnLCD("[ERROR] ADC Not Found");
        while (1);
    }
    else
    {
        Serial.println("[SUCCESS] ADS7830 setup successful!");
    }
}

void loop()
{
    checkSerial();

    adcValue = adc.analogRead(0);
    if (adcValue >= 0)
    {
        if (lowPassEnabled) 
            adcValue = applyLowPassFilter(adcValue);

        voltage = ((adcValue / 255.0) * 3.3) * scaleFactor;
        voltage_mV = voltage * 1000;
        frequency = map(adcValue, 0, 255, frequencyMin, frequencyMax);

        if (toneEnabled) 
            tone(tonePin, frequency);

        if (autoCycle)
        {
            static unsigned long lastUpdate = 0;
            if (millis() - lastUpdate > 2000)
            {
                currentDisplay = (currentDisplay + 1) % numDisplays;
                lastUpdate = millis();
            }
        }

        updateDisplay();
        sendPlotterData(adcValue, voltage, frequency);
    }
    else
    {
        Serial.println("[ERROR] Failed to read ADC!");
        if (lcdEnabled) displayMessageOnLCD("[ERROR] ADC READ");
    }

    delay(200);
}
