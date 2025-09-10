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

#endif