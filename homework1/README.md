# bmp文件读写及RGB和YUV色彩空间转化  

## 文件说明  

- origin.bmp
  - 源bmp文件
- code.cpp  
  - 使用C++完成的读取bmp文件并进行RGB, YUV转化的代码文件
  - 读取origin.bmp，生成结果保存在results文件夹中
  - 使用default.bat脚本时，生成名为program.exe的可执行文件
- show.py
  - 使用PIL和matplotlib进行成果展示
  - 需使用有图形化窗口的操作系统
- default.bat
  - windows shell的脚本，使用该脚本可通过一个指令完成所有步骤
- program.exe
  - default脚本生成的可执行文件，为code.cpp在使用一定优化参数后的编译结果
- results文件夹
  - 用于保存program.exe产生的成果图片

## 环境要求

`此环境仅面向使用default.bat脚本时情况，若需自己编译调试请自行调整配置`

环境下有g++编译器，以及python环境，其中python需确保安装了PIL和matplotlib包，脚本中不含有关于环境配置的指令

## 使用方法

windows环境下，打开shell界面，在本文件夹下输入 ./default.bat {arg1} 即可一键完成所有步骤，其中arg1为改变的Y值，正负皆可，将在原图像转换出的Y值上加上arg1的值（不会超过上下限）  

其他环境下可参考default.bat中的命令进行编译和调试  

如果需要替换源bmp文件，直接更改该文件并更名为origin.bmp即可
