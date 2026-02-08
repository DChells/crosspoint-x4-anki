#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SDCardManager.h>

// Static files directory on SD card
#define WEB_ROOT "/.crosspoint/apps/anki/web"
#define UPLOAD_TEMP_DIR "/.crosspoint/apps/anki/temp"

// Upload limits
#define MAX_UPLOAD_SIZE (10 * 1024 * 1024)  // 10MB
#define UPLOAD_BUFFER_SIZE 1024              // 1KB chunks for streaming

class WebServer {
public:
    WebServer(uint16_t port = 80);
    
    // Initialize and start the web server
    bool begin();
    
    // Stop the web server
    void end();
    
    // Check if server is running
    bool isRunning() const { return running; }

private:
    AsyncWebServer server;
    bool running = false;
    
    // Setup routes
    void setupRoutes();
    
    // Static file handler - serves files from SD card
    void handleStaticFile(AsyncWebServerRequest* request);
    
    // Upload handler - streams JSONL data directly to SD card
    void handleUploadDeck(AsyncWebServerRequest* request, String filename, size_t index, 
                          uint8_t* data, size_t len, bool final);
    
    // Helper to get content type from file extension
    String getContentType(const String& filename);
    
    // Helper to get MIME type for web files
    String getMimeType(const String& path);
};
