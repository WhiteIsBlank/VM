#pragma once
#include<stdint.h>
typedef struct
{
	uint32_t* memory;
	uint32_t re[15];
	double ref[5];
	uint32_t* code_origin;    // �洢ָ���ڴ�����
	uint32_t* data_origin;    // �洢�����ڴ�����
	uint8_t* stack_origin;   // �洢ջ�ڴ��������ʼ��ַ
} VirtualMachine;