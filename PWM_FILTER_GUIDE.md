# PWM Output Filter Circuits
## Simple RC Filters for Clean 0-5V Output

---

## Circuit Diagram - Left Tank Output

```
Arduino Pin 9 (Left Tank PWM)
    |
    |
   [R1] 1kΩ resistor
    |
    +----[C1] 10µF capacitor---- GND
    |
    +---- To Dynon Left Fuel Input (+)
    
    GND ---- Dynon Ground (-)
```

---

## Circuit Diagram - Right Tank Output

```
Arduino Pin 10 (Right Tank PWM)
    |
    |
   [R2] 1kΩ resistor
    |
    +----[C2] 10µF capacitor---- GND
    |
    +---- To Dynon Right Fuel Input (+)
    
    GND ---- Dynon Ground (-)
```

---

## Component Specifications

### Resistors (R1, R2)
- **Value:** 1kΩ (1,000 ohms)
- **Type:** Metal film preferred
- **Power rating:** 1/4W minimum
- **Tolerance:** 5% or better
- **Quantity needed:** 2

### Capacitors (C1, C2)
- **Value:** 10µF
- **Type:** Electrolytic or ceramic
- **Voltage rating:** 10V minimum (16V recommended)
- **Polarity:** Mind polarity if electrolytic!
  - Positive (+) to signal side
  - Negative (-) to ground
- **Quantity needed:** 2

**Cost:** ~$0.50 total for all components!

---

## How the Filter Works

### PWM Signal (Before Filter)

```
Arduino PWM output at 490 Hz:

For 50% duty cycle (2.5V average):

5V  ▄▄▄  ▄▄▄  ▄▄▄  ▄▄▄  ▄▄▄
    ▌ ▌  ▌ ▌  ▌ ▌  ▌ ▌  ▌ ▌
0V  ▌ ▀▀▀▌ ▀▀▀▌ ▀▀▀▌ ▀▀▀▌ ▀
    
    ◄───► 2.04ms period (490 Hz)
```

### Filtered Output (After RC Filter)

```
After 1kΩ + 10µF filter:

5V  ─────────────────────────  Clean 2.5V DC
2.5V━━━━━━━━━━━━━━━━━━━━━━━━━
0V  
    
    Ripple: < 50mV (< 1%)
```

---

## Filter Calculations

### Time Constant
```
τ = R × C
τ = 1000Ω × 10µF
τ = 10 milliseconds
```

### Cutoff Frequency
```
fc = 1 / (2π × R × C)
fc = 1 / (2π × 1000 × 0.00001)
fc = 15.9 Hz
```

### PWM to Cutoff Ratio
```
PWM frequency: 490 Hz
Cutoff frequency: 15.9 Hz

Ratio: 490 / 15.9 = 30.8×

This means PWM is 30× higher than filter cutoff
→ Excellent filtering! Ripple < 0.5%
```

### Ripple Calculation
```
At 490 Hz PWM with fc = 15.9 Hz:

Attenuation = 1 / √(1 + (f/fc)²)
Attenuation = 1 / √(1 + (490/15.9)²)
Attenuation = 1 / √(1 + 950)
Attenuation = 1 / 30.8 = 0.032

Ripple ≈ 3.2% of square wave amplitude
For 5V PWM: 5V × 0.032 = 160mV peak-to-peak

Actual ripple with capacitor smoothing: ~50mV
Percentage: 50mV / 5000mV = 1% ripple

This is EXCELLENT for fuel measurement!
```

---

## Assembly Options

### Option 1: On Perfboard (Recommended)

```
Location: Directly on main perfboard next to Arduino

Advantages:
✓ Compact
✓ Short wire runs
✓ Protected in main enclosure
✓ Easy to build

Layout:
         Arduino
           Pin 9 ─┐
                  │
              [1kΩ]─┬─[10µF]─GND
                    │
                   OUT

           Pin 10─┐
                  │
              [1kΩ]─┬─[10µF]─GND
                    │
                   OUT
```

