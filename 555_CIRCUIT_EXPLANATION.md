# How the 555 Timer Measures Capacitance
## Complete Circuit Analysis and Component Functions

---

## Overview: The Big Picture

The 555 timer is configured as an **astable oscillator** - it oscillates (switches on and off) continuously. The frequency of oscillation depends on the capacitance of the fuel probe. By measuring this frequency, we determine the capacitance, which tells us the fuel level.

**Simple version:**
```
More fuel → Higher capacitance → Slower charging → Lower frequency
Less fuel → Lower capacitance → Faster charging → Higher frequency
```

---

## The Complete Circuit

```
                                    +5V
                                     |
                                [R1: 10kΩ]
                                     |
         +-------[555 Timer]--------+
         |   Pin 8 (Vcc) ← Power    |
         |   Pin 4 (Reset) ← Enable |
         |                          |
    +5V--+                     [Junction Point]
         |                          |
         |                     [R2: 100kΩ]
         |                          |
         |   Pin 7 (Discharge)------+
         |   Pin 6 (Threshold)------+
         |   Pin 2 (Trigger)--------+
         |                          |
         |                   [FUEL PROBE]
         |                   (Capacitor)
         |                    150-600 pF
         |                          |
         |   Pin 1 (GND)-----------GND
         |   Pin 5 (CV)----[0.01µF]--GND
         |
         |   Pin 3 (Output)----[1kΩ]---- To Arduino
         +------------------------
```

---

## Component-by-Component Breakdown

### 1. NE555 Timer IC - The Heart of the Circuit

**What it is:** An integrated circuit containing approximately 25 transistors, resistors, and comparators arranged to create precise timing.

**What it does:** 
- Charges and discharges the capacitor in a controlled cycle
- Outputs a square wave whose frequency depends on R1, R2, and C
- Switches between charging and discharging automatically

**Pin Functions:**

| Pin | Name | Function in Our Circuit |
|-----|------|------------------------|
| 1 | GND | Circuit ground reference |
| 2 | Trigger | Senses when cap voltage drops to 1.67V (starts charge) |
| 3 | Output | Square wave output (~0V or ~5V) |
| 4 | Reset | Tied to +5V to keep timer enabled |
| 5 | Control Voltage | Bypassed with 0.01µF cap for noise immunity |
| 6 | Threshold | Senses when cap voltage reaches 3.33V (starts discharge) |
| 7 | Discharge | Opens/closes to discharge capacitor |
| 8 | Vcc | +5V power supply |

---

### 2. R1 (10kΩ Resistor) - High-Side Charging Resistor

**What it is:** Fixed metal film resistor, brown-black-orange color bands

**What it does:**
- Controls charging current when capacitor is charging
- Works with R2 to set the charging time
- Limits current from +5V supply

**Why this value?**
```
Too small (e.g., 1kΩ):
  - Charges too fast
  - Frequency too high
  - Less resolution
  - More power consumption

Too large (e.g., 100kΩ):
  - Charges too slowly  
  - Frequency too low
  - Longer measurement time
  - More susceptible to noise

10kΩ is the sweet spot:
  - Good frequency range (15-70 kHz)
  - Low power consumption
  - Good noise immunity
  - Fast enough response
```

**Current calculation:**
```
When output is high and discharging:
I = V / R1 = 5V / 10kΩ = 0.5 mA

Power dissipation:
P = I² × R = (0.5mA)² × 10kΩ = 2.5 mW

This is why 1/4W resistor (250mW) is plenty!
```

---

### 3. R2 (100kΩ Resistor) - Low-Side Timing Resistor

**What it is:** Fixed metal film resistor, brown-black-yellow color bands

**What it does:**
- Controls both charging and discharging time
- Creates voltage divider during charging
- Provides discharge path for capacitor

**Why this value?**
```
The ratio R2/R1 affects duty cycle:

R1 = 10kΩ, R2 = 100kΩ
Ratio = 10:1

This gives approximately:
  - 55% duty cycle (output HIGH)
  - 45% duty cycle (output LOW)
  
Close to 50% square wave = good for digital processing
```

