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

    lcd.setFont(u8g2_font_tom_thumb_4x6_mf);

    char buffer[16];

    for (int i = 0; i < 6; i++)
    {
        if (state.selectedIdx == i + 1)
        {
            lcd.setDrawColor(0);
        }
        else
        {
            lcd.setDrawColor(1);
        }

        sprintf(buffer, "%03d", state.params[state.bgMode][i]);
        lcd.drawStr(14 * i, 2 * lineHeight, buffer);
    }

    lcd.setDrawColor(1);

    lcd.setFont(u8g2_font_open_iconic_play_2x_t);
    lcd.drawGlyph(0, 63, 0x0040 + 5);

    lcd.drawXBM(64,3 * lineHeight, 13, 13, dialXBM[state.params[state.bgMode][2] / 16]);

    lcd.sendBuffer();
}