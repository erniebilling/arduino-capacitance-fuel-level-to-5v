/*
 * 555 Timer Fuel Level Sender - DUAL TANK (PWM Version)
 * For RV-7 Aircraft with Capacitive Fuel Probes
 * 
 * Direct frequency-to-voltage conversion using Arduino PWM outputs
 * NO external DACs needed - uses built-in PWM with low-pass filter
 * 
 * Hardware:
 * - Arduino Nano
 * - Two 555 timer circuits (one per tank, fuel probe as timing capacitor)
 * - Two simple RC filters (1kΩ + 10µF per output)
 * - Three pushbuttons for calibration
 * - Status LED
 * 
 * PWM Outputs:
 * - Pin 9: Left tank (0-5V via PWM + filter)
 * - Pin 10: Right tank (0-5V via PWM + filter)
 * 
 * Calibration (for each tank):
 * 1. Press TANK SELECT button to choose LEFT or RIGHT
 * 2. Drain selected tank completely
 * 3. Press and hold "EMPTY" button for 2 seconds (sets 0V point)
 * 4. Fill selected tank to capacity
 * 5. Press and hold "FULL" button for 2 seconds (sets 5V point)
 * 6. Repeat for other tank
 * 
 * The system simply maps:
 *   Empty tank frequency → 0V output (PWM = 0)
 *   Full tank frequency → 5V output (PWM = 255)
 *   Everything in between is linear interpolation
 * 
 * Author: Custom design for RV-7
 * Date: 2026
 */

#include <EEPROM.h>

// Pin definitions
#define LEFT_FREQ_PIN 2         // Left 555 timer output (interrupt 0)
#define RIGHT_FREQ_PIN 3        // Right 555 timer output (interrupt 1)
#define EMPTY_BUTTON_PIN 4      // Button to set empty calibration
#define FULL_BUTTON_PIN 5       // Button to set full calibration (NOT PWM capable but we're using as input)
#define TANK_SELECT_PIN 6       // Button to select which tank to calibrate (NOT PWM capable but we're using as input)
#define LEFT_PWM_PIN 9          // Left tank PWM output (490 Hz)
#define RIGHT_PWM_PIN 10        // Right tank PWM output (490 Hz)
#define STATUS_LED_PIN 13       // Built-in LED for status indication

// Frequency measurement - dual tanks
volatile unsigned long leftPulseCount = 0;
volatile unsigned long rightPulseCount = 0;
unsigned long lastMeasureTime = 0;
float leftFrequency = 0;
float rightFrequency = 0;

// Calibration values - separate for each tank
// Just frequency to voltage mapping, no fuel capacity needed
struct TankCalibration {
  unsigned long emptyFrequency;  // Frequency when tank empty (0V output)
  unsigned long fullFrequency;   // Frequency when tank full (5V output)
};

TankCalibration leftTank = {50000, 20000};   // Defaults for left
TankCalibration rightTank = {50000, 20000};  // Defaults for right

// Tank selection for calibration
enum SelectedTank { LEFT_TANK, RIGHT_TANK };
SelectedTank selectedTank = LEFT_TANK;

// EEPROM addresses for dual tank storage
#define EEPROM_MAGIC_ADDR 0
#define EEPROM_LEFT_EMPTY_ADDR 4
#define EEPROM_LEFT_FULL_ADDR 8
#define EEPROM_RIGHT_EMPTY_ADDR 12
#define EEPROM_RIGHT_FULL_ADDR 16
#define EEPROM_MAGIC_VALUE 0xCAFE  // To verify EEPROM has been initialized

// Filtering - separate for each tank
#define NUM_SAMPLES 10
float leftFreqSamples[NUM_SAMPLES];
float rightFreqSamples[NUM_SAMPLES];
int sampleIndex = 0;

// Button debounce
unsigned long lastEmptyPress = 0;
unsigned long lastFullPress = 0;
unsigned long lastTankSelectPress = 0;
#define BUTTON_HOLD_TIME 2000    // 2 seconds to confirm calibration
#define DEBOUNCE_TIME 50

// Update rate
#define MEASURE_INTERVAL 1000    // Measure frequency every 1 second
#define OUTPUT_INTERVAL 200      // Update PWM output every 200ms

unsigned long lastOutputUpdate = 0;