**Frequency calculation with R2:**
```
f = 1.44 / ((R1 + 2×R2) × C)

With our values:
R1 = 10,000Ω
R2 = 100,000Ω
Sum = 10k + 200k = 210kΩ

Empty tank (200 pF):
f = 1.44 / (210,000 × 200×10⁻¹²)
f = 1.44 / 0.000042
f ≈ 34,286 Hz ≈ 34 kHz

Full tank (600 pF):
f = 1.44 / (210,000 × 600×10⁻¹²)
f = 1.44 / 0.000126
f ≈ 11,429 Hz ≈ 11 kHz

Range: 11-34 kHz (excellent!)
```

---

### 4. Fuel Probe (Variable Capacitor) - The Sensor

**What it is:** 
Two concentric metal tubes in the fuel tank, separated by an insulating gap. Fuel acts as a dielectric.

**Construction:**
```
Cross-section view:

    [Inner tube - Signal]
         |  |
         |  | <-- Gap (air or fuel)
         |  |
    [Outer tube - Ground]
```

**How it works as a capacitor:**

**Capacitance formula:**
```
C = ε × (A / d)

Where:
  ε = permittivity of dielectric (fuel or air)
  A = area of plates (proportional to fuel height)
  d = distance between plates (fixed gap)
```

**Why capacitance changes with fuel:**
```
Empty tank:
  - Gap filled with air
  - Air permittivity (εᵣ) ≈ 1.0
  - Low capacitance (~150-200 pF)

Full tank:
  - Gap filled with fuel
  - Fuel permittivity (εᵣ) ≈ 2.0 (varies with fuel type)
  - High capacitance (~550-650 pF)
  
The fuel has ~2× the permittivity of air,
so capacitance roughly doubles when submerged!
```

**Typical values for RV-7:**
```
Empty: 150-200 pF
1/4 full: 250-300 pF
1/2 full: 350-450 pF
3/4 full: 500-550 pF
Full: 600-650 pF

Total range: ~450 pF change
```

---

### 5. C1 (0.01µF Bypass Capacitor) - Noise Filter

**What it is:** Small ceramic capacitor connected to pin 5

**What it does:**
- Filters noise on the control voltage pin
- Prevents false triggering from electrical interference
- Stabilizes internal voltage reference

**Why pin 5 needs this:**
```
Internally, the 555 creates reference voltages:
  - 3.33V (threshold for pin 6)
  - 1.67V (threshold for pin 2)

These are created by a resistor divider inside the chip.

Pin 5 provides access to this voltage reference.

Without bypass cap:
  - Noise can couple into this reference
  - Causes jitter in switching thresholds
  - Erratic frequency output
  
With 0.01µF bypass:
  - AC noise shorted to ground
  - DC reference stays stable
  - Clean, stable oscillation
```

**Why 0.01µF?**
```
Impedance at noise frequencies:

At 1 MHz (typical EMI):
Z = 1/(2π × f × C)
Z = 1/(2π × 1MHz × 0.01µF)
Z = 16Ω

Very low impedance = excellent noise bypass!

At oscillation frequency (30 kHz):
Z = 531Ω
Still effective at filtering oscillator harmonics
```

**Alternative values:**
- 0.001µF (1nF): Acceptable, slightly less filtering
- 0.1µF: Too large, can affect operation
- None: BAD! Circuit will be noise-sensitive

---

### 6. Output Resistor (1kΩ) - Protection and Current Limiting

**What it is:** Small resistor between pin 3 and Arduino input

**What it does:**
- Limits current if Arduino pin accidentally set to output
- Provides impedance matching
- Protects 555 output driver
- Reduces ringing on long wires

**Why needed?**
```
555 output can source/sink ~200mA
Arduino input pin is high-impedance (~100MΩ)

Without resistor:
  - If Arduino pin set to OUTPUT by mistake = short circuit!
  - 555 could be damaged
  - Arduino could be damaged
  
With 1kΩ resistor:
  - Current limited to 5V/1kΩ = 5mA (safe)
  - Both chips protected
  - Signal still strong enough for Arduino
```

