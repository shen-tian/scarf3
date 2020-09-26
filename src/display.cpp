#include "display.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C lcd(U8G2_R2, 19, 18, U8X8_PIN_NONE);
// U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C lcd(U8G2_R2, U8X8_PIN_NONE);
//U8G2_SSD1327_WS_128X128_F_4W_SW_SPI lcd(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 8);
// U8G2_SSD1327_WS_128X128_F_4W_HW_SPI lcd(U8G2_R0, /* cs=*/ 10, /* dc=*/ 7, /* reset=*/ 8);
void initDisplay()
{
    lcd.begin();
}

void drawParam(uint8_t x, uint8_t y, uint8_t value, paramType type, bool selected)
{
    lcd.setDrawColor(selected ? 1 : 0);
    lcd.drawBox(x, y, 15, 22);

    lcd.setDrawColor(selected ? 0 : 1);
    lcd.setFont(u8g2_font_tom_thumb_4x6_mf);

    switch (type)
    {
    case NORMAL:
    case CIRCULAR:
        lcd.drawXBM(x + 1, y + 1, 13, 13, dialXBM[value / 13]);
        break;
    case OCTAVE:
        switch (value)
        {
        case 0 ... 31:
            lcd.drawStr(x + 1, y + 10, "1/8");
            break;
        case 32 ... 63:
            lcd.drawStr(x + 1, y + 10, "1/4");
            break;
        case 64 ... 95:
            lcd.drawStr(x + 1, y + 10, "1/2");
            break;
        case 96 ... 127:
            lcd.drawStr(x + 1, y + 10, " 1x");
            break;
        case 128 ... 159:
            lcd.drawStr(x + 1, y + 10, " 2x");
            break;
        case 160 ... 191:
            lcd.drawStr(x + 1, y + 10, " 4x");
            break;
        case 192 ... 223:
            lcd.drawStr(x + 1, y + 10, " 8x");
            break;
        case 224 ... 255:
            lcd.drawStr(x + 1, y + 10, "16x");
            break;
        }
        break;
    case NONE:
        break;
    }

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
    float intPart, fracPart;
    fracPart = modff(bpm, &intPart);
    sprintf(buffer, "%3.0f", intPart);
    lcd.setFont(u8g2_font_t0_11b_mn);
    lcd.drawStr(x, y, buffer);
    lcd.setFont(u8g2_font_tom_thumb_4x6_mf);
    sprintf(buffer, ".%1.0f", fracPart * 10);
    lcd.drawStr(x + 18, y, buffer);

    for (int i = 0; i < 4; i++)
    {
        if (i == beat)
        {
            lcd.drawBox(x + 7 * i, y + 3, 6, 3);
        }
        else
        {
            lcd.drawFrame(x + 7 * i, y + 3, 6, 3);
        }
    }
}

void drawLevel(uint8_t x, uint8_t y, uint8_t level)
{

    for (int i = 0; i < 7; i++)
    {
        lcd.drawHLine(x, y + i * 3, 2);
        lcd.drawHLine(x + 14, y + i * 3, 2);
    }

    int barHeight = (level / 14) + 1;
    lcd.drawBox(x + 5, y + 19 - barHeight, 6, barHeight);
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
        case 0 ... 4:
            lcd.drawStr(10, 51, state.getPatternLabel(state.bgMode));
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

    drawBeat(14, 16, state.bpm, state.currentBeat);

    // lcd.drawVLine(45, 0, 55);
    drawLevel(48 - 3, 4, state.visibleParam(0));

    drawStatus(state.fps);

    for (int i = 0; i < 6; i++)
    {
        uint8_t x = 128 - (16 * 3) + (i % 3) * 16;
        uint8_t y = 1 + 22 * (i / 3);
        drawParam(x, y, state.visibleParam(i), state.visibleParamType(i), (state.selectedIdx == i + 1));
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