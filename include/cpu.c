#include "cpu.h"

Register register_lut[] = {
    {"GPA", 0}, // general purpose a
    {"GPB", 1}, // general purpose b
    {"GPC", 2}, // general purpose c
    {"GPD", 3}, // general purpose d
    {"PGC", 4}, // program counter
    {"STP", 5}, // stack pointer
    {"ISR", 6}, // instruction register
    {"FLG", 7}  // flags
};