**Signal integrity:**
```
Arduino input impedance: ~100MΩ
Output resistor: 1kΩ

Voltage divider ratio:
Vin/Vout = (100MΩ)/(100MΩ + 1kΩ) ≈ 1.00

Negligible voltage drop!

But provides protection if something goes wrong.
```

---

## How the Oscillation Cycle Works

Let me walk through one complete cycle:

### Phase 1: Charging the Capacitor (Output HIGH)

**Initial state:**
- Capacitor voltage: 1.67V (just dropped below trigger threshold)
- Pin 3 output: Switches HIGH (~5V)
- Pin 7 discharge: OPEN (high impedance, not conducting)

**What happens:**
```
Current flow path:

+5V → R1 (10kΩ) → Junction → R2 (100kΩ) → Fuel Probe Cap → GND
                              ↓
                        (Voltage builds up)
```

**The charging equation:**
```
V(t) = 5V × (1 - e^(-t/RC))

Where:
  R = R1 + R2 = 10kΩ + 100kΩ = 110kΩ
  C = fuel probe capacitance
  
Time constant τ = R × C

For 400 pF:
τ = 110,000 × 400×10⁻¹² = 44 microseconds
```

**Charging proceeds:**
```
t = 0:     V = 1.67V (starting point)
t = τ:     V = 3.16V (63% toward 5V)
t = 1.5τ:  V = 3.33V (THRESHOLD REACHED!)
```

**When voltage reaches 3.33V:**
- Pin 6 (threshold) detects this
- Internal comparator triggers
- Flip-flop inside 555 changes state
- **Charging ends, discharging begins**

**Charging time calculation:**
```
T_charge = 0.693 × (R1 + R2) × C

For 200 pF (empty):
T_charge = 0.693 × 110,000 × 200×10⁻¹² = 15.2 µs

For 600 pF (full):
T_charge = 0.693 × 110,000 × 600×10⁻¹² = 45.7 µs

More capacitance = longer charging time!
```

---

### Phase 2: Discharging the Capacitor (Output LOW)

**State change:**
- Capacitor voltage: 3.33V (just reached threshold)
- Pin 3 output: Switches LOW (~0V)
- Pin 7 discharge: CLOSES (shorts to ground)

**What happens:**
```
Current flow path:

Fuel Probe Cap → R2 (100kΩ) → Pin 7 → GND
                       ↓
               (Voltage decreases)
               
R1 is NOT in discharge path!
Only R2 affects discharge time.
```

**The discharging equation:**
```
V(t) = 3.33V × e^(-t/RC)

Where:
  R = R2 only = 100kΩ
  C = fuel probe capacitance
  
Time constant τ = R × C

For 400 pF:
τ = 100,000 × 400×10⁻¹² = 40 microseconds
```

**Discharging proceeds:**
```
t = 0:     V = 3.33V (starting point)
t = τ:     V = 1.22V (37% of start)
t = 0.7τ:  V = 1.67V (TRIGGER REACHED!)
```

**When voltage reaches 1.67V:**
- Pin 2 (trigger) detects this
- Internal comparator triggers
- Flip-flop inside 555 changes state
- **Discharging ends, charging begins again**

**Discharge time calculation:**
```
T_discharge = 0.693 × R2 × C

For 200 pF (empty):
T_discharge = 0.693 × 100,000 × 200×10⁻¹² = 13.9 µs

For 600 pF (full):
T_discharge = 0.693 × 100,000 × 600×10⁻¹² = 41.6 µs

More capacitance = longer discharge time!
```

---

### Complete Cycle

**Total period:**
```
T_total = T_charge + T_discharge

T_total = 0.693 × (R1 + R2) × C + 0.693 × R2 × C
T_total = 0.693 × (R1 + 2×R2) × C
T_total = 0.693 × (10k + 200k) × C
T_total = 0.693 × 210k × C
```

