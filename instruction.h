#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef struct {
    const char *mnemonic;    // human readable register name
    int opcode;              // 6 bit opcode
    char *dest;              // destination register
    char *src;               // source memory location
} Instruction;

#endif 