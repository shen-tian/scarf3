#include "display.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C lcd(U8G2_R2, 19, 18, U8X8_PIN_NONE);
// U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C lcd(U8G2_R2, U8X8_PIN_NONE);
//U8G2_SSD1327_WS_128X128_F_4W_SW_SPI lcd(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 8);
// U8G2_SSD1327_WS_128X128_F_4W_HW_SPI lcd(U8G2_R0, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 8);
void initDisplay()
{
    lcd.begin();
}

void drawParam(uint8_t x, uint8_t y, uint8_t value, bool selected)
{

    lcd.setDrawColor(selected ? 1 : 0);
    lcd.drawBox(x, y, 15, 22);

    lcd.setDrawColor(selected ? 0 : 1);
    lcd.drawXBM(x + 1, y + 1, 13, 13, dialXBM[value / 13]);

    char buffer[3];
    sprintf(buffer, "%03d", value);
    lcd.setFont(u8g2_font_tom_thumb_4x6_mf);
    lcd.drawStr(x + 2, y + 21, buffer);
}

void drawLayer(uint8_t x, uint8_t y, bool selected)
{
    if (selected)
    {
        lcd.setDrawColor(1);
        lcd.drawRBox(x, y, 9, 15, 1);
    }
    lcd.setDrawColor(selected ? 0 : 1);
    lcd.drawBox(x + 3, y + 3, 3, 9);
}

void drawBeat(uint8_t x, uint8_t y, float bpm, int beat)
{
    char buffer[5];
    sprintf(buffer, "%5.1f", bpm);
    lcd.setFont(u8g2_font_profont17_mf);
    lcd.drawStr(x, y, buffer);

    for (int i = 0; i < 4; i++)
    {
        if (i == beat)
        {
            lcd.drawBox(x + 12 * i, y + 5, 10, 5);
        }
        else
        {
            lcd.drawFrame(x + 12 * i, y + 5, 10, 5);
        }
    }
}

void drawStatus(uint8_t fps)
{
    lcd.setDrawColor(1);
    lcd.drawBox(10, 64 - 7, 116, 7);

    lcd.setDrawColor(0);
    char buffer[3];
    sprintf(buffer, "%03d", fps);
    lcd.setFont(u8g2_font_tom_thumb_4x6_mf);
    lcd.drawStr(114, 63, buffer);
}

int pageIdx = 0;

void updateDisplay(State &state)
{
    lcd.clearBuffer();

    lcd.setFont(u8g2_font_profont10_mf);
    // lcd.drawRFrame(40, 5, 30, 22, 7);

    if (state.selectedIdx == 0)
    {
        lcd.setDrawColor(0);
    }
    else
    {
        lcd.setDrawColor(1);
    }

    if (state.selectedLayer == 1)
    {
        switch (state.bgMode)
        {
        case 0:
            lcd.drawStr(10, 11, "Cloud");
            break;
        case 1:
            lcd.drawStr(10, 11, "Classic");
            break;
        case 2:
            lcd.drawStr(10, 11, "Wave");
            break;
        case 3:
            lcd.drawStr(10, 11, "Rainbow");
            break;
        case 4:
            lcd.drawStr(10, 11, "V.Pulse");
            break;
        default:
            break;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        uint8_t y = 2 + (3 - i) * 15;
        drawLayer(0, y, (state.selectedLayer == i));
    }

    lcd.setDrawColor(1);
    lcd.drawFrame(8, 0, 120, 65);

    drawBeat(13, 16, state.bpm, state.currentBeat);

    drawStatus(state.fps);

    for (int i = 0; i < 6; i++)
    {
        uint8_t x = 128 - (16 * 3) + (i % 3) * 16;
        uint8_t y = 1 + 22 * (i / 3);
        drawParam(x, y, state.visibleParam(i), (state.selectedIdx == i + 1));
    }

    //lcd.drawGlyph(0, 63, 0x0040 + 5);

    //lcd.sendBuffer();
    lcd.updateDisplayArea(0, pageIdx, 16, 1);

    pageIdx++;
    if (pageIdx > 7)
    {
        pageIdx = 0;
    }
}