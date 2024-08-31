#pragma once
#include<stdint.h>
#include<F:\ÄæÏò\SRE¿¼ºË\Summer\VM\VM\vm.h>
#define MAX_INSTR 50
#define MAX_LENGTH 100
struct runningState
{
	bool debugMode;
	bool normalMode;
	bool continueLoop;
	bool StepOverCall;
	uint32_t nextInstr;
};
void binaryToString(uint32_t binaryNum, char* string);
void debugPrint(VirtualMachine* vm, char m[][33], int count,char instrs[][MAX_LENGTH]);
struct runningState* debugControl(VirtualMachine* vm, uint32_t instruction);