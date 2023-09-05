
# VN Architecture Simulator
- [VN Architecture Simulator](#vn-architecture-simulator)
  - [Brief Summary](#brief-summary)
  - [Instruction Set Architecture - ISA](#instruction-set-architecture---isa)
  - [Demonstration](#demonstration)
    - [Sum Two Numbers And Store - Double Speed](#sum-two-numbers-and-store---double-speed)
      - [Code: ](#code-)
    - [Infinite Loop Using JMP Command - Normal Speed](#infinite-loop-using-jmp-command---normal-speed)
      - [Code:](#code)
    - [Multiplying Two Numbers - Maximum Speed](#multiplying-two-numbers---maximum-speed)
      - [Code:](#code-1)
  - [Steps to Run (on Mac)](#steps-to-run-on-mac)
  - [UI Features](#ui-features)

## Brief Summary
VN Architecture Simulator was created to help CS students and just computer inthusiasts visualize the data movements in the registers and memory within a tiny, learning computer architecture.

This is an accumulator-machine (and not a add-subtract-register machine like MIPS for example). The architecture was adopted and inspired from Von Neumann Architecture, who is practically one of the fathers of computing.

Users are welcome to follow the steps to run this simulator, write their own tiny "assembly" code following the ISA described below and run their own probrams on this simulator. Sample programs are provided in description.

There is a lot of abstraction in this simulator. Going one abstraction level lower would invlolve representing the registers and the components in the logic-gate-level.
## Instruction Set Architecture - ISA
- 0000 - 00 - FETCH
- 0001 - 01 - LOAD
- 0010 - 02 - ADD
- 0011 - 03 - STORE
- 0100 - 04 - SUB
- 0101 - 05 - INPUT
- 0110 - 06 - OUTPUT
- 0111 - 07 - HALT
- 1000 - 08 - JMP
- 1001 - 09 - SKIPZ
- 1010 - 10 - SKIPG
- 1011 - 11 - FETCL
  
## Demonstration
  ### Sum Two Numbers And Store - Double Speed
  ![Screen-Recording-2023-08-29-at-1 (2)](https://github.com/maksim-petrushin/VN-Machine-simulator/assets/136845116/ba8437f1-dcb2-4284-8f85-9e1ba7c400a6)
  #### Code: ![image](https://github.com/maksim-petrushin/VN-Machine-simulator/assets/136845116/e512b25a-fead-4430-b67f-3cf84b801988)
  ### Infinite Loop Using JMP Command - Normal Speed
![Screen-Recording-2023-08-29-at-1 (1)](https://github.com/maksim-petrushin/VN-Machine-simulator/assets/136845116/c908d6ee-39c0-4e8e-b39d-33d7628de4d9)
  #### Code: 
![image](https://github.com/maksim-petrushin/VN-Machine-simulator/assets/136845116/151bda80-b71f-476c-b46f-c502dfaf085c)
  ### Multiplying Two Numbers - Maximum Speed
![Screen-Recording-2023-08-29-at-1](https://github.com/maksim-petrushin/VN-Machine-simulator/assets/136845116/21fa4475-faa7-4538-84bd-3e2bdea16d9b)
  #### Code: 
![image](https://github.com/maksim-petrushin/VN-Machine-simulator/assets/136845116/ba8a773a-7776-4e32-a704-489415354d9f)

## Steps to Run (on Mac)
1. Clone this repository into your computer directory
2. Write a "assembly" VM-program and name it specifically "input.txt". (in the same directory as proj.c)
    Structure of program lines: 
                    %d %d //Coments
                    Example: 1 0 //Load memory address 0

3. Go to the directory of this cloned repository. Type command "make". Then type command "./runme"

4. NOTE: address one in the Memory is reserved for value 1 (one)

5. NOTE: the written "assembly" code should assume that first line of it starts at a line 20. This is because lines 0-19 are reserved for calculations and variables. 

6. NOTE: the input.txt code is space friendly, except for the very first line. There should be no whitespace before the first line of code (the first line should exclusively start with the code).
## UI Features
- ### Keyboard Keys
1. SHIFT (left) key on keyboard starts the execution of the program (sets the fetch-execute cycle to fetch the operation)
2. N key on keyboard sets the speed at normal (1000 ms per standard animation)
3. D key on keyboard sets the speed at double (500 ms per standard animation)
4. Q key on keyboard sets the speed at quadriple (250 ms per standard animation)
5. M key on keyboard sets the speed at maximum (45 ms per standard animation)
6. A key on keyboard toggles the animation (greed register values traveling between registers)
7. To write input to console, click on the console, enter a number, and press ENTER
8. ESC key on keyboard halts the simulation.
- ### Drag the Screen using Mouse
![Screen-Recording-2023-08-29-at-1 (3)](https://github.com/maksim-petrushin/VN-Machine-simulator/assets/136845116/88c1bb44-805f-45d7-8535-8c091adfce60)






