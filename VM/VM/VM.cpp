#define _CRT_SECURE_NO_WARNINGS
#include "vm.h"
#include<math.h>
#include<Windows.h>
#define MAX_MEMORY_SIZE 65536 //�Զ���������ڴ�ռ��С
#define MAX_STACK_SIZE 256 //�Զ���ջ�ռ�Ĵ�С
#define NUM_DIGITS 32//�ļ���ÿһ��Ϊ16���ַ�
#define EPSILON 1e-8 //���������
// ��ʼ�������
VirtualMachine* initializeVM()
{
	VirtualMachine* vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
	vm->memory = (uint32_t*)malloc(sizeof(uint32_t) * MAX_MEMORY_SIZE);
	memset(vm->memory, 0, sizeof(uint32_t) * MAX_MEMORY_SIZE);
	vm->code_origin = vm->memory;       // ����code_originΪmemory����ʼλ��
	vm->data_origin = &(vm->memory)[0x8000];   // ����data_originΪ�����ڴ��������ʼλ��
	vm->stack_origin = (uint8_t*)&(vm->memory)[0xE000]; // ����stack_originΪջ�ڴ��������ʼλ��
	memset(vm->re, 0, sizeof(uint32_t) * 15);
	memset(vm->ref, 0, sizeof(double) * 5);
	vm->re[IP] = 0x0000;//ָ�������ڳ�ʼλ��
	vm->re[SP] = 0xFFFF;//ջ��ָ��������ջ�ռ����ߵ�ַ
	vm->re[BP] = 0xFFFF;
	vm->re[DS] = 0x8000;
	return vm;
}
// ���������
void destroyVM(VirtualMachine* vm)
{
	free(vm->memory);
	free(vm);
}
void openFile(const char** filePath,FILE** pfile)
{
	*pfile = fopen(*filePath, "r");
	if (!pfile)
	{
		printf("Failed to successfully open file��");
		exit(1);
	}
}
//��ȡ�ļ���Ϣ�������
int readFileToCode(const char* filePath, VirtualMachine* vm, char(*s)[33])
{
	FILE* fp = NULL;
	int count = 0;
	char line[NUM_DIGITS + 1];  // �洢ÿ�е��ַ�����+1 ��Ϊ�˴洢�ַ���������'\0'
	openFile(&filePath,&fp);
	while (fgets(line, sizeof(line), fp) != NULL) {
		if (line[0] == '\n')
			continue; // ��������
		strncpy(s[count], line, NUM_DIGITS);
		s[count][NUM_DIGITS] = '\0';
		(vm->code_origin)[count] = (uint32_t)strtol(line, NULL, 2);
		count++;
	}
	// �ر��ļ�
	fclose(fp);
	return count;//�����ļ����ж�����ָ��
}
//��ȡָ��
uint32_t Read(uint32_t address, VirtualMachine* vm)
{
	uint32_t* ptr = (uint32_t*)(address + vm->code_origin);
	uint32_t data = *ptr;
	return data;
}
//��ȡ���Ե�ַ
uint32_t* offset_address(uint32_t* address, uint32_t offset)
{
	uint32_t* new_address = address;
	new_address += offset;  // ƫ�Ƶ�ַ
	return (uint32_t*)new_address;  // ��ƫ�ƺ�ĵ�ַת����uint32_t*ָ�����Ͳ�����
}
//��n(n < 32)λ����������չ��32λ��������
uint32_t extendImmediate(uint32_t x, int n)
{
	if ((x >> (n - 1)) & 1)
		x |= (0xFFFFFFFF << n);
	return x;
}
//�Ƿ�Ϊ0
bool isZero(uint32_t x)
{
	if (x == 0)
	{
		printf("ERROR\n\
Reason for error:The denominator cannot be zero\n");
		return TRUE;
	}
	return FALSE;
}
//����cmp��Ľ��
void setValue(uint32_t r, int pos, VirtualMachine* vm)
{
	vm->re[r] |= (1 << (pos - 1));
}
void changeF(int r, VirtualMachine* vm)
{
	setValue(F-5, r+1, vm);
}
//��flags�Ĵ�����ĳһλ�ϸ�ֵ(����)
void changeRegisterCmp(int f, VirtualMachine* vm)
{
	if (f != 0)
		setValue(FLAGS, NE, vm);
	if (f == 0)
		setValue(FLAGS, EE, vm);
	if (f > 0)
		setValue(FLAGS, G, vm);
	if (f >= 0)
		setValue(FLAGS, GE, vm);
	if (f < 0)
		setValue(FLAGS, L, vm);
	if (f <= 0)
		setValue(FLAGS, LE, vm);
}
//��flags�Ĵ�����ĳһλ�ϸ�ֵ(������)
void fChangeRegisterCmp(double f, VirtualMachine* vm)
{
	if (f > EPSILON || f < (-EPSILON))
		setValue(FLAGS, NE, vm);
	if (f >= (-EPSILON) && f <= EPSILON)
		setValue(FLAGS, EE, vm);
	if (f > EPSILON)
		setValue(FLAGS, G, vm);
	if (f >= (-EPSILON))
		setValue(FLAGS, GE, vm);
	if (f < (-EPSILON))
		setValue(FLAGS, L, vm);
	if (f <= EPSILON)
		setValue(FLAGS, LE, vm);
}
//������С������Ӧ�ó˶���
float mulDecimal(uint32_t dep)
{
	int count = 0;
	while (dep != 0)
	{
		dep /= 10;
		++count;
	}
	return pow(0.1, count);
}
void Push(uint32_t inst, VirtualMachine* vm)
{
	uint32_t* s = NULL;
	s = offset_address(vm->code_origin, vm->re[SP]);//��ȡ��ǰSPָ����ָ���ջ�ռ���Ե�ַ
	// ����ջ�Ƿ����㹻�Ŀռ�
	uint32_t flag = (inst >> 22) & 0b11;//��ȡflag
	if (flag == 0)
	{
		uint32_t r1 = (inst >> 17) & 0b11111;
		*s = vm->re[r1];
		vm->re[SP]--;
	}
	else
	{
		uint32_t imm22 = extendImmediate(inst & 0x3FFFFF, 22);
		*s = imm22;
		vm->re[SP]--;
	}
	changeF(SP, vm);
}
void Pop(uint32_t inst, VirtualMachine* vm)
{
	uint32_t* s = NULL;
	s = offset_address(vm->code_origin, ++(vm->re[SP]));
	uint32_t flag = (inst >> 22) & 0b11;
	if (flag == 0)
	{
		uint32_t r1 = (inst >> 17) & 0b11111;
		vm->re[r1] = *s;
		*s = 0;
		changeF(r1, vm);
	}
	else
	{
		*s = 0;
	}
	changeF(SP, vm);
}
void Add(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		vm->re[r0] = vm->re[r1] + vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm12 = extendImmediate(inst & 0xFFF, 12);
		vm->re[r0] = vm->re[r1] + imm12;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = vm->re[r1] + *s;
	}
	changeF(r0, vm);
}
void Sub(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		vm->re[r0] = vm->re[r1] - vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm12 = extendImmediate(inst & 0xFFF, 12);
		vm->re[r0] = vm->re[r1] - imm12;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = vm->re[r1] - *s;
	}
	changeF(r0, vm);
}
void Mul(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		vm->re[r0] = vm->re[r1] * vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm12 = extendImmediate(inst & 0xFFF, 12);
		vm->re[r0] = vm->re[r1] * imm12;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = vm->re[r1] * (*s);
	}
	changeF(r0, vm);
}
void Div(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	bool f;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		f = isZero(vm->re[r2]);
		if (f)
			exit(1);
		else
			vm->re[r0] = vm->re[r1] / vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm12 = extendImmediate(inst & 0xFFF, 12);
		f = isZero(imm12);
		if (f)
			exit(1);
		else
			vm->re[r0] = vm->re[r1] / imm12;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		f = isZero(*s);
		if (f)
			exit(1);
		else
			vm->re[r0] = vm->re[r1] / *s;
	}
	changeF(r0, vm);
}
void Mod(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	bool f;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		f = isZero(vm->re[r2]);
		if (f)
			exit(1);
		else
			vm->re[r0] = vm->re[r1] % vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm12 = extendImmediate(inst & 0xFFF, 12);
		f = isZero(imm12);
		if (f)
			exit(1);
		else
			vm->re[r0] = vm->re[r1] % imm12;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		f = isZero(*s);
		if (f)
			exit(1);
		else
			vm->re[r0] = vm->re[r1] % *s;
	}
	changeF(r0, vm);
}
void Mov(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t flag = (inst >> 17) & 0b11;
	if (flag == 0)
	{
		uint32_t r1 = inst & 0b11111;
		vm->re[r0] = vm->re[r1];
	}
	else if (flag == 1)
	{
		uint32_t imm17 = extendImmediate(inst & 0x1FFFF, 17);
		vm->re[r0] = imm17;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = *s;
	}
	changeF(r0, vm);
}
void And(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		vm->re[r0] = vm->re[r1] & vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm12 = extendImmediate(inst & 0xFFF, 12);
		vm->re[r0] = vm->re[r1] & imm12;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = vm->re[r1] & *s;
	}
	changeF(r0, vm);
}
void Or(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		vm->re[r0] = vm->re[r1] | vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm12 = extendImmediate(inst & 0xFFF, 12);
		vm->re[r0] = vm->re[r1] | imm12;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = vm->re[r1] | *s;
	}
	changeF(r0, vm);
}
void Xor(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		vm->re[r0] = vm->re[r1] ^ vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm12 = extendImmediate(inst & 0xFFF, 12);
		vm->re[r0] = vm->re[r1] ^ imm12;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = vm->re[r1] ^ *s;
	}
	changeF(r0, vm);
}
void Not(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t flag = (inst >> 17) & 0b11;
	if (flag == 0)
	{
		uint32_t r1 = inst & 0b11111;
		vm->re[r0] = ~vm->re[r1];
	}
	else if (flag == 1)
	{
		uint32_t imm17 = extendImmediate(inst & 0x1FFFF, 17);
		vm->re[r0] = ~imm17;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = ~(*s);
	}
	changeF(r0, vm);
}
void Shl(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		vm->re[r0] = vm->re[r1] << vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm6 = extendImmediate(inst & 0x3F, 6);
		vm->re[r0] = vm->re[r1] << imm6;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = vm->re[r1] << *s;
	}
	changeF(r0, vm);
}
void Shr(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t r1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t r2 = inst & 0b11111;
		vm->re[r0] = vm->re[r1] >> vm->re[r2];
	}
	else if (flag == 1)
	{
		uint32_t imm6 = extendImmediate(inst & 0x3F, 6);
		vm->re[r0] = vm->re[r1] >> imm6;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->re[r0] = vm->re[r1] >> *s;
	}
	changeF(r0, vm);
}
void Cmp(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t flag = (inst >> 17) & 0b11;
	int f;
	if (flag == 0)
	{
		uint32_t r1 = inst & 0b11111;
		f = vm->re[r0] - vm->re[r1];
	}
	else if (flag == 1)
	{
		uint32_t imm17 = extendImmediate(inst & 0x1FFFF, 17);
		f = vm->re[r0] - imm17;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		f = vm->re[r0] - *s;
	}
	changeRegisterCmp(f, vm);
	changeF(FLAGS, vm);
}
void fAdd(uint32_t inst, VirtualMachine* vm)
{
	uint32_t st0 = (inst >> 19) & 0b11111;
	uint32_t st1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t st2 = inst & 0b11111;
		vm->ref[st0 - 14] = vm->ref[st1 - 14] + vm->ref[st2 - 14];
	}
	else if (flag == 1)
	{
		uint32_t dep = inst & 0x7F;
		uint32_t inp = (inst >> 7) & 0b1111;
		uint32_t s = (inst >> 11) & 0b1;
		double n = mulDecimal(dep);
		double m = inp + dep * n;
		if (s == 0)
			vm->ref[st0 - 14] = vm->ref[st1 - 14] + m;
		else
			vm->ref[st0 - 14] = vm->ref[st1 - 14] - m;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->ref[st0 - 14] = vm->ref[st1 - 14] + *s;
	}
	changeF(st0, vm);
}
void fSub(uint32_t inst, VirtualMachine* vm)
{
	uint32_t st0 = (inst >> 19) & 0b11111;
	uint32_t st1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t st2 = inst & 0b11111;
		vm->ref[st0 - 14] = vm->ref[st1 - 14] - vm->ref[st2 - 14];
	}
	else if (flag == 1)
	{
		uint32_t dep = inst & 0x7F;
		uint32_t inp = (inst >> 7) & 0b1111;
		uint32_t s = (inst >> 11) & 0b1;
		double n = mulDecimal(dep);
		double m = inp + dep * n;
		if (s == 0)
			vm->ref[st0 - 14] = vm->ref[st1 - 14] - m;
		else
			vm->ref[st0 - 14] = vm->ref[st1 - 14] + m;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->ref[st0 - 14] = vm->ref[st1 - 14] - *s;
	}
	changeF(st0, vm);
}
void fMul(uint32_t inst, VirtualMachine* vm)
{
	uint32_t st0 = (inst >> 19) & 0b11111;
	uint32_t st1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	if (flag == 0)
	{
		uint32_t st2 = inst & 0b11111;
		vm->ref[st0 - 14] = vm->ref[st1 - 14] * vm->ref[st2 - 14];
	}
	else if (flag == 1)
	{
		uint32_t dep = inst & 0x7F;
		uint32_t inp = (inst >> 7) & 0b1111;
		uint32_t s = (inst >> 11) & 0b1;
		double n = mulDecimal(dep);
		double m = inp + dep * n;
		if (s == 0)
			vm->ref[st0 - 14] = vm->ref[st1 - 14] * m;
		else
			vm->ref[st0 - 14] = vm->ref[st1 - 14] * (0 - m);
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->ref[st0 - 14] = vm->ref[st1 - 14] * *s;
	}
	changeF(st0, vm);
}
void fDiv(uint32_t inst, VirtualMachine* vm)
{
	uint32_t st0 = (inst >> 19) & 0b11111;
	uint32_t st1 = (inst >> 14) & 0b11111;
	uint32_t flag = (inst >> 12) & 0b11;
	bool f;
	if (flag == 0)
	{
		uint32_t st2 = inst & 0b11111;
		f = isZero(vm->ref[st2 - 14]);
		if (f)
			exit(1);
		else
			vm->ref[st0 - 14] = vm->ref[st1 - 14] / vm->ref[st2 - 14];

	}
	else if (flag == 1)
	{
		uint32_t dep = inst & 0x7F;
		uint32_t inp = (inst >> 7) & 0b1111;
		uint32_t s = (inst >> 11) & 0b1;
		double n = mulDecimal(dep);
		double m = inp + dep * n;
		f = isZero(m);
		if (f)
			exit(1);
		else
		{
			if (s == 0)
				vm->ref[st0 - 14] = vm->ref[st1 - 14] / m;
			else
				vm->ref[st0 - 14] = vm->ref[st1 - 14] / (0 - m);
		}
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		f = isZero(*s);
		if (f)
			exit(1);
		else
			vm->ref[st0 - 14] = vm->ref[st1 - 14] / *s;
	}
	changeF(st0, vm);
}
void fMov(uint32_t inst, VirtualMachine* vm)
{
	uint32_t st0 = (inst >> 19) & 0b11111;
	uint32_t flag = (inst >> 17) & 0b11;
	if (flag == 0)
	{
		uint32_t st1 = inst & 0b11111;
		vm->ref[st0 - 14] = vm->ref[st1 - 14];
	}
	else if (flag == 1)
	{
		uint32_t dep = inst & 0x3FF;
		uint32_t inp = (inst >> 10) & 0x3F;
		uint32_t s = (inst >> 16) & 0b1;
		double n = mulDecimal(dep);
		double m = inp + dep * n;
		if (s == 0)
			vm->ref[st0 - 14] = m;
		else
			vm->ref[st0 - 14] = 0 - m;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		vm->ref[st0 - 14] = *s;
	}
	changeF(st0, vm);
}
void fCmp(uint32_t inst, VirtualMachine* vm)
{
	uint32_t st0 = (inst >> 19) & 0b11111;
	uint32_t flag = (inst >> 17) & 0b11;
	double f;
	if (flag == 0)
	{
		uint32_t st1 = inst & 0b11111;
		f = vm->ref[st0 - 14] - vm->ref[st1 - 14];
	}
	else if (flag == 1)
	{
		uint32_t dep = inst & 0x3FF;
		uint32_t inp = (inst >> 10) & 0x3F;
		uint32_t s = (inst >> 16) & 0b1;
		double n = mulDecimal(dep);
		double m = inp + dep * n;
		if (s == 0)
			f = vm->ref[st0 - 14] - m;
		else
			f = vm->ref[st0 - 14] + m;
	}
	else
	{
		uint32_t ad = inst & 0b11111;
		uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
		uint32_t* s = NULL;
		s = offset_address(vm->code_origin, vm->re[ad] + x);
		f = vm->ref[st0 - 14] - *s;
	}
	fChangeRegisterCmp(f, vm);
	changeF(FLAGS, vm);
}
void Loop(uint32_t inst, VirtualMachine* vm)
{
	if (vm->re[R2] == 0)
		return;
	else
	{
		vm->re[IP] = vm->re[JP];
		changeF(IP, vm);
	}
}
void Jmp(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = inst & 0b11111;
	uint32_t flag = (inst >> 21) & 0b111;
	if (flag == 0)
		vm->re[IP] = vm->re[r0];//��������ת
	else
	{
		if ((vm->re[FLAGS] >> (flag - 1)) & 0b1)
			vm->re[IP] = vm->re[r0];
		else
			return;
	}
	changeF(IP, vm);
}
void Call(uint32_t inst, VirtualMachine* vm)
{
	uint32_t instruction;
	instruction = extendImmediate(0b00000001000100000000000000000000, 24);
	Push(instruction, vm); //push ip
	uint32_t instruction1;
	instruction1 = 0b00010111000000000000000000001010;//jmp jp
	Jmp(instruction1, vm);
}
void Ret(uint32_t inst, VirtualMachine* vm)
{
	uint32_t ty = inst & 0b11;
	if (ty == 1)
		printf("��������ֵΪ��%d\n", vm->re[R0]);
	uint32_t* s = NULL;
	s = offset_address(vm->code_origin, (++vm->re[SP]));
	vm->re[IP] = *s;
	*s = 0;
	changeF(IP, vm);
}
void Store(uint32_t inst, VirtualMachine* vm)
{
	uint32_t flag = (inst >> 12) & 0b1;
	uint32_t ad = inst & 0b11111;
	uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
	uint32_t* s = NULL;
	s = offset_address(vm->code_origin, vm->re[ad] + x);
	if (flag == 0)
	{
		uint32_t r0 = (inst >> 19) & 0b11111;
		*s = vm->re[r0];
	}
	else
	{
		uint32_t imm11 = extendImmediate((inst >> 13) & 0x7FF, 11);
		*s = imm11;
	}
}
void Load(uint32_t inst, VirtualMachine* vm)
{
	uint32_t r0 = (inst >> 19) & 0b11111;
	uint32_t ad = inst & 0b11111;
	uint32_t x = extendImmediate((inst >> 5) & 0x7F, 7);
	uint32_t* s = NULL;
	s = offset_address(vm->code_origin, vm->re[ad] + x);
	vm->re[r0] = *s;
	changeF(r0, vm);
}
//ִ���Զ���ָ��
bool executeInstruction(VirtualMachine* vm, uint32_t instruction) {
	// ����ָ��
	uint8_t opcode = (instruction >> 24) & 0xFF;
	//ִ��ָ��
	switch (opcode)
	{
	case HALT:
		printf("\nEnd of program\n");
		return FALSE;
		break;
	case PUSH:
		Push(instruction, vm);
		break;
	case POP:
		Pop(instruction, vm);
		break;
	case ADD:
		Add(instruction, vm);
		break;
	case SUB:
		Sub(instruction, vm);
		break;
	case MUL:
		Mul(instruction, vm);
		break;
	case DIV:
		Div(instruction, vm);
		break;
	case MOD:
		Mod(instruction, vm);
		break;
	case MOV:
		Mov(instruction, vm);
		break;
	case AND:
		And(instruction, vm);
		break;
	case OR:
		Or(instruction, vm);
		break;
	case XOR:
		Xor(instruction, vm);
	case NOT:
		Not(instruction, vm);
		break;
	case SHL:
		Shl(instruction, vm);
		break;
	case SHR:
		Shr(instruction, vm);
		break;
	case CMP:
		Cmp(instruction, vm);
		break;
	case FADD:
		fAdd(instruction, vm);
		break;
	case FSUB:
		fSub(instruction, vm);
		break;
	case FMUL:
		fMul(instruction, vm);
		break;
	case FDIV:
		fDiv(instruction, vm);
		break;
	case FMOV:
		fMov(instruction, vm);
		break;
	case FCMP:
		fCmp(instruction, vm);
		break;
	case LOOP:
		Loop(instruction, vm);
		break;
	case JMP:
		Jmp(instruction, vm);
		break;
	case CALL:
		Call(instruction, vm);
		break;
	case RET:
		Ret(instruction, vm);
		break;
	case STORE:
		Store(instruction, vm);
		break;
	case LOAD:
		Load(instruction, vm);
		break;
	case NOP:
		break;
	default:
		printf("Illegal instruction operation!!!\n");
		exit(1);
		break;
	}
	return TRUE;
}