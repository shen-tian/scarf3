#include "display.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C lcd(U8G2_R2, 19, 18, U8X8_PIN_NONE);

void initDisplay()
{
    lcd.begin();
}

int lineHeight = 16;

void updateDisplay(State &state)
{
    lcd.clearBuffer();

    lcd.setFont(u8g2_font_t0_13_mf);
    lcd.drawRFrame(40, 5, 30, 22, 7);

    if (state.selectedIdx == 0)
    {
        lcd.setDrawColor(0);
    }
    else
    {
        lcd.setDrawColor(1);
    }
    
    switch (state.bgMode)
    {
    case 0:
        lcd.drawStr(0, lineHeight, "cloud");
        break;
    case 1:
        lcd.drawStr(0, lineHeight, "classic");
        break;
    case 2:
        lcd.drawStr(0, lineHeight, "wave");
        break;
    case 3:
        lcd.drawStr(0, lineHeight, "rainbow");
        break;
    case 4:
        lcd.drawStr(0, lineHeight, "v.pluse");
        break;
    default:
        break;
    }

        if (state.selectedIdx == 1)
    {
        lcd.setDrawColor(0);
    }
    else
    {
        lcd.setDrawColor(1);
    }

    char buffer[16];
    sprintf(buffer, "%d", state.fooParam);
    lcd.drawStr(64, lineHeight, buffer);

    lcd.setDrawColor(1);

    if (state.dim)
    {
        lcd.drawStr(0, lineHeight * 2, "dim");
    }
    else
    {
        lcd.drawStr(0, lineHeight * 2, "bright");
    }

    lcd.setFont(u8g2_font_open_iconic_play_2x_t);
    lcd.drawGlyph(0, 63, 0x0040 + 5);

    lcd.sendBuffer();
}