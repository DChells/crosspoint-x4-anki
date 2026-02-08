#pragma once
#include <Arduino.h>
#include "../data/DeckMetadata.h"
#include "../data/Card.h"
#include "../data/Progress.h"
#include <SDCardManager.h>
#include <vector>

class CardStream {
private:
    FsFile file;
    
public:
    bool open(const String& deckId);
    bool hasNext();
    Card readNext();
    void close();
};

class DeckStorage {
public:
    static std::vector<DeckMetadata> listDecks();
    static DeckMetadata loadDeckMetadata(const String& deckId);
    static bool openCardStream(const String& deckId, CardStream& stream);
    static DeckProgress loadProgress(const String& deckId);
    static bool saveProgress(const String& deckId, const DeckProgress& progress);
    static String getDeckPath(const String& deckId);
    static String getProgressPath(const String& deckId);
};
