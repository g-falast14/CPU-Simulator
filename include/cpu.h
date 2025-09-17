#ifndef CPU_H   // include guard (prevents duplicate inclusion)
#define CPU_H

typedef struct {
    const char *name;
    int code;
    int value;
} Register;

typedef struct {
    const char *name;
    int code;
    char *value;
} Instr_Register;

typedef struct {
    Register GPA;
    Register GPB;
    Register GPC;
    Register GPD;
    Register PGC;
    Register STP;
    Instr_Register ISR;
    Register FLG;
} CPU;


extern Register register_lut[];
int get_register_code(char *reg);

#endif