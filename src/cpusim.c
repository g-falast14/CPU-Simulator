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

// individual process control block
typedef struct {
    int pid; 
    int priority;
    int pc; // program counter
    int sp; // stack pointer
    int state; // current execution state
    int *registers;
    int main_mem_base; // first address in main mem
    int main_mem_limit;
    int instr_count; // how much memory is allocated
} PCB;

// process state enum
typedef enum {
    NEW = 0,        // Just created, not yet admitted to ready queue
    READY = 1,      // Waiting to be scheduled on CPU
    RUNNING = 2,    // Currently executing on CPU
    WAITING = 3,    // Waiting for I/O or an event
    TERMINATED = -1  // Finished execution
} ProcessState;

typedef struct {
    const char *name; // func name
    void (*func)(char**, int);  // function pointer with args, argc
} Command;



// methods
void execute_program(char **tokens, int token_count);
void strip_newline(char *str);
int is_blank(const char *s);
Command* find_command(const char *name);
PCB* create_process(char **tokens, int token_count);
void print_PCB(PCB *pcb);
int allocate_PID();
int allocate_mem(PCB *pcb);
void load_program(char *filename, PCB *pcb);


// Define and initialize the global CPU
CPU cpu = {
    {"GPA", 0, 0},
    {"GPB", 1, 0},
    {"GPC", 2, 0},
    {"GPD", 3, 0},
    {"PGC", 4, 0},
    {"STP", 5, 0}, 
    {"ISR", 6, NULL}, // instruction reg holds buffer instead of int value
    {"FLG", 7, 0} 
};

Command commands[] = {
    {"exe", execute_program}
    //{"goodbye", goodbye}
};

// global data
char *main_mem[MAIN_MEM_SIZE]; // main memory table
int main_mem_bitmap[MAIN_MEM_SIZE]; // represents whether an address is allocated
PCB *pcb_table[PCB_TABLE_SIZE]; 
int next_PCB_Idx = 0; // next available pcb table index

int main() {
    char user_inp[100]; // stdin array

    while (1) {
        printf("gcmd > ");

        fgets(user_inp, sizeof(user_inp), stdin);
        /* check for empty input
        if (is_blank(user_inp)) {
            continue;
        } */

        // replace newline from fgets with null
        strip_newline(user_inp);

        char *command = strtok(user_inp, " ");

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
    }

    return 0;
}

void execute_program(char **tokens, int token_count) {
    printf("Entered execute_program\n");
    PCB *pcb = create_process(tokens, token_count);

    if (allocate_mem(pcb) == 67) {
        // ERROR
    }

    char *filename = tokens[1];

    // load program into memory
    load_program(filename, pcb);

    printf("Load program complete.\n");

    // load first value in pc
    cpu.PGC.value = pcb->main_mem_base;

    // store first instruction
    cpu.ISR.value = main_mem[cpu.PGC.value];

    // execute instructions
    while (1) {
        
        // mutable copy
        char buffer[100];
        strcpy(buffer, cpu.ISR.value);

        char *tokens[3];
        int token_count = 0;

        // tokenize instructions
        char *instruction = strtok(buffer, " ");
        while (instruction != NULL) {
            tokens[token_count++] = instruction;
            instruction = strtok(NULL, " ");
        }

        if (strcmp(tokens[0], "0") == 0) { // Load instruction
            printf("LOD instruction\n");
        } else if (strcmp(tokens[0], "1") == 0) {
            printf("STO instruction\n");
        } else if (strcmp(tokens[0], "2") == 0) {
            printf("ADD instruction\n");
        } else if (strcmp(tokens[0], "3") == 0) {
            printf("JEQ instruction\n");
        } else if (strcmp(tokens[0], "4") == 0) {
            printf("ADI instruction\n");
        } else if (strcmp(tokens[0], "5") == 0) {
            printf("JMP instruction\n");
        } else if (strcmp(tokens[0], "6") == 0) {
            printf("JMZ instruction\n");
        } else if (strcmp(tokens[0], "7") == 0) {
            printf("HLT instruction\n");
        }
        
    }

}

PCB* create_process(char **tokens, int token_count) {
    PCB *pcb = malloc(sizeof(PCB));
    int instr_count, pid;

    instr_count = count_instr(tokens[1]);
    pid = allocate_PID();

    // set appropriate fields
    pcb->pid = pid;
    pcb->state = NEW;
    pcb->priority = -1; // default priority
    pcb->registers = malloc(sizeof(int) * 8); // 8 temp registers
    pcb->instr_count = instr_count;

    // return pointer to newly created pcb object
    return pcb;
}

int allocate_PID() {
    for (int i = 0; i < PCB_TABLE_SIZE; i++) {
        if (pcb_table[i] != NULL) {
            return i;
        }
    }
    return -1;
}

int allocate_mem(PCB *pcb) {
    int targetMemSize = pcb->instr_count;
    int tail = 0, front = 0; // sliding window

    while (front < MAIN_MEM_SIZE) {
        // If current slot is free
        if (main_mem_bitmap[front] == 0) {
            // Check if window is large enough
           // printf("%d\n", front - tail + 1);
            if (front - tail + 1 == targetMemSize) {
                // Allocate memory
                for (int i = tail; i <= front; i++) {
                    main_mem_bitmap[i] = 1;
                }
                // set main memory base and limit
                pcb->main_mem_base = tail;
                pcb->main_mem_limit = tail + pcb->instr_count;
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

Command* find_command(const char *name) {
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp(name, commands[i].name) == 0) {
            return &commands[i];  // return pointer to matching command
        }
    }
    return NULL; // not found
}

void load_program(char *filename, PCB *pcb) {

    printf("Entered load program\n");
    FILE *fptr = fopen(filename, "r");

    char buffer[50];
    
    int mem_base = pcb->main_mem_base;

    // read line by line into mem
    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        
        strip_newline(buffer);
        printf("%s\n", buffer);
        
        // copy the entire line into main_mem[mem_base]
        main_mem[mem_base] = malloc(strlen(buffer) + 1); // allocate memory for the string
        strcpy(main_mem[mem_base++], buffer);
        printf("Reached end while\n");
    }

    // set program counter
    pcb->pc = pcb->main_mem_base;

    // set next pointer in pcb table
    pcb_table[next_PCB_Idx++] = pcb;
}

void printPCB(PCB *pcb) {
    printf("PID: %d\n", pcb->pid);
    printf("State: %d\n", pcb->state);
    printf("Priority: %d\n", pcb->priority);
    printf("PC: %d\n", pcb->pc);
    printf("Main Memory Base: %d\n", pcb->main_mem_base);
    printf("Main Memory Limit: %d\n", pcb->main_mem_limit);
}

void strip_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}