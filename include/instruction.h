#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef struct {
    const char *mnemonic;    // human readable register name
    int opcode;              // 6 bit opcode
    int dest;              // destination register
    int src;               // source memory location
} Instruction;

Instruction opcode_lut[] = {
    {"LOD", 0}, // LOAD 5 
    {"STO", 1},
    {"ADD", 2},
    {"ADI", 3},
    {"JMP", 4},
    {"JMZ", 5},
    {"JEQ", 6},
    {"HLT", 7}
};

void execute_lod();

#endif 