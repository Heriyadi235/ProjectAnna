文件说明
BridgeInterface.exe 今年的平台
ava.exe
belle.exe
carol.exe 三个示例AI

循环赛匹配系统
config.py 配置文件，用于配置各个参赛程序名称，每幅牌随机种子

运行BridgeMatch.py,程序会根据配置，生成所有对局，比赛制为循
环赛，有若干副牌，每副牌双方将交换位置一次。
例如现在文件夹中有3个选手，3副牌，则程序会生成如下18场对局，
（牌谱文件也会如此命名）
局号_南北_vs_东西_牌号
1_ava.exe_vs_bella.exe_11111
2_ava.exe_vs_bella.exe_22222
3_ava.exe_vs_bella.exe_33333
4_bella.exe_vs_ava.exe_11111
5_bella.exe_vs_ava.exe_22222
6_bella.exe_vs_ava.exe_33333
7_ava.exe_vs_carol.exe_11111
8_ava.exe_vs_carol.exe_22222
9_ava.exe_vs_carol.exe_33333
10_carol.exe_vs_ava.exe_11111
11_carol.exe_vs_ava.exe_22222
12_carol.exe_vs_ava.exe_33333
13_bella.exe_vs_carol.exe_11111
14_bella.exe_vs_carol.exe_22222
15_bella.exe_vs_carol.exe_33333
16_carol.exe_vs_bella.exe_11111
17_carol.exe_vs_bella.exe_22222
18_carol.exe_vs_bella.exe_33333

BridgeMatch.py 将根据生成的对局，调用BridgeInterface.exe进行比赛，
实时输出比赛结果，同时生成牌谱。
对局过程差不多如【截图】所示

异常处理办法：BridgeMatch.py 支持初步的断点继续功能，如果对局意外
终端，重新运行脚本即可恢复，也可通过config.py强制指定重开位置。

