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

unsigned char ram[255];
unsigned char hiRAM[65535];
char currentInst;
int PC=0;
int ADDR=0;
int RegA=0;
int RegB=0;
int hiAddr=0;

string hiAddrTemp="";
bool CARRY = false;
bool running = true;
string splits[16];
string path_exe="";
string progFileName="";
string ramDumpAfterCompile="";

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
    ramDumpAfterCompile=splitString(file_getLine(path_exe+"\\settings.txt",3),"= ")[1].c_str();
}
void loadProgram(){
    string compArg = splitString(file_getLine(path_exe+progFileName,0)," =")[0];
    beginAddr = atoi(splitString(file_getLine(path_exe+progFileName,0)," =")[2].c_str());
    string currInst="";
    long hiAddTemp=0;
    int godverdomme=0;
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
            } else if(splits[0]=="LHA"||splits[0]=="LHB"){
                if(splits[0]=="LHA"){
                    ram[ramPos]='\x10';
                } else {
                    ram[ramPos]='\x11';
                }
                ramPos++;
                ram[ramPos]=stoi(splits[1].substr(0,2).c_str(),0,16);
                ramPos++;
                ram[ramPos]=stoi(splits[1].substr(2,2).c_str(),0,16);
            } else if(splits[0]=="STH"){
                ram[ramPos]='\x12';
                ramPos++;
                ram[ramPos]=stoi(splits[1].substr(0,2).c_str(),0,16);
                ramPos++;
                ram[ramPos]=stoi(splits[1].substr(2,2).c_str(),0,16);
            } else if(splits[0]=="DAT"){
                if(splits[2].size()==2){
                    ram[stoi(splits[2],0,16)]==stoi(splits[1],0,16);
                } else if(splits[2].size()==4){
                    hiRAM[stoi(splits[2],0,16)]==stoi(splits[1],0,16);
                } else {
                    printf("ERROR - COULD NOT RESOLVE ADDRESS, ADDRESS SIZE %d NOT SUPPORTED\n",splits[1].size());
                }
            } else {
                ram[ramPos]=atoi(splits[0].c_str());
            }
            i++;
            if(splits[0].c_str()=="DAT"){
                printf("Loading data %s in address %s\n",splits[0].c_str(),splits[1].c_str());
            } else {
                printf("Loading command %s in address %d\n",splits[0].c_str(),ramPos);
            }
            ramPos++;
        };
        printf(" *** PROGRAM COMPILED SUCCESSFULLY\n");
        int tempAdd=0;
        if(ramDumpAfterCompile=="0"){

        } else if(ramDumpAfterCompile=="1"){
            for(int i=0;i<8;i++){
                for(int j=0;j<8;j++){
                    printf("%02X ",ram[tempAdd]);
                    tempAdd++;
                }
                printf("\n");
            }
        } else if(ramDumpAfterCompile=="2"){
            for(int i=0;i<32;i++){
                printf("%04X ",tempAdd);
                for(int j=0;j<8;j++){
                    printf("%02X ",ram[tempAdd]);
                    tempAdd++;
                }
                printf("\n");
            }
            printf("----------------------------- Lo-Hi divider\n");
            for(int i=255;i<8415;i++){
                    printf("%04X ",tempAdd);
                for(int j=0;j<8;j++){
                    printf("%02X ",hiRAM[tempAdd]);
                    tempAdd++;
                }
                printf("\n");
            }
        }

    } else {
        printf(" *** PROGRAM EXEPTION - FILE TYPE NOT RECOGNISED\n");
        return;
    }
}


void execute(char inst){
    printf("Current Instruction: 0x%02x\n",inst & 0xFF);
    printf("PC: %i, ADDR: %x, RegA: %i, RegB: %i \n",PC,ADDR,RegA,RegB);
    char temp[2];
    int godverdegodver = hiAddr;

    switch(inst){
        case '\xFF':
        case '\x00': running = false;
                     printf("HALTED AT %02X\n",ADDR);
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
        case '\x10': ADDR++;
                     itoa(ram[ADDR],temp,16);
                     hiAddrTemp=temp;
                     ADDR++;
                     itoa(ram[ADDR],temp,16);
                     hiAddrTemp+=temp;
                     ADDR++;
                     hiAddr=strtoul(hiAddrTemp.c_str(),0,16);
                     RegA=hiRAM[hiAddr];
                     break;
        case '\x11': ADDR++;
                     itoa(ram[ADDR],temp,16);
                     hiAddrTemp=temp;
                     ADDR++;
                     itoa(ram[ADDR],temp,16);
                     hiAddrTemp+=temp;
                     ADDR++;
                     hiAddr=strtoul(hiAddrTemp.c_str(),0,16);
                     RegB=hiRAM[hiAddr];
                     break;
        case '\x12': ADDR++;
                     itoa(ram[ADDR],temp,16);
                     hiAddrTemp=temp;
                     ADDR++;
                     itoa(ram[ADDR],temp,16);
                     hiAddrTemp+=temp;
                     ADDR++;
                     printf("Address is %s\n",hiAddrTemp.c_str());
                     hiAddr=strtoul(hiAddrTemp.c_str(),0,16);
                     godverdegodver = hiAddr;
                     printf("Converted Address is %lu\n",hiAddr);
                     hiRAM[godverdegodver]=RegA;
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
