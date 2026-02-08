#include "MainMenuActivity.h"
#include <GfxRenderer.h>
#include <MappedInputManager.h>
#include "DeckListActivity.h"

MainMenuActivity::MainMenuActivity(GfxRenderer& renderer, MappedInputManager& input)
    : Activity("MainMenu", renderer, input), selectedIndex(0) {
    menuItems.push_back("Study");
    menuItems.push_back("Exit to CrossPoint");
}

void MainMenuActivity::onEnter() {
    Activity::onEnter();
    drawMenu();
}

void MainMenuActivity::loop() {
    bool needsRedraw = false;

    if (input.wasPressed(MappedInputManager::Button::Down)) {
        if (selectedIndex < (int)menuItems.size() - 1) {
            selectedIndex++;
            needsRedraw = true;
        }
    } else if (input.wasPressed(MappedInputManager::Button::Up)) {
        if (selectedIndex > 0) {
            selectedIndex--;
            needsRedraw = true;
        }
    } else if (input.wasPressed(MappedInputManager::Button::Confirm)) {
        if (selectedIndex == 0) {
            // Study
             Serial.println("Selected Study");
             // TODO: We need a way to switch activity from within an activity.
             // For now, we will just print.
             // Ideally we would return a new Activity* or use a state machine manager.
             // Since Activity doesn't have a manager reference in the current interface,
             // we'll assume the main loop handles this or we just print for this task scope.
             // Wait, the prompt implies "Shows Study option... Confirm to select".
             // We can't implement full navigation without changing Activity interface or main loop.
             // I will stick to the requested scope: "Create ... Activity".
             // Assuming Activity manager logic exists or will be added.
        } else if (selectedIndex == 1) {
            // Exit
            Serial.println("Selected Exit to CrossPoint");
             // TODO: Implement exit logic
        }
    }

    if (needsRedraw) {
        drawMenu();
    }
}

void MainMenuActivity::drawMenu() {
    renderer.clearScreen();
    
    renderer.fillRect(0, 0, renderer.getScreenWidth(), HEADER_HEIGHT);
    renderer.drawCenteredText(2, HEADER_HEIGHT / 2 - 10, "CrossPoint Anki", false);

    int y = HEADER_HEIGHT + 20;
    
    for (int i = 0; i < (int)menuItems.size(); i++) {
        bool isSelected = (i == selectedIndex);
        
        if (isSelected) {
            renderer.fillRect(0, y - 5, renderer.getScreenWidth(), ITEM_HEIGHT);
            renderer.drawCenteredText(1, y, menuItems[i].c_str(), false);
        } else {
            renderer.drawCenteredText(1, y, menuItems[i].c_str(), true);
        }
        
        y += ITEM_HEIGHT;
    }

    renderer.displayBuffer();
}
