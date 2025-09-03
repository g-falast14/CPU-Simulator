#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPCODE_TABLE_SIZE 1
#define MAX_PROGRAM_SIZE 60
#define OPCODE_MASK   0xE000  // 1110 0000 0000 0000
#define REG_MASK      0x1F00  // 0001 1111 0000 0000
#define OPERAND_MASK  0x00FF  // 0000 0000 1111 1111

typedef struct {
    const char *mnemonic;    // human readable register name
    int opcode;              // 6 bit opcode
    char *dest;              // destination register
    char *src;               // source memory location
} Instruction;

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
void translate(Instruction *instr);

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

        // fetch appropriate format
        char *mnemonic_lookup = strtok(buffer, " ");
        Instruction* format = opcode_lu(mnemonic_lookup);

        // create instruction
        Instruction *instr = create_instr(format, args);

        // translate into machine code
       // writeToFile();

        print_instruction(instr);
    }
    
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
    // check for oob mem access
    uint8_t opcode = (format->opcode & 0xE000) >> 13; 
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

void print_instruction(const Instruction *instr) {
    printf("Instruction:\n");
    printf("  Mnemonic:   %s\n", instr->mnemonic);
    printf("  Opcode:     0x%X (%d)\n", instr->opcode, instr->opcode);
    printf("  Dest Reg:   %s\n", instr->dest);    // print as string
    printf("  Mem Src:    %s\n", instr->src);     // print as string
}

int get_register_code(Instruction* instr) {
    for (int i = 0; i < sizeof(register_lut); i++) {
        if (strcmp(register_lut[i].name, instr->dest) == 0) {
            return register_lut[i].code;
        }
    }
    return -1;
}

void writeToFile(Instruction *instr, char *filename) {
    
    int opcode = instr->opcode;

    // store immediate as destination if sto, jmp, jmz, or hlt instruction
    int destination = (opcode == 0x1 || opcode == 0x5 || opcode == 0x6 || opcode == 0x7) ? instr->dest : get_register_code(instr);

    //TODO NEED TO TRANSLATE THIS TO MACHINE HEX!!

    // store immediate as source if lod, adi, jmp, or hlt instruction
    int source = (opcode == 0x0 || opcode == 0x3 || opcode == 0x5 || opcode == 0x7) ? instr->dest : get_register_code(instr);

    FILE *f = fopen(filename, "w");
    char line[100];
    // snprintf(buffer, sizeof(buffer), "%d %d %d", );
    fprintf(f, "%d %d %d\n", opcode, destination, source);


}

