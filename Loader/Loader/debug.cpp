#include<stdio.h>
#include<Windows.h>
#include <conio.h>
#include"debug.h"
void binaryToString(uint32_t binaryNum, char* string)
{
	for (int i = 31; i >= 0; i--)
	{
		if (binaryNum & (1 << i))
			string[31 - i] = '1';
		else
			string[31 - i] = '0';
	}
	string[32] = '\0';
}
uint32_t* offset_address(uint32_t* address, uint32_t offset)
{
	uint32_t* new_address = address;
	new_address += offset;
	return (uint32_t*)new_address;
}
void printStack(int s, int m, VirtualMachine* vm)
{
	for (int i = s; i < m; i++)//打印栈空间
	{
		uint32_t* n = NULL;
		n = offset_address(vm->code_origin, 0xffff - i);
		char s2[33];
		binaryToString(*n, s2);
		printf("binary:%s           data:%-8d          .stack:%p \n", s2, *n, n);
	}
}
void printData(int s, int m, VirtualMachine* vm)
{
	for (int i = s; i < m; i++)
	{
		uint32_t* m = NULL;
		m = offset_address(vm->code_origin, 0x8000 + i);
		char s[33];
		binaryToString(*m, s);
		printf("binary:%s           data:%-8d          .data:%p \n", s, *m, m);
	}
}
void debugPrint(VirtualMachine* vm, char m[][33],int count, char instrs[][MAX_LENGTH])
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	printf("-------------------------------------------");
	printf("----------Debug Mode--------------------------------");
	printf("----------------------\n");
	fflush(stdout);
	printf("                 offest                       binary");
	printf("                                 instruction\n");
	int h = 0;//计算循环了多少次
	for (int i = (vm->re[IP] / 10) * 10;i < (vm->re[IP] / 10) * 10 + 10 && i < count; i++)
	{
		if (i == vm->re[IP])
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		else
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		printf("\t\t%08x\t%s\t\t%s\n",(uint8_t)i, m[i],instrs[i]);
		h++;
	}
	if (h < 9)
	{
		for (;h < 10;h++)
			printf("\n");
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("Register\n");
	for (int i = 0; i < 19; i++)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		if ((vm->re[F - 5] >> i) & 0b1)
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
		char s[33];
		if (i < 14)
		{
			binaryToString(vm->re[i], s);
			if (i <= 5)
			{
				printf("R%d:0b%s   ", i, s);
				if ((i + 1) % 3 == 0)
					printf("\n");
			}
			else if (i == 6)
				printf("SP:0b%s   ", s);
			else if (i == 7)
				printf("BP:0b%s\n", s);
			else if (i == 8)
				printf("IP:0b%s\n", s);
			else if (i == 9)
				printf("FLAGS:0b%s\n", s);
			else if (i == 10)
				printf("JP:0b%s  ", s);
			else if (i == 11)
				printf("OP:0b%s\n", s);
			else if (i == 12)
				printf("CS:0b%s  ", s);
			else if (i == 13)
				printf("DS:0b%s\n", s);
		}
		else
			printf("ST%d:%lf  ", i - 14, vm->ref[i - 14]);
	}
	printf("\n");
	printf("Stack\n");
	printStack(0, 4, vm);
	printf("Data\n");
	printData(0, 2, vm);
	vm->re[F - 5] = 0;
}
struct runningState* debugControl(VirtualMachine* vm,uint32_t instruction)
{
	struct runningState* s = (struct runningState*)malloc(sizeof(struct runningState));
	memset(s, 0, sizeof(runningState));
	s->debugMode = TRUE;
	s->normalMode = FALSE;
	while (!_kbhit())
	{
		s->continueLoop = FALSE;
	}
	// 如果有键盘输入
	int key = _getch();
	if (key == 'R' || key == 'r')
	{
		s->continueLoop = TRUE;
		s->debugMode = FALSE;
		s->normalMode = TRUE;
	}
	else if (key == 'C' || key == 'c')
	{
		bool run = TRUE;
		int m = 1;
		int f = 0;
		int h = 0;
		while (run)
		{
			if (m)
				system("cls");
			if (m)
			{
				printf("请输入下个指令参数('s'栈,'d'数据段,'q'退出)\n");
				m--;
			}
			else
				printf("请输入下个指令参数('s'栈,'d'数据段,'q'退出,'b'上翻一页，'f'下翻一页)\n");
			int k = _getch();
			if (k == 'f')
			{
				f += 26;
				k = h;
			}
			if (k == 'b')
			{
				f -= 26;
				k = h;
				if (f < 0)
				{
					printf("前面没有了哦\n");
					Sleep(1000);
					f += 26;
				}
			}
			system("cls");
			if (k == 's')
			{
				printf("-------------------------------------------");
				printf("--------------Stack------------------------------------");
				printf("----------------------\n");
				printStack(f, f + 26, vm);
			}
			else if (k == 'd')
			{
				printf("-------------------------------------------");
				printf("--------------Data------------------------------------");
				printf("----------------------\n");
				printData(f, f + 26, vm);
			}
			else if (k == 'q')
			{
				run = FALSE;
				s->continueLoop = TRUE;
			}
			else
				printf("非法指令参数，请重新输入\n");
			h = k;
		}
	}
	if (key == 0 || key == 0xE0) 
	{
		key = _getch();
		if (key == 65)   // F7 的键码为 65
			s->continueLoop = TRUE;
		else if (key == 66) //F8步过
		{
			s->continueLoop = TRUE;
			if (((instruction >> 24) & 0xFF) == 24)
			{
				s->normalMode = TRUE;
				s->debugMode = FALSE;
				s->StepOverCall = 1;
				uint32_t* next = NULL;
				next = offset_address(vm->code_origin, vm->re[SP] + 1);
				s->nextInstr = *next;
			}
		}
	}
	system("cls");
	return s;
}