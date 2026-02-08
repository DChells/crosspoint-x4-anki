#include "SessionCompleteActivity.h"
#include <GfxRenderer.h>
#include <MappedInputManager.h>

SessionCompleteActivity::SessionCompleteActivity(GfxRenderer& renderer, MappedInputManager& input, int reviewed, int remaining)
    : Activity("SessionComplete", renderer, input), cardsReviewed(reviewed), cardsRemaining(remaining) {}

void SessionCompleteActivity::onEnter() {
    Activity::onEnter();
    drawScreen();
}

void SessionCompleteActivity::loop() {
    if (input.wasPressed(MappedInputManager::Button::Confirm) || input.wasPressed(MappedInputManager::Button::Back)) {
        // TODO: Return to deck list
        Serial.println("Returning to deck list");
    }
}

void SessionCompleteActivity::drawScreen() {
    renderer.clearScreen();
    
    int centerX = renderer.getScreenWidth() / 2;
    int centerY = renderer.getScreenHeight() / 2;
    
    renderer.drawCenteredText(2, centerY - 60, "Session Complete!", false);
    
    String reviewedStr = "Reviewed: " + String(cardsReviewed);
    renderer.drawCenteredText(1, centerY - 10, reviewedStr.c_str(), false);
    
    String remainingStr = "Remaining: " + String(cardsRemaining);
    renderer.drawCenteredText(1, centerY + 20, remainingStr.c_str(), false);
    
    renderer.fillRect(40, centerY + 60, renderer.getScreenWidth() - 80, 40);
    renderer.drawCenteredText(1, centerY + 70, "Back to Decks", false); // Text on black rect logic depends on renderer impl (usually XOR or white on black)
    // Assuming renderer handles text color or we need to set it.
    // Given existing code doesn't show text color setting, we assume default or auto-inversion.
    
    renderer.displayBuffer();
}
