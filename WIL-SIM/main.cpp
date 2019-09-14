#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <string.h>

using namespace std;

///--- Settings ---\\\

bool StepCLK = false;
int CLK = 2; //In Hz, 1MHz max
int beginAddr = '\x00';

///----------------///

char ram[255];
char currentInst;
int PC=0;
int ADDR=0;
int RegA=0;
int RegB=0;

bool CARRY = false;
bool running = true;
string splits[16];
string path_exe="";
string progFileName="";

#ifdef __linux__
    void getPath(){
        sprintf(path_exe, "/proc/%d/exe", getpid());
    }
#elif _WIN32
    #include <Windows.h>
    void getPath(){
        char path_exe_temp[1024];
        GetModuleFileName(NULL, path_exe_temp, 1024);       ///Gebruikt nog windows.h, vind wat beters hiervoor
        int pos=string(path_exe_temp).find_last_of("\\/");
        path_exe=string(path_exe_temp).substr( 0, pos+1);
    }
#endif

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

void loadSettings(){
    StepCLK=atoi(splitString(file_getLine(path_exe+"\\settings.txt",0),"=")[1].c_str());
    CLK=atoi(splitString(file_getLine(path_exe+"\\settings.txt",1),"=")[1].c_str());
    progFileName=splitString(file_getLine(path_exe+"\\settings.txt",2),"=")[1].c_str();
}
void loadProgram(){
    string compArg = splitString(file_getLine(path_exe+progFileName,0)," =")[0];
    beginAddr = atoi(splitString(file_getLine(path_exe+progFileName,0)," =")[2].c_str());
    string currInst="";
    int i=2;
    int ramPos=0;
    if(compArg=="HEX"){
        printf(" *** LOADING MEMORY...\n");
        while(ramPos<255){
            currInst = file_getLine(path_exe+progFileName,i);
            splitString(currInst," ");
            printf("%02X | %s\n",ramPos,currInst.c_str());
            for(int j=0;j<8;j++){
                ram[ramPos]=stoi(splits[j],0,16);
                ramPos++;
            }
            i++;
        }
        printf(" *** MEMORY LOADED SUCCESSFULLY\n");
    } else if(compArg=="ASSEMBLY"){
        printf(" *** COMPILING PROGRAM...\n");
        while(true){
            splits[0]="";
            currInst = file_getLine(path_exe+progFileName,i);///"\\program.txt"
            splitString(currInst," ");
            if(splits[0]==""||ramPos==256||i==255){break;}; //||splits[0]=="EOF"
            if(splits[0]=="HLT"){
                ram[ramPos]='\x00';
            } else if(splits[0]=="LDA"){
                ram[ramPos]='\x01';
                ramPos++;
                ram[ramPos]=atoi(splits[1].c_str());
            } else if(splits[0]=="LDB"){
                ram[ramPos]='\x02';
                ramPos++;
                ram[ramPos]=atoi(splits[1].c_str());
            } else if(splits[0]=="ADD"){
                ram[ramPos]='\x05';
            } else if(splits[0]=="SUB"){
                ram[ramPos]='\x06';
            } else if(splits[0]=="STO"){
                ram[ramPos]='\x07';
                ramPos++;
                ram[ramPos]=atoi(splits[1].c_str());
            } else if(splits[0]=="JMP"){
                ram[ramPos]='\x09';
                ramPos++;
                ram[ramPos]=atoi(splits[1].c_str());
            }  else if(splits[0]=="JZ"){
                ram[ramPos]='\x0A';
                ramPos++;
                ram[ramPos]=atoi(splits[1].c_str());
            } else if(splits[0]=="JE"){
                ram[ramPos]='\x0B';
                ramPos++;
                ram[ramPos]=atoi(splits[1].c_str());
            } else if(splits[0]=="JC"){
                ram[ramPos]='\x0C';
                ramPos++;
                ram[ramPos]=atoi(splits[1].c_str());
            } else {
                ram[ramPos]=atoi(splits[0].c_str());
            }
            i++;
            printf("Loading command %s in address %d\n",splits[0].c_str(),ramPos);
            ramPos++;
        };
        printf(" *** PROGRAM COMPILED SUCCESSFULLY\n");
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
    ADDR=beginAddr;
    while(running){
        currentInst = ram[ADDR];
        execute(currentInst);
        if(StepCLK){
            getch();
        } else {
            this_thread::sleep_for(chrono::microseconds(1000000/CLK));
            ///usleep(1000000/CLK);
            ///Sleep(CLK);
        }
    }
}

int main()
{
    printf(" ----- WIL-SIM V0.1 -----\n");
    printf(" *** Loading settings...\n");
    getPath();
    loadSettings();
    loadProgram();
    printf(" *** Press any key to start the simulation\n");
    getch();
    printf(" *** Starting program at address 0x%02x\n",beginAddr & 0xFF);
    run();
    printf(" *** Simulation ended\n");
    printf(" *** Press any key to exit. . .\n");
    getch();
    return 0;
}
