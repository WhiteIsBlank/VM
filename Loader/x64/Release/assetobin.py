import sys
import re
def main():
    # 检查是否传递了文件名参数
    if len(sys.argv) < 2:
        print("未指定文件名")
        return
    # 获取文件名
    filename = sys.argv[1]
    try:
        with open(filename, "r",encoding="utf-8") as input_file:
            lines = input_file.readlines()
            instructions = []
            for line in lines:
                line = line.strip()  # 去除行尾的换行符和空格
                if line:  # 确保读取到的行不为空
                    inst = handle(line)
                    instructions.append(inst)
            binaryfile = "output.bin"  
            savebinary(binaryfile, instructions)
            print("指令转换完成")
    except FileNotFoundError:
        print(f"文件 '{filename}' 不存在")
    except Exception as e:
        print("读取文件时出错:", str(e))
def handle(line):
    result = re.findall(r'[a-zA-Z0-9-]+', line)
    inst = judge(result) 
    return inst
def savebinary(binaryfile, instructions):
    with open(binaryfile, 'w',encoding="utf-8") as output_file: 
        for binary in instructions:
            output_file.write(binary) 
def judge(result):
    inst = ""
    if result[0] == 'HALT':inst = halt_inst(result)
    elif result[0] == 'PUSH':inst = push_inst(result)
    elif result[0] == 'POP':inst = pop_inst(result)
    elif result[0] == 'ADD':inst = add_inst(result)
    elif result[0] == 'SUB':inst = sub_inst(result)
    elif result[0] == 'MUL':inst = mul_inst(result)
    elif result[0] == 'DIV':inst = div_inst(result)
    elif result[0] == 'MOD':inst = mod_inst(result)
    elif result[0] == 'MOV':inst = mov_inst(result)
    elif result[0] == 'AND':inst = and_inst(result)
    elif result[0] == 'OR':inst = or_inst(result)
    elif result[0] == 'XOR':inst = xor_inst(result)
    elif result[0] == 'NOT':inst = not_inst(result)
    elif result[0] == 'SHL':inst = shl_inst(result)
    elif result[0] == 'SHR':inst = shr_inst(result)
    elif result[0] == 'CMP':inst = cmp_inst(result)
    elif result[0] == 'FADD':inst = fadd_inst(result)
    elif result[0] == 'FSUB':inst = fsub_inst(result)
    elif result[0] == 'FMUL':inst = fmul_inst(result)
    elif result[0] == 'FDIV':inst = fdiv_inst(result)
    elif result[0] == 'FMOV':inst = fmov_inst(result)
    elif result[0] == 'FCMP':inst = fcmp_inst(result)
    elif result[0] == 'LOOP':inst = loop_inst(result)
    elif result[0] == 'JMP' or result[0] == 'JBE' or result[0] == 'JB' or result[0] == 'JAE' or result[0] == 'JA' or result[0] == 'JE' or result[0] == 'JNE':inst = jmp_inst(result)
    elif result[0] == 'CALL':inst = call_inst(result)
    elif result[0] == 'RET':inst = ret_inst(result)
    elif result[0] == 'STORE':inst = store_inst(result)
    elif result[0] == 'LOAD':inst = load_inst(result)
    elif result[0] == 'NOP':inst = nop_inst(result)
    else:
        print("Illegal operand, please check your instructions")
        sys.exit(1)
    return inst        
def halt_inst(result):
    return "00000000000000000000000000000000"
def push_inst(result):
    inst = "00000001"
    if len(result) == 2:
        if result[1][0].isalpha():
            inst += "00"
            inst += register_code(result[1])
            inst += "00000000000000000"
        elif result[1][0].isdigit() or (result[1][0] == '-' and result[1][1].isdigit()):
            inst += "01"
            inst += imm_code(result[1],'0>22b')
        else:
            print("Floating-point stack not supported\n")
            printBad()
    else:
        printBad()
    return inst
def pop_inst(result):
    inst = "00000010"
    if len(result) == 2:
        inst += "00"
        inst += register_code(result[1])
        inst += "00000000000000000"
    elif len(result) == 1:
        inst += "01"
        inst += "0000000000000000000000"
    else:
        printBad()
    return inst
