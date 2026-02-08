#pragma once
#include <WString.h>
#include <map>
#include <ArduinoJson.h>

struct CardProgress {
    float ease;
    int interval;
    int repetitions;
    String due;
    
    CardProgress() : ease(2.5f), interval(0), repetitions(0) {}
};

struct DeckProgress {
    String deckId;
    String lastReview;
    std::map<String, CardProgress> cards;
    
    static DeckProgress fromJson(const String& json) {
        DeckProgress progress;
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, json);
        
        if (!error) {
            progress.deckId = doc["deckId"].as<String>();
            progress.lastReview = doc["lastReview"].as<String>();
            
            JsonObject cardsObj = doc["cards"];
            for (JsonPair p : cardsObj) {
                CardProgress cp;
                cp.ease = p.value()["ease"].as<float>();
                cp.interval = p.value()["interval"].as<int>();
                cp.repetitions = p.value()["repetitions"].as<int>();
                cp.due = p.value()["due"].as<String>();
                progress.cards[p.key().c_str()] = cp;
            }
        }
        return progress;
    }
    
    String toJson() const {
        JsonDocument doc;
        doc["deckId"] = deckId;
        doc["lastReview"] = lastReview;
        
        JsonObject cardsObj = doc["cards"].to<JsonObject>();
        for (auto const& pair : cards) {
            const String& id = pair.first;
            const CardProgress& cp = pair.second;
            JsonObject cpObj = cardsObj[id].to<JsonObject>();
            cpObj["ease"] = cp.ease;
            cpObj["interval"] = cp.interval;
            cpObj["repetitions"] = cp.repetitions;
            cpObj["due"] = cp.due;
        }
        
        String output;
        serializeJson(doc, output);
        return output;
    }
    
    CardProgress& getCardProgress(const String& cardId) {
        return cards[cardId];
    }
};
