#include <Arduino.h>
#include <BluetoothSerial.h>

// ==================== PINOUT ====================
#define PIN_IR_SENSOR       34
#define PIN_FLAME_SENSOR    35
#define PIN_BUZZER          25
#define PIN_RGB_RED         26
#define PIN_RGB_GREEN       27
#define PIN_RGB_BLUE        14

// ==================== CONSTANTS ====================
#define DOOR_ALARM_THRESHOLD_MS     2000
#define DEBOUNCE_DELAY_MS           50
#define ALARM_MAX_DURATION_MS       30000

#define BUZZER_DOOR_FREQ    1000
#define BUZZER_FIRE_FREQ    2000

// ==================== SYSTEM STATES ====================
enum SystemState {
    DISARMED,
    ARMED,
    DOOR_ALARM,
    FIRE_ALARM
};

// ==================== DATA STRUCTURE ====================
struct SystemData {
    SystemState state;
    bool doorOpen;
    unsigned long doorOpenTime;
    bool alarmActive;
    unsigned long alarmStartTime;
    int intrusionCount;
};

// ==================== GLOBAL VARIABLES ====================
BluetoothSerial SerialBT;
SystemData data;

bool lastDoorState = false;
unsigned long lastDoorChangeTime = 0;

unsigned long lastBeepTime = 0;
bool buzzerOn = false;

unsigned long lastLedBlinkTime = 0;
bool ledOn = false;

// ==================== FUNCTION PROTOTYPES ====================
void initHardware();
void controlLED();
bool readIRSensor();
bool readFlameSensor();
void sendBluetoothState();
void processBluetoothCommands();
void triggerDoorAlarm();
void triggerFireAlarm();
void stopAlarm();
void handleBuzzer();
void handleAlarmLED();

// ==================== INITIALIZATION ====================
void initHardware() {
    pinMode(PIN_IR_SENSOR, INPUT);
    pinMode(PIN_FLAME_SENSOR, INPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_RGB_RED, OUTPUT);
    pinMode(PIN_RGB_GREEN, OUTPUT);
    pinMode(PIN_RGB_BLUE, OUTPUT);
    
    ledcSetup(0, 2000, 8);
    ledcAttachPin(PIN_BUZZER, 0);
    ledcWriteTone(0, 0);
    
    data.state = DISARMED;
    data.doorOpen = false;
    data.doorOpenTime = 0;
    data.alarmActive = false;
    data.alarmStartTime = 0;
    data.intrusionCount = 0;
    
    digitalWrite(PIN_RGB_RED, LOW);
    digitalWrite(PIN_RGB_GREEN, LOW);
    digitalWrite(PIN_RGB_BLUE, LOW);
    
    SerialBT.begin("ESP32_SecuSys");
    SerialBT.println("Security System Ready");
    SerialBT.println("Commands: arm, disarm, status, reset, help");
    
    controlLED();
}

// ==================== LED CONTROL ====================
void controlLED() {
    if (data.alarmActive) return;
    
    switch(data.state) {
        case ARMED:
            if (data.doorOpen) {
                digitalWrite(PIN_RGB_RED, LOW);
                digitalWrite(PIN_RGB_GREEN, LOW);
                digitalWrite(PIN_RGB_BLUE, HIGH);
            } else {
                digitalWrite(PIN_RGB_RED, LOW);
                digitalWrite(PIN_RGB_GREEN, HIGH);
                digitalWrite(PIN_RGB_BLUE, LOW);
            }
            break;
            
        case DISARMED:
            digitalWrite(PIN_RGB_RED, HIGH);
            digitalWrite(PIN_RGB_GREEN, HIGH);
            digitalWrite(PIN_RGB_BLUE, LOW);
            break;
            
        default:
            break;
    }
}

// ==================== SENSOR READING ====================
bool readIRSensor() {
    return (digitalRead(PIN_IR_SENSOR) == HIGH);
}

bool readFlameSensor() {
    return (digitalRead(PIN_FLAME_SENSOR) == LOW);
}

// ==================== BLUETOOTH COMMUNICATION ====================
void sendBluetoothState() {
    String stateStr = "";
    switch(data.state) {
        case ARMED:      stateStr = "ARMED"; break;
        case DISARMED:   stateStr = "DISARMED"; break;
        case DOOR_ALARM: stateStr = "DOOR_ALARM"; break;
        case FIRE_ALARM: stateStr = "FIRE_ALARM"; break;
    }
    
    SerialBT.print("[" + stateStr + "] ");
    SerialBT.print("Door:" + String(data.doorOpen ? "OPEN" : "closed") + " ");
    SerialBT.print("Intrusions:" + String(data.intrusionCount));
    
    if (data.state == ARMED && data.doorOpen && !data.alarmActive) {
        unsigned long openDuration = millis() - data.doorOpenTime;
        if (openDuration < DOOR_ALARM_THRESHOLD_MS) {
            long remaining = (DOOR_ALARM_THRESHOLD_MS - openDuration) / 1000;
            SerialBT.print(" | Alarm in " + String(remaining) + "s");
        }
    }
    SerialBT.println();
}

