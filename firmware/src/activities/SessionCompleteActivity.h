#pragma once

#include "Activity.h"

class SessionCompleteActivity : public Activity {
private:
    int cardsReviewed;
    int cardsRemaining;

    void drawScreen();

public:
    SessionCompleteActivity(GfxRenderer& renderer, MappedInputManager& input, int reviewed, int remaining);
    void onEnter() override;
    void loop() override;
};
