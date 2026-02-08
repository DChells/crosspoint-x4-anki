#pragma once

namespace SM2 {
    int calculateInterval(int repetitions, float easeFactor);
    
    float updateEaseFactor(float currentEF, int quality);
    
    enum Quality {
        AGAIN = 0,
        HARD = 2,
        GOOD = 3,
        EASY = 5
    };
}
