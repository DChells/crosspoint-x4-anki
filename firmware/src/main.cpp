#include <Arduino.h>
#include <GfxRenderer.h>
#include <HalDisplay.h>
#include <HalGPIO.h>
#include <SDCardManager.h>
#include <builtinFonts/all.h>

#include "MappedInputManager.h"
#include "activities/MainMenuActivity.h"
#include "utils/BootUtils.h"
#include "fontIds.h"

// Hardware
HalDisplay display;
HalGPIO gpio;
MappedInputManager mappedInputManager(gpio);
GfxRenderer renderer(display);

// Current activity
Activity* currentActivity = nullptr;

// Fonts - using a minimal set for the app
EpdFont bookerly14RegularFont(&bookerly_14_regular);
EpdFont bookerly14BoldFont(&bookerly_14_bold);
EpdFont bookerly14ItalicFont(&bookerly_14_italic);
EpdFont bookerly14BoldItalicFont(&bookerly_14_bolditalic);
EpdFontFamily bookerly14FontFamily(&bookerly14RegularFont, &bookerly14BoldFont, 
                                   &bookerly14ItalicFont, &bookerly14BoldItalicFont);

EpdFont ui12RegularFont(&ubuntu_12_regular);
EpdFont ui12BoldFont(&ubuntu_12_bold);
EpdFontFamily ui12FontFamily(&ui12RegularFont, &ui12BoldFont);

EpdFont smallFont(&notosans_8_regular);
EpdFontFamily smallFontFamily(&smallFont);

// Power button hold duration for sleep (ms)
static constexpr unsigned long POWER_BUTTON_DURATION = 1000;
// Auto-sleep timeout (ms) - 5 minutes
static constexpr unsigned long SLEEP_TIMEOUT_MS = 5 * 60 * 1000;

void exitActivity() {
    if (currentActivity) {
        currentActivity->onExit();
        delete currentActivity;
        currentActivity = nullptr;
    }
}

void enterNewActivity(Activity* activity) {
    currentActivity = activity;
    currentActivity->onEnter();
}

void enterDeepSleep() {
    Serial.printf("[%lu] Entering deep sleep...\n", millis());
    
    // Clear screen before sleep
    renderer.clearScreen();
    renderer.drawCenteredText(UI_12_FONT_ID, renderer.getScreenHeight() / 2, "Sleeping...", true);
    renderer.displayBuffer(HalDisplay::FAST_REFRESH);
    
    display.deepSleep();
    gpio.startDeepSleep();
}

void showError(const char* message) {
    renderer.clearScreen();
    renderer.drawCenteredText(UI_12_FONT_ID, renderer.getScreenHeight() / 2 - 20, "Error", true, EpdFontFamily::BOLD);
    renderer.drawCenteredText(UI_12_FONT_ID, renderer.getScreenHeight() / 2 + 20, message, true);
    renderer.displayBuffer(HalDisplay::FAST_REFRESH);
}

void setup() {
    // Initialize GPIO first
    gpio.begin();
    
    // Start serial if USB connected
    Serial.begin(115200);
    unsigned long start = millis();
    while (!Serial && (millis() - start) < 2000) {
        delay(10);
    }
    
    Serial.printf("[%lu] Anki X4 Starting (version %s)\n", millis(), ANKI_VERSION);
    
    // Initialize SD card
    if (!SdMan.begin()) {
        Serial.printf("[%lu] SD card initialization failed!\n", millis());
        display.begin();
        renderer.insertFont(UI_12_FONT_ID, ui12FontFamily);
        showError("SD card error");
        delay(3000);
        CrossPoint::returnToCrossPoint();
        return;
    }
    Serial.printf("[%lu] SD card initialized\n", millis());
    
    // Initialize display
    display.begin();
    Serial.printf("[%lu] Display initialized\n", millis());
    
    // Setup fonts
    renderer.insertFont(BOOKERLY_14_FONT_ID, bookerly14FontFamily);
    renderer.insertFont(UI_12_FONT_ID, ui12FontFamily);
    renderer.insertFont(SMALL_FONT_ID, smallFontFamily);
    Serial.printf("[%lu] Fonts loaded\n", millis());
    
    // Show boot screen
    renderer.clearScreen();
    renderer.drawCenteredText(UI_12_FONT_ID, renderer.getScreenHeight() / 2 - 20, "Anki X4", true, EpdFontFamily::BOLD);
    renderer.drawCenteredText(SMALL_FONT_ID, renderer.getScreenHeight() / 2 + 20, ANKI_VERSION, true);
    renderer.displayBuffer(HalDisplay::FAST_REFRESH);
    
    delay(500);
    
    // Enter main menu
    exitActivity();
    enterNewActivity(new MainMenuActivity(renderer, mappedInputManager));
    
    Serial.printf("[%lu] Setup complete, entering main loop\n", millis());
}

void loop() {
    static unsigned long lastActivityTime = millis();
    static unsigned long lastMemPrint = 0;
    
    // Update button states
    gpio.update();
    
    // Memory logging (every 10 seconds)
    if (Serial && millis() - lastMemPrint >= 10000) {
        Serial.printf("[%lu] [MEM] Free: %d bytes, Min Free: %d bytes\n", 
                      millis(), ESP.getFreeHeap(), ESP.getMinFreeHeap());
        lastMemPrint = millis();
    }
    
    // Check for user activity
    if (gpio.wasAnyPressed() || gpio.wasAnyReleased()) {
        lastActivityTime = millis();
    }
    
    // Auto-sleep after timeout
    if (millis() - lastActivityTime >= SLEEP_TIMEOUT_MS) {
        Serial.printf("[%lu] Auto-sleep triggered\n", millis());
        enterDeepSleep();
        return;
    }
    
    // Power button long press -> sleep
    if (gpio.isPressed(HalGPIO::BTN_POWER) && gpio.getHeldTime() > POWER_BUTTON_DURATION) {
        enterDeepSleep();
        return;
    }
    
    // Run current activity
    if (currentActivity) {
        currentActivity->loop();
    }
    
    // Small delay to prevent tight spinning
    delay(10);
}
