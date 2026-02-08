#pragma once

#include <Arduino.h>

#define REPLACEMENT_GLYPH 0xFFFD

static uint32_t utf8NextCodepoint(const uint8_t** str) {
    if (!*str || !**str) return 0;
    
    const uint8_t* s = *str;
    uint32_t cp = 0;
    
    if (*s < 0x80) {
        cp = *s++;
    } else if ((*s & 0xE0) == 0xC0) {
        cp = (*s++ & 0x1F) << 6;
        if ((*s & 0xC0) == 0x80) { cp |= (*s++ & 0x3F); }
    } else if ((*s & 0xF0) == 0xE0) {
        cp = (*s++ & 0x0F) << 12;
        if ((*s & 0xC0) == 0x80) { cp |= (*s++ & 0x3F) << 6; }
        if ((*s & 0xC0) == 0x80) { cp |= (*s++ & 0x3F); }
    } else if ((*s & 0xF8) == 0xF0) {
        cp = (*s++ & 0x07) << 18;
        if ((*s & 0xC0) == 0x80) { cp |= (*s++ & 0x3F) << 12; }
        if ((*s & 0xC0) == 0x80) { cp |= (*s++ & 0x3F) << 6; }
        if ((*s & 0xC0) == 0x80) { cp |= (*s++ & 0x3F); }
    } else {
        s++; // Invalid
        cp = REPLACEMENT_GLYPH;
    }
    
    *str = s;
    return cp;
}

static void utf8RemoveLastChar(std::string& str) {
    if (str.empty()) return;
    
    while (!str.empty()) {
        char c = str.back();
        str.pop_back();
        if ((c & 0xC0) != 0x80) break; // Stop at start byte or ASCII
    }
}
