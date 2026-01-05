# Assembly Guide - Dual Tank 555 Timer Fuel Sender
## Best Practices for Building a Reliable Aircraft System

---

## Three Assembly Options

I'll present three different assembly methods, from easiest to most professional:

### Option 1: Breadboard Prototype (Testing Only)
**Difficulty:** Easy  
**Time:** 2-3 hours  
**Cost:** $5 extra  
**Use:** Initial testing and verification  
**NOT suitable for aircraft installation** (vibration will cause failures)

### Option 2: Perfboard/Protoboard Build (Recommended)
**Difficulty:** Moderate  
**Time:** 6-8 hours  
**Cost:** $5-10 extra  
**Use:** Permanent installation  
**Best balance of ease and reliability**

### Option 3: Custom PCB (Professional)
**Difficulty:** Advanced (requires PCB design)  
**Time:** 2 weeks (incl. PCB fabrication)  
**Cost:** $30-50 extra  
**Use:** Maximum reliability  
**Best for multiple builds**

---

## RECOMMENDED: Option 2 - Perfboard Assembly

This is the **best approach** for a one-off aircraft installation. Here's the complete guide:

---

## Tools and Materials Needed

### Essential Tools
- [ ] Soldering iron (25-40W temperature controlled)
- [ ] Solder (60/40 or 63/37 rosin core, 0.031" diameter)
- [ ] Wire strippers (22-18 AWG)
- [ ] Diagonal cutters (flush cut)
- [ ] Needle-nose pliers
- [ ] Multimeter with continuity tester
- [ ] Desoldering pump or wick (for mistakes)
- [ ] Helping hands or PCB holder

### Helpful Tools
- [ ] Magnifying glass or loupe
- [ ] Anti-static wrist strap
- [ ] Heat-shrink tubing kit
- [ ] Wire labels or label maker
- [ ] Hot glue gun (for strain relief)

### Materials
- [ ] Perfboard (recommend: 10cm x 15cm minimum)
- [ ] 22 AWG solid core wire (various colors)
- [ ] IC sockets: 2x 8-pin DIP (for 555 timers)
- [ ] Pin headers (for Arduino Nano)
- [ ] Screw terminals (for external connections)
- [ ] Standoffs and mounting hardware

---

## Physical Layout Strategy

### Component Placement Philosophy

**Key Principle:** Minimize wire lengths, especially for high-frequency signals.

```
Recommended Layout (Top View):

+--------------------------------------------------+
|                                                  |
|  [Left 555]  [Arduino Nano]  [Right 555]        |
|                                                  |
|      [Left DAC]      [Right DAC]                |
|                                                  |
|              [Power Supply]                      |
|  (LM7805, caps, diode)                          |
|                                                  |
|  [Screw Terminals for External Connections]     |
|                                                  |
+--------------------------------------------------+
```

**Why this layout?**
- Arduino in center (shortest I2C runs to both DACs)
- Each 555 close to its corresponding DAC
- Power supply at bottom (distributes to all)
- External connections at edge (easy access)

---

## Step-by-Step Assembly Process

### STEP 1: Prepare the Perfboard

**1.1 Cut to Size**
- Minimum size: 10cm x 15cm (about 4" x 6")
- Larger is fine and easier to work with
- Sand edges smooth

**1.2 Plan Component Placement**
- Use masking tape to temporarily position components
- Mark holes with a fine-tip marker
- Take a photo for reference!

**1.3 Label the Board**
- Use a permanent marker to label key areas:
  - "+5V RAIL" along one edge
  - "GND RAIL" along opposite edge  
  - "LEFT TANK" and "RIGHT TANK" sections
  - "I2C BUS" area

---

### STEP 2: Install IC Sockets and Headers

**Why use sockets?**
- Protects ICs from soldering heat
- Makes replacement easy if component fails
- Professional practice

**2.1 555 Timer Sockets (×2)**
```
Installation:
1. Insert 8-pin DIP socket into perfboard
2. Ensure Pin 1 notch faces consistent direction
3. Bend pins slightly outward to hold socket
4. Solder ONE corner pin first
5. Check alignment - socket should be flat and straight
6. If crooked, reheat and adjust
7. Solder remaining pins
8. Trim excess leads on bottom
```

**2.2 Arduino Nano Headers**
```
Best Method - "Sandwich Technique":
1. Insert pin headers through perfboard
2. Place Arduino Nano on headers (DON'T solder yet!)
3. Arduino holds headers perfectly aligned
4. Flip entire assembly over
5. Solder headers to perfboard
6. Remove Arduino (it's not soldered - just alignment aid)
7. Later, you'll plug Arduino into these headers
```

**2.3 MCP4725 DAC Headers**
- Most MCP4725 breakouts come with headers you solder to them
- Solder headers to DAC breakouts first
- Then plug into perfboard and solder

---

### STEP 3: Create Power Rails

This is **critical** - good power rails make everything else easier.

**3.1 +5V Power Rail**
```
Using Solid Wire:

1. Strip a long piece of 22 AWG wire (about 15cm)
2. Bend into shape along one edge of board
3. Solder to every 5th hole or so
4. This becomes your +5V distribution bus
5. Mark with RED tape or paint

Alternative - Tinned Copper Bus Wire:
- Use bare solid copper wire (18-20 AWG)
- Tin entire length with solder first
- Forms a solid power bus
- Very professional appearance
```

**3.2 Ground Rail**
```
Same process as +5V rail:
1. Use BLACK wire
2. Run along opposite edge from +5V
3. Solder to board at regular intervals
4. Mark with BLACK tape
5. Make it thick and robust (ground is critical!)
```

**3.3 Power Rail Connections**
```
Connect to rails using short jumpers:
- Component needs +5V → solder short wire to +5V rail
- Component needs GND → solder short wire to GND rail

This keeps the board organized and clean!
```

---

### STEP 4: Build Power Supply Section

**Build order:** Bottom of perfboard, isolated area

**4.1 Install LM7805**
```
1. Bend LM7805 leads to fit 0.1" spacing if needed
2. Insert through board (heatsink tab UP)
3. Solder IN, GND, OUT pins
4. Attach small heatsink with thermal paste
5. Or bend tab flat against board for heat dissipation
```

**4.2 Install Input Protection**
```
D1 (1N4001 Diode):
- Cathode (banded end) toward LM7805 input
- Anode toward aircraft power input
- Solder in place

Input Capacitor (10µF electrolytic):
- Negative lead to ground rail
- Positive lead to LM7805 input
- Mind polarity! (stripe = negative)
```

**4.3 Install Output Filter**
```
Output Capacitor (0.1µF ceramic):
- Between LM7805 output and ground
- Polarity doesn't matter (ceramic)
- Keep leads SHORT
```

**4.4 Connect to Power Rails**
```
LM7805 Output → +5V Rail (use thick wire)
LM7805 Ground → GND Rail (use thick wire)
```

---

### STEP 5: Build Left 555 Timer Circuit

**5.1 Insert 555 IC Socket** (already done in Step 2)

**5.2 Install Resistors**
```
R1 (10kΩ) - High side:
1. One lead to +5V rail
2. Other lead to "junction point"
3. Solder both ends
4. Trim excess

R2 (100kΩ) - Low side:
1. One lead to junction point (same hole as R1)
2. Other lead to GND rail
3. Solder both ends
4. Trim excess

Junction Point:
- This is where R1, R2, and 555 pins 2/6/7 connect
- Use a pad on the perfboard
- All connections meet here
```

**5.3 Connect 555 Power**
```
Pin 8 (Vcc) → +5V rail (short wire)
Pin 4 (Reset) → +5V rail (can share same rail connection)
Pin 1 (GND) → GND rail (short wire)
```

**5.4 Connect 555 Timing Pins**
```
Pin 2 (Trigger) → Junction point (wire or trace)
Pin 6 (Threshold) → Junction point (wire or trace)  
Pin 7 (Discharge) → Junction point (wire or trace)

Tip: Use single bare wire bent to connect all three
```

**5.5 Install Pin 5 Bypass Capacitor**
```
0.01µF ceramic cap:
- One lead to Pin 5 of 555
- Other lead to GND rail
- Keep leads SHORT to minimize noise pickup
```

**5.6 Connect Output**
```
1kΩ resistor in series:
- 555 Pin 3 → 1kΩ resistor → Arduino Pin 2 header
- Provides protection and impedance matching
```

---

### STEP 6: Build Right 555 Timer Circuit

**Repeat Step 5 exactly**, but:
- Use R3 (10kΩ) and R4 (100kΩ) labels
- Output goes to Arduino Pin 3 instead of Pin 2
- Everything else identical

---

### STEP 7: Install Arduino Headers

**Already done in Step 2!**

Just verify connections needed:
```
Vin → +5V rail
GND → GND rail
Pin 2 → Left 555 output (via 1kΩ)
Pin 3 → Right 555 output (via 1kΩ)
Pins 4, 5, 6 → Will connect to external buttons
A4 → I2C SDA bus
A5 → I2C SCL bus
Pin 13 → External LED (via 330Ω resistor)
```

---

### STEP 8: Create I2C Bus

**8.1 Run I2C Traces**
```
SDA Bus (Blue wire):
Arduino A4 → Left DAC SDA → Right DAC SDA
- Use single continuous wire OR
- Solder pads together in line

SCL Bus (White wire):
Arduino A5 → Left DAC SCL → Right DAC SCL
- Same technique as SDA
```

**8.2 Install Pull-up Resistors**
```
R_SDA (10kΩ):
- SDA line → +5V rail

R_SCL (10kΩ):
- SCL line → +5V rail

These are CRITICAL for I2C to work!
Many breakouts have them built-in, but adding
them here doesn't hurt and ensures reliability.
```

---

### STEP 9: Install DAC Breakouts

**9.1 Prepare Right DAC (CRITICAL STEP!)**
```
BEFORE soldering, modify the RIGHT DAC:

1. Locate A0 solder jumper on back of board
2. Use soldering iron to bridge the two pads
3. Verify continuity with multimeter
4. This changes address from 0x62 to 0x63

DO NOT modify the left DAC!
```

**9.2 Install Both DACs**
```
If using sockets (recommended):
1. Insert DAC breakout pin headers into perfboard
2. Solder headers to perfboard
3. DACs can be plugged/unplugged

If soldering directly:
1. Insert DAC breakout
2. Verify orientation (usually has labels)
3. Solder VDD, GND, SDA, SCL pins
4. Solder OUT pin
```

**9.3 Connect DAC Pins**
```
Left DAC:
- VDD → +5V rail
- GND → GND rail
- SDA → SDA bus
- SCL → SCL bus
- OUT → Screw terminal (left output)

Right DAC:
- VDD → +5V rail
- GND → GND rail
- SDA → SDA bus
- SCL → SCL bus
- OUT → Screw terminal (right output)
```

---

### STEP 10: Install External Connection Points

**10.1 Screw Terminals**

Install 2-position screw terminals for:
```
1. Power Input (12V + GND)
2. Left Fuel Probe (Signal + Shield)
3. Right Fuel Probe (Signal + Shield)
4. Left Output to Dynon (Signal + GND)
5. Right Output to Dynon (Signal + GND)
6. Buttons (or use pin headers if buttons mount on board)
```

**10.2 Wire Screw Terminals**
```
Power Input:
- Terminal 1 → LM7805 input (via diode)
- Terminal 2 → GND rail

Left Probe:
- Terminal 1 → Left 555 junction point
- Terminal 2 → GND rail (shield ground)

Right Probe:
- Terminal 1 → Right 555 junction point
- Terminal 2 → GND rail (shield ground)

Left Output:
- Terminal 1 → Left DAC OUT
- Terminal 2 → GND rail

Right Output:
- Terminal 1 → Right DAC OUT
- Terminal 2 → GND rail
```

---

### STEP 11: Install Button Connections

**Options:**

**Option A: Buttons External (Recommended)**
```
Use 2-position screw terminals or pin headers:

Empty Button:
- Terminal 1 → Arduino Pin 4
- Terminal 2 → GND rail

Full Button:
- Terminal 1 → Arduino Pin 5
- Terminal 2 → GND rail

Tank Select Button:
- Terminal 1 → Arduino Pin 6
- Terminal 2 → GND rail

Buttons mount in enclosure, wired to board
```

**Option B: Buttons On-Board**
```
Mount pushbuttons directly on perfboard:
- More compact
- Less flexible for enclosure design
- Harder to access for calibration
```

---

### STEP 12: Install Status LED

**12.1 LED Circuit**
```
330Ω Resistor + LED:

Arduino Pin 13 → 330Ω resistor → LED anode (+)
LED cathode (-) → GND rail

LED orientation:
- Longer lead = anode (+)
- Shorter lead = cathode (-)
- Flat edge on LED body = cathode side
```

**12.2 Mounting Options**
```
Option A: Board-mounted
- LED soldered directly to perfboard
- Must drill hole in enclosure for viewing

Option B: Panel-mounted
- LED in panel mount bezel
- Wires from board to LED
- Professional appearance
```

---

## Critical Soldering Techniques

### Good Solder Joint Characteristics
```
✓ Shiny and smooth (not dull or grainy)
✓ Concave fillet shape (not ball-shaped)
✓ Solder flows onto both pad and component
✓ No "cold joints" (lumpy, crystalline appearance)
✓ No solder bridges between adjacent pads
```

### Soldering Process (Each Joint)
```
1. Clean tip on wet sponge
2. Tin tip with fresh solder
3. Heat BOTH pad and component lead (2-3 seconds)
4. Apply solder to joint (NOT to iron tip)
5. Solder should flow smoothly onto joint
6. Remove solder first, then iron
7. Don't disturb joint while cooling (2-3 seconds)
8. Inspect joint with magnifier
```

### Common Mistakes to Avoid
```
❌ Too much solder (blobs and bridges)
❌ Too little solder (weak connection)
❌ Overheating (lifts pads from board)
❌ Cold solder joint (didn't heat enough)
❌ Disturbing joint while cooling
❌ Dirty or oxidized tip
```

---

## Assembly Order Summary

**Do steps in this order for best results:**

1. ✓ Install sockets and headers
2. ✓ Create power rails
3. ✓ Build power supply
4. ✓ Build left 555 circuit
5. ✓ Build right 555 circuit
6. ✓ Create I2C bus
7. ✓ **Modify right DAC (bridge A0)**
8. ✓ Install DAC breakouts
9. ✓ Install screw terminals
10. ✓ Install button connections
11. ✓ Install status LED
12. ✓ Visual inspection
13. ✓ Continuity testing
14. ✓ Power-up testing

---

## Testing Before Final Assembly

### Visual Inspection
- [ ] No solder bridges between pads
- [ ] All joints shiny and smooth
- [ ] No cold solder joints
- [ ] No lifted pads
- [ ] All component orientations correct
- [ ] ICs not installed yet (test sockets empty)

### Continuity Testing
```
Using Multimeter in Continuity Mode:

Power Rails:
✓ +5V rail has continuity along entire length
✓ GND rail has continuity along entire length
✓ NO continuity between +5V and GND (very important!)

IC Sockets:
✓ Pin 8 → +5V rail
✓ Pin 1 → GND rail
✓ Other pins isolated (no shorts)

I2C Bus:
✓ Both DAC SDA pins connected together
✓ Both DAC SCL pins connected together
✓ Pull-up resistors present

External Connections:
✓ Verify each screw terminal connection
✓ Check for shorts to adjacent terminals
```

### Power-On Test (No ICs Installed!)
```
1. DO NOT install 555 ICs or Arduino yet
2. Apply 12V to power input
3. Measure voltage at LM7805 output: should be 5.0V ±0.1V
4. Measure voltage at +5V rail: should be 5.0V ±0.1V
5. Check for any hot components (sign of short)
6. If all good, power off and install ICs
```

---

## Strain Relief and Mechanical Security

**Critical for aircraft - vibration WILL cause failures!**

### Wire Strain Relief
```
Method 1 - Hot Glue:
- Dab of hot glue where wire enters board
- Don't use too much (makes repair impossible)
- Good for low-stress wires

Method 2 - Zip Ties:
- Mount zip-tie anchor points on board
- Bundle wires together
- Secure to board before entering connections

Method 3 - Cord Grips:
- Use proper cord grips in enclosure
- Best for external connections
```

### Component Security
```
High-vibration components:
- Add dab of hot glue to base of electrolytic caps
- Secure heavy components (transformers, relays)
- Don't use hot glue on ICs (heat buildup)

Perfboard to enclosure:
- Use standoffs with lock washers
- Mount at 3-4 points minimum
- Don't overtighten (can crack board)
```

---

## Enclosure Mounting

### Enclosure Selection
- Aluminum enclosure (EMI shielding)
- Size: ~15cm x 20cm x 5cm (6" x 8" x 2")
- Ventilation holes (components generate heat)

### Board Mounting
```
1. Use metal or nylon standoffs (6-10mm height)
2. Secure with machine screws and lock washers
3. Ensure board doesn't flex when connectors used
4. Ground enclosure to aircraft ground
```

### Panel Cutouts
```
Required holes:
- Power input connector
- 2× Fuel probe connectors
- 2× Output connectors to Dynon
- 3× Pushbuttons (or 1× multi-button assembly)
- 1× Status LED
- 1× USB port (for programming/debug) - optional
```

---

## Final Assembly Checklist

### Before Closing Enclosure
- [ ] All components installed and soldered
- [ ] Visual inspection passed
- [ ] Continuity test passed  
- [ ] Power supply test passed (5V output)
- [ ] ICs installed in correct orientation
- [ ] I2C scanner shows 0x62 and 0x63
- [ ] All external connections tested
- [ ] Buttons function correctly
- [ ] LED blinks at startup
- [ ] Strain relief applied
- [ ] Wiring neat and organized
- [ ] All connections labeled
- [ ] Photos taken for documentation

### First Power-Up (Full System)
- [ ] Connect to aircraft 12V via fuse
- [ ] Check 5V regulator output
- [ ] Upload Arduino code via USB
- [ ] Open Serial Monitor
- [ ] Verify both DACs detected
- [ ] Test frequency inputs (should see ~50kHz)
- [ ] Test button functions
- [ ] Calibrate one tank (empty/full)
- [ ] Verify output voltage changes
- [ ] Document all values

---

## Troubleshooting Assembly Issues

### No Power (No 5V Output)
```
Check:
1. Input voltage present at LM7805
2. Diode installed correct direction
3. LM7805 installed correctly (pins in right order)
4. No short between input and ground
5. LM7805 not damaged (measure pin voltages)
```

### I2C Devices Not Detected
```
Check:
1. Pull-up resistors installed (10kΩ to +5V)
2. SDA/SCL not swapped
3. DACs receiving power (measure VDD pins)
4. No solder bridges on DAC breakouts
5. Right DAC A0 properly bridged
```

### 555 Not Oscillating
```
Check:
1. Pin 8 has +5V
2. Pin 1 connected to ground
3. Pin 4 connected to +5V (reset must be high)
4. R1 and R2 values correct
5. Junction point connections good
6. Pin 5 bypass cap installed
```

### Poor Solder Joints
```
Fix by:
1. Reheat joint with fresh flux
2. Add small amount of solder if needed
3. If too much solder, use desoldering wick
4. Practice on scrap board first!
```

---

## Alternative: Breadboard for Initial Testing

**Use only for proof-of-concept, NOT for flight!**

### Advantages
- No soldering required
- Easy to modify
- Quick to build
- Good for learning

### Disadvantages
- Unreliable connections (vibration)
- NOT suitable for aircraft
- Limited current capacity
- Can't be enclosed easily

### If You Build on Breadboard
```
Testing only - plan to rebuild on perfboard!

1. Build one side first (single 555 + DAC)
2. Verify it works completely
3. Then add second side
4. Use for bench testing and code development
5. MUST transfer to perfboard for aircraft use
```

---

## Pro Tips for Success

1. **Take Photos** - Before each major step, photograph your work
2. **Test Incrementally** - Don't build everything then test
3. **Use Flux** - Makes soldering much easier
4. **Keep It Clean** - Wipe rosin residue with isopropyl alcohol  
5. **Double-Check Polarity** - Caps, diodes, LEDs all have polarity
6. **Measure Twice, Cut Once** - Especially for wire lengths
7. **Label Everything** - Future-you will thank you
8. **Save Mistakes** - Bad solder joints teach you what not to do
9. **Take Breaks** - Tired = mistakes
10. **Ask for Help** - Post photos on forums if stuck

---

## Time Estimates

**Perfboard Assembly** (first-time builder):
- Component placement and planning: 1 hour
- Power supply section: 1 hour  
- 555 circuits (both): 2 hours
- DACs and I2C bus: 1 hour
- External connections: 1 hour
- Testing and debugging: 2 hours
- **Total: 8-10 hours**

**Experienced builder:** 4-6 hours

**Don't rush!** This goes in an aircraft. Quality matters more than speed.

---

## Conclusion

The perfboard assembly method offers the best balance of:
- **Reliability** - Proper solder joints won't fail
- **Flexibility** - Easy to modify if needed
- **Cost** - Much cheaper than custom PCB
- **Time** - Faster than waiting for PCB fabrication

Follow this guide carefully, test thoroughly, and you'll have a professional-quality fuel sender that will serve you reliably for years to come.

**Remember:** This system monitors your aircraft's fuel. Build it right!
