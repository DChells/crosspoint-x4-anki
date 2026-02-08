#pragma once

#include <Arduino.h>

struct CrossPointSettings {
    enum SIDE_BUTTON_LAYOUT {
        STANDARD = 0,
        INVERTED = 1
    };

    int sideButtonLayout = STANDARD;
    int frontButtonBack = 0;
    int frontButtonConfirm = 1;
    int frontButtonLeft = 2;
    int frontButtonRight = 3;
};

// Global settings instance - declaring as extern would be better in real code, 
// but for shim we might need to define it or trick the linker.
// However, the error says SETTINGS is not declared, so we need a global instance or macro.
// Assuming it's a global object named SETTINGS.

extern CrossPointSettings SETTINGS;
