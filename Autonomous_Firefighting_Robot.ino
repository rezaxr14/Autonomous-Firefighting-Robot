// Autonomous Firefighting Robot
// Created by [Reza Nadimi]
// Version 2.0 - Optimized and Enhanced

// 📌 PIN DEFINITIONS
// L298N Motor Driver Pins
#define ENA_MOTOR_A 10 // Enable Pin for Motor A (Right Motor) 
#define IN1_MOTOR_A 9  // Direction Pin 1 for Motor A 
#define IN2_MOTOR_A 8  // Direction Pin 2 for Motor A 

#define IN3_MOTOR_B 7  // Direction Pin 1 for Motor B (Left Motor) 
#define IN4_MOTOR_B 6  // Direction Pin 2 for Motor B 
#define ENB_MOTOR_B 5  // Enable Pin for Motor B 

// Infrared (IR) Flame Sensor Pins (Analog)
#define IR_SENSOR_RIGHT A0 // Right-side flame sensor 
#define IR_SENSOR_FRONT A1 // Front-facing flame sensor 
#define IR_SENSOR_LEFT  A2 // Left-side flame sensor 

// Actuator Pins
#define SERVO_PIN A4  // Servo motor for aiming 
#define PUMP_PIN  A5  // Water pump control 

// Button Pins for Manual Control
#define BTN_MOTOR_TEST 2 // Push button to test motors 
#define BTN_SERVO_TEST 3 // Push button to test servo 

// ⚙️ GLOBAL VARIABLES
int motorSpeed = 120; // Set motor speed (0-255) [cite: 3]
int sensorRightVal, sensorFrontVal, sensorLeftVal;
int blueCal, orangeCal, greenCal; // Calibration values for the sensors

// 🚀 FUNCTION PROTOTYPES
void forward(int duration = 0);
void backward(int duration = 0);
void turnRight(int duration = 0);
void turnLeft(int duration = 0);
void stopRobot(int duration = 0);
void servoPulse(int pin, int angle);
int getAverageRead(int pin, int numReadings);

// -----------------------------------------------------------------------------
// 🏁 SETUP: Runs once when the robot is powered on or reset
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(9600); // Start serial communication for debugging 
  
  // Set Pin Modes
  pinMode(IR_SENSOR_RIGHT, INPUT);
  pinMode(IR_SENSOR_FRONT, INPUT);
  pinMode(IR_SENSOR_LEFT,  INPUT);
  pinMode(BTN_MOTOR_TEST,  INPUT);
  pinMode(BTN_SERVO_TEST,  INPUT);
  pinMode(ENA_MOTOR_A, OUTPUT);
  pinMode(IN1_MOTOR_A, OUTPUT);
  pinMode(IN2_MOTOR_A, OUTPUT);
  pinMode(IN3_MOTOR_B, OUTPUT);
  pinMode(IN4_MOTOR_B, OUTPUT);
  pinMode(ENB_MOTOR_B, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);

  // 🧪 CALIBRATION
  // Get initial readings for ambient light
  blueCal  = getAverageRead(IR_SENSOR_RIGHT, 10);
  orangeCal= getAverageRead(IR_SENSOR_FRONT, 10);
  greenCal = getAverageRead(IR_SENSOR_LEFT,  10);
  
  // Set initial motor speed
  analogWrite(ENA_MOTOR_A, motorSpeed);
  analogWrite(ENB_MOTOR_B, motorSpeed);
  
  delay(500); // Wait for components to stabilize
}