void processBluetoothCommands() {
    if (!SerialBT.available()) return;
    
    String cmd = SerialBT.readStringUntil('\n');
    cmd.toLowerCase();
    cmd.trim();
    
    if (cmd == "arm") {
        if (data.state == DISARMED) {
            data.state = ARMED;
            data.alarmActive = false;
            data.doorOpen = false;
            data.doorOpenTime = 0;
            ledcWriteTone(0, 0);
            controlLED();
            SerialBT.println("System ARMED - Surveillance active");
        } else {
            SerialBT.println("System already armed or in alarm");
        }
    }
    else if (cmd == "disarm") {
        stopAlarm();
        data.state = DISARMED;
        data.alarmActive = false;
        ledcWriteTone(0, 0);
        controlLED();
        SerialBT.println("System DISARMED");
    }
    else if (cmd == "status") {
        sendBluetoothState();
    }
    else if (cmd == "reset") {
        stopAlarm();
        data.state = DISARMED;
        data.intrusionCount = 0;
        data.doorOpen = false;
        data.doorOpenTime = 0;
        data.alarmActive = false;
        ledcWriteTone(0, 0);
        controlLED();
        SerialBT.println("System RESET");
    }
    else if (cmd == "help") {
        SerialBT.println("=== COMMANDS ===");
        SerialBT.println("  arm     - Activate surveillance");
        SerialBT.println("  disarm  - Deactivate surveillance");
        SerialBT.println("  status  - Show system status");
        SerialBT.println("  reset   - Reset system");
        SerialBT.println("  help    - Show this help");
    }
    else {
        SerialBT.println("Unknown command. Type 'help'");
    }
}

// ==================== ALARM HANDLING ====================
void triggerDoorAlarm() {
    if (!data.alarmActive && data.state == ARMED) {
        data.alarmActive = true;
        data.state = DOOR_ALARM;
        data.alarmStartTime = millis();
        data.intrusionCount++;
        
        SerialBT.println("DOOR ALARM - Door left open too long");
        SerialBT.println("Intrusion #" + String(data.intrusionCount));
    }
}

void triggerFireAlarm() {
    if (!data.alarmActive && data.state == ARMED) {
        data.alarmActive = true;
        data.state = FIRE_ALARM;
        data.alarmStartTime = millis();
        
        SerialBT.println("FIRE ALARM - Flame detected");
    }
}

void stopAlarm() {
    data.alarmActive = false;
    data.doorOpenTime = 0;
    data.doorOpen = false;
    ledcWriteTone(0, 0);
    buzzerOn = false;
    ledOn = false;
}

// ==================== BUZZER HANDLER ====================
void handleBuzzer() {
    if (!data.alarmActive) return;
    
    unsigned long interval = (data.state == FIRE_ALARM) ? 200 : 500;
    
    if (millis() - lastBeepTime > interval) {
        if (buzzerOn) {
            ledcWriteTone(0, 0);
            buzzerOn = false;
        } else {
            int frequency = (data.state == FIRE_ALARM) ? BUZZER_FIRE_FREQ : BUZZER_DOOR_FREQ;
            ledcWriteTone(0, frequency);
            buzzerOn = true;
        }
        lastBeepTime = millis();
    }
    
    if (millis() - data.alarmStartTime > ALARM_MAX_DURATION_MS) {
        stopAlarm();
        data.state = DISARMED;
        controlLED();
        SerialBT.println("Alarm stopped automatically - System disarmed");
    }
}

// ==================== ALARM LED HANDLER ====================
void handleAlarmLED() {
    if (!data.alarmActive) return;
    
    if (millis() - lastLedBlinkTime > 300) {
        ledOn = !ledOn;
        if (ledOn) {
            digitalWrite(PIN_RGB_RED, HIGH);
            digitalWrite(PIN_RGB_GREEN, LOW);
            digitalWrite(PIN_RGB_BLUE, LOW);
        } else {
            digitalWrite(PIN_RGB_RED, LOW);
            digitalWrite(PIN_RGB_GREEN, LOW);
            digitalWrite(PIN_RGB_BLUE, LOW);
        }
        lastLedBlinkTime = millis();
    }
}

// ==================== SETUP ====================
void setup() {
    initHardware();
}

// ==================== MAIN LOOP ====================
void loop() {
    processBluetoothCommands();
    
    bool currentDoorState = readIRSensor();
    bool flameDetected = readFlameSensor();
    
    if (flameDetected && data.state == ARMED && !data.alarmActive) {
        triggerFireAlarm();
    }
    
    if (data.state == ARMED && !data.alarmActive) {
        if (currentDoorState != lastDoorState) {
            lastDoorChangeTime = millis();
        }
        
        if (millis() - lastDoorChangeTime > DEBOUNCE_DELAY_MS) {
            if (currentDoorState && !data.doorOpen) {
                data.doorOpen = true;
                data.doorOpenTime = millis();
                controlLED();
                SerialBT.println("Door OPEN - Timer started (5s)");
            }
            else if (!currentDoorState && data.doorOpen) {
                unsigned long openDuration = millis() - data.doorOpenTime;
                data.doorOpen = false;
                controlLED();
                SerialBT.println("Door CLOSED - Duration: " + String(openDuration) + " ms");
                if (openDuration < DOOR_ALARM_THRESHOLD_MS) {
                    SerialBT.println("No alarm - Normal operation");
                }
            }
        }
        
        if (data.doorOpen && (millis() - data.doorOpenTime > DOOR_ALARM_THRESHOLD_MS)) {
            triggerDoorAlarm();
        }
        
        lastDoorState = currentDoorState;
    }
    else if (data.state != ARMED) {
        data.doorOpen = currentDoorState;
        lastDoorState = currentDoorState;
    }
    
    handleBuzzer();
    handleAlarmLED();
    
    static unsigned long lastStateSend = 0;
    if (millis() - lastStateSend > 10000 && data.state == ARMED && !data.alarmActive) {
        sendBluetoothState();
        lastStateSend = millis();
    }
    
    delay(20);
}