#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "fileops.h"

#define DEFAULT_HEAP_SIZE 256
#define MAIN_MEM_SIZE 2048
#define PCB_TABLE_SIZE 10

typedef struct {
    int pid;
    enum JobState state;
    int priority;
    int pc;
    int *registers;
    int instructionBase; // where instructions begin in logical mem
    int mainMemBase; // first address in main mem
    int mainMemLimit; // last address in main mem
    int requiredSize; // how much memory is allocated
} PCB;

typedef struct Node {
    PCB job;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
} Queue;

// global data
int mainMemory[MAIN_MEM_SIZE]; // main memory table
int mainMemoryBitmap[MAIN_MEM_SIZE]; // represents whether an address is allocated
PCB *PCBTable[PCB_TABLE_SIZE]; 
int nextPCBIdx = 0; // next available pcb table index

// methods
PCB* createProcess(char *processInfo);
void printPCB(PCB *pcb);
int allocatePID();
int allocateMemory(PCB *pcb);

int main() {
  
    char buffer[256];
    int numProcesses;

    // read program metadata
    fgets(buffer, sizeof(buffer), stdin);

    // create process control block
    PCB* pcb = createProcess(buffer);

    // allocate main memory
    int test = allocateMemory(pcb);

    // load program into memory
    loadProgram(pcb);

    while (fgets(buffer, sizeof(buffer), stdin)) {
        // fetch instruction
        char *mnemonic = strtok(buffer, " ");


        // check for new program
        
        
        

        
        printf("Base address: %d\n", test);


        // TODO: load process into main memory
        // loadProcess(pcb);


        printPCB(pcb);
    }

    return 0;
}

PCB* createProcess(char *processInfo) {
    PCB *pcb = malloc(sizeof(PCB));
    int requiredSize, instructionBase, mainMemLimit, pid;

    // parse process info
    char *token = strtok(processInfo, " ");
    requiredSize = atoi(token); // first int is required memory size

    token = strtok(NULL, " ");
    instructionBase = atoi(token); // second int is instruction base

    pid = allocatePID();

    // set appropriate fields
    pcb->pid = pid;
    pcb->instructionBase = instructionBase;
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

int allocatePID() {
    for (int i = 0; i < PCB_TABLE_SIZE; i++) {
        if (PCBTable[i] != NULL) {
            return i;
        }
    }
    return -1;
}

int allocateMemory(PCB *pcb) {
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

void loadProgram() {

}

void tokenizeInstruction(char *buffer) {
    
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

