# 555 Timer Fuel Level Sender - DUAL TANK SYSTEM
## Complete Design for RV-7 Aircraft - Both Tanks

---

## System Overview

This design uses **one Arduino** to monitor **two independent fuel tanks**, each with its own 555 timer circuit and DAC output. Simple button calibration for each tank.

**Key Features:**
- **Dual tank support** - simultaneous monitoring of left and right tanks
- **Independent outputs** - separate 0-5V signal for each tank to Dynon D120
- **Simple calibration** - three buttons (Tank Select, Empty, Full)
- **All through-hole components** - no SMD soldering
- **Total cost: ~$30**

---

## Hardware Requirements

### Components List

| Part | Description | Qty | Price |
|------|-------------|-----|-------|
| **NE555 Timer IC** | Standard 555 timer (DIP-8) | **2** | $1.00 |
| 8-pin DIP socket | For 555 ICs | 2 | $0.50 |
| **Arduino Nano** | ATmega328P microcontroller | **1** | $5 |
| **MCP4725 Breakout** | 12-bit DAC with I2C | **2** | $10 |
| LM7805 | 5V voltage regulator (TO-220) | 1 | $0.50 |
| R1, R2: 10kΩ | 1/4W metal film | 2 | $0.20 |
| R3, R4: 100kΩ | 1/4W metal film | 2 | $0.20 |
| R5, R6: 1kΩ | Output protection | 2 | $0.20 |
| R7: 330Ω | LED current limit | 1 | $0.10 |
| C1, C2: 0.01µF | Ceramic, 555 pin 5 bypass | 2 | $0.20 |
| C3: 10µF electrolytic | Power supply filter | 2 | $0.20 |
| C4-C7: 0.1µF ceramic | Decoupling caps | 4 | $0.40 |
| LED | 5mm red or green | 1 | $0.20 |
| **Pushbuttons** | SPST momentary (NO) | **3** | $1.50 |
| D1: 1N4001 diode | Reverse protection | 1 | $0.10 |
| Perfboard or PCB | For mounting | 1 | $3 |
| Enclosure | Aluminum for EMI shielding | 1 | $12 |

**Total Cost: ~$30-35**

---

## Complete Pin Connections

### Arduino Nano Connections

```
Power:
  Vin -------- +5V (from LM7805)
  GND -------- Common Ground

Frequency Inputs:
  Pin 2 ------ Left 555 Output (interrupt 0)
  Pin 3 ------ Right 555 Output (interrupt 1)

Calibration Buttons:
  Pin 4 ------ EMPTY button (to GND when pressed)
  Pin 5 ------ FULL button (to GND when pressed)
  Pin 6 ------ TANK SELECT button (to GND when pressed)

Status:
  Pin 13 ----- Status LED + 330Ω resistor to GND

I2C Bus:
  A4 (SDA) --- MCP4725 Left SDA + MCP4725 Right SDA
  A5 (SCL) --- MCP4725 Left SCL + MCP4725 Right SCL
```

### Left Tank 555 Timer Circuit

```
Pin 1 (GND) -------- Ground
Pin 2 (Trigger) ---- Junction (R3 + Left Probe)
Pin 3 (Output) ----- Arduino Pin 2 (via 1kΩ)
Pin 4 (Reset) ------ +5V
Pin 5 (Control) ---- 0.01µF to GND
Pin 6 (Threshold) -- Junction (R3 + Left Probe)
Pin 7 (Discharge) -- Junction (R3 + Left Probe)
Pin 8 (Vcc) -------- +5V

Timing Network:
  +5V --- [R1 10kΩ] --- Junction
  Junction --- [R3 100kΩ] --- Left Fuel Probe --- GND
```

### Right Tank 555 Timer Circuit

```
Pin 1 (GND) -------- Ground
Pin 2 (Trigger) ---- Junction (R4 + Right Probe)
Pin 3 (Output) ----- Arduino Pin 3 (via 1kΩ)
Pin 4 (Reset) ------ +5V
Pin 5 (Control) ---- 0.01µF to GND
Pin 6 (Threshold) -- Junction (R4 + Right Probe)
Pin 7 (Discharge) -- Junction (R4 + Right Probe)
Pin 8 (Vcc) -------- +5V

Timing Network:
  +5V --- [R2 10kΩ] --- Junction
  Junction --- [R4 100kΩ] --- Right Fuel Probe --- GND
```

### MCP4725 DACs

