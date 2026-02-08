#include "ReviewActivity.h"
#include <Arduino.h>

// Font constants - assuming these are available in GfxRenderer
// If not, we'll need to use what's available or load them
#define FONT_LARGE 4
#define FONT_MEDIUM 2
#define FONT_SMALL 1

ReviewActivity::ReviewActivity(String deckId, GfxRenderer& renderer, MappedInputManager& input)
    : Activity("Review", renderer, input), 
      deckId(deckId), 
      currentState(SHOWING_FRONT),
      hasMoreCards(true) {
}

void ReviewActivity::onEnter() {
    Activity::onEnter();
    
    // Load progress and open stream
    deckProgress = DeckStorage::loadProgress(deckId);
    
    if (DeckStorage::openCardStream(deckId, cardStream)) {
        // Get first card
        if (cardStream.hasNext()) {
            currentCard = cardStream.readNext();
            hasMoreCards = true;
            showFront();
        } else {
            hasMoreCards = false;
            showFinished();
        }
    } else {
        Serial.println("Failed to open card stream");
        hasMoreCards = false;
        showFinished(); // Or error state
    }
}

void ReviewActivity::loop() {
    // Handle input based on state
    if (currentState == FINISHED) {
        if (input.wasPressed(MappedInputManager::Button::Back) || 
            input.wasPressed(MappedInputManager::Button::Confirm)) {
            // Signal to go back (how this is handled depends on the ActivityManager, 
            // but usually popping the activity is done by the manager when we're done)
            // For now, we just stay here or could implement a request to exit
        }
        return;
    }

    if (currentState == SHOWING_FRONT) {
        if (input.wasPressed(MappedInputManager::Button::Confirm)) {
            showBack();
        } else if (input.wasPressed(MappedInputManager::Button::Back)) {
            // Handle back if needed, or let main loop handle it to exit activity
        }
    } else if (currentState == SHOWING_BACK) {
        if (input.wasPressed(MappedInputManager::Button::Confirm)) {
            showRating();
        }
    } else if (currentState == RATING) {
        if (input.wasPressed(MappedInputManager::Button::Left)) {
             processRating(SM2::AGAIN); // Map Left to Again
        } else if (input.wasPressed(MappedInputManager::Button::Down)) {
             processRating(SM2::HARD);  // Map Down to Hard
        } else if (input.wasPressed(MappedInputManager::Button::Up)) {
             processRating(SM2::GOOD);  // Map Up to Good
        } else if (input.wasPressed(MappedInputManager::Button::Right)) {
             processRating(SM2::EASY);  // Map Right to Easy
        }
    }
}

void ReviewActivity::showFront() {
    currentState = SHOWING_FRONT;
    renderer.clearScreen(0xFF);
    
    // Draw header
    renderer.drawText(FONT_SMALL, 10, 10, ("Review: " + deckId).c_str());
    renderer.drawLine(0, 30, renderer.getScreenWidth(), 30);
    
    // Draw content
    drawCardContent(currentCard.front, "FRONT");
    
    // Draw controls hint
    renderer.drawText(FONT_SMALL, 10, renderer.getScreenHeight() - 20, "Press Confirm to reveal");
    
    renderer.displayBuffer();
}

void ReviewActivity::showBack() {
    currentState = SHOWING_BACK;
    // Keep front visible, maybe clear just bottom or redraw all
    renderer.clearScreen(0xFF);
    
    // Draw header
    renderer.drawText(FONT_SMALL, 10, 10, ("Review: " + deckId).c_str());
    renderer.drawLine(0, 30, renderer.getScreenWidth(), 30);
    
    // Draw front (smaller or top)
    renderer.drawText(FONT_SMALL, 10, 40, "Front:");
    renderer.drawText(FONT_MEDIUM, 10, 60, currentCard.front.c_str());
    
    renderer.drawLine(0, renderer.getScreenHeight() / 2, renderer.getScreenWidth(), renderer.getScreenHeight() / 2);
    
    // Draw back
    drawCardContent(currentCard.back, "BACK");
    
    // Draw controls hint
    renderer.drawText(FONT_SMALL, 10, renderer.getScreenHeight() - 20, "Press Confirm to rate");
    
    renderer.displayBuffer();
}

void ReviewActivity::showRating() {
    currentState = RATING;
    // Overlay rating controls or redraw bottom area
    
    int y = renderer.getScreenHeight() - 60;
    renderer.fillRect(0, y, renderer.getScreenWidth(), 60, 0xFF);
    renderer.drawLine(0, y, renderer.getScreenWidth(), y);
    
    renderer.drawText(FONT_SMALL, 10, y + 10, "Rate recall:");
    
    // Draw rating options corresponding to buttons
    // Left: Again, Down: Hard, Up: Good, Right: Easy
    int w = renderer.getScreenWidth();
    int col = w / 4;
    
    renderer.drawText(FONT_SMALL, 5, y + 30, "Again");
    renderer.drawText(FONT_SMALL, col + 5, y + 30, "Hard");
    renderer.drawText(FONT_SMALL, 2*col + 5, y + 30, "Good");
    renderer.drawText(FONT_SMALL, 3*col + 5, y + 30, "Easy");
    
    renderer.displayBuffer();
}

void ReviewActivity::showFinished() {
    currentState = FINISHED;
    renderer.clearScreen(0xFF);
    renderer.drawCenteredText(FONT_LARGE, renderer.getScreenHeight() / 2 - 20, "Deck Complete!");
    renderer.drawCenteredText(FONT_SMALL, renderer.getScreenHeight() / 2 + 20, "Press Back to exit");
    renderer.displayBuffer();
}

void ReviewActivity::processRating(SM2::Quality quality) {
    // Update card progress
    CardProgress& cp = deckProgress.getCardProgress(currentCard.id);
    
    // Apply SM-2 algorithm
    float newEase = SM2::updateEaseFactor(cp.ease, quality);
    int newInterval = SM2::calculateInterval(cp.repetitions, newEase);
    
    // Update stats
    cp.ease = newEase;
    cp.interval = newInterval;
    if (quality >= SM2::GOOD) {
        cp.repetitions++;
    } else {
        cp.repetitions = 0; // Reset on failure
    }
    
    // Save progress
    // In a real app, we might batch save or save on exit, but safety first
    DeckStorage::saveProgress(deckId, deckProgress);
    
    // Move to next card
    if (cardStream.hasNext()) {
        currentCard = cardStream.readNext();
        showFront();
    } else {
        hasMoreCards = false;
        showFinished();
    }
}

void ReviewActivity::drawCardContent(const String& content, const char* title) {
    int yStart = (String(title) == "FRONT") ? 60 : renderer.getScreenHeight() / 2 + 30;
    
    if (title) {
        renderer.drawText(FONT_SMALL, 10, yStart - 20, title);
    }
    
    // Simple word wrap or just print (drawText handles some wrapping usually or we need to implement)
    // Assuming drawText handles basic rendering. For long text we might need GfxRenderer::truncatedText 
    // or a custom wrap function, but for now using direct drawText as requested.
    renderer.drawText(FONT_MEDIUM, 10, yStart, content.c_str());
}
