#include "Arduino.h"

extern void setup(void);
extern void loop(void);

// Initialize system
void init(void) {
    clock_init();
}

// Main entry point
void main(void) {
    init();
    setup();
    
    while(1) {
        loop();
    }
}
