#pragma once
#include<stdint.h>
typedef struct
{
	uint32_t* memory;
	uint32_t re[15];
	double ref[5];
	uint32_t* code_origin;    // 存储指令内存区域
	uint32_t* data_origin;    // 存储数据内存区域
	uint8_t* stack_origin;   // 存储栈内存区域的起始地址
} VirtualMachine;