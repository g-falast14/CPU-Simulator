#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef struct {
    const char *mnemonic;    // human readable register name
    int opcode;              // 6 bit opcode
    char *dest;              // destination register
    char *src;               // source memory location
} Instruction;

Instruction opcode_lut[] = {
    {"LOD", 0x0}, // LOAD 5 
    {"STO", 0x1},
    {"ADD", 0x2},
    {"ADI", 0x3},
    {"AND", 0x4},
    {"JMP", 0x5},
    {"JMZ", 0x6},
    {"HLT", 0x7}
};

void execute_lod();

#endif 