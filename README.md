## ProjectAnna

- ProjectAnna is a bridge game program based our later work for CGA  
- ProjectAnna 是一个基于我们在计算机博弈锦标赛作品~~开发~~瞎几把改出来的桥牌游戏  

  `桥牌` `计算机博弈`

### 实现multAI所做的改动
计划在这个分支中实现的功能
- 可自由配置的AI数量以及AI方位

_多AI的主要设计逻辑_  
- 配置字符串的变化  
之前的配置字符串方位参数个数由单一的aiposition变为五个，分别为ai的数量以及四方位ai的编号，ai编号按座次顺时针从0开始，非ai此处值为-1  
例如  
```markdown
3,0,1,-1,2	#表示3个ai， 0 1 2 号ai 分别位于 北 东 西 方向
2,-1,-1,0,1	#表示2个ai， 0 1 号ai 分别位于 南 西 方向
```  
- 数据结构的变化  
 1. ai 标记  
 `int table[4]`存储了每个方位的ai编号 用以在各个轮询中判断应该调用player中的通讯方法 或makemove中等待人类操作的方法（通过是否为-1）
 2. Player  
 Player 类负责管理每个子进程，现在程序启动时，为了省事，并未开启ai编号的实际功能。在任何情况下都将创建4个子进程用以运行ai，各个ai启动路径目前只能于drawtable.cpp 43 行  
 `Player aiPosi[4] = { _T("./ai.exe") ,_T("./ai.exe") ,_T("./ai.exe") ,_T("./ai.exe") };`  
 处修改  
 由于多ai的存在，目前使用 `Player aiPosi[4]` 对他们进行统一的管理 规定 **该对象数组下标为方位（北东南西对应着0123）**
 3. 轮询  
 main函数中的while(1)控制着整个游戏进程 受两个关键量决定 `currentStep`与 `currentPlayer`   
 之前的轮询逻辑如下  
 ```markdown
 //以叫牌为例
if (currentPlayer == aiposition)
 //判断currentPlayer是否为ai(==aiposition)  
 {
    //真 调用Palyer类中对应的方法  
   result = ai.InquireBid();
   wsprintf(output,_T("ai叫牌%d\n"), result);
   outtextxy(0, 14, output);
 }
 else
 {
   //否则 调用MakeMove中对应方法  
   result = MakeBid(currentPlayer, avalibleBid[0]);
 }
 ```  
现在的过程仍然与之前一致，只不过加入了对多个ai的区分  
例:  
```markdown
·判断table[currentPlayer]是否为ai(!= -1 )
·真 调用类aiPosition[currentPlayer] 中的方法  
·否则 调用MakeMove中对应方法  
```
对于诸如gameinfo gameover deal 等直接可以广播的命令 可以处理为
```markdown
//以发牌为例
for (int i = 0; i < 4; i++)
{
  if (table[i] != -1)
    aiPosi[i].UploadDeal(i, playerHands[i]);
  else
    continue;
}
```

### 简介
I have an ~~apple~~ _computer brige AI_  
I have a ~~pen~~ _bridge game platform_   
 **duang!**  
Computer Bridge Game!
### 文件
```markdown
这我哪知道啊，我就知道那小娘们儿长得 嘿！
```markdown
### 食用方法
```markdown
这我哪知道啊，我就知道那小娘们儿长得 嘿！
```
### 目前的bug
```markdown
这我哪知道啊，我就知道那小娘们儿长得 嘿！
```
### 最后一项
```markdown
这我哪知道啊，不过啊，我知道她爹姓许，就是东关卖水那许老头
```
[ 作者的邮箱 ](mailto:1945893330@qq.com) _和_
[ 作者的邮箱 ](mailto:785629645@qq.com)  
[ 一个主页 ](https://heriyadi235.github.io/ProjectAnna/)