void setup() {
  Serial.begin(115200);
  
  Serial.println("555 Timer Fuel Sender - DUAL TANK (PWM)");
  Serial.println("==========================================");
  
  // Setup pins
  pinMode(LEFT_FREQ_PIN, INPUT);
  pinMode(RIGHT_FREQ_PIN, INPUT);
  pinMode(EMPTY_BUTTON_PIN, INPUT_PULLUP);
  pinMode(FULL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(TANK_SELECT_PIN, INPUT_PULLUP);
  pinMode(LEFT_PWM_PIN, OUTPUT);
  pinMode(RIGHT_PWM_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // Initialize PWM outputs to 0
  analogWrite(LEFT_PWM_PIN, 0);
  analogWrite(RIGHT_PWM_PIN, 0);
  
  // Setup interrupts for frequency measurement
  attachInterrupt(digitalPinToInterrupt(LEFT_FREQ_PIN), countLeftPulse, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_FREQ_PIN), countRightPulse, RISING);
  
  // Load calibration from EEPROM
  loadCalibration();
  
  // Initialize sample buffers
  for (int i = 0; i < NUM_SAMPLES; i++) {
    leftFreqSamples[i] = leftTank.emptyFrequency;
    rightFreqSamples[i] = rightTank.emptyFrequency;
  }
  
  Serial.println("\n=== Calibration Values ===");
  Serial.println("LEFT TANK:");
  Serial.print("  Empty frequency: "); Serial.print(leftTank.emptyFrequency); Serial.println(" Hz");
  Serial.print("  Full frequency: "); Serial.print(leftTank.fullFrequency); Serial.println(" Hz");
  Serial.println("RIGHT TANK:");
  Serial.print("  Empty frequency: "); Serial.print(rightTank.emptyFrequency); Serial.println(" Hz");
  Serial.print("  Full frequency: "); Serial.print(rightTank.fullFrequency); Serial.println(" Hz");
  
  Serial.println("\n=== System Ready ===");
  Serial.println("PWM Outputs:");
  Serial.println("  Pin 9: Left tank (0-5V via RC filter)");
  Serial.println("  Pin 10: Right tank (0-5V via RC filter)");
  Serial.println("\nTo calibrate:");
  Serial.println("  1. Press TANK SELECT to choose LEFT or RIGHT");
  Serial.println("  2. Drain selected tank, hold EMPTY button for 2 seconds");
  Serial.println("  3. Fill selected tank, hold FULL button for 2 seconds");
  Serial.println("  4. Repeat for other tank");
  Serial.println();
  
  // Show current selection
  Serial.print("Currently selected: ");
  Serial.println(selectedTank == LEFT_TANK ? "LEFT TANK" : "RIGHT TANK");
  
  // Blink LED to show ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(100);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(100);
  }
}

void loop() {
  // Check tank selection button
  checkTankSelectButton();
  
  // Check calibration buttons
  checkCalibrationButtons();
  
  // Measure frequency every second
  if (millis() - lastMeasureTime >= MEASURE_INTERVAL) {
    lastMeasureTime = millis();
    
    // Calculate frequencies from pulse counts
    noInterrupts();
    unsigned long leftCount = leftPulseCount;
    unsigned long rightCount = rightPulseCount;
    leftPulseCount = 0;
    rightPulseCount = 0;
    interrupts();
    
    leftFrequency = (float)leftCount;   // Hz (counts per second)
    rightFrequency = (float)rightCount; // Hz (counts per second)
    
    // Add to sample buffers for averaging
    leftFreqSamples[sampleIndex] = leftFrequency;
    rightFreqSamples[sampleIndex] = rightFrequency;
    sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
  }
  
  // Update outputs every 200ms
  if (millis() - lastOutputUpdate >= OUTPUT_INTERVAL) {
    lastOutputUpdate = millis();
    
    // Calculate averaged frequencies
    float leftAvgFreq = averageSamples(leftFreqSamples);
    float rightAvgFreq = averageSamples(rightFreqSamples);
    
    // Convert frequency directly to PWM value (0-255)
    uint8_t leftPWM = frequencyToPWM(leftAvgFreq, leftTank);
    uint8_t rightPWM = frequencyToPWM(rightAvgFreq, rightTank);
    
    // Output to PWM pins
    analogWrite(LEFT_PWM_PIN, leftPWM);
    analogWrite(RIGHT_PWM_PIN, rightPWM);
    
    // Calculate equivalent voltage for display
    float leftVoltage = (leftPWM / 255.0) * 5.0;
    float rightVoltage = (rightPWM / 255.0) * 5.0;
    
    // Debug output
    Serial.print("L: "); Serial.print(leftAvgFreq, 0); Serial.print(" Hz → PWM:");
    Serial.print(leftPWM); Serial.print(" ("); Serial.print(leftVoltage, 2); Serial.print("V) | ");
    Serial.print("R: "); Serial.print(rightAvgFreq, 0); Serial.print(" Hz → PWM:");
    Serial.print(rightPWM); Serial.print(" ("); Serial.print(rightVoltage, 2); Serial.println("V)");
  }
}

