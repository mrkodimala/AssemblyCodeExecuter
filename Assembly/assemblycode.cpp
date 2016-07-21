#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct Symbol{
	char name[24];
	int address;
	int size;
};

struct SymbolTable{
	struct Symbol symbols[127];
	int count;
};

struct Memory{
	int numbers[1023];
	int count;
};

struct BlockAddress{
	char name[12];
	int address;
};

struct BlockCollection{
	struct BlockAddress bd[63];
	int count;
};

struct Instruction{
	int insno;
	int opcode;
	int param[4];
	int count;
	char unused[4];
};


struct InsCollection{
	struct Instruction instructions[95];
	int count;
};

struct Label{
	char label[16];
	int address;
};

struct LabelTable{
	struct Label labels[16];
	int count;
};

int Stack[32];
int top;

void push(int value)
{
	Stack[++top] = value;
}

int pop()
{
	if (top >= 0)
	{
		return Stack[top--];
	}
	return -1;
}


struct InstructionStrings
{
	char name[16];
};

struct InstructionStrings instructions[20];
struct SymbolTable s;
struct InsCollection ic;

void LoadInstructionSet()
{
	strcpy(instructions[0].name, "mov");
	strcpy(instructions[1].name, "mov");
	strcpy(instructions[2].name, "add");
	strcpy(instructions[3].name, "sub");
	strcpy(instructions[4].name, "mul");
	strcpy(instructions[5].name, "jump");
	strcpy(instructions[6].name, "if");
	strcpy(instructions[7].name, "eq");
	strcpy(instructions[8].name, "lt");
	strcpy(instructions[9].name, "gt");
	strcpy(instructions[10].name, "lteq");
	strcpy(instructions[11].name, "gteq");
	strcpy(instructions[12].name, "print");
	strcpy(instructions[13].name, "read");
	strcpy(instructions[14].name, "data");
	strcpy(instructions[15].name, "const");
	strcpy(instructions[16].name, "start:");
	strcpy(instructions[17].name, "end");
	strcpy(instructions[18].name, "else");
	strcpy(instructions[19].name, "IFEND");
}

int startflag;

int InstructionNumber(char *instruction)
{
	for (int i = 0; i < 20; i++)
	{
		if (!strcmpi(instruction, instructions[i].name))
		{
			return i + 1;
		}
	}
	int length = strlen(instruction);
	if (instruction[length - 1] == ':')
		return 0;
	return -1;
}

int RegisterSet[8];

int checkifRegister(char *name)
{
	if (!strcmpi(name, "AX"))return 0;
	if (!strcmpi(name, "BX"))return 1;
	if (!strcmpi(name, "CX"))return 2;
	if (!strcmpi(name, "DX"))return 3;
	if (!strcmpi(name, "EX"))return 4;
	if (!strcmpi(name, "FX"))return 5;
	if (!strcmpi(name, "GX"))return 6;
	if (!strcmpi(name, "HX"))return 7;
	return -1;
}

int checkifVariableExists(char *name)
{
	for (int i = 0; i < s.count; i++)
	{
		if (!strcmp(s.symbols[i].name, name))
			return s.symbols[i].address;
	}
	return -1;
}

int checkifNameisArray(char *name)
{
	int i;
	int bracketcount = 0;
	int validbrackets=0;
	int numberscount = 0;
	int gapbtwnbrackets = 0;
	int flag=0;
	for (i = 0; name[i] != '\0'; i++)
	{
		if (name[i] == '[')
		{
			bracketcount++;
			validbrackets++;
			flag = 1;
		}
		else if (name[i] == ']')
		{
			bracketcount++;
			validbrackets--;
			flag = 0;
		}
		else if (name[i] >= '0'&&name[i] <= '9')
		{
			numberscount++;
			if (flag == 1)
				gapbtwnbrackets++;
		}
	}
	if (bracketcount == 2 && validbrackets == 0 && numberscount == gapbtwnbrackets)
	{
		for (i = 0; name[i] != '['; i++);
		name[i] = '\0';
		i++;
		char temp[5];
		int offset = 0;
		for (; name[i] != ']'; i++)
			temp[offset++] = name[i];
		temp[offset] = '\0';
		return atoi(temp);
	}
	return -1;
}

