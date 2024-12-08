#include <LiquidCrystal.h>
#include <Encoder.h>

//=============== Define Pins ===============
// LCD Pins
#define LCD_RS_GPO  12
#define LCD_EN_GPO  11
#define LCD_D4_GPO  10
#define LCD_D5_GPO  9
#define LCD_D6_GPO  8
#define LCD_D7_GPO  7
#define LCD_BL_GPO  6

// Encoder Pins
#define ENCODER_PIN_A  2
#define ENCODER_PIN_B  4
#define ENCODER_BTN_PIN  5

// Buzzer Pin
#define BUZZER_PIN  13

// Resonans Pin
#define RESONANS_PIN  3
//--------------- Define Pins ---------------


//============= Program scheduler ===========
#define LCD_TASK_MS 20
volatile unsigned long lcd_task_last_exec = 0;

#define ENCODER_TASK_MS 1
volatile unsigned long encoder_task_last_exec = 0;

#define BUZZER_TASK_MS 1
volatile unsigned long buzzer_task_last_exec = 0;

#define RESONANS_TASK_MS 1
volatile unsigned long resonans_task_last_exec = 0;
//------------- Program scheduler -----------

//============= Deseases Definitions ========
// Define a struct to hold the disease name and associated freq
struct Disease {
    const char* name;
    unsigned int numbers[10]; // Adjust size to maximum non-zero freq in any disease
    byte sub_index;       // Count of valid numbers/minutes
};

const int deseases_count = 31; // Number of diseases
Disease diseases[deseases_count] = {
  {"Angina",                  {787, 776, 727, 690, 465, 428, 660},                           7},
  {"Stomachache",             {10000, 3000, 95},                                             3},
  {"Pain in general",         {3000, 2720, 95, 666, 80, 40},                                 6},
  {"Headaches",               {10000, 144, 160, 520, 304},                                   5},
  {"Infection",               {3000, 95, 880, 1550, 802, 787, 776, 727},                     8},
  {"Acute pain",              {3000, 95, 10000, 1550, 802, 880, 787, 727, 690, 666},        10},
  {"Back pain 2",             {787, 784, 776, 728, 727, 465, 432},                           7},
  {"Arthralgia",              {160, 500, 1600, 5000, 324, 528},                              6},
  {"Toothache",               {5170, 3000, 2720, 2489, 1800, 1600, 1550, 880, 832, 666},    10},
  {"No appetite",             {10000, 465, 444, 1865, 125, 95, 72, 880, 787, 727},          10},
  {"No taste",                {10000, 20},                                                   2},
  {"Motion sickness",         {10000, 5000, 648, 624, 600, 465, 440, 648, 444, 1865},       10},
  {"Hoarseness",              {880, 760, 727},                                               3},
  {"Dolegl. gastric",         {10000, 1550, 802, 880, 832, 787, 727, 465},                   8},
  {"Prostate ailments",       {2050, 880, 1550, 802, 787, 727, 465, 20},                     8},
  {"Deafness",                {10000, 1550, 880, 802, 787, 727, 20},                         7},
  {"Flu",                     {954, 889, 841, 787, 763, 753, 742, 523, 513, 482},           10},
  {"Hemorrhoids",             {4474, 6117, 774, 1550, 447, 880, 802, 727},                   8},
  {"Kidney stones",           {10000, 444, 727, 787, 880, 6000, 3000, 1552},                 8},
  {"Cough",                   {7760, 7344, 3702, 3672, 1550, 1500, 1234, 776, 766, 728},    10},
  {"Runny nose",              {1800, 1713, 1550, 802, 800, 880, 787, 727, 444, 20},         10},
  {"Hair loss",               {10000, 5000, 2720, 2170, 1552, 880, 800, 787, 727, 465},     10},
  {"Hypertension",            {10000, 3176, 2112, 95, 324, 528, 880, 787, 727, 304},        10},
  {"Low pressure",            {727, 787, 880},                                               3},
  {"Disease. thyroid gland",  {16000, 10000, 160, 80, 35},                                   5},
  {"Bad breath",              {1550, 802, 880, 787, 727},                                    5},
  {"General herpes",          {2950, 1900, 1577, 1550, 1489, 1488, 629, 464, 450, 383},     10},
  {"Epilepsy",                {10000, 880, 802, 787, 727, 700, 650, 600, 210, 125},         10},
  {"Constipation",            {3176, 1550, 880, 832, 802, 787, 776, 727, 444, 422},         10},
  {"Dizziness",               {1550, 880, 802, 784, 787, 786, 766, 522, 727, 72},           10},
  {"Alcoholism",              {10000},                                                       1},
};
//------------- Deseases Definitions --------


