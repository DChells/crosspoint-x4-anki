#pragma once
#include <WString.h>
#include <vector>
#include <ArduinoJson.h>

struct DeckMetadata {
    String id;
    String name;
    String description;
    int cardCount;
    String created;
    std::vector<String> tags;
    String filePath;

    static DeckMetadata fromJson(const String& json) {
        DeckMetadata meta;
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, json);
        
        if (!error) {
            meta.id = doc["id"].as<String>();
            meta.name = doc["name"].as<String>();
            meta.description = doc["description"].as<String>();
            meta.cardCount = doc["cardCount"].as<int>();
            meta.created = doc["created"].as<String>();
            
            JsonArray tagsArray = doc["tags"];
            for (JsonVariant tag : tagsArray) {
                meta.tags.push_back(tag.as<String>());
            }
            
            meta.filePath = doc["filePath"].as<String>();
        }
        return meta;
    }
    
    String toJson() const {
        JsonDocument doc;
        doc["id"] = id;
        doc["name"] = name;
        doc["description"] = description;
        doc["cardCount"] = cardCount;
        doc["created"] = created;
        
        JsonArray tagsArray = doc["tags"].to<JsonArray>();
        for (const String& tag : tags) {
            tagsArray.add(tag);
        }
        
        doc["filePath"] = filePath;
        
        String output;
        serializeJson(doc, output);
        return output;
    }
};
