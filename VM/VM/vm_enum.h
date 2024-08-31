#pragma once
//指令
typedef enum
{
	HALT,
	//栈操作
	PUSH, POP,
	//整型运算
	//算术运算
	ADD, SUB, MUL, DIV, MOD, MOV,
	//逻辑运算
	AND, OR, XOR, NOT, SHL, SHR,
	//比较指令
	CMP,
	//浮点数运算
	//算术运算
	FADD, FSUB, FMUL, FDIV, FMOV,
	//比较指令
	FCMP,
	//流指令
	LOOP, JMP, CALL, RET,
	//数据段操作
	STORE, LOAD,
	NOP
}Instruction;//自定义不定长操作指令
//寄存器
typedef enum
{
	//通用寄存器
	R0,//累加寄存器，存放算术运算的结果	
	R1,//存储数据和指针 
	R2,//循环记数，累加 
	R3,//通用数据寄存器 
	R4, R5,
	//栈指针寄存器
	SP,//存储栈内存的起始地址或栈顶位置
	BP,//存储栈的栈底地址
	//程序指针寄存器
	IP,//用于存储下一条要执行的指令的地址
	//标志寄存器
	FLAGS,//条件标志位
	//目标地址寄存器
	JP,//存储jmp，loop等指令的目标地址
	OP,//运算目的寄存器（储存运算中数据偏移地址）
	//段寄存器
	CS,//代码段
	DS,//数据段
	//浮点数存储寄存器
	ST0, ST1, ST2, ST3, ST4,
	//调试模式下观察寄存器是否发生变化
	F
}Register;//自定义寄存器
//条件标志
typedef enum
{
	LE = 1, //<=
	L = 2, //<
	GE = 3, //>=
	G = 4, //>
	EE = 5, //==
	NE = 6 //!=
}flags;