**Left Tank DAC (Address 0x62 - default):**
```
VDD ---- +5V
GND ---- Ground
SDA ---- Arduino A4
SCL ---- Arduino A5
OUT ---- Dynon D120 Left Fuel Input
```

**Right Tank DAC (Address 0x63 - A0 bridged):**
```
VDD ---- +5V
GND ---- Ground
SDA ---- Arduino A4
SCL ---- Arduino A5
A0 ----- Bridge to VDD (sets address to 0x63)
OUT ---- Dynon D120 Right Fuel Input
```

**CRITICAL:** One MCP4725 must have its A0 address pin bridged to change address from 0x62 to 0x63!

---

## Calibration Procedure

### Initial Setup

1. Upload Arduino code via USB
2. Open Serial Monitor (115200 baud)
3. Verify both DACs detected (0x62 and 0x63)
4. Note default tank selection (LEFT)

### Calibrate Left Tank

1. Press **TANK SELECT** button once
   - LED blinks **1 time** = LEFT tank selected
   - Serial shows: ">>> SELECTED: LEFT TANK <<<"

2. **Drain LEFT tank completely**
   - Wait 5 minutes for fuel to settle

3. Press and **hold EMPTY button for 2 seconds**
   - LED stays on while holding
   - LED blinks 3 times when saved
   - Serial shows: "LEFT TANK - EMPTY CALIBRATION SET"

4. **Fill LEFT tank to capacity**
   - Wait 5 minutes for fuel to settle

5. Press and **hold FULL button for 2 seconds**
   - LED stays on while holding
   - LED blinks 5 times when saved
   - Serial shows: "LEFT TANK - FULL CALIBRATION SET"

### Calibrate Right Tank

1. Press **TANK SELECT** button once
   - LED blinks **2 times** = RIGHT tank selected
   - Serial shows: ">>> SELECTED: RIGHT TANK <<<"

2. **Drain RIGHT tank completely**
   - Wait 5 minutes for fuel to settle

3. Press and **hold EMPTY button for 2 seconds**
   - LED stays on while holding
   - LED blinks 3 times when saved
   - Serial shows: "RIGHT TANK - EMPTY CALIBRATION SET"

4. **Fill RIGHT tank to capacity**
   - Wait 5 minutes for fuel to settle

5. Press and **hold FULL button for 2 seconds**
   - LED stays on while holding
   - LED blinks 5 times when saved
   - Serial shows: "RIGHT TANK - FULL CALIBRATION SET"

### Verification

- Check serial monitor for both tank readings
- Verify left output: 0V empty to 5V full
- Verify right output: 0V empty to 5V full
- Test with intermediate fuel levels
- Compare to dipstick measurements

**All calibration stored in EEPROM - survives power cycles!**

---

## LED Status Indicators

| LED Pattern | Meaning |
|-------------|---------|
| 3 quick blinks at startup | System initialized OK |
| Rapid blinking | DAC initialization error |
| 1 blink | LEFT tank selected |
| 2 blinks | RIGHT tank selected |
| Solid on (while holding button) | Calibration button held |
| 3 blinks | EMPTY calibration saved |
| 5 blinks | FULL calibration saved |

---

## Wiring Diagram Summary

```
POWER FLOW:
Aircraft 12V → [Fuse] → [Diode] → [LM7805] → +5V
                                        ↓
                    Arduino + Both 555s + Both DACs

SIGNAL FLOW - LEFT TANK:
Left Fuel Probe → Left 555 Timer → Frequency → Arduino Pin 2
                                                      ↓
                                            MCP4725 (0x62) → 0-5V
                                                      ↓
                                            Dynon Left Input

SIGNAL FLOW - RIGHT TANK:
Right Fuel Probe → Right 555 Timer → Frequency → Arduino Pin 3
                                                       ↓
                                            MCP4725 (0x63) → 0-5V
                                                       ↓
                                            Dynon Right Input

CALIBRATION:
TANK SELECT button → Arduino Pin 6 → Select which tank to calibrate
EMPTY button → Arduino Pin 4 → Set empty point for selected tank
FULL button → Arduino Pin 5 → Set full point for selected tank
```

---

## Typical Frequency Ranges

**Left Tank (example values):**
- Empty (~200 pF): 68 kHz
- Half full (~400 pF): 35 kHz
- Full (~600 pF): 17 kHz

**Right Tank (example values):**
- Empty (~200 pF): 68 kHz
- Half full (~400 pF): 35 kHz
- Full (~600 pF): 17 kHz

