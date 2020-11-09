#ifndef PHYSICAL_CONTROLS_H
#define PHYSICAL_CONTROLS_H

#include <AceButton.h>

#include "State.h"

#define BTN_ENC_PIN 0
#define BTN_ONE_PIN 1
#define BTN_TWO_PIN 2

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
    State* __state;
};

#endif