int ProcessMoveInstruction(char *instruction)
{
	if (startflag == 0)
		return -1;
	int i;
	for (i = 0; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char temp_ins[16];
	int offset = 0;
	for (; instruction[i] != ' '&&instruction[i] != '\t'; i++)
		temp_ins[offset++] = instruction[i];
	temp_ins[offset] = '\0';
	char name1[8];
	for (; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	offset = 0;
	for (; instruction[i] != ' '&&instruction[i] != '\t'&&instruction[i]!=','; i++)
		name1[offset++] = instruction[i];
	name1[offset] = '\0';
	for (; instruction[i] == ' ' || instruction[i] == '\t'||instruction[i]==','; i++);
	offset = 0;
	char name2[8];
	for (; instruction[i] != ' '&&instruction[i] != '\t'&&instruction[i] != '\n'&&instruction[i]!='\0'; i++)
		name2[offset++] = instruction[i];
	name2[offset] = '\0';
	int regflag1 = checkifRegister(name1);
	int regflag2 = checkifRegister(name2);
	int arrayflag1 = checkifNameisArray(name1);
	int arrayflag2 = checkifNameisArray(name2);
	int symflag1 = checkifVariableExists(name1);
	int symflag2 = checkifVariableExists(name2);
	//printf("%d\t%d\t%d\t%d\t%d\t%d\n", regflag1, regflag2, arrayflag1, arrayflag2, symflag1, symflag2);
	if (regflag1 == -1 && regflag2 == -1)
		return -1;
	if (regflag1 != -1 && symflag2 != -1)
	{
		struct Instruction ins;
		ins.opcode = 2;
		ins.insno = ic.count+1;
		ins.count = 0;
		ins.param[ins.count++] = regflag1;
		if (arrayflag2 != -1)
		{
			ins.param[ins.count++] = symflag2 + arrayflag2;
		}
		else{
			ins.param[ins.count++] = symflag2;
		}
		ic.instructions[ic.count++] = ins;
	}
	else if (symflag1 != -1 && regflag2 != -1)
	{
		struct Instruction ins;
		ins.opcode = 1;
		ins.insno = ic.count + 1;
		ins.count = 0;
		if (arrayflag1 != -1){
			ins.param[ins.count++] = symflag1 + arrayflag1;
		}
		else{
			ins.param[ins.count++] = symflag1;
		}
		ins.param[ins.count++] = regflag2;
		ic.instructions[ic.count++] = ins;
	}
	/*struct Instruction ins;
	ins = ic.instructions[ic.count - 1];
	printf("%d\t%d\t", ins.insno, ins.opcode);
	for (i = 0; i < ins.count; i++)
		printf("%d\t", ins.param[i]);
	printf("\n");*/
	return 0;
}


int ProcessDataInstruction(char *instruction)
{
	if (startflag == 1)
		return -1;
	int i;
	for (i = 0; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char temp[16];
	int offset = 0;
	for (; instruction[i] != ' '&&instruction[i] != '\t'; i++)
		temp[offset++] = instruction[i];
	temp[offset] = '\0'; 
	for (; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char name[16];
	offset = 0;
	for (; instruction[i] != '\n'&&instruction[i] != '\0'; i++)
		name[offset++] = instruction[i];
	name[offset] = '\0';
	int constvalue = -999;
	if (!strcmpi(temp, "const"))
	{
		for (i = 0; name[i] != ' '&&name[i] != '='; i++);
		name[i] = '\0';
		i++;
		char value[10];
		offset = 0;
		for (; name[i] == ' ' || name[i] == '='||name[i]=='\t'; i++);
		for (; name[i] != '\0'; i++)
			value[offset++] = name[i];
		value[offset] = '\0';
		constvalue = atoi(value);
	}
	int flag = checkifNameisArray(name);
	struct Symbol symbol;
	int nextfreeadress;
	if (s.count > 0)
	{
		symbol = s.symbols[s.count - 1];
		nextfreeadress = symbol.address + symbol.size;
	}
	else{
		nextfreeadress = 0;
	}
	symbol.address = nextfreeadress;
	strcpy(symbol.name, name);
	if (flag < 0){
		symbol.size = 1;
	}
	else{
		symbol.size = flag;
	}
	s.symbols[s.count++] = symbol;
	//printf("%s\t%d\t%d\n", symbol.name, symbol.address, symbol.size);
	if (constvalue != -999)
	{
		FILE *file = fopen("memory.bin", "rb+");
		struct Memory m;
		fread(&m, sizeof(struct Memory), 1, file);
		m.numbers[symbol.address] = constvalue;
		fseek(file, 0, SEEK_SET);
		fwrite(&m, sizeof(m), 1, file);
		fclose(file);
	}
	return 0;
}


int ProcessReadInstruction(char *instruction,int opcode)
{
	if (startflag == 0)
		return -1;
	int i;
	for (i = 0; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char temp[16];
	int offset=0;
	for (; instruction[i] != ' '&&instruction[i]!='\t'; i++)
		temp[offset++] = instruction[i];
	temp[offset] = '\0';
	for (; instruction[i] == ' '||instruction[i] == '\t'; i++);
	offset = 0;
	char name[8];
	for (; instruction[i] != ' '&&instruction[i] != '\0'&& instruction[i] != '\n'; i++)
		name[offset++] = instruction[i];
	name[offset] = '\0';
	int flag = checkifRegister(name);
	if (flag < 0)
		return flag;
	struct Instruction ins;
	ins.insno = ic.count + 1;
	ins.opcode = opcode;
	ins.param[0] = flag;
	ins.count = 1;
	ic.instructions[ic.count++] = ins;
	return 0;
}



int ProcessArithmeticInstruction(char *instruction, int opcode)
{
	if (startflag == 0)
		return -1;
	int i;
	for (i = 0; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char temp[16];
	int offset = 0;
	for (; instruction[i] != ' '&&instruction[i] != '\t'; i++)
		temp[offset++] = instruction[i];
	temp[offset] = '\0';
	for (; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char name1[5];
	offset = 0;
	for (; instruction[i] != ' '&&instruction[i] != ','; i++)
		name1[offset++] = instruction[i];
	name1[offset] = '\0';
	for (; instruction[i] == ' ' || instruction[i] == ','; i++);
	char name2[5];
	offset = 0;
	for (; instruction[i] != ' '&&instruction[i] != ','; i++)
		name2[offset++] = instruction[i];
	name2[offset] = '\0';
	for (; instruction[i] == ' ' || instruction[i] == ','; i++);
	char name3[5];
	offset = 0;
	for (; instruction[i] != ' '&&instruction[i] != ','&&instruction[i]!='\0'; i++)
		name3[offset++] = instruction[i];
	name3[offset] = '\0';
	int regflag1 = checkifRegister(name1);
	int regflag2 = checkifRegister(name2);
	int regflag3 = checkifRegister(name3);
	if (regflag1 == -1 || regflag2 == -1 || regflag3 == -1)
		return -1;
	struct Instruction ins;
	ins.count = 0;
	ins.insno = ic.count + 1;
	ins.opcode = opcode;
	ins.param[ins.count++] = regflag1;
	ins.param[ins.count++] = regflag2;
	ins.param[ins.count++] = regflag3;
	ic.instructions[ic.count++] = ins;
	/*ins = ic.instructions[ic.count - 1];
	printf("%d\t%d\t", ins.insno, ins.opcode);
	for (i = 0; i < ins.count; i++)
		printf("%d\t", ins.param[i]);
	printf("\n");*/
	return 0;
}

struct LabelTable lbletble;

void PrintLabelTable()
{
	printf("\nLabel Table\n");
	for (int i = 0; i < lbletble.count; i++)
	{
		printf("%s\t%d\n", lbletble.labels[i].label, lbletble.labels[i].address);
	}
}

void PrintStack()
{
	printf("\nStack Table\n");
	for (int i = 0; i <= top; i++)
		printf("%d\t", Stack[i]);
}


int ProcessLabelInstruction(char *labelname)
{
	struct Label label;
	int length = strlen(labelname);
	if (labelname[length - 1] == ':')
		labelname[length - 1] = '\0';
	strcpy(label.label, labelname);
	label.address = ic.count+1;
	lbletble.labels[lbletble.count++] = label;
	return 0;
}

int ProcessIfInstruction(char *instruction)
{
	int i;
	for (i = 0; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	int offset;
	for (; instruction[i] != ' ' && instruction[i] != '\t'; i++);
	for (; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char name1[5];
	offset = 0;
	for (; instruction[i] != ' ' && instruction[i] != '\t'&&instruction[i] != '\0'&&instruction[i] != '\n'; i++)
		name1[offset++] = instruction[i];
	name1[offset] = '\0';
	for (; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char condition[5];
	offset = 0;
	for (; instruction[i] != ' ' && instruction[i] != '\t'&&instruction[i] != '\0'&&instruction[i] != '\n'; i++)
		condition[offset++] = instruction[i];
	condition[offset] = '\0';
	for (; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	char name2[5];
	offset = 0;
	for (; instruction[i] != ' ' && instruction[i] != '\t'&&instruction[i] != '\0'&&instruction[i] != '\n'; i++)
		name2[offset++] = instruction[i];
	name2[offset] = '\0';
	int k1 = checkifRegister(name1);
	int k2 = checkifRegister(name2);
	int condition_flag = InstructionNumber(condition);
	if (k1 == -1 || k2 == -1 || condition_flag == -1)
		return -1;
	struct Instruction ins;
	ins.insno = ic.count+1;
	ins.count = 0;
	ins.opcode = 7;
	ins.param[ins.count++] = k1;
	ins.param[ins.count++] = condition_flag;
	ins.param[ins.count++] = k2;
	ins.param[ins.count++] = -25;
	ic.instructions[ic.count++] = ins;
	push(ins.insno);
	return 0;
}

int ProcessElseInstruction(char *name)
{
	struct Label lbl;
	strcpy(lbl.label, name);
	lbl.address = ic.count + 2;
	lbletble.labels[lbletble.count++] = lbl;
	struct Instruction ins;
	ins.insno = ic.count + 1;
	ins.count = 0;
	ins.opcode = 6;
	ins.param[ins.count++] = -25;
	ic.instructions[ic.count++] = ins;
	push(ins.insno);
	return 0;
}

int CheckifLabelExists(char *name)
{
	int i = 0;
	for (i=0; i < lbletble.count; i++)
	{
		if (!strcmpi(name, lbletble.labels[i].label))
		{
			return  lbletble.labels[i].address;
		}
	}
	return -1;
}

int ProcessJumpInstruction(char *instruction)
{
	int i;
	for (i = 0; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	for (; instruction[i] != ' '&&instruction[i] != '\t'; i++);
	int offset = 0;
	char name[16];
	for (; instruction[i] == ' ' || instruction[i] == '\t'; i++);
	for (; instruction[i] != ' '&&instruction[i] != '\t'&&instruction[i] != '\0'&&instruction[i] != '\n'; i++)
		name[offset++] = instruction[i];
	name[offset] = '\0';
	int k = CheckifLabelExists(name);
	if (k <= 0)
		return -1;
	struct Instruction ins;
	ins.insno = ic.count + 1;
	ins.opcode = 6;
	ins.count = 0;
	ins.param[ins.count++] = k;
	ic.instructions[ic.count++] = ins;
	return 0;
}



int checkifinstructionIsvalid(char *instruction)
{
	int i;
	for (i = 0; instruction[i] == ' '||instruction[i]=='\t'; i++);
	char temp_ins[16];
	int offset = 0;
	for (; instruction[i] != ' '&&instruction[i]!='\t'&&instruction[i]!='\0'&&instruction[i]!='\n'; i++)
		temp_ins[offset++] = instruction[i];
	temp_ins[offset] = '\0';
	int n = InstructionNumber(temp_ins);
	if (n < 0)
	{
		printf("Invalid instruction\n");
		return 0;
	}
	if (n == 0)
	{
		ProcessLabelInstruction(temp_ins);
	}
	else if (n == 1)
	{
		int flag = ProcessMoveInstruction(instruction);
		if (flag == -1)
		{
			printf("There is some error in instructions order");
			return -1;
		}
	}
	else if (n == 2)
	{
		printf("%d\n", n);
	}
	else if (n == 3)
	{
		int flag = ProcessArithmeticInstruction(instruction,n);
		if (flag == -1)
		{
			printf("There is some error in instructions order");
			return -1;
		}
	}
	else if (n == 4)
	{
		int flag = ProcessArithmeticInstruction(instruction, n);
		if (flag == -1)
		{
			printf("There is some error in instructions order");
			return -1;
		}
	}
	else if (n == 5)
	{
		int flag = ProcessArithmeticInstruction(instruction, n);
		if (flag == -1)
		{
			printf("There is some error in instructions order");
			return -1;
		}
	}
	else if (n == 6)
	{
		int flag=ProcessJumpInstruction(instruction);
		if (flag == -1)
		{
			printf("There is some error in intructions\n");
			return -1;
		}
	}
	else if (n == 7)
	{
		ProcessIfInstruction(instruction);
	}
	else if (n == 13)
	{
		int flag = ProcessReadInstruction(instruction, 13);
		if (flag == -1)
		{
			printf("There is some error in instructions order");
			return -1;
		}
	}
	else if (n == 14)
	{
		int flag = ProcessReadInstruction(instruction,14);
		if (flag == -1)
		{
			printf("There is some error in instructions order");
			return -1;
		}
	}
	else if (n == 15)
	{
		int flag = ProcessDataInstruction(instruction);
		if (flag == -1)
		{
			printf("There is some error in instructions order");
			return -1;
		}
	}
	else if (n == 16)
	{
		int flag = ProcessDataInstruction(instruction);
		if (flag == -1)
		{
			printf("There is some error in instructions order");
			return -1;
		}
	}
	else if (n == 17)
	{
		startflag = 1;
	}
	else if (n == 18)
	{
		return -1;
	}
	else if (n == 19)
	{
		ProcessElseInstruction(temp_ins);
	}
	else if (n == 20)
	{
		ProcessLabelInstruction(temp_ins);
	}
	return 1;

}

void ReadFromtheFile(char *filename)
{
	FILE *file = fopen(filename, "r");
	char buffer[1024];
	int count = 0;
	while (!feof(file))
	{
		count++;
		fgets(buffer, 1024, file);
		//printf("%s", buffer);
		int i;
		for (i = 0; buffer[i] != '\n'&&buffer[i] != '\0'; i++);
		buffer[i] = '\0';
		int flag=checkifinstructionIsvalid(buffer);
		if (flag == -1)
			return;
	}
	fclose(file);
}

void LoadAllGlobals()
{
	LoadInstructionSet();
	startflag = 0;
	s.count = 0;
	ic.count = 0;
	top = -1;
	lbletble.count = 0;
}

void PrintInstructions()
{
	struct Instruction ins;
	for (int i = 0; i < ic.count; i++)
	{
		ins = ic.instructions[i];
		printf("%d\t%d\t", ins.insno, ins.opcode);
		for (int j = 0; j < ins.count; j++)
			printf("%d\t", ins.param[j]);
		printf("\n");
	}
}

void ClearMemory()
{
	struct Memory m;
	FILE *file = fopen("memory.bin", "wb");
	memset(&m, 0, sizeof(m));
	fwrite(&m, sizeof(m), 1, file);
	fclose(file);
}


void PerformMoveOperation(struct Instruction ins)
{
	int opcode = ins.opcode;
	FILE *file = fopen("memory.bin", "rb+");
	struct Memory m;
	fread(&m, sizeof(m), 1, file);
	if (opcode == 1)
	{
		int data = RegisterSet[ins.param[1]];
		m.numbers[ins.param[0]] = data;
	}
	else if (opcode == 2)
	{
		int data = m.numbers[ins.param[1]];
		RegisterSet[ins.param[0]] = data;
	}
	fseek(file, 0, SEEK_SET);
	fwrite(&m, sizeof(m), 1, file);
	fclose(file);
}

void PerformArithmeticOperation(struct Instruction ins)
{
	int a = RegisterSet[ins.param[1]];
	int b = RegisterSet[ins.param[2]];
	int c;
	switch (ins.opcode)
	{
	case 3:c = a + b; break;
	case 4:c = a - b; break;
	case 5:c = a*b; break;
	}
	RegisterSet[ins.param[0]] = c;
}

void PerformReadOperation(struct Instruction ins)
{
	printf("Enter the Number\n");
	int n;
	scanf("%d", &n);
	RegisterSet[ins.param[0]] = n;
}

int PerformIfOperation(struct Instruction ins)
{
	int a = ins.param[0]; a = RegisterSet[a];
	int b = ins.param[1]; b = RegisterSet[b];
	int condition = ins.param[2];
	if (condition == 8)
	{
		if (a == b)
			return ins.insno;
		else
			return ins.param[3] - 1;
	}
	else if (condition == 9)
	{
		if (a < b)
			return ins.insno;
		else
			return ins.param[3] - 1;
	}
	else if (condition == 10)
	{
		if (a > b)
			return ins.insno;
		else
			return ins.param[3] - 1;
	}
	else if (condition == 11)
	{
		if (a <= b)
			return ins.insno;
		else
			return ins.param[3] - 1;
	}
	else if (condition == 12)
	{
		if (a >= b)
			return ins.insno;
		else
			return ins.param[3] - 1;
	}
}

int ProcessInstructionSet()
{
	struct Instruction ins;
	for (int i = 0; i < ic.count; i++)
	{
		ins = ic.instructions[i];
		int k = ins.opcode;
		if (k == 1 || k == 2)
		{
			PerformMoveOperation(ins);
		}
		else if (k == 3 || k == 4 || k == 5)
		{
			PerformArithmeticOperation(ins);
		}
		else if (k == 14)
		{
			PerformReadOperation(ins);
		}
		else if (k == 13)
		{
			printf("Value=%d\n", RegisterSet[ins.param[0]]);
		}
		else if (k == 6)
		{
			i = ins.param[0]-1;
		}
		else if (k == 7)
		{
			i=PerformIfOperation(ins);
		}
	}
	getchar();
	return 0;
}

void ProcessStackAddresses()
{
	int insno = pop();
	while (insno >= 0)
	{
		struct Instruction ins = ic.instructions[insno - 1];
		if (ins.opcode == 6)
		{
			int k = CheckifLabelExists("ifend");
			if (k == -1)
			{
				printf("There is some error\n");
				return;
			}
			else{
				ins.param[0] = k;
			}
			ic.instructions[insno - 1] = ins;
		}
		else if (ins.opcode == 7)
		{
			int k = CheckifLabelExists("else");
			if (k == -1)
			{
				printf("There is some error\n");
				return;
			}
			else{
				ins.param[3] = k;
			}
			ic.instructions[insno - 1] = ins;
		}
		else{
			printf("Some error encountered please review your code\n");
			return;
		}
		insno = pop();
	}
}

int main()
{

	FILE *file;
	errno_t e = fopen_s(&file, "memory.bin", "rb");
	if (e != 0)
	{
		char command[100] = "fsutil file createnew memory.bin 4194304";
		system(command);
	}
	else{
		fclose(file);
	}
	LoadAllGlobals();
	ClearMemory();
	ReadFromtheFile("program.asm");
	//PrintInstructions();
	//PrintLabelTable();
	//PrintStack();
	ProcessStackAddresses();
	//PrintInstructions();
	ProcessInstructionSet();
	getchar();
	return 0;
}



