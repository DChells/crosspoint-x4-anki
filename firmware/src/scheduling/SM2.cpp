#include "SM2.h"
#include <Arduino.h>
#include <cmath>
#include <algorithm>

namespace SM2 {

int calculateInterval(int repetitions, float easeFactor) {
    if (repetitions <= 0) {
        return 0;
    }
    if (repetitions == 1) {
        return 1;
    }
    if (repetitions == 2) {
        return 6;
    }
    
    float interval = 6.0f;
    for (int i = 3; i <= repetitions; ++i) {
        interval *= easeFactor;
    }
    
    return static_cast<int>(std::round(interval));
}

float updateEaseFactor(float currentEF, int quality) {
    float q = static_cast<float>(quality);
    float newEF = currentEF + (0.1f - (5.0f - q) * (0.08f + (5.0f - q) * 0.02f));
    
    if (newEF < 1.3f) {
        newEF = 1.3f;
    }
    
    return newEF;
}

}
