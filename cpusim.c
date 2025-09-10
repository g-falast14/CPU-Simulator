#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "processops.h"
#include "cpu.h"

#define DEFAULT_HEAP_SIZE 256
#define MAIN_MEM_SIZE 2048
#define PCB_TABLE_SIZE 10
#define NUM_COMMANDS 2

// Structs
// command struct for global table
typedef struct {
    const char *name;
    void (*func)(char**, int);  
} Command;

// individual process control block
typedef struct {
    int pid; 
    int priority;
    int pc; // program counter
    int sp; // stack pointer
    int state; // current execution state
    int *registers;
    int instructionBase; // where instructions begin in logical mem
    int mainMemBase; // first address in main mem
    int mainMemLimit; // last address in main mem
    int requiredSize; // how much memory is allocated
} PCB;

// process state enum
typedef enum {
    NEW = 0,        // Just created, not yet admitted to ready queue
    READY = 1,      // Waiting to be scheduled on CPU
    RUNNING = 2,    // Currently executing on CPU
    WAITING = 3,    // Waiting for I/O or an event
    TERMINATED = -1  // Finished execution
} ProcessState;

// methods
void execute_program(char **tokens, int token_count);
//void goodbye();
int is_blank(const char *s);
Command* find_command(const char *name);

PCB* create_process(char **tokens, int token_count);
void print_PCB(PCB *pcb);
int allocate_PID();
int allocate_mem(PCB *pcb);
void load_program(char *filename);

Command commands[] = {
    {"exe", execute_program}
    //{"goodbye", goodbye}
};

// global data
CPU *cpu;
int mainMemory[MAIN_MEM_SIZE]; // main memory table
int mainMemoryBitmap[MAIN_MEM_SIZE]; // represents whether an address is allocated
PCB *PCBTable[PCB_TABLE_SIZE]; 
int nextPCBIdx = 0; // next available pcb table index



int main() {
    
    char user_inp[100]; // stdin array
    cpu = malloc(sizeof(cpu)); // single-core cpu


    while (1) {
        printf("gcmd > ");

        fgets(user_inp, sizeof(user_inp), stdin);
        
        /* check for empty input
        if (is_blank(user_inp)) {
            continue;
        } */

        char *command = strtok(user_inp, " ");
        // check for exit
        if (strcmp(command, "goodbye") == 0) {
            break;
        }

        // tokenize input
        char *tokens[10];
        int token_count = 0;
        while (command != NULL) {
            tokens[token_count++] = command;
            command = strtok(NULL, " ");
        }

        Command* cmd = find_command(tokens[0]);
        
        if (cmd) {
            cmd->func(tokens, token_count);
        } else {
            printf("Command %s not found\n", cmd->name);
        }
        // end loop
    }

    return 0;
}

void execute_program(char **tokens, int token_count) {
    PCB *pcb = create_process(tokens, token_count);

    if (allocate_mem(pcb) == 67) {
        // ERROR
    }

    // read instructions and load program into memory
    load_program("temp");

}


PCB* create_process(char **tokens, int token_count) {
    PCB *pcb = malloc(sizeof(PCB));
    int requiredSize, instructionBase, mainMemLimit, pid;

    int instr_count = count_instr(tokens[0]);

    pid = allocate_PID();

    // set appropriate fields
    pcb->pid = pid;
    pcb->instructionBase = 0; // change in future 
    pcb->state = NEW;
    pcb->priority = -1; // default priority
    pcb->registers = malloc(sizeof(int) * 8); // 8 temp registers
    pcb->mainMemLimit = mainMemLimit;
    pcb->requiredSize = requiredSize;

    // set next pointer in pcb table
    PCBTable[nextPCBIdx++] = pcb;

    // return pointer to newly created pcb object
    return pcb;
}

int allocate_PID() {
    for (int i = 0; i < PCB_TABLE_SIZE; i++) {
        if (PCBTable[i] != NULL) {
            return i;
        }
    }
    return -1;
}

int allocate_mem(PCB *pcb) {
    int targetMemSize = pcb->requiredSize;
    int tail = 0, front = 0; // sliding window

    while (front < MAIN_MEM_SIZE) {
        // If current slot is free
        if (mainMemoryBitmap[front] == 0) {
            // Check if window is large enough
           // printf("%d\n", front - tail + 1);
            if (front - tail + 1 == targetMemSize) {
                // Allocate memory
                for (int i = tail; i <= front; i++) {
                    mainMemoryBitmap[i] = 1;
                }
                // set main memory base and limit
                pcb->mainMemBase = tail;
                pcb->mainMemLimit = tail + pcb->requiredSize;
                return tail; // base address
            }
            front++;
        } else {
            // Encountered occupied slot, reset window
            front++;
            tail = front;
        }
    }

    // No space found
    return 67;
}

// TODO fix to check for blank user input
/*
int is_blank(const char *s) {
    while (*s) {
        if (!isspace((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
} */

Command* find_command(const char *name) {
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp(name, commands[i].name) == 0) {
            return &commands[i];  // return pointer to matching command
        }
    }
    return NULL; // not found
}

void load_program(char *filename) {

}

void printPCB(PCB *pcb) {
    printf("PID: %d\n", pcb->pid);
    printf("State: %d\n", pcb->state);
    printf("Priority: %d\n", pcb->priority);
    printf("PC: %d\n", pcb->pc);
    printf("Instruction Base: %d\n", pcb->instructionBase);
    printf("Main Memory Base: %d\n", pcb->mainMemBase);
    printf("Main Memory Limit: %d\n", pcb->mainMemLimit);
}