**Frequency:**
```
f = 1 / T_total
f = 1 / [0.693 × (R1 + 2×R2) × C]
f = 1.44 / [(R1 + 2×R2) × C]

This is the standard 555 astable formula!

The magic number 1.44 comes from:
1 / 0.693 ≈ 1.44
```

---

## Visualizing the Waveforms

### Capacitor Voltage (Internal Signal)

```
Voltage at fuel probe:

3.33V ──────────┐             ┐             ┐
       Charge   │  Discharge  │             │
                │     ↓       │             │
                │      ╲      │             │
       ↑        │       ╲     │             │
       │        │        ╲    │             │
1.67V  └────────┘         └───┴─────────────┴───
       
       ◄─ T_charge ─►◄T_discharge►
       
Sawtooth wave oscillating between 1.67V and 3.33V
```

### Output Voltage (Pin 3)

```
5V     ████████████╗           ╔═══════════╗
       ║            ║           ║           ║
       ║  HIGH      ║  LOW      ║           ║
       ║            ║           ║           ║
0V     ╚════════════╝═══════════╝═══════════╝

       ◄─ T_charge ─►◄T_discharge►
       
Square wave output (approximately 55% duty cycle)
```

### Frequency vs Capacitance

```
Frequency (kHz)
    70 │ ●  ← Empty tank (low capacitance)
       │   ╲
    60 │     ●
       │      ╲
    50 │        ●
       │          ╲
    40 │            ●
       │              ╲
    30 │                ●
       │                  ╲
    20 │                    ●
       │                      ╲
    10 │                        ● ← Full tank (high capacitance)
       └────────────────────────────────
       150   250   350   450   550   650
                Capacitance (pF)

Inverse relationship: More capacitance = lower frequency
```

---

## Why This Design Works for Fuel Measurement

### 1. Linear Response
```
The frequency is inversely proportional to capacitance:
f = 1.44 / [(R1 + 2×R2) × C]

If C doubles, f halves
If C increases 10%, f decreases 10%

Linear relationship = easy calibration!
```

### 2. Wide Dynamic Range
```
Our circuit handles:
  Minimum: ~50 pF
  Maximum: ~5000 pF
  
100:1 range! Far exceeds fuel probe range of 150-650 pF
```

### 3. High Resolution
```
For 450 pF range (empty to full):
Frequency change: ~23 kHz

Arduino measures this every second
Can detect 1 Hz change = 0.02 pF resolution

Physical sensor resolution: ~5 pF
This circuit: 100× better than sensor!
```

### 4. Noise Immunity
```
Frequency-based measurement is inherently noise-resistant:

Voltage noise: ±100mV doesn't affect frequency much
Timing noise: Averaged over thousands of cycles
EMI: Filtered by pin 5 bypass cap

Much better than measuring voltage directly!
```

### 5. Simple Interface
```
Output: Clean digital square wave
Arduino: Just count pulses!
No ADC needed
No precision voltage measurement
Just frequency counting = simple and reliable
```

---

## Component Tolerances and Their Effects

### Resistor Tolerance (±5%)

**Effect on frequency:**
```
R1 = 10kΩ ±5% = 9.5k to 10.5k
R2 = 100kΩ ±5% = 95k to 105k

Worst case:
  Low resistors: f = 1.44/[(9.5k + 190k) × C] = higher freq
  High resistors: f = 1.44/[(10.5k + 210k) × C] = lower freq
  
Total variation: ±5% frequency shift

Impact: NONE! We calibrate empty and full points
The calibration absorbs all component tolerances!
```

### Capacitor Tolerance

**The fuel probe capacitance varies anyway!**
```
Fuel type affects permittivity:
  - 100LL Avgas: ε ≈ 1.9
  - Jet-A: ε ≈ 2.1
  - Mogas: ε ≈ 2.0
  
Temperature affects capacitance:
  - Cold fuel: slightly higher ε
  - Hot fuel: slightly lower ε
  
This is why we calibrate with actual fuel in YOUR tanks!
The calibration compensates for everything.
```

---

## What Could Go Wrong?

### Problem 1: No Oscillation

**Symptoms:** No output from pin 3, or stuck high/low

