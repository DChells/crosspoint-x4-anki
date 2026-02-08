#include "DeckStorage.h"
#include <ArduinoJson.h>

#define ANKI_BASE_DIR "/.crosspoint/apps/anki"
#define DECKS_DIR ANKI_BASE_DIR "/decks"
#define PROGRESS_DIR ANKI_BASE_DIR "/progress"

bool CardStream::open(const String& deckId) {
    String path = DeckStorage::getDeckPath(deckId) + "/cards.jsonl";
    if (!SdMan.exists(path.c_str())) {
        return false;
    }
    file = SdMan.open(path.c_str(), O_RDONLY);
    return file && !file.isDirectory();
}

bool CardStream::hasNext() {
    return file && file.available();
}

Card CardStream::readNext() {
    if (!file || !file.available()) {
        return Card();
    }
    String line = file.readStringUntil('\n');
    return Card::fromJson(line);
}

void CardStream::close() {
    if (file) {
        file.close();
    }
}

std::vector<DeckMetadata> DeckStorage::listDecks() {
    std::vector<DeckMetadata> decks;
    
    if (!SdMan.ready()) return decks;
    
    FsFile decksDir = SdMan.open(DECKS_DIR, O_RDONLY);
    if (!decksDir || !decksDir.isDirectory()) {
        if (decksDir) decksDir.close();
        return decks;
    }
    
    char name[128];
    FsFile entry = decksDir.openNextFile();
    while (entry) {
        if (entry.isDirectory()) {
            entry.getName(name, sizeof(name));
            String deckId = String(name);
            DeckMetadata meta = loadDeckMetadata(deckId);
            if (!meta.id.isEmpty()) {
                decks.push_back(meta);
            }
        }
        entry.close();
        entry = decksDir.openNextFile();
    }
    decksDir.close();
    
    return decks;
}

DeckMetadata DeckStorage::loadDeckMetadata(const String& deckId) {
    String path = getDeckPath(deckId) + "/deck-metadata.json";
    if (!SdMan.exists(path.c_str())) {
        return DeckMetadata();
    }
    
    String json = SdMan.readFile(path.c_str());
    if (json.isEmpty()) {
        return DeckMetadata();
    }
    
    return DeckMetadata::fromJson(json);
}

bool DeckStorage::openCardStream(const String& deckId, CardStream& stream) {
    return stream.open(deckId);
}

DeckProgress DeckStorage::loadProgress(const String& deckId) {
    String path = getProgressPath(deckId);
    if (!SdMan.exists(path.c_str())) {
        DeckProgress progress;
        progress.deckId = deckId;
        return progress;
    }
    
    String json = SdMan.readFile(path.c_str());
    if (json.isEmpty()) {
        DeckProgress progress;
        progress.deckId = deckId;
        return progress;
    }
    
    return DeckProgress::fromJson(json);
}

bool DeckStorage::saveProgress(const String& deckId, const DeckProgress& progress) {
    String path = getProgressPath(deckId);
    String tmpPath = path + ".tmp";
    
    if (!SdMan.ensureDirectoryExists(PROGRESS_DIR)) {
        return false;
    }
    
    String json = progress.toJson();
    if (!SdMan.writeFile(tmpPath.c_str(), json)) {
        return false;
    }
    
    if (SdMan.exists(path.c_str())) {
        SdMan.remove(path.c_str());
    }
    
    return SdMan.rename(tmpPath.c_str(), path.c_str());
}

String DeckStorage::getDeckPath(const String& deckId) {
    return String(DECKS_DIR) + "/" + deckId;
}

String DeckStorage::getProgressPath(const String& deckId) {
    return String(PROGRESS_DIR) + "/" + deckId + ".json";
}
