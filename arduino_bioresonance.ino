#include <LiquidCrystal.h>

//=============== Define Pins ===============
// LCD Pins
#define LCD_RS_GPO  12
#define LCD_EN_GPO  11
#define LCD_D4_GPO  10
#define LCD_D5_GPO  9
#define LCD_D6_GPO  8
#define LCD_D7_GPO  7
#define LCD_BL_GPO  6

//--------------- Define Pins ---------------


//============= Program scheduler ===========
#define LCD_TASK_MS 20
volatile unsigned long lcd_task_last_exec = 0;

//------------- Program scheduler -----------


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
    static unsigned long lastUpdate = 0;
    static bool blinkState = false;
    static int brightness = 0;
    static int fadeAmount = 5;

    unsigned long currentMillis = millis();

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

        case ON:
            if (currentMillis - lastUpdate >= gradualitySpeed) {
                lastUpdate = currentMillis;
                if (brightness < 255) {
                    brightness += fadeAmount;
                    brightness = constrain(brightness, 0, 255);
                    analogWrite(LCD_BL_GPO, brightness);
                }
            }
            break;

        case OFF:
            if (currentMillis - lastUpdate >= gradualitySpeed) {
                lastUpdate = currentMillis;
                if (brightness > 0) {
                    brightness -= fadeAmount;
                    brightness = constrain(brightness, 0, 255);
                    analogWrite(LCD_BL_GPO, brightness);
                }
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


void setup()
{
    lcd_init();     //Init the lcd and backLight
}

void loop()
{
    if(millis() - lcd_task_last_exec >= LCD_TASK_MS)
    {
        lcd_task_last_exec = millis();
        lcd_task();
    }
}