#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "cpu.h"


#define OPCODE_TABLE_SIZE 8
#define REG_TABLE_SIZE 8
#define MAX_PROGRAM_SIZE 60
#define OPCODE_MASK   0xE000  // 1110 0000 0000 0000
#define REG_MASK      0x1F00  // 0001 1111 0000 0000
#define OPERAND_MASK  0x00FF  // 0000 0000 1111 1111

typedef struct {
    const char *name; // human readable register name
    uint8_t code; // numeric register code
} Register;

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

Instruction* opcode_lu(char *mnemonic);
Instruction* create_instr(Instruction *format, char **args);
void print_instruction(const Instruction *instr);
int get_register_code(char *reg);
void write_to_file(Instruction* instr, char *filename);


int main() {
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), stdin)) {
        printf("Begin loop\n");
        char *args[3]; // holds op string and added args
        int args_idx = 0;

        // fetch next opcode
        char *user_instr = strtok(buffer, " ");
        while (user_instr != NULL) {
            args[args_idx++] = user_instr;
            user_instr = strtok(NULL, " ");
        }

        // After args[] is filled:
        for (int i = 0; i < args_idx; i++) {
            args[i][strcspn(args[i], "\n")] = '\0';
        }

        for (int i = 0; i < args_idx; i++) {
            printf("args[%d] = %s\n", i, args[i]);
        }

        // fetch appropriate format
        char *mnemonic_lookup = strtok(buffer, " ");
        Instruction* format = opcode_lu(mnemonic_lookup);

        // create instruction
        Instruction *instr = create_instr(format, args);
        print_instruction(instr);

        write_to_file(instr, "output.txt");
        printf("File succesfully written to\n");

    }
    
    printf("Executed succesfully, exiting\n");
    return 0;
}

Instruction* opcode_lu(char *mnemonic) {
    // locate opcode format
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        printf("%s == %s\n", mnemonic, opcode_lut[i].mnemonic);
        if (strcmp(mnemonic, opcode_lut[i].mnemonic) == 0) {
            // matching mnemonic, instruction format
            printf("Opcode match found\n");
            return &opcode_lut[i];
        }
    }
    printf("No opcode found\n");
    return NULL; // change to throw invalid instruction exception
}

Instruction* create_instr(Instruction *format, char **args) {
    // check for oob mem access
    int opcode = format->opcode; 
    if ((opcode == 0x0 || opcode == 0x5 || opcode == 0x6) && atoi(args[2]) > 0xFF) { // check mem location for load and jmp/jz
        return NULL;  
    }   
    if (opcode == 0x1 && atoi(args[1]) > 0xFF) { // check mem location for store
        return NULL;
    } 

    Instruction *instr = malloc(sizeof(Instruction));

    // set fields
    instr->mnemonic = args[0];
    instr->dest = args[1];
    instr->src = args[2];
    instr->opcode = format->opcode;

    return instr;
}

int get_register_code(char *reg) {
    for (int i = 0; i < REG_TABLE_SIZE; i++) {
        printf("%s == %s\n", register_lut[i].name, reg);
        if (strcmp(register_lut[i].name, reg) == 0) {
            return register_lut[i].code;
        }
    }
    printf("No register code found\n");
    return -1;
}

void write_to_file(Instruction *instr, char *filename) {
    
    int opcode = instr->opcode;
    // store immediate as destination if sto, jmp, jmz, or hlt instruction
    int destination = (opcode == 0x1 || opcode == 0x5 || opcode == 0x6 || opcode == 0x7) ? atoi(instr->dest) : get_register_code(instr->dest);

    // store immediate as source if lod, adi, jmp, or hlt instruction
    int source = (opcode == 0x0 || opcode == 0x3 || opcode == 0x5 || opcode == 0x7) ? atoi(instr->src) : get_register_code(instr->src);

    FILE *f = fopen(filename, "a");

    fprintf(f, "%d %d %d\n", opcode, destination, source);

    fclose(f);

}

void print_instruction(const Instruction *instr) {
    printf("Instruction:\n");
    printf("  Mnemonic:   %s\n", instr->mnemonic);
    printf("  Opcode:     0x%X (%d)\n", instr->opcode, instr->opcode);
    printf("  Dest Reg:   %s\n", instr->dest);    // print as string
    printf("  Mem Src:    %s\n", instr->src);     // print as string
}