//=============== LCD Functions ==============
// Default values
#define DEFAULT_SCROLL_SPEED 700
#define DEFAULT_PAUSE_DURATION (DEFAULT_SCROLL_SPEED * 3)
#define DEFAULT_BACKLIGHT_MODE ON
#define DEFAULT_BLINK_ON_DURATION 500
#define DEFAULT_BLINK_OFF_DURATION 500
#define DEFAULT_GRADUALITY_SPEED 10

//Define LCD object
LiquidCrystal lcd(LCD_RS_GPO, LCD_EN_GPO, LCD_D4_GPO, LCD_D5_GPO, LCD_D6_GPO, LCD_D7_GPO);

unsigned long scrollSpeed; // Scroll speed in milliseconds
unsigned long pauseDuration; // Pause duration

String lcd_line1 = "This is long text on line 1.";
String lcd_line2 = "this is line 2.";

enum BacklightMode {
    BREATHING,
    ON,
    OFF,
    BLINK
};

BacklightMode currentBacklightMode;
unsigned long blinkOnDuration;
unsigned long blinkOffDuration;
unsigned long gradualitySpeed;

void setBacklightMode(BacklightMode mode) {
    currentBacklightMode = mode;
}

void setScrollSpeed(unsigned long speed) {
    scrollSpeed = speed;
    pauseDuration = scrollSpeed * 3;
}

void setPauseDuration(unsigned long duration) {
    pauseDuration = duration;
}

void setBlinkOnDuration(unsigned long duration) {
    blinkOnDuration = duration;
}

void setBlinkOffDuration(unsigned long duration) {
    blinkOffDuration = duration;
}

void setGradualitySpeed(unsigned long speed) {
    gradualitySpeed = speed;
}

void backlight_loop(BacklightMode mode) {
    static BacklightMode lastMode = OFF;  // Track the previous mode
    static unsigned long lastUpdate = 0;
    static bool blinkState = false;
    static int brightness = 0;
    static int fadeAmount = 5;

    unsigned long currentMillis = millis();

    // Reset state on mode change
    if (mode != lastMode) {
        lastMode = mode;
        blinkState = false;
        fadeAmount = 5;

        // Reset brightness based on the new mode
        switch (mode) {
            case ON:
                brightness = 255; // Set to maximum brightness
                analogWrite(LCD_BL_GPO, brightness);
                return;

            case OFF:
                brightness = 0; // Set to minimum brightness
                analogWrite(LCD_BL_GPO, brightness);
                return;

            case BREATHING:
                brightness = 70; // Start at minimum breathing brightness
                fadeAmount = 5;  // Ensure proper fade
                break;

            case BLINK:
                blinkState = true;
                analogWrite(LCD_BL_GPO, 255);
                lastUpdate = currentMillis; // Reset blink timer
                break;
        }
    }

    // Handle behavior for the current mode
    switch (mode) {
        case BREATHING:
            if (currentMillis - lastUpdate >= gradualitySpeed) {
                lastUpdate = currentMillis;
                brightness += fadeAmount;
                if (brightness <= 70 || brightness >= 255) {
                    fadeAmount = -fadeAmount;
                }
                brightness = constrain(brightness, 70, 255);
                analogWrite(LCD_BL_GPO, brightness);
            }
            break;

        case BLINK:
            if (blinkState && currentMillis - lastUpdate >= blinkOnDuration) {
                lastUpdate = currentMillis;
                blinkState = false;
                analogWrite(LCD_BL_GPO, 0);
            } else if (!blinkState && currentMillis - lastUpdate >= blinkOffDuration) {
                lastUpdate = currentMillis;
                blinkState = true;
                analogWrite(LCD_BL_GPO, 255);
            }
            break;

        // ON and OFF are handled in the reset state logic above
    }
}

void lcd_init()
{   
    setScrollSpeed(DEFAULT_SCROLL_SPEED);
    setPauseDuration(DEFAULT_PAUSE_DURATION);
    setBacklightMode(DEFAULT_BACKLIGHT_MODE);
    setBlinkOnDuration(DEFAULT_BLINK_ON_DURATION);
    setBlinkOffDuration(DEFAULT_BLINK_OFF_DURATION);
    setGradualitySpeed(DEFAULT_GRADUALITY_SPEED);

    pinMode(LCD_BL_GPO, OUTPUT);
    lcd.begin(16, 2);
    lcd.clear();
}