def add_inst(result):return "00000011"+arithmetic_code(result)
def sub_inst(result):return "00000100"+arithmetic_code(result)
def mul_inst(result):return "00000101"+arithmetic_code(result)
def div_inst(result):return "00000110" + arithmetic_code(result)
def mod_inst(result):return "00000111"+arithmetic_code(result)
def mov_inst(result):
    inst = "00001000"
    inst += register_code(result[1])
    if len(result) == 3:
        if result[2][0].isalpha():
            inst += "00"
            inst += "000000000000"
            inst += register_code(result[2])
        elif result[2][0].isdigit() or (result[2][0] == '-' and result[2][1].isdigit()):
            inst += "01"
            inst += imm_code(result[2],'0>17b')
        else:
            printBad()
    elif len(result) == 4:
        inst += "10"
        inst += "00000"
        inst += imm_code(result[3],'0>7b')
        inst += register_code(result[2])
    else:
        printBad()
    return inst
def and_inst(result):return "00001001"+arithmetic_code(result)
def or_inst(result):return "00001010" + arithmetic_code(result)
def xor_inst(result):return "00001011"+arithmetic_code(result)
def not_inst(result):
    inst = "00001100"
    inst += register_code(result[1])
    if len(result) == 3:
        if result[2][0].isalpha():
            inst += "00"
            inst += "000000000000"
            inst += register_code(result[2])
        elif result[2][0].isdigit() or (result[2][0] == '-' and result[2][1].isdigit()):
            inst += "01"
            inst += imm_code(result[2],'0>17b')
        else:
            printBad()
    elif len(result) == 4:
        inst += "10"
        inst += "00000"
        inst += imm_code(result[3],'0>7b')
        inst += register_code(result[2])
    else:
        printBad()
    return inst
def shl_inst(result):return "00001101"+arithmetic_code(result)
def shr_inst(result):return "00001110"+arithmetic_code(result)
def cmp_inst(result):
    inst = "00001111"
    inst += register_code(result[1])
    if len(result) == 3:
        if result[2][0].isalpha():
            inst += "00"
            inst += "000000000000"
            inst += register_code(result[2])
        elif result[2][0].isdigit() or (result[2][0] == '-' and result[2][1].isdigit()):
            inst += "01"
            inst += imm_code(result[2],'0>17b')
        else:
            printBad()
    elif len(result) == 4:
        inst += "10"
        inst += "00000"
        inst += imm_code(result[3],'0>7b')
        inst += register_code(result[2])
    else:
        printBad()
    return inst
def fadd_inst(result):return "00010000" + farithmetic_code(result)
def fsub_inst(result):return "00010001" + farithmetic_code(result)
def fmul_inst(result):return "00010010" + farithmetic_code(result)
def fdiv_inst(result):return "00010011" + farithmetic_code(result)
def fmov_inst(result):
    inst = "00010100"
    inst += register_code(result[1])
    if len(result) == 3:
        inst += "00"
        inst += "000000000000"
        inst += register_code(result[2])
    elif len(result) == 4:
        if result[2][0].isalpha():
            inst += "10"
            inst += "00000"
            inst += imm_code(result[3],'0>7b')
            inst += register_code(result[2])
        elif result[2][0].isdigit() or (result[2][0] == '-' and result[2][1].isdigit()):
            inst += "01"
            if int(result[2]) >= 0:inst += "0"
            else:inst += "1"
            inst += imm_code(str(abs(int(result[2]))),'0>6b')
            inst += imm_code(result[3],'0>10b')
        else:printBad()
    else:printBad()
    return inst
def fcmp_inst(result):
    inst = "00010101"
    inst += register_code(result[1])
    if len(result) == 3:
        inst += "00"
        inst += "000000000000"
        inst += register_code(result[2])
    elif len(result) == 4:
        if result[2][0].isalpha():
            inst += "10"
            inst += imm_code(result[3],'0>7b')
            inst += register_code(result[2])
        elif result[2][0].isdigit() or (result[2][0] == '-' and result[2][1].isdigit()):
            inst += "01"
            if int(result[2]) >= 0:inst += "0"
            else:inst += "1"
            inst += imm_code(str(abs(int(result[2]))),'0>6b')
            inst += imm_code(result[3],'0>10b')
        else:printBad()
    else:printBad()
    return inst
def loop_inst(result):return "00010110" + "0000000000000000000" + register_code(result[1])
def jmp_inst(result):return "00010111" + jp_flag(result[0]) + "0000000000000000" + register_code(result[1]) 
def call_inst(result):return "00011000" + "0000000000000000000" + register_code(result[1])
def ret_inst(result):
    if result[1] == 'V':return "00011001000000000000000000000000"
    elif result[1] == 'I':return "00011001000000000000000000000001"
    elif result[1] == "F":return "00011001000000000000000000000010"
    else:print("The return value type is not defined, currently only null , integer and floating-point types are supported")
