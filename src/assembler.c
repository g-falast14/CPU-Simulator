#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "cpu.h"
#include "processops.h"

#define OPCODE_TABLE_SIZE 8
#define REG_TABLE_SIZE 8
#define MAX_PROGRAM_SIZE 60
#define OPCODE_MASK   0xE000  // 1110 0000 0000 0000
#define REG_MASK      0x1F00  // 0001 1111 0000 0000
#define OPERAND_MASK  0x00FF  // 0000 0000 1111 1111

Instruction* opcode_lu(char *mnemonic);
Instruction* create_instr(Instruction *format, char **args);
void print_instruction(const Instruction *instr);
int get_register_code(char *reg);
void write_to_file(Instruction* instr, char *filename);


int main() {
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), stdin)) {
        char *args[3]; // holds op string and added args
        int args_idx = 0;

        // fetch next opcode
        char *user_instr = strtok(buffer, " ");
        while (user_instr != NULL) {
            args[args_idx++] = user_instr;
            user_instr = strtok(NULL, " ");
        }

        // After args[] is filled, replace newlines with nulls
        for (int i = 0; i < args_idx; i++) {
            args[i][strcspn(args[i], "\n")] = '\0';
        } 

        // fetch appropriate format
        char *mnemonic_lookup = strtok(buffer, " ");
        Instruction* format = opcode_lu(mnemonic_lookup);

        // create instruction
        Instruction *instr = create_instr(format, args);

        write_to_file(instr, "output2.txt");
    }
    
    printf("Executed succesfully, exiting\n");
    return 0;
}

Instruction* opcode_lu(char *mnemonic) {
    // locate opcode format
    for (int i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (strcmp(mnemonic, opcode_lut[i].mnemonic) == 0) {
            // matching mnemonic, instruction format
            return &opcode_lut[i];
        }
    }
    return NULL; // change to throw invalid instruction exception
}

Instruction* create_instr(Instruction *format, char **args) {
    // TODO check for oob mem access
    /*
    int opcode = format->opcode; 
    if ((opcode == 0 || opcode == 5 || opcode == 6) && atoi(args[2]) > 0xFF) { // check mem location for load and jmp/jz
        return NULL;  
    }   
    if (opcode == 0x1 && atoi(args[1]) > 2048) { // check mem location for store
        return NULL;
    }  */

    Instruction *instr = malloc(sizeof(Instruction));

    int opcode = format->opcode;
    int destination = -1, source = -1;

    if (opcode == 0 || opcode == 2 || opcode == 3) { // lookup register code for LOD, ADD, ADI
        destination = get_register_code(args[1]);
    } else {  // otherwise, store passed memory location 
        destination = atoi(args[1]);
    }

    if (opcode == 1 || opcode == 2 || opcode == 5 || opcode == 6) { // lookup register code for STO, ADD, JMQ, JEQ
        source = get_register_code(args[2]);
    } else {  // otherwise, store passed memory location 
        source = atoi(args[2]);
    }

    // set fields
    instr->mnemonic = args[0];
    instr->dest = destination; //args[1];
    instr->src = source; //args[2];
    instr->opcode = format->opcode;

    return instr;
}

int get_register_code(char *reg) {
    for (int i = 0; i < REG_TABLE_SIZE; i++) {
        if (strcmp(register_lut[i].name, reg) == 0) {
            return register_lut[i].code;
        }
    }
    printf("No register code found\n");
    return -1;
}

void write_to_file(Instruction *instr, char *filename) {

    FILE *f = fopen(filename, "a");

    fprintf(f, "%d %d %d\n", instr->opcode, instr->dest, instr->src);

    fclose(f);
}

void print_instruction(const Instruction *instr) {
    printf("Instruction:\n");
    printf("  Mnemonic:   %s\n", instr->mnemonic);
    printf("  Opcode:     0x%X (%d)\n", instr->opcode, instr->opcode);
    printf("  Destination:   %d\n", instr->dest);    // print as string
    printf("  Source:    %d\n", instr->src);     // print as string
}