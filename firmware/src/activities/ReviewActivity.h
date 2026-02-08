#pragma once

#include "Activity.h"
#include "../data/Card.h"
#include "../data/Progress.h"
#include "../storage/DeckStorage.h"
#include "../scheduling/SM2.h"
#include <GfxRenderer.h>
#include <MappedInputManager.h>

class ReviewActivity : public Activity {
public:
    enum State {
        SHOWING_FRONT,
        SHOWING_BACK,
        RATING,
        FINISHED
    };

    ReviewActivity(String deckId, GfxRenderer& renderer, MappedInputManager& input);
    virtual ~ReviewActivity() = default;

    void onEnter() override;
    void loop() override;

private:
    String deckId;
    State currentState;
    CardStream cardStream;
    Card currentCard;
    DeckProgress deckProgress;
    bool hasMoreCards;

    void showFront();
    void showBack();
    void showRating();
    void showFinished();
    
    void processRating(SM2::Quality quality);
    void drawCardContent(const String& content, const char* title);
};