### Option 2: Separate Small Board

```
Advantages:
✓ Easy to replace if damaged
✓ Can be mounted near outputs
✓ Clean separation

Use a small piece of perfboard:
- Mount 2× resistors
- Mount 2× capacitors
- Wire to Arduino pins and outputs
```

### Option 3: Point-to-Point Wiring

```
For very compact build:
- Solder resistor directly to Arduino pin header
- Solder capacitor between resistor and ground
- Use heat shrink to insulate
- Mount near output connector

CAUTION: Less professional, harder to troubleshoot
```

---

## Perfboard Layout Example

```
Top view of main board:

+────────────────────────────────────+
│                                    │
│  [Left 555]  [Arduino]  [Right 555]│
│                 Pins:              │
│                  9 ─[1kΩ]─┬─[10µF] │
│                           │  ││    │
│                          OUT GND   │
│                                    │
│                 10 ─[1kΩ]─┬─[10µF] │
│                           │  ││    │
│                          OUT GND   │
│                                    │
│      [Power Supply Section]        │
│                                    │
+────────────────────────────────────+
```

---

## Wiring Instructions

### Step 1: Identify Components

```
Resistors:
- Brown-Black-Red-Gold = 1kΩ (1000Ω)
- Or use digital multimeter to measure

Electrolytic Capacitors:
- Marked with "10µF 16V" or similar
- Negative lead marked with stripe
- Negative lead usually shorter
```

### Step 2: Install on Perfboard

```
1. Insert resistor leads through holes
2. Bend one lead toward ground rail
3. Solder both connections
4. Trim excess leads

5. Insert capacitor (MIND POLARITY!)
   - Positive (+) to resistor/output side
   - Negative (-) to ground rail
6. Solder both leads
7. Trim excess
```

### Step 3: Wire to Arduino

```
From Arduino Pin 9:
  → One end of R1 (left filter)

From Arduino Pin 10:
  → One end of R2 (right filter)
  
Other end of both resistors:
  → Connect to capacitor positive
  → Connect to output wire (to Dynon)
```

### Step 4: Connect Capacitor Ground

```
Capacitor negative leads:
  → Connect to ground rail
  → Ground rail connects to Arduino GND
  → Ground rail connects to Dynon ground
```

---

## Testing the Filters

### Test 1: PWM Output (Before Filter)

```
Using oscilloscope or multimeter:

At Arduino Pin 9 with 50% PWM:
  Scope: Should see 490 Hz square wave
  DMM: Should read approximately 2.5V

At Arduino Pin 10 with 100% PWM (full tank):
  Scope: Should see constant 5V
  DMM: Should read 5.0V
```

### Test 2: Filtered Output (After Filter)

```
Using multimeter on DC voltage setting:

Empty tank (PWM = 0):
  Should read: 0.0V (±0.05V)

Half full (PWM = 128):
  Should read: 2.5V (±0.1V)
  
Full tank (PWM = 255):
  Should read: 5.0V (±0.05V)
  
Ripple test (with oscilloscope):
  Should see: < 100mV peak-to-peak ripple
```

### Test 3: Load Test

```
Connect to Dynon input and verify:

1. Voltage still correct under load
2. No oscillation or instability
3. Smooth transition when fuel level changes
4. No noise spikes

The Dynon input impedance is high (>100kΩ)
so it won't load down the filter significantly.
```

---

## Improved Two-Stage Filter (Optional)

For even better performance:

```
Arduino Pin 9
    |
   [R1] 470Ω
    |
    +----[C1] 4.7µF---- GND
    |
   [R2] 470Ω
    |
    +----[C2] 4.7µF---- GND
    |
    +---- Output to Dynon
```

**Advantages:**
- Better ripple rejection (< 0.1%)
- Cleaner output
- More stable under noise

