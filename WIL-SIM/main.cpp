#include <chrono>
#include <thread>

#include <iostream>
#include <stdio.h>
#include <conio.h>

using namespace std;

///--- Settings ---\\\

bool StepCLK = false;
int CLK = 2; //Max 1MHz
int beginAddr = '\x00';

///----------------///

char ram[255] = {'\x01','\x09','\x00','\x00'};
char currentInst;
int PC=0;
int ADDR=0;
int RegA=0;
int RegB=0;

bool CARRY = false;
bool running = true;



void execute(char inst){


    printf("Current Instruction: 0x%02x\n",inst & 0xFF);
    printf("PC: %i, ADDR: %x, RegA: %i, RegB: %i \n",PC,ADDR,RegA,RegB);


    switch(inst){
        case '\xFF':
        case '\x00': running = false;
                     printf("HALTED\n");
                     break;
        case '\x01': ADDR++;                   ///LDA - Load next byte into RegA
                     RegA = ram[ADDR];
                     ADDR++;
                     break;
        case '\x02': ADDR++;                   ///LDB - Load next byte into RegB
                     RegB = ram[ADDR];
                     ADDR++;
                     break;
        case '\x05': RegA = RegA + RegB;       ///ADD - Add RegA and RegB together and store in RegA
                     if(RegA>255){
                        RegA-=256;
                        CARRY = true;
                     } else {
                        CARRY = false;
                     }
                     ADDR++;
                     break;
        case '\x06': RegA = RegA - RegB;       ///SUB - Subtract RegA from RegB and store in RegA
                    if(RegA<0){
                        RegA+=256;
                        CARRY = true;
                     } else {
                        CARRY = false;
                     }
                     ADDR++;
                     break;
        case '\x07': ADDR++;                  ///STO - Store RegA in next byte
                     ram[ram[ADDR]] = RegA;
                     break;
        case '\x09': ADDR++;                  ///JMP - Jump to address in next byte
                     ADDR = ram[ADDR];
                     break;
        case '\x0A': ADDR++;                  ///JZ  - Jump to address in next byte if RegA is zero
                     if(RegA == '\x00'){
                        ADDR = ram[ADDR];
                     } else {
                        ADDR++;
                     }
                     break;
        case '\x0B': ADDR++;                  ///JE  - Jump to address in next byte if RegA is equal to RegB
                     if(RegA == RegB){
                        ADDR = ram[ADDR];
                     } else {
                        ADDR++;
                     }
                     break;
        case '\x0C': ADDR++;                  ///JC - Jump to address in next byte if the CARRY flag is set
                     if(CARRY){
                        ADDR = ram[ADDR];
                     } else {
                        ADDR++;
                     }
                     break;


        default: printf("ERROR - Could not resolve opcode\n");
                 running = false;
                 break;
    }



    PC = ADDR - beginAddr;

}

void run(){
    while(running){
        currentInst = ram[ADDR];
        execute(currentInst);
        if(StepCLK){
            getch();
        } else {
            this_thread::sleep_for(chrono::microseconds(1000000/CLK));
        }
    }
}

int main()
{
    cout << "WIL-SIM V0.1" << endl;
    printf("Starting program at address 0x%02x\n",beginAddr & 0xFF);

    run();

    printf("Simulation ended\n");
    return 0;
}