void lcd_task()
{
    static int scrollIndex1 = 0;
    static int scrollIndex2 = 0;
    static bool scrollDirection1 = true;
    static bool scrollDirection2 = true;
    static unsigned long lastScrollTime1 = 0;
    static unsigned long lastScrollTime2 = 0;
    static bool pause1 = false;
    static bool pause2 = false;

    unsigned long currentMillis = millis();

    //workaround
    if (lcd_line1.startsWith("Playing: ")) {
        lcd_line1 = "Playing: " + String(convertToMMSS(getRemainingTime())) + "  ";
    }

    // Update line 1
    if (lcd_line1.length() > 16) {
        if (currentMillis - lastScrollTime1 >= (pause1 ? pauseDuration : scrollSpeed)) {
            lastScrollTime1 = currentMillis;
            pause1 = false;
            if (scrollDirection1) {
                scrollIndex1++;
                if (scrollIndex1 + 16 >= lcd_line1.length()) {
                    scrollDirection1 = false;
                    pause1 = true;
                }
            } else {
                scrollIndex1--;
                if (scrollIndex1 <= 0) {
                    scrollDirection1 = true;
                    pause1 = true;
                }
            }
        }
        lcd.setCursor(0, 0);
        lcd.print(lcd_line1.substring(scrollIndex1, scrollIndex1 + 16));
    } else {
        lcd.setCursor(0, 0);
        lcd.print(lcd_line1 + String(' ', 16 - lcd_line1.length()));
    }

    // Update line 2
    if (lcd_line2.length() > 16) {
        if (currentMillis - lastScrollTime2 >= (pause2 ? pauseDuration : scrollSpeed)) {
            lastScrollTime2 = currentMillis;
            pause2 = false;
            if (scrollDirection2) {
                scrollIndex2++;
                if (scrollIndex2 + 16 >= lcd_line2.length()) {
                    scrollDirection2 = false;
                    pause2 = true;
                }
            } else {
                scrollIndex2--;
                if (scrollIndex2 <= 0) {
                    scrollDirection2 = true;
                    pause2 = true;
                }
            }
        }
        lcd.setCursor(0, 1);
        lcd.print(lcd_line2.substring(scrollIndex2, scrollIndex2 + 16));
    } else {
        lcd.setCursor(0, 1);
        lcd.print(lcd_line2 + String(' ', 16 - lcd_line2.length()));
    }

    backlight_loop(currentBacklightMode);
}
//--------------- LCD Functions --------------

//============= Resonans Generation ==========
unsigned long programStartTime = 0;
unsigned long resonansStartTime = 0;
unsigned long resonansDuration = 0;
bool resonansActive = false;
int currentFrequencyIndex = 0;
int diseaseIndex = 0;

void resonans_init() {
    pinMode(RESONANS_PIN, OUTPUT);
    digitalWrite(RESONANS_PIN, LOW);
}

void playDisease(int diseaseNumber) {
    diseaseIndex = diseaseNumber;
    if (diseaseIndex >= 0 && diseaseIndex < deseases_count) {
        resonansStartTime = millis();
        programStartTime = millis();
        resonansDuration = diseases[diseaseIndex].sub_index * 60000;
        resonansActive = true;
        currentFrequencyIndex = 0;
        tone(RESONANS_PIN, diseases[diseaseIndex].numbers[currentFrequencyIndex]);
        setBuzzer(1000);
    }
}

void stopDisease() {
    resonansActive = false;
    noTone(RESONANS_PIN);
    digitalWrite(RESONANS_PIN, LOW);
    setBuzzer(1500);
    set_is_playing(false);
}

unsigned long getRemainingTime() {
    if (resonansActive) {
        unsigned long elapsedTime = millis() - programStartTime;
        unsigned long remainingTime = resonansDuration - elapsedTime;
        return remainingTime / 1000; // Return remaining time in seconds
    }
    return 0;
}

void resonans_task() {
    if (resonansActive) {
        unsigned long elapsedTime = millis() - resonansStartTime;

        // Each frequency runs for 1 minute (60000 ms)
        if (elapsedTime >= 60000) {
            resonansStartTime = millis(); // Reset the start time
            currentFrequencyIndex++;     // Move to the next frequency
            
            if (currentFrequencyIndex < diseases[diseaseIndex].sub_index) {
                // Play the next frequency
                tone(RESONANS_PIN, diseases[diseaseIndex].numbers[currentFrequencyIndex]);
            } else {
                // Stop the program when all frequencies are played
                stopDisease();
                lcd_line1 = "Select disease: ";
                setBacklightMode(ON);
            }
        }
    }
}

//------------- Resonans Generation ----------


//============= Menu Functions ===============
bool is_playing = 0;
int deseaseindex = 0;

