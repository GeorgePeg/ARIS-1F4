

#include <CytronMakerSumo.h>

// Edge sensor thresholds
#define EDGE_L_THRESHOLD 764
#define EDGE_R_THRESHOLD 860

// Direction constants
#define LEFT  0
#define RIGHT 1

// IR receiver
#define start_pin 2 

// Global variables
int searchDir = LEFT;


void setup() {
    MakerSumo.begin();
    pinMode(LED, OUTPUT);
    pinMode(start_pin,INPUT);
    digitalWrite(LED, LOW);
    Serial.begin(115200);
    Serial.println("ARIS-1F4 Initializing...");
    // IR receiver initialization
    
    // Battery voltage check
    float battery_Voltage = MakerSumo.readBatteryVoltage();
    Serial.print("Battery Voltage: ");
    Serial.println(battery_Voltage);
    Serial.println("Robot initialized and ready for commands.");
    //Call start routincone

    bool robot_ready_state = false ;

    int initial_start_pin_state=digitalRead(start_pin);

    bool program_button = false;

    if(initial_start_pin_state == 1){

        while(true){



      }
    }else if(initial_start_pin_state == 0){

      bool robot_ready_state = true ;
    }    




 }


void startRoutine(){
   
  // Turn right around 45 degrees.
  MakerSumo.setMotorSpeed(MOTOR_L, 50);
  MakerSumo.setMotorSpeed(MOTOR_R, 0);
  delay(500);
  
  // Go straight.
  MakerSumo.setMotorSpeed(MOTOR_L, 80);
  MakerSumo.setMotorSpeed(MOTOR_R, 80);
  delay(1500);
  
  // Turn left until opponent is detected.
  MakerSumo.setMotorSpeed(MOTOR_L, 0);
  MakerSumo.setMotorSpeed(MOTOR_R, 50);
  
  unsigned long startTimestamp = millis();
  while (digitalRead(OPP_FC)) {
    // Quit if opponent is not found after timeout.
    if (millis() - startTimestamp > 1000) {
      break;
    }
  }
}
// Main robot navigation function
void robotNavigation(){
    // Edge detection
    if (analogRead(EDGE_L) < EDGE_L_THRESHOLD) {
        comeBack(RIGHT);
        searchDir ^= 1;  // Toggle direction
    } else if (analogRead(EDGE_R) < EDGE_R_THRESHOLD) {
        comeBack(LEFT);
        searchDir ^= 1;
    } else {
        // Opponent detection: now considers each sensor individually
        if ( (digitalRead(OPP_FC) == HIGH) &&
             (digitalRead(OPP_FL) == HIGH) &&
             (digitalRead(OPP_FR) == HIGH) &&
             (digitalRead(OPP_L) == HIGH) &&
             (digitalRead(OPP_R) == HIGH) ){
            search(searchDir); 
             // Search if no opponent is detected
        } else {
            // Attack if any sensor detects an opponent
            attack();
        }
    }

    delay(50);  // Stabilize sensor readings
}

// Search function to locate opponent
void search(int dir) {
    digitalWrite(LED, HIGH);  // Indicate search mode
    if (dir == LEFT) {
        MakerSumo.setMotorSpeed(MOTOR_L,60);
        MakerSumo.setMotorSpeed(MOTOR_R,80);
    } else {
        MakerSumo.setMotorSpeed(MOTOR_L,80);
        MakerSumo.setMotorSpeed(MOTOR_R,60);
    }
}

// Attack function for engaging opponent
void attack() {
    digitalWrite(LED, LOW);  // Indicate attack mode
    if (digitalRead(OPP_FC) == LOW) {
        MakerSumo.setMotorSpeed(MOTOR_L, 255);
        MakerSumo.setMotorSpeed(MOTOR_R, 255);
    } else if (digitalRead(OPP_FR) == LOW) {
        MakerSumo.turnRight(60);
        delay(100);
        MakerSumo.setMotorSpeed(MOTOR_L,255);
        MakerSumo.setMotorSpeed(MOTOR_R,0);
    } else if (digitalRead(OPP_FL) == LOW && digitalRead(OPP_L) == LOW) {
        MakerSumo.turnLeft(60);
        delay(100);
        MakerSumo.setMotorSpeed(MOTOR_L,0);
        MakerSumo.setMotorSpeed(MOTOR_R,255);
    } else if (digitalRead(OPP_R) == LOW && digitalRead(OPP_R) == LOW) {
        MakerSumo.turnRight(100);
        /*
        MakerSumo.setMotorSpeed(MOTOR_L, 100);
        MakerSumo.setMotorSpeed(MOTOR_R, -100);*/
        delay(100);
    } else if (digitalRead(OPP_L) == LOW) {
        MakerSumo.turnLeft(100);
        /*MakerSumo.setMotorSpeed(MOTOR_L, -100);
        MakerSumo.setMotorSpeed(MOTOR_R, 100); */
        delay(100);
    }
}

// Function to avoid ring edges
void comeBack(int dir) {
    MakerSumo.setMotorSpeed(MOTOR_L, -60);
    MakerSumo.setMotorSpeed(MOTOR_R, -60);
    delay(50);
    
    if (dir == LEFT) {
        MakerSumo.setMotorSpeed(MOTOR_L, -60);
        MakerSumo.setMotorSpeed(MOTOR_R, 60);
    } else {
        MakerSumo.setMotorSpeed(MOTOR_L, 60);
        MakerSumo.setMotorSpeed(MOTOR_R, -60);
    }
    delay(100);
}

// Emergency stop function
void kill() {
    MakerSumo.stop();
    MakerSumo.setMotorSpeed(MOTOR_L, 0);
    MakerSumo.setMotorSpeed(MOTOR_R, 0);
    digitalWrite(LED, LOW);

    while(1) delay(30000);
}

void loop() {
 /*   if (irrecv.decode(&results)) {
        code = results.value;
        Serial.print("RC5 Code: ");
        Serial.println(code, HEX);

        delay(50);  
        irrecv.resume(); 
    }

    if (code == 0x2371 || code == 0xF1D || code == 0x841){
       code = 0x41;
    }
    if (code == 0xFFFFC4B8 || code == 0x2584 || code == 0x842){
      code = 0x42;
    }

    switch (code) {
        case 0x41:
            Serial.println("ARIS-1F4 started!");
            if (!isRunning) {  // Only run start routine if not already running
                Serial.println("ARIS-1F4 started!");
                startRoutine();   // Run the start routine once
                isRunning = true; // Set running state
            }
            robotNavigation();
            break;
        case 0x42:
            Serial.println("Stopping!");
            MakerSumo.stop();
            digitalWrite(LED, LOW);  // Indicate stopped status
            isRunning = false; // Reset running state
            code = 0;  // Reset code to prevent looping stop command
            break;
        default:
            if (isRunning) {
                // Only continue navigating if the robot is in a running state
                robotNavigation();
            } else {
                // Ensure the robot remains stopped when no command is active
                MakerSumo.stop();
            }
            break;

    }*/
  bool current_start_pin_state = 0;
  bool robot_ready_state = true;
  while(current_start_pin_state == 0){


    current_start_pin_state=digitalRead(start_pin);
  }
  if(current_start_pin_state ==1 && robot_ready_state == true ){
    while(current_start_pin_state == 1 ){
      current_start_pin_state=digitalRead(start_pin);
      startRoutine();
      robotNavigation();


    }
    if(current_start_pin_state ==0){
      kill();
      MakerSumo.stop();
      while (true){


        
      }
      }
  

  }






}
