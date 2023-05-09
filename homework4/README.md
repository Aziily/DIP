# 图像转换

## 文件说明  

- origin.bmp
  - 源bmp文件
- code.cpp  
  - 使用C++完成的读取bmp，以及增强和均值化操作
  - 生成结果保存在results文件夹中
  - 使用default.bat脚本时，生成名为program.exe的可执行文件
- show.py
  - 使用PIL和matplotlib进行成果展示
  - 需使用有图形化窗口的操作系统
- default.bat
  - windows shell的脚本，使用该脚本可通过一个指令完成所有步骤
- program.exe
  - default脚本生成的可执行文件，为code.cpp在使用一定编译参数后的编译结果
  - 无需传入参数
- results文件夹
  - 用于保存program.exe产生的成果图片

## 环境要求

`此环境仅面向使用default.bat脚本时情况，若需自己编译调试请自行调整配置`

环境下有g++编译器，以及python环境（3.9。0及以上版本）

## 使用方法

windows环境下，打开shell界面，在本文件夹下输入 ./default.bat 即可一键完成所有步骤（或直接点击脚本文件）

其他环境下可参考default.bat中的命令进行编译和调试  

如果需要替换源bmp文件，直接更改该文件并更名为origin.bmp即可，注意需要为标准的bmp格式（可通过在windows用图画打开图像并保存为bmp）

本次实验中将用户输入参数过程写在了程序当中，需要在运行脚本后根据提示进行输入，才能正常使用程序