Your actual values will depend on your specific fuel probes!

---

## Advantages Over Single-Tank Design

✅ Both tanks monitored simultaneously
✅ Independent calibration for each tank
✅ Separate outputs to Dynon (no switching needed)
✅ Only one Arduino needed
✅ Still simple button calibration
✅ Both tanks update at same rate (no lag)

---

## Installation Notes

### Physical Layout

**Recommended component placement:**
```
[Left 555] --- [Arduino] --- [Right 555]
                   |
            [Left DAC] [Right DAC]
```

Keep 555 circuits close to their respective fuel probe connections.

### Fuel Probe Connections

- Use **RG-174 coax** or similar shielded cable
- Maximum length: 3 feet per probe
- Shield grounded at converter end ONLY
- Route away from ignition wiring
- Label clearly: "LEFT FUEL" and "RIGHT FUEL"

### Output Connections

- Two separate coax cables to Dynon
- Label clearly: "LEFT OUTPUT" and "RIGHT OUTPUT"
- Verify correct connection to Dynon left/right inputs
- Common ground between fuel sender and Dynon

### Button Mounting

- Mount all three buttons where accessible
- Recessed or guarded to prevent accidental press
- Clear labels:
  - "TANK SELECT"
  - "EMPTY CAL"
  - "FULL CAL"

---

## Troubleshooting

### Only One Tank Reading

**Check:**
- Both 555 circuits powered
- Both frequency inputs connected to Arduino
- Serial monitor shows both frequencies
- Both DACs responding (I2C scanner)

### Tank Selection Not Working

**Check:**
- TANK SELECT button wired to Pin 6
- Button connects to GND when pressed
- Serial monitor shows selection changes
- LED blink count (1=LEFT, 2=RIGHT)

### One DAC Not Working

**Check:**
- Power to DAC
- I2C connections (SDA/SCL)
- Address: one at 0x62, one at 0x63
- A0 pin bridged on right DAC only

### Calibration Not Saving

**Check:**
- Holding button for full 2 seconds
- Correct tank selected first
- LED confirmation blinks
- Serial monitor confirmation message

---

## Serial Monitor Output Example

```
555 Timer Fuel Sender - DUAL TANK
===================================
Left DAC initialized at 0x62
Right DAC initialized at 0x63

=== Calibration Values ===
LEFT TANK:
  Empty frequency: 68000 Hz
  Full frequency: 17000 Hz
  Capacity: 20.0 gallons
RIGHT TANK:
  Empty frequency: 67500 Hz
  Full frequency: 17500 Hz
  Capacity: 20.0 gallons

=== System Ready ===
Currently selected: LEFT TANK

L: 45000 Hz (10.2 gal, 2.55V) | R: 44800 Hz (10.5 gal, 2.63V)
L: 45100 Hz (10.1 gal, 2.53V) | R: 44750 Hz (10.6 gal, 2.65V)
```

---

## Expanding Features

### Different Tank Capacities

If your tanks have different capacities, edit the code:

```cpp
TankCalibration leftTank = {50000, 20000, 18.0};   // 18 gallons
TankCalibration rightTank = {50000, 20000, 22.0};  // 22 gallons
```

### Low Fuel Warning

Add warning output:

```cpp
#define LOW_FUEL_WARNING_PIN 7
#define LOW_FUEL_THRESHOLD 3.0  // gallons

if (leftGallons < LOW_FUEL_THRESHOLD || 
    rightGallons < LOW_FUEL_THRESHOLD) {
  digitalWrite(LOW_FUEL_WARNING_PIN, HIGH);
}
```

### Total Fuel Display

Calculate total fuel across both tanks:

```cpp
float totalGallons = leftGallons + rightGallons;
Serial.print("Total: "); 
Serial.print(totalGallons, 1); 
Serial.println(" gal");
```

---

## Maintenance

- **Monthly:** Verify readings against dipstick
- **After 50 hours:** Check all connections
- **Annual:** Re-verify calibration
- **After maintenance:** Re-check wiring

---

## Final Notes

This dual-tank design maintains all the simplicity and reliability of the single-tank version while monitoring both tanks simultaneously. The tank selection button makes calibration intuitive - just select the tank, then use the same EMPTY/FULL buttons you're already familiar with.

**Total button count: 3**
- TANK SELECT (choose which to calibrate)
- EMPTY (calibrate empty point)
- FULL (calibrate full point)

Simple, effective, and proven technology!