// Interrupt service routines - count pulses
void countLeftPulse() {
  leftPulseCount++;
}

void countRightPulse() {
  rightPulseCount++;
}

float averageSamples(float* samples) {
  float sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += samples[i];
  }
  return sum / NUM_SAMPLES;
}

uint8_t frequencyToPWM(float frequency, TankCalibration tank) {
  // Higher frequency = less capacitance = empty tank = PWM 0 (0V)
  // Lower frequency = more capacitance = full tank = PWM 255 (5V)
  
  // Constrain to calibrated range
  if (frequency >= tank.emptyFrequency) return 0;
  if (frequency <= tank.fullFrequency) return 255;
  
  // Linear interpolation from frequency to PWM value
  // Note: frequency decreases as fuel increases, so we invert the calculation
  float fraction = (float)(tank.emptyFrequency - frequency) / 
                   (float)(tank.emptyFrequency - tank.fullFrequency);
  
  // fraction = 0.0 (empty) to 1.0 (full)
  // Convert to PWM: 0 (0V) to 255 (5V)
  int pwm = (int)(fraction * 255.0);
  
  // Constrain output
  if (pwm < 0) pwm = 0;
  if (pwm > 255) pwm = 255;
  
  return (uint8_t)pwm;
}

void checkTankSelectButton() {
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(TANK_SELECT_PIN);
  
  // Detect button press (transition from HIGH to LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(DEBOUNCE_TIME);  // Debounce
    
    // Toggle tank selection
    if (selectedTank == LEFT_TANK) {
      selectedTank = RIGHT_TANK;
      Serial.println("\n>>> SELECTED: RIGHT TANK <<<");
    } else {
      selectedTank = LEFT_TANK;
      Serial.println("\n>>> SELECTED: LEFT TANK <<<");
    }
    
    // Blink LED to confirm selection
    for (int i = 0; i < (selectedTank == LEFT_TANK ? 1 : 2); i++) {
      digitalWrite(STATUS_LED_PIN, HIGH);
      delay(150);
      digitalWrite(STATUS_LED_PIN, LOW);
      delay(150);
    }
  }
  
  lastButtonState = currentButtonState;
}

void checkCalibrationButtons() {
  unsigned long now = millis();
  
  // Check EMPTY button
  if (digitalRead(EMPTY_BUTTON_PIN) == LOW) {  // Button pressed (active low)
    if (lastEmptyPress == 0) {
      lastEmptyPress = now;
      Serial.println("\nEMPTY button pressed - hold for 2 seconds to calibrate...");
      digitalWrite(STATUS_LED_PIN, HIGH);
    } else if (now - lastEmptyPress >= BUTTON_HOLD_TIME) {
      // Button held for 2 seconds - calibrate empty
      calibrateEmpty();
      lastEmptyPress = 0;
      digitalWrite(STATUS_LED_PIN, LOW);
      delay(500);  // Debounce
    }
  } else {
    if (lastEmptyPress > 0) {
      Serial.println("EMPTY button released before 2 seconds");
      digitalWrite(STATUS_LED_PIN, LOW);
    }
    lastEmptyPress = 0;
  }
  
  // Check FULL button
  if (digitalRead(FULL_BUTTON_PIN) == LOW) {  // Button pressed (active low)
    if (lastFullPress == 0) {
      lastFullPress = now;
      Serial.println("\nFULL button pressed - hold for 2 seconds to calibrate...");
      digitalWrite(STATUS_LED_PIN, HIGH);
    } else if (now - lastFullPress >= BUTTON_HOLD_TIME) {
      // Button held for 2 seconds - calibrate full
      calibrateFull();
      lastFullPress = 0;
      digitalWrite(STATUS_LED_PIN, LOW);
      delay(500);  // Debounce
    }
  } else {
    if (lastFullPress > 0) {
      Serial.println("FULL button released before 2 seconds");
      digitalWrite(STATUS_LED_PIN, LOW);
    }
    lastFullPress = 0;
  }
}

