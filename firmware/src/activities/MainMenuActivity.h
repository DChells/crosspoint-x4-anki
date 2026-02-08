#pragma once

#include "Activity.h"
#include <vector>

class MainMenuActivity : public Activity {
private:
    std::vector<String> menuItems;
    int selectedIndex;
    static const int ITEM_HEIGHT = 40;
    static const int HEADER_HEIGHT = 60;

    void drawMenu();

public:
    MainMenuActivity(GfxRenderer& renderer, MappedInputManager& input);
    void onEnter() override;
    void loop() override;
};
