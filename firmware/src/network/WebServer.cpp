#include "WebServer.h"
#include <WiFi.h>

WebServer::WebServer(uint16_t port) : server(port) {}

bool WebServer::begin() {
    if (running) {
        return true;
    }
    
    if (!SdMan.ready()) {
        Serial.println("WebServer: SD card not ready");
        return false;
    }
    
    setupRoutes();
    server.begin();
    running = true;
    
    Serial.println("WebServer: Started on port " + String(80));
    return true;
}

void WebServer::end() {
    if (running) {
        server.end();
        running = false;
        Serial.println("WebServer: Stopped");
    }
}

void WebServer::setupRoutes() {
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->redirect("/upload.html");
    });
    
    server.on("/upload-deck", HTTP_POST, 
        [](AsyncWebServerRequest* request) {},
        [this](AsyncWebServerRequest* request, String filename, size_t index, 
               uint8_t* data, size_t len, bool final) {
            handleUploadDeck(request, filename, index, data, len, final);
        }
    );
    
    server.onNotFound([this](AsyncWebServerRequest* request) {
        handleStaticFile(request);
    });
}

void WebServer::handleStaticFile(AsyncWebServerRequest* request) {
    String path = request->url();

    if (path.endsWith("/")) {
        path += "index.html";
    }

    if (path.indexOf("..") != -1 || path.indexOf("//") != -1) {
        request->send(403, "text/plain", "Forbidden");
        return;
    }

    String fullPath = String(WEB_ROOT) + path;

    if (!SdMan.exists(fullPath.c_str())) {
        request->send(404, "text/plain", "File not found");
        return;
    }

    FsFile* file = new FsFile(SdMan.open(fullPath.c_str(), O_RDONLY));
    if (!file || !(*file) || file->isDirectory()) {
        if (file && (*file)) file->close();
        delete file;
        request->send(404, "text/plain", "File not found");
        return;
    }

    size_t fileSize = file->size();
    String contentType = getMimeType(path);

    AsyncWebServerResponse* response = request->beginResponse(
        contentType,
        fileSize,
        [file](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
            if (!file || !(*file)) return 0;
            size_t bytesRead = file->read(buffer, maxLen);
            if (bytesRead == 0 || !file->available()) {
                file->close();
                delete file;
            }
            return bytesRead;
        }
    );

    response->addHeader("Cache-Control", "public, max-age=3600");
    request->send(response);
}

void WebServer::handleUploadDeck(AsyncWebServerRequest* request, String filename, size_t index,
                                  uint8_t* data, size_t len, bool final) {
    static FsFile uploadFile;
    static size_t totalBytes = 0;
    static String tempPath;
    static bool hasError = false;
    
    if (index == 0) {
        totalBytes = 0;
        hasError = false;
        uploadFile.close();
        
        if (!request->hasParam("deckId", true)) {
            request->send(400, "application/json", "{\"error\":\"Missing deckId parameter\"}");
            hasError = true;
            return;
        }
        
        String deckId = request->getParam("deckId", true)->value();
        if (deckId.length() == 0 || deckId.length() > 64) {
            request->send(400, "application/json", "{\"error\":\"Invalid deckId\"}");
            hasError = true;
            return;
        }
        
        for (size_t i = 0; i < deckId.length(); i++) {
            char c = deckId[i];
            if (!isalnum(c) && c != '-' && c != '_') {
                request->send(400, "application/json", "{\"error\":\"Invalid deckId characters\"}");
                hasError = true;
                return;
            }
        }
        
        SdMan.ensureDirectoryExists(UPLOAD_TEMP_DIR);
        tempPath = String(UPLOAD_TEMP_DIR) + "/" + deckId + ".jsonl.tmp";
        
        if (SdMan.exists(tempPath.c_str())) {
            SdMan.remove(tempPath.c_str());
        }
        
        uploadFile = SdMan.open(tempPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
        if (!uploadFile) {
            request->send(500, "application/json", "{\"error\":\"Failed to create upload file\"}");
            hasError = true;
            return;
        }
        
        Serial.println("WebServer: Starting upload for deck: " + deckId);
    }
    
    if (hasError) {
        return;
    }
    
    if (len > 0 && uploadFile) {
        totalBytes += len;
        
        if (totalBytes > MAX_UPLOAD_SIZE) {
            uploadFile.close();
            SdMan.remove(tempPath.c_str());
            request->send(413, "application/json", "{\"error\":\"File too large (max 10MB)\"}");
            hasError = true;
            return;
        }
        
        size_t written = uploadFile.write(data, len);
        if (written != len) {
            uploadFile.close();
            SdMan.remove(tempPath.c_str());
            request->send(500, "application/json", "{\"error\":\"Write failed\"}");
            hasError = true;
            return;
        }
    }
    
    if (final) {
        uploadFile.close();
        
        if (hasError) {
            return;
        }
        
        String deckId = request->getParam("deckId", true)->value();
        String deckDir = String("/.crosspoint/apps/anki/decks/") + deckId;
        String finalPath = deckDir + "/cards.jsonl";
        
        SdMan.ensureDirectoryExists(deckDir.c_str());
        
        if (SdMan.exists(finalPath.c_str())) {
            SdMan.remove(finalPath.c_str());
        }
        
        if (!SdMan.rename(tempPath.c_str(), finalPath.c_str())) {
            SdMan.remove(tempPath.c_str());
            request->send(500, "application/json", "{\"error\":\"Failed to finalize upload\"}");
            return;
        }
        
        Serial.println("WebServer: Upload complete for deck: " + deckId + " (" + String(totalBytes) + " bytes)");
        
        String response = "{\"success\":true,\"bytes\":" + String(totalBytes) + ",\"deckId\":\"" + deckId + "\"}";
        request->send(200, "application/json", response);
    }
}

String WebServer::getMimeType(const String& path) {
    if (path.endsWith(".html")) return "text/html";
    if (path.endsWith(".htm")) return "text/html";
    if (path.endsWith(".css")) return "text/css";
    if (path.endsWith(".js")) return "application/javascript";
    if (path.endsWith(".json")) return "application/json";
    if (path.endsWith(".wasm")) return "application/wasm";
    if (path.endsWith(".png")) return "image/png";
    if (path.endsWith(".jpg")) return "image/jpeg";
    if (path.endsWith(".jpeg")) return "image/jpeg";
    if (path.endsWith(".gif")) return "image/gif";
    if (path.endsWith(".svg")) return "image/svg+xml";
    if (path.endsWith(".ico")) return "image/x-icon";
    if (path.endsWith(".woff")) return "font/woff";
    if (path.endsWith(".woff2")) return "font/woff2";
    if (path.endsWith(".ttf")) return "font/ttf";
    if (path.endsWith(".txt")) return "text/plain";
    return "application/octet-stream";
}