**Causes:**
```
1. No power to 555 (check pin 8)
2. Reset pin (4) not connected to +5V
3. Capacitor open circuit or too small
4. Resistor open circuit
5. Bad 555 chip
```

**Diagnosis:**
```
Check with multimeter:
  - Pin 8: Should be +5V
  - Pin 4: Should be +5V  
  - Pin 1: Should be 0V (ground)
  - Pin 3: Should toggle (measure with scope)
```

### Problem 2: Wrong Frequency Range

**Symptoms:** All frequencies too high or too low

**Causes:**
```
1. Wrong resistor values (check color codes!)
2. Extra capacitance in circuit (bad layout)
3. Fuel probe disconnected or shorted
```

**Diagnosis:**
```
Measure frequency with known capacitor:
  - Remove fuel probe
  - Connect 220 pF test capacitor
  - Should read ~32 kHz
  - If not, check R1 and R2 values
```

### Problem 3: Unstable Frequency

**Symptoms:** Frequency jumps around, noisy

**Causes:**
```
1. Missing pin 5 bypass capacitor
2. Poor ground connection
3. EMI pickup on fuel probe wire
4. Long unshielded wires
```

**Diagnosis:**
```
1. Verify 0.01µF cap on pin 5 to ground
2. Check ground continuity
3. Move probe wire away from ignition
4. Add shielding to probe wire
```

---

## Advanced: Inside the 555

**What's actually happening inside the chip:**

### Internal Block Diagram

```
    Pin 8 (+5V)
        │
    ┌───┴────────────────────┐
    │                        │
    │   Voltage Divider      │
    │   ┌─────────┐         │
    │   │  5kΩ    │         │
    │   ├─────────┤ 3.33V ──┼── Comparator 1 (Threshold)
    │   │  5kΩ    │         │         │
    │   ├─────────┤ 1.67V ──┼── Comparator 2 (Trigger)
    │   │  5kΩ    │         │         │
    │   └─────────┘         │         │
    │                       │         │
    │   Flip-Flop ◄─────────┴─────────┘
    │      │                          │
    │      │                          │
    │      └──► Output Driver ────────┼── Pin 3 (Output)
    │      │                          │
    │      └──► Discharge Transistor ─┼── Pin 7 (Discharge)
    │                                  │
    └───────────────────────────────┘
        │
    Pin 1 (GND)
```

**How it works:**
1. Voltage divider creates 3.33V and 1.67V references
2. Comparator 1 watches capacitor voltage vs 3.33V
3. Comparator 2 watches capacitor voltage vs 1.67V
4. Flip-flop toggles when either comparator triggers
5. Output driver creates square wave on pin 3
6. Discharge transistor shorts pin 7 to ground when flip-flop is reset

---

## Summary: The Complete Story

**What each component does:**

| Component | Primary Function | Why This Value |
|-----------|-----------------|----------------|
| 555 Timer | Creates oscillation | Industry standard, reliable |
| R1 (10kΩ) | Controls charge rate | Sweet spot for our frequency range |
| R2 (100kΩ) | Controls charge/discharge | 10:1 ratio gives ~50% duty cycle |
| Fuel Probe (C) | Variable capacitance sensor | Changes with fuel level |
| 0.01µF bypass | Noise filtering on pin 5 | Low impedance at RF frequencies |
| 1kΩ output | Protection resistor | Limits current if something goes wrong |

**How they work together:**
1. Capacitor charges through R1+R2 until reaching 3.33V
2. 555 detects threshold, switches output LOW, enables discharge
3. Capacitor discharges through R2 until reaching 1.67V
4. 555 detects trigger, switches output HIGH, disables discharge
5. Cycle repeats forever!
6. Frequency depends on capacitance
7. Arduino counts pulses to measure frequency
8. Software converts frequency to voltage output

**The beauty of this design:**
- Simple: Only 6 external components
- Reliable: Proven design from 1972
- Self-contained: No external timing reference needed
- Tolerant: Component variations absorbed by calibration
- Robust: Works in harsh aircraft environment

This is why the 555 timer has been used for fuel senders for decades!
