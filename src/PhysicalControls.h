#ifndef PHYSICAL_CONTROLS_H
#define PHYSICAL_CONTROLS_H

#include <AceButton.h>
#include <Encoder.h>

#include "State.h"

#define BTN_ENC_PIN 0
#define BTN_ONE_PIN 1
#define BTN_TWO_PIN 2

#define ENC_PIN_A 3
#define ENC_PIN_B 4

using namespace ace_button;

class PhysicalControls : public IEventHandler
{
    public:
    PhysicalControls(State* state);
    void update();

    private:
    void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) override;
    
    AceButton btnOne;
    AceButton btnTwo;
    AceButton btnEncoder;

    Encoder* knob;
    int knobPos;

    State* __state;
};

#endif