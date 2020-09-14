#include "display.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C lcd(U8G2_R2, 19, 18, U8X8_PIN_NONE);

void initDisplay()
{
    lcd.begin();
}

void updateDisplay(State &state)
{
    lcd.clearBuffer();
    lcd.setDrawColor(1);
    lcd.setFont(u8g2_font_t0_11_mf);
    lcd.drawRFrame(20, 5, 30, 22, 7);

    switch (state.bgMode)
    {
    case 0:
        lcd.drawStr(0, 12, "cloud");
        break;
    case 1:
        lcd.drawStr(0, 12, "classic");
        break;
    case 2:
        lcd.drawStr(0, 12, "wave");
        break;
    case 3:
        lcd.drawStr(0, 12, "rainbow");
        break;
    case 4:
        lcd.drawStr(0, 12, "v.pluse");
        break;
    default:
        break;
    }

    if (state.dim)
    {
        lcd.drawStr(0, 24, "dim");
    }
    else
    {
        lcd.drawStr(0, 24, "bright");
    }

    lcd.sendBuffer();
}