#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include"debug.h"
#include"detect.h"

//����ָ�붨��
typedef VirtualMachine* (*lpinitializeVM)(); 
typedef void (*lpdestroyVM)(VirtualMachine*); 
typedef void (*lpopenFile)(const char**, FILE**);
typedef int (*lpreadFileToCode)(const char*, VirtualMachine*, char(*)[33]);
typedef uint32_t(*lpRead)(uint32_t, VirtualMachine*);
typedef bool  (*lpexecuteInstruction)(VirtualMachine*, uint32_t);

int main(int argc, char* argv[])
{
	if (checkDebug())
	{
		MessageBoxA(NULL, "�������Ѽ�⵽��", "����", MB_OK | MB_ICONSTOP);
		ExitProcess(0);
	}
	HINSTANCE hDll; //DLL���
	hDll = LoadLibrary(L"VM.dll"); //���ض�̬��
	lpinitializeVM initializevm;
	lpdestroyVM destroyvm; 
	lpopenFile openfile;
	lpreadFileToCode readfiletocode;
	lpRead read;
	lpexecuteInstruction executeinstruction;
	//��������״̬
	struct runningState* state = (struct runningState*)malloc(sizeof(struct runningState));
	memset(state, 0, sizeof(runningState));
	//�Ƿ����debugģʽ
	state->debugMode = FALSE;
	state->normalMode = FALSE;
	if (argc != 3)
	{
		printf("Usage: normal : ./Loader input.txt -nd\n\t\b\b debug :  ./Loader input.txt -d");
		return 1;
	}
	if (strcmp(argv[2], "-d") == 0 || strcmp(argv[2], "-nd") == 0)
	{
		if (strcmp(argv[2], "-d") == 0)
			state->debugMode = TRUE;
		else
			state->normalMode = TRUE;
	}
	else
		printf("bad command!!!\n\"%s\"->����δ����\n", argv[2]);
	//��ָ�����������
	FILE* file;
	char instr[MAX_LENGTH];
	char instrs[MAX_INSTR][MAX_LENGTH];
	int count = 0;
	// ������洢ָ����Ϣ
	file = fopen(argv[1], "r");
	if (file == NULL) 
	{
		printf("�޷����ļ�����ȷ���ļ�·���Ƿ���ȷ\n");
		return 1;
	}
	while (fgets(instr, MAX_LENGTH, file) != NULL) {
		// ȥ�����з�
		instr[strcspn(instr, "\n")] = '\0';
		strcpy(instrs[count], instr);
		count++;
		if (count >= MAX_INSTR) 
		{
			printf("ָ���������࣬�뼰ʱ����MAX_INSTR\n");
			break;
		}
	}
	fclose(file);
	//����python�ű���ָ�����Ϊ�����Ʊ���
	char command[256];
	snprintf(command, sizeof(command), "python assetobin.py %s", argv[1]);
	int ret = system(command);
	if (ret != 0) 
	{
		printf("���� Python �ű�ʧ�ܡ�\n");
		exit(1);
	}
	//�����Ʊ���·��
	const char* filePath = "output.bin";
	if (hDll != NULL)
	{
		printf("load success!\n");
		initializevm = (lpinitializeVM)GetProcAddress(hDll, "initializeVM");	//��ȡ��̬�����ָ�룬����ֵ������ָ�����
		destroyvm = (lpdestroyVM)GetProcAddress(hDll, "destroyVM");
		openfile = (lpopenFile)GetProcAddress(hDll, "openFile");
		readfiletocode = (lpreadFileToCode)GetProcAddress(hDll, "readFileToCode");
		read = (lpRead)GetProcAddress(hDll, "Read");
		executeinstruction = (lpexecuteInstruction)GetProcAddress(hDll, "executeInstruction");
		bool isGetSuccess =( initializevm != NULL )&&( destroyvm != NULL )&&( executeinstruction != NULL) && (readfiletocode != NULL) && (read != NULL);
		if (isGetSuccess)
		{
			printf("get success!\n");
			if (state->debugMode) {
				printf("�������ģʽ...\n");
				Sleep(1000);
			}
			if (state->normalMode) {
				printf("������ͨģʽ...\n");
				Sleep(1000);
			}
			VirtualMachine* vm = initializevm();//��ʼ��
			bool run = TRUE;//����״̬
			char m[50][33];//�洢ָ��

			int count = readfiletocode(filePath, vm, m);//��ȡ�������ļ��������
			state->continueLoop = TRUE;
			int flag = 0;
			system("cls");
			while (run && state->continueLoop)
			{
				if (state->StepOverCall == 1 && state->nextInstr == vm->re[IP])
				{
					state->StepOverCall = 0;
					state->debugMode = TRUE;
					state->normalMode = FALSE;
				}
				if (state->debugMode)
					debugPrint(vm, m, count, instrs);
				uint32_t instruction = read(vm->re[IP]++, vm);
				run = executeinstruction(vm, instruction);//����ָ��
				if (state->debugMode)
					state = debugControl(vm,instruction);
				else
					state->continueLoop = TRUE;
			}
			destroyvm(vm);//�ݻ������
		}
		FreeLibrary(hDll);
	}
	else
		printf("δ�ɹ�����dll�ļ�����鿴dll�ļ�·���Ƿ���ȷ\n");
	system("pause");
	return 0;
}