void calibrateEmpty() {
  // Take current frequency as empty reading for selected tank
  if (selectedTank == LEFT_TANK) {
    float avgFreq = averageSamples(leftFreqSamples);
    leftTank.emptyFrequency = (unsigned long)avgFreq;
    
    Serial.println("\n*** LEFT TANK - EMPTY CALIBRATION SET ***");
    Serial.print("Empty frequency: "); Serial.print(leftTank.emptyFrequency); Serial.println(" Hz");
  } else {
    float avgFreq = averageSamples(rightFreqSamples);
    rightTank.emptyFrequency = (unsigned long)avgFreq;
    
    Serial.println("\n*** RIGHT TANK - EMPTY CALIBRATION SET ***");
    Serial.print("Empty frequency: "); Serial.print(rightTank.emptyFrequency); Serial.println(" Hz");
  }
  
  // Save to EEPROM
  saveCalibration();
  
  // Blink LED 3 times to confirm
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(200);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
}

void calibrateFull() {
  // Take current frequency as full reading for selected tank
  if (selectedTank == LEFT_TANK) {
    float avgFreq = averageSamples(leftFreqSamples);
    leftTank.fullFrequency = (unsigned long)avgFreq;
    
    Serial.println("\n*** LEFT TANK - FULL CALIBRATION SET ***");
    Serial.print("Full frequency: "); Serial.print(leftTank.fullFrequency); Serial.println(" Hz");
  } else {
    float avgFreq = averageSamples(rightFreqSamples);
    rightTank.fullFrequency = (unsigned long)avgFreq;
    
    Serial.println("\n*** RIGHT TANK - FULL CALIBRATION SET ***");
    Serial.print("Full frequency: "); Serial.print(rightTank.fullFrequency); Serial.println(" Hz");
  }
  
  // Save to EEPROM
  saveCalibration();
  
  // Blink LED 5 times to confirm
  for (int i = 0; i < 5; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(200);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
}

void saveCalibration() {
  // Write magic value to indicate EEPROM is initialized
  EEPROM.put(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
  
  // Save left tank calibration (just frequencies)
  EEPROM.put(EEPROM_LEFT_EMPTY_ADDR, leftTank.emptyFrequency);
  EEPROM.put(EEPROM_LEFT_FULL_ADDR, leftTank.fullFrequency);
  
  // Save right tank calibration (just frequencies)
  EEPROM.put(EEPROM_RIGHT_EMPTY_ADDR, rightTank.emptyFrequency);
  EEPROM.put(EEPROM_RIGHT_FULL_ADDR, rightTank.fullFrequency);
  
  Serial.println("Calibration saved to EEPROM");
}

void loadCalibration() {
  uint16_t magic;
  EEPROM.get(EEPROM_MAGIC_ADDR, magic);
  
  if (magic == EEPROM_MAGIC_VALUE) {
    // EEPROM has valid calibration data
    EEPROM.get(EEPROM_LEFT_EMPTY_ADDR, leftTank.emptyFrequency);
    EEPROM.get(EEPROM_LEFT_FULL_ADDR, leftTank.fullFrequency);
    
    EEPROM.get(EEPROM_RIGHT_EMPTY_ADDR, rightTank.emptyFrequency);
    EEPROM.get(EEPROM_RIGHT_FULL_ADDR, rightTank.fullFrequency);
    
    Serial.println("Calibration loaded from EEPROM");
  } else {
    Serial.println("No calibration found - using defaults");
    Serial.println("Please calibrate both tanks:");
    Serial.println("  1. Press TANK SELECT to choose tank");
    Serial.println("  2. Use EMPTY and FULL buttons to calibrate");
  }
}
