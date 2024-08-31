# README

### 运行

通过直接在终端中加载loader程序即可，程序可以进行debug调试指令

```
./Loader input.txt -nd (normal)
./Loader input.txt -d (debug)
```

**debug模式下**：

①输入其他非法指令将直接终止程序

②F7是单步，F8是步过

③c代表查看栈或者数据段信息：输入s查看栈，d查看数据段，q退出

​													     b代表往后查看，f代表往前查看

##### 路径配置：

运行前需要确定Loader程序源码中的VM.dll动态链接库的路径是否正确

##### 检测调试器

程序具有一定的检测调试器的功能，当在x64_dbg等软件中运行程序时会报错

### 文件作用

**Loader文件夹**:加载程序源码（其中包含debug模式源码，检测调试器源码)

**VM文件夹**:vm指令执行源码

**assetobin.py**：将指令转为二进制编码

**input.txt**:指令文件(目前已经写好一个指令，可以直接运行)

**output.bin**:存储指令转换为的二进制编码

**instructions_annotation.txt**：指令集和寄存器说明信息

