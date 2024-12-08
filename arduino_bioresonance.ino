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

void lcd_init()
{
    pinMode(LCD_BL_GPO, OUTPUT);
    digitalWrite(LCD_BL_GPO, HIGH);
    lcd.begin(16, 2);
}

void lcd_task()
{
    lcd.setCursor(0, 0);
    lcd.print("Hello, World!");
    lcd.setCursor(0, 1);
    lcd.print(millis());
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