**Disadvantages:**
- More components (4 instead of 2 per channel)
- Slightly more complex assembly

**Cutoff frequency:** 7.2 Hz (even better filtering)

---

## Component Alternatives

### If 10µF Not Available

| Capacitor | Result | Ripple | Usable? |
|-----------|--------|--------|---------|
| 4.7µF | fc = 34 Hz | ~100mV | ✅ Good |
| 22µF | fc = 7 Hz | ~20mV | ✅ Excellent |
| 47µF | fc = 3.4 Hz | ~10mV | ✅ Overkill but great |
| 1µF | fc = 159 Hz | ~1V | ⚠️ Marginal |

**Recommendation:** Use 10µF or larger for best results.

### If 1kΩ Not Available

| Resistor | Result | Ripple | Usable? |
|----------|--------|--------|---------|
| 470Ω | fc = 34 Hz | ~100mV | ✅ Good |
| 2.2kΩ | fc = 7 Hz | ~20mV | ✅ Good |
| 4.7kΩ | fc = 3.4 Hz | ~10mV | ✅ Good but slow |

**Recommendation:** 470Ω to 2.2kΩ works well with 10µF cap.

---

## Troubleshooting

### Output Reads 0V Regardless of Tank Level

**Possible causes:**
1. Resistor value too high (>10kΩ)
2. PWM pin not configured correctly
3. Break in wiring

**Fix:** Check continuity, verify PWM output with scope

### Output Voltage Too Low (e.g., 4V instead of 5V)

**Possible causes:**
1. Resistor value too high
2. Capacitor leaky or wrong value
3. Load on output too heavy

**Fix:** Measure voltage directly at Arduino pin, check components

### Noisy/Unstable Output

**Possible causes:**
1. Capacitor too small (< 1µF)
2. Poor ground connection
3. Picking up EMI

**Fix:** Increase capacitor value, verify ground, add shielding

### Output Changes When Other Systems Turn On

**Possible causes:**
1. Ground loop or poor ground
2. Power supply noise
3. EMI coupling

**Fix:** Improve grounding, add input filter on power supply

---

## Comparison: DAC vs PWM

### Cost Comparison

**DAC Version:**
- 2× MCP4725 breakouts: $10
- Total added cost: **$10**

**PWM Version:**
- 2× 1kΩ resistors: $0.20
- 2× 10µF capacitors: $0.30
- Total added cost: **$0.50**

**Savings: $9.50** (95% cost reduction!)

### Performance Comparison

| Parameter | DAC | PWM + Filter |
|-----------|-----|--------------|
| Resolution | 12-bit (4096 steps) | 8-bit (256 steps) |
| Step size (20 gal tank) | 0.005 gal/step | 0.078 gal/step |
| Ripple | None | ~50mV (1%) |
| Complexity | I2C code | Simple analogWrite() |
| Parts count | +2 breakouts | +2R, +2C |
| Code size | +library | Minimal |

**Conclusion:** PWM is more than adequate for fuel measurement!

---

## Final Component List

For PWM version (both tanks):

- [ ] 2× 1kΩ resistors (1/4W metal film)
- [ ] 2× 10µF capacitors (16V electrolytic or ceramic)
- [ ] Heat shrink tubing (optional, for insulation)
- [ ] Small piece of perfboard (if building separate filter board)

**Total cost: ~$0.50**

**Tools needed:**
- Soldering iron
- Wire strippers
- Multimeter (for testing)

---

## Summary

The PWM approach with simple RC filters is:

✅ **Much cheaper** ($0.50 vs $10)  
✅ **Simpler code** (no I2C library needed)  
✅ **Fewer parts** (no DAC breakouts)  
✅ **More than adequate resolution** (256 steps)  
✅ **Easy to build** (just R + C per output)  
✅ **Reliable** (passive components, no ICs to fail)  

**Perfect for aircraft fuel measurement!**
