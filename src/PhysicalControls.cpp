#include "PhysicalControls.h"

PhysicalControls::PhysicalControls(State* state){
    __state = state;

    pinMode(BTN_ENC_PIN, INPUT_PULLUP);
    btnEncoder.init((uint8_t) BTN_ENC_PIN);
    ButtonConfig* buttonConfigEncoder = btnEncoder.getButtonConfig();
    buttonConfigEncoder->setIEventHandler(this);
    buttonConfigEncoder->setFeature(ButtonConfig::kFeatureClick);

    pinMode(BTN_ONE_PIN, INPUT_PULLUP);
    btnOne.init(BTN_ONE_PIN);
    ButtonConfig* buttonConfigOne = btnOne.getButtonConfig();
    buttonConfigOne->setIEventHandler(this);
    buttonConfigOne->setFeature(ButtonConfig::kFeatureClick);

    pinMode(BTN_TWO_PIN, INPUT_PULLUP);
    btnTwo.init(BTN_TWO_PIN);
    ButtonConfig* buttonConfigTwo = btnTwo.getButtonConfig();
    buttonConfigTwo->setIEventHandler(this);
    // buttonConfigTwo->setFeature(ButtonConfig::kFeatureClick);
}

void PhysicalControls::handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState)
{

  // Print out a message for all events.
  Serial.print(F("handleEvent(): eventType: "));
  Serial.print(eventType);
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  if (button == &btnOne && eventType == AceButton::kEventClicked){
      __state->cycleLayer();
  }

  if (button == &btnTwo && eventType == AceButton::kEventPressed){
      __state->cycleParam();
  }

  if (button == &btnEncoder && eventType == AceButton::kEventClicked){
      __state->zeroSelected();
  }
}

void PhysicalControls::update()
{
    btnEncoder.check();
    btnOne.check();
    btnTwo.check();
}