def store_inst(result):
    inst = "00011010"
    if len(result) == 4:
        if result[3][0].isalpha():
            inst += register_code(result[3])
            inst += "000000"
            inst += "0"
        elif result[3][0].isdigit() or (result[3][0] == '-' and result[3][1].isdigit()):
            inst += imm_code(result[3],'0>11b')
            inst += "1"
        else:printBad()
    else:printBad()
    inst += imm_code(result[2],'0>7b')
    inst += register_code(result[1])
    return inst
def load_inst(result):return "00011011"+register_code(result[1])+"0000000"+imm_code(result[3],'0>7b')+register_code(result[2])
def nop_inst(result):return "00011100000000000000000000000000"
def printBad():
    print("Illegal instruction detected")
    sys.exit(1)
def arithmetic_code(result):
    code = ""
    if len(result) != 4 or len(result) != 5:
        code += register_code(result[1])
        code += register_code(result[2])
        if len(result) == 4:
            if result[3][0].isalpha():
                code += "00"
                code += "0000000"
                code += register_code(result[3])
            elif result[3][0].isdigit() or (result[3][0] == '-' and result[3][1].isdigit()):
                code += "01"
                code += imm_code(result[3],'0>12b')
            else:
                print("Illegal instruction detected")
                sys.exit(1)
        else:
            code += "10"
            code += imm_code(result[4],'0>7b')
            code += register_code(result[3])
    else:
        print("Illegal instruction detected")
        sys.exit(1)
    return code
def farithmetic_code(result):
    code = ""
    if len(result) != 4 or len(result) != 5:
        code += register_code(result[1])
        code += register_code(result[2])
        if len(result) == 4:
            code += "00"
            code += "0000000"
            code += register_code(result[3])
        else:
            if result[3][0].isalpha():
                code += "10"
                code += imm_code(result[4],'0>7b')
                code += register_code(result[3])
            elif result[3][0].isdigit() or (result[3][0] == '-' and result[3][1].isdigit()):
                code += "01"
                if int(result[3]) >= 0:code += "0"
                else:code += "1"
                code += imm_code(str(abs(int(result[3]))),'0>4b')
                code += imm_code(result[4],'0>7b')
            else:printBad()
    else:printBad()
    return code
def imm_code(imm,n):
    code = ""
    try:
        if len(imm) > 1:
            if imm[1] == "x" or imm[1] == "X":code = format(int(imm,16),n)
            elif imm[1] == "o" or imm[1] == "O":code = format(int(imm,8),n)
            elif imm[1] == "b" or imm[1] == "B":code = format(int(imm,2),n)
            else:code = convert_to_binary(int(imm),int(re.findall(r'>(\d+)b', n)[0]) )
        else: code = convert_to_binary(int(imm),int(re.findall(r'>(\d+)b', n)[0]) )
    except ValueError:
        print("Illegal immediate value, please check instructions")
        sys.exit(1)
    return code
def convert_to_binary(num, width):
    if num < 0:
        num = 2 ** width + num  # 将负数转换为对应的正数表示
    binary = bin(num)[2:]  # 将数字转换为二进制字符串
    binary = binary.zfill(width)  # 在开头补零
    return binary
def jp_flag(f):
    code = ""
    if f == "JMP":code = "000"
    elif f == "JBE":code = "001"
    elif f == "JB":code = "010"
    elif f == "JAE":code = "011"
    elif f == "JA":code = "100"
    elif f == "JE":code = "101"
    elif f == "JNE":code = "110"
    else:
        print("Illegal jump command, please check your command")
        sys.exit(1)
    return code
def register_code(r):
    code = ""
    if r == "R0":code = "00000"
    elif r == "R1":code = "00001"
    elif r == "R2":code = "00010"
    elif r == "R3":code = "00011"
    elif r == "R4":code = "00100"
    elif r == "R5":code = "00101"
    elif r == "SP":code = "00110"
    elif r == "BP":code = "00111"
    elif r == "IP":code = "01000"
    elif r == "FLAGS":code = "01001"
    elif r == "JP":code = "01010"
    elif r == "OP":code = "01011"
    elif r == "CS":code = "01100"
    elif r == "DS":code = "01101"
    elif r == "ST0":code = "01110"
    elif r == "ST1":code = "01111"
    elif r == "ST2":code = "10000"
    elif r == "ST3":code = "10001"
    elif r == "ST4":code = "10010"
    else:
        print("Illegal register, please check the instruction")
        sys.exit(1)
    return code
if __name__ == "__main__":
    main()