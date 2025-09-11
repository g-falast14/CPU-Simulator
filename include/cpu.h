#ifndef CPU_H   // include guard (prevents duplicate inclusion)
#define CPU_H

typedef struct {
    const char *name; // human readable register name
    int code; // numeric register code
    int value; // numeric cpu value
} Register;

typedef struct {
    Register GPA;
    Register GPB;
    Register GPC;
    Register GPD;
    Register PGC;
    Register STP;
    Register ISR;
    Register FLG;
} CPU;

Register register_lut[] = {
    {"GPA", 0x0}, // general purpose a
    {"GPB", 0x1}, // general purpose b
    {"GPC", 0x2}, // general purpose c
    {"GPD", 0x3}, // general purpose d
    {"PGC", 0x4}, // program counter
    {"STP", 0x5}, // stack pointer
    {"ISR", 0x6}, // instruction register
    {"FLG", 0x7}  // flags
};

#endif