void set_is_playing(bool value) {
    is_playing = value;
}

String displayDisease(int index) {
    if (index >= 0 && index < deseases_count) {
        String diseaseName = diseases[index].name;
        while (diseaseName.length() < 16) {
            diseaseName += ' ';
        }
        return diseaseName;
    } else {
        return "Invalid index";
    }
}
void menu_init() {
    lcd_line1 = "Select disease: ";
    lcd_line2 = displayDisease(deseaseindex);
}

String convertToMMSS(unsigned long seconds) {
    unsigned long minutes = seconds / 60;
    unsigned long remainingSeconds = seconds % 60;
    String mm = String(minutes);
    String ss = remainingSeconds < 10 ? "0" + String(remainingSeconds) : String(remainingSeconds);
    if (minutes < 10) {
        mm = "0" + mm;
    }
    return mm + ":" + ss;
}


void handleMenu(int positionDifference, bool buttonPressed) {
    if (positionDifference != 0 && !is_playing) {
        deseaseindex += positionDifference;
        if (deseaseindex < 0) {
            deseaseindex = 0;
        } else if (deseaseindex >= deseases_count) {
            deseaseindex = deseases_count - 1;
        }
        lcd_line2 = displayDisease(deseaseindex);
    }

    if (buttonPressed) {
        is_playing = !is_playing;
        if (is_playing) {
            playDisease(deseaseindex);
            lcd_line1 = "Playing: ";
            setBacklightMode(BREATHING);
        } else {
            stopDisease();
            lcd_line1 = "Select disease: ";
            setBacklightMode(ON);
        }
        lcd_line2 = displayDisease(deseaseindex);
    }
}

//------------- Menu Functions ---------------

//============= Encoder Functions ============
// Define Encoder object
Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B);

void handleEncoder(int positionDifference, bool buttonPressed) {
    setBuzzer(30);
    handleMenu(positionDifference, buttonPressed);
}

void encoder_init() {
    pinMode(ENCODER_BTN_PIN, INPUT_PULLUP);
    encoder.write(0);
}

void encoder_task()
{
    static int lastPosition = 0; // Stores the last encoder position
    static bool lastButtonState = HIGH; // Tracks the last button state
    static bool buttonPressedHandled = false; // Ensures button is handled only once per press
    static unsigned long lastDebounceTime = 0; // Tracks debounce timing
    const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

    // Read encoder position and button state
    int newPosition = encoder.read() / 4; // Adjust for one click per detent
    bool buttonState = digitalRead(ENCODER_BTN_PIN);

    // Handle encoder rotation
    if (newPosition != lastPosition) {
        handleEncoder(newPosition - lastPosition, false);
        lastPosition = newPosition;
    }

    // Handle button press with debounce
    if (buttonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (buttonState == LOW && !buttonPressedHandled) {
            handleEncoder(0, true);
            buttonPressedHandled = true;
        } else if (buttonState == HIGH) {
            buttonPressedHandled = false;
        }
    }

    lastButtonState = buttonState;
}
//------------- Encoder Functions ------------

//============= Buzzer Functions =============
unsigned long buzzerDuration = 0;
unsigned long buzzerStartTime = 0;
bool buzzerActive = false;

void buzzer_init() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, HIGH);
    setBuzzer(500);
}

void setBuzzer(unsigned int duration) {
    buzzerDuration = duration;
    buzzerStartTime = millis();
    buzzerActive = true;
    digitalWrite(BUZZER_PIN, LOW);
}

void buzzer_task() {
    if (buzzerActive && (millis() - buzzerStartTime >= buzzerDuration)) {
        digitalWrite(BUZZER_PIN, HIGH);
        buzzerActive = false;
    }
}
//------------- Buzzer Functions -------------

void setup()
{
    Serial.begin(9600);
    lcd_init();     //Init the lcd and backLight
    encoder_init(); // Init the encoder
    buzzer_init();  // Init the buzzer
    menu_init();    // Init the menu
    resonans_init(); // Init the resonans
}

void loop()
{
    if(millis() - lcd_task_last_exec >= LCD_TASK_MS)
    {
        lcd_task_last_exec = millis();
        lcd_task();
    }

    if (millis() - encoder_task_last_exec >= ENCODER_TASK_MS) {
        encoder_task_last_exec = millis();
        encoder_task();
    }

    if (millis() - buzzer_task_last_exec >= BUZZER_TASK_MS) {
        buzzer_task_last_exec = millis();
        buzzer_task();
    }

    if (millis() - resonans_task_last_exec >= RESONANS_TASK_MS) {
        resonans_task_last_exec = millis();
        resonans_task();
    }
}