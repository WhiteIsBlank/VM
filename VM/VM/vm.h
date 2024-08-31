#pragma once
#include<stdbool.h>
#include<stdio.h>
#include"vm_struct.h"
#include"vm_enum.h"
extern "C" __declspec(dllexport)
VirtualMachine * initializeVM();
extern "C" __declspec(dllexport)
void destroyVM(VirtualMachine* vm);
extern "C" __declspec(dllexport)
void openFile(const char* *filePath, FILE * *pfile);
extern "C" __declspec(dllexport)
int readFileToCode(const char* filePath, VirtualMachine * vm, char ( * s)[33]);
extern "C" __declspec(dllexport)
uint32_t Read(uint32_t address, VirtualMachine * vm);
extern "C" __declspec(dllexport)
uint32_t* offset_address(uint32_t* address, uint32_t offset);
extern "C" __declspec(dllexport)
uint32_t extendImmediate(uint32_t x, int n);
extern "C" __declspec(dllexport)
bool isZero(uint32_t x);
extern "C" __declspec(dllexport)
void setValue(uint32_t r, int pos, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void changeF(int r, VirtualMachine * vm);
extern "C" __declspec(dllexport)
void changeRegisterCmp(int f, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void fChangeRegisterCmp(double f, VirtualMachine* vm);
extern "C" __declspec(dllexport)
float mulDecimal(uint32_t dep);
extern "C" __declspec(dllexport)
void Push(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Pop(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Add(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Sub(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Mul(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Div(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Mod(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Mov(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void And(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Or(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Xor(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Not(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Shl(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Shr(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Cmp(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void fAdd(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void fSub(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void fMul(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void fDiv(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void fMov(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void fCmp(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Loop(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Jmp(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Call(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Ret(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Store(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
void Load(uint32_t inst, VirtualMachine* vm);
extern "C" __declspec(dllexport)
bool executeInstruction(VirtualMachine* vm, uint32_t instruction);