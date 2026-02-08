#include "DeckListActivity.h"
#include <GfxRenderer.h>
#include <MappedInputManager.h>
#include <algorithm>

DeckListActivity::DeckListActivity(GfxRenderer& renderer, MappedInputManager& input)
    : Activity("DeckList", renderer, input), selectedIndex(0), scrollOffset(0) {}

void DeckListActivity::onEnter() {
    Activity::onEnter();
    decks = DeckStorage::listDecks();
    
    std::sort(decks.begin(), decks.end(), [](const DeckMetadata& a, const DeckMetadata& b) {
        return a.name < b.name;
    });

    drawList();
}

void DeckListActivity::loop() {
    bool needsRedraw = false;

    if (input.wasPressed(MappedInputManager::Button::Down)) {
        if (selectedIndex < (int)decks.size() - 1) {
            selectedIndex++;
            if (selectedIndex >= scrollOffset + ITEMS_PER_PAGE) {
                scrollOffset = selectedIndex - ITEMS_PER_PAGE + 1;
            }
            needsRedraw = true;
        }
    } else if (input.wasPressed(MappedInputManager::Button::Up)) {
        if (selectedIndex > 0) {
            selectedIndex--;
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            }
            needsRedraw = true;
        }
    } else if (input.wasPressed(MappedInputManager::Button::Confirm)) {
        if (!decks.empty()) {
            Serial.printf("Selected deck: %s\n", decks[selectedIndex].name.c_str());
            // TODO: Navigate to StudyActivity with selected deck
        }
    } else if (input.wasPressed(MappedInputManager::Button::Back)) {
        // TODO: Navigate back or exit
        Serial.println("Back pressed");
    }

    if (needsRedraw) {
        drawList();
    }
}

void DeckListActivity::drawList() {
    renderer.clearScreen();
    
    renderer.fillRect(0, 0, renderer.getScreenWidth(), HEADER_HEIGHT);
    renderer.drawCenteredText(2, HEADER_HEIGHT / 2 - 10, "Select Deck", false);

    int y = HEADER_HEIGHT + 10;
    
    if (decks.empty()) {
        renderer.drawCenteredText(1, 200, "No decks found on SD card");
    } else {
        for (int i = 0; i < ITEMS_PER_PAGE; i++) {
            int deckIndex = scrollOffset + i;
            if (deckIndex >= (int)decks.size()) break;

            const auto& deck = decks[deckIndex];
            bool isSelected = (deckIndex == selectedIndex);

            if (isSelected) {
                renderer.fillRect(0, y - 5, renderer.getScreenWidth(), LINE_HEIGHT);
                renderer.drawText(1, 10, y, deck.name.c_str(), false);
                
                String countStr = String(deck.cardCount) + " cards";
                int countWidth = renderer.getTextWidth(1, countStr.c_str());
                renderer.drawText(1, renderer.getScreenWidth() - countWidth - 10, y, countStr.c_str(), false);
            } else {
                renderer.drawText(1, 10, y, deck.name.c_str(), true);
                
                String countStr = String(deck.cardCount) + " cards";
                int countWidth = renderer.getTextWidth(1, countStr.c_str());
                renderer.drawText(1, renderer.getScreenWidth() - countWidth - 10, y, countStr.c_str(), true);
            }

            y += LINE_HEIGHT;
        }
    }

    if (decks.size() > ITEMS_PER_PAGE) {
        int scrollBarHeight = (ITEMS_PER_PAGE * renderer.getScreenHeight()) / decks.size();
        int scrollBarY = HEADER_HEIGHT + (scrollOffset * (renderer.getScreenHeight() - HEADER_HEIGHT)) / decks.size();
        renderer.fillRect(renderer.getScreenWidth() - 4, scrollBarY, 4, scrollBarHeight);
    }

    renderer.displayBuffer();
}