// -----------------------------------------------------------------------------
// ♾️ LOOP: The main control loop that runs continuously
// -----------------------------------------------------------------------------
void loop() {
  // Read button states for manual testing
  int servoButtonState = digitalRead(BTN_SERVO_TEST);
  int motorButtonState = digitalRead(BTN_MOTOR_TEST);

  // Manual Motor Test
  if (motorButtonState == HIGH) {
    forward(500);
    stopRobot(100);
    backward(500);
    stopRobot(100);
    turnRight(1000);
    stopRobot(100);
    turnLeft(1000);
    stopRobot(100);
  }

  // Manual Servo Test
  if (servoButtonState == HIGH) {
    for (int angle = 90; angle <= 140; angle += 5) {
      servoPulse(SERVO_PIN, angle);
    }
    for (int angle = 140; angle >= 40; angle -= 5) {
      servoPulse(SERVO_PIN, angle);
    }
    for (int angle = 40; angle <= 95; angle += 5) {
      servoPulse(SERVO_PIN, angle);
    }
  }

  // Read sensor values
  sensorRightVal = analogRead(IR_SENSOR_RIGHT);
  sensorFrontVal = analogRead(IR_SENSOR_FRONT);
  sensorLeftVal  = analogRead(IR_SENSOR_LEFT);

  // Debugging output
  Serial.print("Right: ");
  Serial.print(sensorRightVal);
  Serial.print("\tFront: ");
  Serial.print(sensorFrontVal);
  Serial.print("\tLeft: ");
  Serial.println(sensorLeftVal);
  
  delay(10); // Short delay for stable readings

  // 🚒 AUTONOMOUS FIREFIGHTING LOGIC
  
  // 🔥 Fire detected directly in front
  if (sensorFrontVal < 350) {
    Serial.println("🔥 Fire straight ahead! Extinguishing...");
    stopRobot();
    digitalWrite(PUMP_PIN, HIGH);
    // Sweep the servo to spray a wide area
    for(int angle = 90; angle <= 140; angle += 3){
      servoPulse(SERVO_PIN, angle);
    }
    for(int angle = 140; angle >= 40; angle -= 3){
      servoPulse(SERVO_PIN, angle);
    }
    for(int angle = 40; angle <= 90; angle += 3){
      servoPulse(SERVO_PIN, angle);
    }
  } 
  // 🔥 Fire detected to the right
  else if (sensorRightVal < 250) {
    Serial.println("🔥 Fire to the right! Extinguishing...");
    stopRobot();
    digitalWrite(PUMP_PIN, HIGH);
    // Sweep the servo to the right
    for(int angle = 90; angle >= 40; angle -= 3){
      servoPulse(SERVO_PIN, angle);
    }
    for(int angle = 40; angle <= 90; angle += 3){
      servoPulse(SERVO_PIN, angle);
    }
  } 
  // 🔥 Fire detected to the left
  else if (sensorLeftVal < 250) {
    Serial.println("🔥 Fire to the left! Extinguishing...");
    stopRobot();
    digitalWrite(PUMP_PIN, HIGH);
    // Sweep the servo to the left
    for(int angle = 90; angle <= 140; angle += 3){
      servoPulse(SERVO_PIN, angle);
    }
    for(int angle = 140; angle >= 90; angle -= 3){
      servoPulse(SERVO_PIN, angle);
    }
  } 
  // 🔍 Searching for fire: Robot maneuvers to find flames
  else if (sensorFrontVal < (orangeCal - 160)) {
    Serial.println("Searching... Found a hint of fire ahead!");
    forward();
    digitalWrite(PUMP_PIN, HIGH);
    delay(200);
  } else if (sensorLeftVal < (greenCal - 200)) {
    Serial.println("Searching... Turning right to investigate!");
    forward();
    delay(100);
    turnRight();
    digitalWrite(PUMP_PIN, HIGH);
    delay(200);
  } else if (sensorRightVal < (blueCal - 150)) {
    Serial.println("Searching... Turning left to investigate!");
    forward();
    delay(100);
    turnLeft();
    digitalWrite(PUMP_PIN, HIGH);
    delay(200);
  } 
  // 😴 No fire detected, robot is idle
  else {
    digitalWrite(PUMP_PIN, LOW);
    stopRobot();
  }
  
  delay(50); // Small delay to prevent rapid state changes
}

// -----------------------------------------------------------------------------
// 🤖 HELPER FUNCTIONS FOR MOTOR AND SERVO CONTROL
// -----------------------------------------------------------------------------
void servoPulse(int pin, int angle) {
  int pwm = (angle * 11) + 500; // Convert angle to pulse width (microseconds) 
  digitalWrite(pin, HIGH);
  delayMicroseconds(pwm);
  digitalWrite(pin, LOW);
  delay(50); // Refresh cycle for the servo 
}

void forward(int duration = 0) {
  digitalWrite(IN1_MOTOR_A, HIGH);
  digitalWrite(IN2_MOTOR_A, LOW);
  digitalWrite(IN3_MOTOR_B, HIGH);
  digitalWrite(IN4_MOTOR_B, LOW);
  delay(duration);
}

void backward(int duration = 0) {
  digitalWrite(IN1_MOTOR_A, LOW);
  digitalWrite(IN2_MOTOR_A, HIGH);
  digitalWrite(IN3_MOTOR_B, LOW);
  digitalWrite(IN4_MOTOR_B, HIGH);
  delay(duration);
}

void turnRight(int duration = 0) {
  digitalWrite(IN1_MOTOR_A, HIGH);
  digitalWrite(IN2_MOTOR_A, LOW);
  digitalWrite(IN3_MOTOR_B, LOW);
  digitalWrite(IN4_MOTOR_B, HIGH);
  delay(duration);
}

void turnLeft(int duration = 0) {
  digitalWrite(IN1_MOTOR_A, LOW);
  digitalWrite(IN2_MOTOR_A, HIGH);
  digitalWrite(IN3_MOTOR_B, HIGH);
  digitalWrite(IN4_MOTOR_B, LOW);
  delay(duration);
}

void stopRobot(int duration = 0) {
  digitalWrite(IN1_MOTOR_A, LOW);
  digitalWrite(IN2_MOTOR_A, LOW);
  digitalWrite(IN3_MOTOR_B, LOW);
  digitalWrite(IN4_MOTOR_B, LOW);
  delay(duration);
}

int getAverageRead(int pin, int numReadings) {
  int total = 0;
  for (int i = 0; i < numReadings; i++) {
    total += analogRead(pin);
  }
  return total / numReadings;
}