#pragma once

#include "Activity.h"
#include "../storage/DeckStorage.h"
#include <vector>

class DeckListActivity : public Activity {
private:
    std::vector<DeckMetadata> decks;
    int selectedIndex;
    int scrollOffset;
    static const int ITEMS_PER_PAGE = 10;
    static const int LINE_HEIGHT = 40;
    static const int HEADER_HEIGHT = 50;

    void drawList();

public:
    DeckListActivity(GfxRenderer& renderer, MappedInputManager& input);
    void onEnter() override;
    void loop() override;
};
