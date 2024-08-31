#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include"debug.h"
#include"detect.h"

//函数指针定义
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
		MessageBoxA(NULL, "调试器已检测到！", "错误", MB_OK | MB_ICONSTOP);
		ExitProcess(0);
	}
	HINSTANCE hDll; //DLL句柄
	hDll = LoadLibrary(L"VM.dll"); //加载动态库
	lpinitializeVM initializevm;
	lpdestroyVM destroyvm; 
	lpopenFile openfile;
	lpreadFileToCode readfiletocode;
	lpRead read;
	lpexecuteInstruction executeinstruction;
	//定义运行状态
	struct runningState* state = (struct runningState*)malloc(sizeof(struct runningState));
	memset(state, 0, sizeof(runningState));
	//是否进入debug模式
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
		printf("bad command!!!\n\"%s\"->符号未定义\n", argv[2]);
	//将指令存入数组中
	FILE* file;
	char instr[MAX_LENGTH];
	char instrs[MAX_INSTR][MAX_LENGTH];
	int count = 0;
	// 用数组存储指令信息
	file = fopen(argv[1], "r");
	if (file == NULL) 
	{
		printf("无法打开文件，请确认文件路径是否正确\n");
		return 1;
	}
	while (fgets(instr, MAX_LENGTH, file) != NULL) {
		// 去除换行符
		instr[strcspn(instr, "\n")] = '\0';
		strcpy(instrs[count], instr);
		count++;
		if (count >= MAX_INSTR) 
		{
			printf("指令数量过多，请及时调整MAX_INSTR\n");
			break;
		}
	}
	fclose(file);
	//调用python脚本将指令解析为二进制编码
	char command[256];
	snprintf(command, sizeof(command), "python assetobin.py %s", argv[1]);
	int ret = system(command);
	if (ret != 0) 
	{
		printf("调用 Python 脚本失败。\n");
		exit(1);
	}
	//二进制编码路径
	const char* filePath = "output.bin";
	if (hDll != NULL)
	{
		printf("load success!\n");
		initializevm = (lpinitializeVM)GetProcAddress(hDll, "initializeVM");	//获取动态库变量指针，并赋值给函数指针变量
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
				printf("进入调试模式...\n");
				Sleep(1000);
			}
			if (state->normalMode) {
				printf("进入普通模式...\n");
				Sleep(1000);
			}
			VirtualMachine* vm = initializevm();//初始化
			bool run = TRUE;//运行状态
			char m[50][33];//存储指令

			int count = readfiletocode(filePath, vm, m);//读取二进制文件到代码段
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
				run = executeinstruction(vm, instruction);//运行指令
				if (state->debugMode)
					state = debugControl(vm,instruction);
				else
					state->continueLoop = TRUE;
			}
			destroyvm(vm);//摧毁虚拟机
		}
		FreeLibrary(hDll);
	}
	else
		printf("未成功加载dll文件，请查看dll文件路径是否正确\n");
	system("pause");
	return 0;
}
