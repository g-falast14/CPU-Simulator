# CPU-Simulator
This is a CPU simulator I coded from scratch, including an assembler and my own RISC assembly language that mimics other modern assemblies. 
As of right now, the assembler is only a single-pass assembler, so no variables/labels can be declared and stored in a lookup table. 

The following is an outline of my simulated architecture:

Main memory is simulated as an array of 2048 entries. Each entry represents one slot in memory and holds a pointer to an instruction (a string), meaning the memory can store up to 2048 instructions. There are 8 internal CPU registers:

- GPA/GPB/GPC/GPD: general-purpose registers used for basic arithmetic/register operations.
- PGC: the program counter that holds the address of the next instruction. In my simulation, it simply holds an index into the main memory array.
- STP: the stack pointer that is being saved for future use.
- ISR: the instruction register that holds a copy of the current instruction in memory.
- FLG: status/flag registers for future use.

My instruction set contains 8 instructions that require 2 arguments each for simplicity, even though not every instruction will use both. The following is an outline of each instruction:

LOD: Load (Opcode 0)
- Arg1: Always an internal CPU register
- Arg2: Always a valid memory address

----------------------------------------

STO: Store (Opcode 1)
- Arg1: Always a valid memory address
- Arg2: Always an internal CPU register

----------------------------------------

ADD: Add (Opcode 2)
- Arg1: Always an internal CPU register
- Arg2: Always an internal CPU register

----------------------------------------

ADI: Add Immediate (Opcode 3)
- Arg1: Always an internal CPU register
- Arg2: Always a signed decimal value

----------------------------------------

JMP: Jump (Opcode 4)
- Arg1: Always a valid memory address
- Note: JMP requires 2 args, but the second
arg will not be used.

----------------------------------------

JMZ: Jump If Zero (Opcode 5)
- Arg1: Always a valid memory address
- Arg2: Always an internal CPU register

----------------------------------------

JEQ: Jump If Equal (Opcode 6)
- Arg1: Always a valid memory address
- Arg2: Always an internal CPU register
- Note: JEQ will always compare Arg2 with
the value in the GPA register to determine 
whether to jump or not.

----------------------------------------

HLT: Halt (Opcode 7)
- Note: 2 args need to be passed for HLT, 
but they will both be ignored. -1 is passed 
by convention.

----------------------------------------


