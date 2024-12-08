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
//Define LCD object
LiquidCrystal lcd(LCD_RS_GPO, LCD_EN_GPO, LCD_D4_GPO, LCD_D5_GPO, LCD_D6_GPO, LCD_D7_GPO);

unsigned long scrollSpeed = 700; // Scroll speed in milliseconds
unsigned long pauseDuration = scrollSpeed * 3; // Pause duration

String lcd_line1 = "This is long text on line 1.";
String lcd_line2 = "this is line 2.";

void lcd_init()
{
    pinMode(LCD_BL_GPO, OUTPUT);
    digitalWrite(LCD_BL_GPO, HIGH);
    lcd.begin(16, 2);
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
        lcd.print(lcd_line1);
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
        lcd.print(lcd_line2);
    }
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