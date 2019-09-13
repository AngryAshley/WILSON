#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <string>

using namespace std;

///--- Settings ---\\\

bool StepCLK = false;
int CLK = 2; //In Hz, 1MHz max
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
string splits[16];
string path_exe="";

string* splitString(string temp, char* tokens){
    int s=0;
    char str[255];
    //string temp="";
    string* newTemp=new string[15];
    strcpy(str, temp.c_str());
    char * pch;
    pch = strtok (str,tokens);
    while (pch != NULL){
        splits[s] = pch;
        newTemp[s] = pch;
        pch = strtok (NULL, tokens);
        s++;
    }
    return newTemp;
}
string file_getLine(string path, int lineNumber){
    ifstream file(path);
    string temp;
    if(file.good()){
        for(int i=0; i<=lineNumber; i++){
            getline(file,temp);
        }
    file.close();
    }
    return temp;
}
void getPath() {
    char path_exe_temp[1024];
    GetModuleFileName(NULL, path_exe_temp, 1024);
    int pos=string(path_exe_temp).find_last_of("\\/");
    path_exe=string(path_exe_temp).substr( 0, pos+1);
}
void loadSettings(){
    StepCLK=atoi(splitString(file_getLine(path_exe+"\\settings.txt",0),"=")[1].c_str());
    CLK=atoi(splitString(file_getLine(path_exe+"\\settings.txt",1),"=")[1].c_str());
}
void loadProgram(){
    string compArg = splitString(file_getLine(path_exe+"\\settings.txt",0)," =")[0];
    beginAddr = atoi(splitString(file_getLine(path_exe+"\\settings.txt",0)," =")[2]);
    if(compArg=="HEX"){



    } else if(compArg=="ASSEMBLY"){
        int i=1;
        int ramPos=0;
        string currInst="";
        while(true){
            currInst = file_getLine(path_exe+"\\settings.txt",i);
            if(currInst==""){break;};
            splitString(currInst," ");
            if(splits[0]=="LDA"){
                ram[rampos]=='\x01';
                rampos++;
                ram[rampos]==atoi(splits[1]);
            } else if(splits[0]=="LDB"){
                ram[rampos]=='\x02';
                rampos++;
                ram[rampos]==atoi(splits[1]);
            } else if(splits[0]=="ADD"){

            }
            rampos++;
        };
    } else {
        printf(" *** PROGRAM EXEPTION - FILE TYPE NOT RECOGNISED\n");
        return;
    }
}


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
            ///this_thread::sleep_for(chrono::microseconds(1000000/CLK));
            ///usleep(1000000/CLK);
            Sleep(1);
        }
    }
}

int main()
{
    printf(" ----- WIL-SIM V0.1 -----\n");
    printf(" *** Loading settings...\n");
    getPath();
    loadSettings();
    printf(" *** Starting program at address 0x%02x\n",beginAddr & 0xFF);
    run();
    printf(" *** Simulation ended\n");
    printf(" *** Press any key to exit. . .\n");
    getch();
    return 0;
}
