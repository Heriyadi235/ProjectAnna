"""
匹配系统(联网版)
启动服务，等待参赛选手连接后，拉起牌桌
生成每个对局
"""
import os
import pickle
import socket
import threading
import config
import pandas as pd
from BridgeGameServer import BridgeGameServer
from RemoteBridgePlayer import RemoteBridgePlayer
from RemoteBridgePlayer import RemoteBridgeTeam
from datetime import datetime
import warnings
NORTH = '0'
EAST = '1'
SOUTH = '2'
WEST = '3'
NONE = '0'
NS = '1'
EW = '2'
BOTH = '3'


class BridgeTable(object):
    """
    保存每个对局信息的数据结构
    """

    def __init__(self, north:RemoteBridgePlayer, south:RemoteBridgePlayer, east:RemoteBridgePlayer, west:RemoteBridgePlayer, seed=000000, roundNum='1', dealer=NORTH, vulnerable=NONE):
        # 四方RemoteBridgePlayer
        self.north = north
        self.south = south
        self.east= east
        self.west = west
        self.playerList:list[RemoteBridgePlayer] = [self.north, self.east, self.south, self.west]
        self.roundId = roundNum  # 局号
        self.dealer = dealer  # 发牌方
        self.vulnerable = vulnerable  # 局况
        self.seed = seed  # 发牌种子
        self.cmd = ''  # 本对局的执行命令
        self.info = "%s_%s_vs_%s_%s" % (self.roundId, self.north.name, self.east.name, self.seed)  # 用来存点注释

    def __str__(self):
        self.info = "%s_%s_vs_%s_%s" % (self.roundId, self.north.name, self.east.name, self.seed)  # 用来存点注释
        return self.info

    @property
    def execute(self):
        """
        准备对局，复制并重命名每个文件
        :return: 执行平台所需的参数
        """
        self.clear()

        self.cmd = "4,0,1,2,3,%s,%s,4,1200,1,4,20,%s,%s" % (self.dealer, self.roundId, self.vulnerable, self.seed)
        pbnName = ''
        score = ''

        
        # 创建本地代理AI套接字
        server_socket_agent = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        host = 'localhost'
        port = 37000
        server_socket_agent.bind((host, port))
        server_socket_agent.listen(1)
        print(f"服务器已启动，正在监听端口 {port}...")
        print(f"正在拉起牌桌...")
        response = os.popen("BridgeInterface.exe %s" % self.cmd, 'r')  # 从管道获取平台输出
        #等待Agent连接

        #此处启动牌桌
        agent_socket_list = [None, None, None, None]
        client_address_list = [None, None, None, None]
        for i in range(4):
            agent_socket, client_address = server_socket_agent.accept()
            data = agent_socket.recv(1024).decode('utf-8')
            agent_socket.send("OK PATH".encode('utf-8'))
            if "North.exe" in data:
                agent_socket_list[0], client_address_list[0] = agent_socket, client_address
                print(f"接收到北家来自 {client_address} 的连接")
                AgentNorththread = threading.Thread(target=self.handle_agent_client, args=(agent_socket_list[0], 0))
                AgentNorththread.start()
            elif "East.exe" in data:
                agent_socket_list[1], client_address_list[1] = agent_socket, client_address
                print(f"接收到东家来自 {client_address} 的连接")
                AgentEastthread = threading.Thread(target=self.handle_agent_client, args=(agent_socket_list[1], 1))
                AgentEastthread.start()
            elif "South.exe" in data:
                agent_socket_list[2], client_address_list[2] = agent_socket, client_address
                print(f"接收到南家来自 {client_address} 的连接")
                AgentSouththread = threading.Thread(target=self.handle_agent_client, args=(agent_socket_list[2], 2))
                AgentSouththread.start()
            elif "West.exe" in data:
                agent_socket_list[3], client_address_list[3] = agent_socket, client_address
                print(f"接收到西家来自 {client_address} 的连接")
                AgentWestthread = threading.Thread(target=self.handle_agent_client, args=(agent_socket_list[3], 3))
                AgentWestthread.start()
            else:
                print(f"拉起牌桌失败")
                return "0", 0, "0", 0

        
        for line in response.readlines():  # 获取输出信息
            print(line)
            if 'Error' in line:
                score = line.strip('\n')
            if 'pbn' in line:
                pbnName = line.strip('\n')
            if line[0] == 'N' or line[0] == 'E':
                score = line.strip('\n')
        
        # TODO：如何处理对局异常?
        
        # 结束对局
        #for i in range(4):
            #agent_socket_list[i].send("SHUTDOWN".encode('utf-8'))()
            
        # 修改pbn
        
        with open(pbnName, 'r') as f1, open(self.info + '.pbn', 'w') as f2:
            for line in f1:
                if '%NorthName%' in line:
                    line = line.replace('%NorthName%', self.north.name)
                elif '%SouthName%' in line:
                    line = line.replace('%SouthName%', self.south.name)
                elif '%EastName%' in line:
                    line = line.replace('%EastName%', self.east.name)
                elif '%WestName%' in line:
                    line = line.replace('%WestName%', self.west.name)                                                                                                                  
                f2.write(line)
        os.remove(pbnName)
  
        # 计分
        if 'NS' in score:
            name1 = self.north.name
            score1 = int(score[2:])
            name2 = self.east.name
            score2 = -int(score[2:])

        elif 'EW' in score:
            name1 = self.east.name
            score1 = int(score[2:])
            name2 = self.north.name
            score2 = -int(score[2:])
        else:
            name1 = self.north.name
            score1 = score
            name2 = self.east.name
            score2 = score
        
        server_socket_agent.close()
        return name1, score1, name2, score2
        
        #input("Enter continue")
        

    
        # 静态方法，本地Agent处理线程函数
    
    def handle_agent_client(self,agent_socket, position):
        totalMsgCount = 0
        MazMsgCount = 100
        while True:
            totalMsgCount += 1
            if(MazMsgCount<totalMsgCount):
                break
            try:
                data = agent_socket.recv(1024).decode('utf-8')
            except ConnectionResetError:
                break
            if not data:
                break
            print(f"接收到来自本地代理 {position} 的消息: {data},转发至：{self.playerList[position].name}")
            reply = self.playerList[position].upload(data)
            agent_socket.send(reply.encode('utf-8'))
            print(f"回复已发送给 {position}: {reply}")
        # 关闭与客户端的连接
        agent_socket.close()
        print(f"与 {position} 的连接已关闭")

    @staticmethod
    def clear():
        """
        TODO：在这里杀一下没正确结束的进程
        :return:
        """


class Score(object):
    """
    简单实现一个计分表，主要是个字典，键为程序名，值为包含每局得分的列表
    """

    def __init__(self, nameList):
        self.score = {}
        for name in nameList:
            self.score[name] = []

    def record(self, name, value, name1, value1):
        for key in self.score:
            if name[0:3] == key:
                self.score[key].append(value)
            elif name1[0:3] == key:
                self.score[key].append(value1)
            else:
                self.score[key].append(0)


class MatchData(object):
    """
    循环赛数据选项，包括生成比赛，保存，终端管理
    """

    def __init__(self, teams:list[RemoteBridgeTeam], randseeds:list):
        """
        :param players: list with each name in str
        :param randseeds: list with each deal seed in str
        """
        self.total = 0  # 总计的比赛数
        self.current = 0  # 目前正在进行的比赛
        self.teamList = teams  # 玩家列表(程序名)
        self.seedList = randseeds
        self.matchList = []  # 所有比赛数
        self.team_name_list = list(map(lambda obj: obj.name, teams))
        self.score = Score(self.team_name_list)

    def create_round_robin(self):
        """
        创建一场循环赛，每个对局信息保存于self.matchList中
        :return:
        """
        self.total = 0
        self.current = 0
        self.matchList = []  # 开始前先清理一下变量
        for idx, team_A in enumerate(self.teamList):
            # 干脆直接嵌套个大循环把匹配做了
            if len(self.teamList) == 1:
                warnings.warn("Only one team exist",UserWarning)
            else:
                restTeamList:list[RemoteBridgeTeam] = self.teamList[idx + 1:]

            for team_B in restTeamList:
                # 进入这里之后就是player1和2了
                north = team_A.player_1
                south = team_A.player_2
                east = team_B.player_1
                west = team_B.player_2
                for seed in self.seedList:
                    match = BridgeTable(north, south, east, west, seed=seed)
                    match.roundId = str(len(self.matchList) + 1)
                    self.matchList.append(match)

                # 交换方向（为了清晰一些，交换前后就分开写了）
                north = team_B.player_1
                south = team_B.player_2
                east = team_A.player_1
                west = team_A.player_2

                for seed in self.seedList:
                    match = BridgeTable(north, south, east, west)
                    match.seed = seed
                    match.roundId = str(len(self.matchList) + 1)
                    self.matchList.append(match)

        self.total = len(self.matchList)

    def start(self, idx=0):
        """
        执行比赛, 默认从第一局开始，如果之前有异常退出，则从断点开始
        联网版中，对局中断功能可能失效了
        :return:
        """
        if idx != 0:
            warnings.warn("对局中断功能当前不可用",UserWarning)
            #matchList = self.matchList[idx:]
        else:
            matchList:list[BridgeTable] = self.matchList[self.current:]
            
        for eachMatch in matchList:
            print('%s=========================================' % eachMatch.roundId)
            print('     %s     ' % eachMatch.north.name)
            print()
            print('%s             %s' % (eachMatch.west.name, eachMatch.east.name))
            print()
            print('     %s     ' % eachMatch.south.name)
            print('===========================================')
            for keys in self.score.score:
                print('%s:%s' % (keys, self.score.score[keys]))

            name1, value1, name2, value2 = eachMatch.execute
            self.score.record(name1, value1, name2, value2)
            self.current += 1
            # 每完成一局，备份得分
            scoreBackup = open('score.tmp', 'wb')
            pickle.dump(self.score, scoreBackup)
            scoreBackup.close()
            eachMatch.clear()


if __name__ == '__main__':
    #启动服务器等待队伍列表加载
    
    host = 'localhost'
    port = 37001
    server = BridgeGameServer(host,port)
    timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
    input(f"{timestamp} 等待玩家连接，回车键开始对局\n")


    team_list = server.player_ready() #team_list包含了所有参赛队伍玩家以及相应连接信息的RemoteBridgeTeam列表
    
    game = MatchData(team_list, config.seeds)

    print('选手程序列表：')
    for each in team_list:
        print(each)
    print('牌号：')
    for each in config.seeds:
        print(each)

    game.create_round_robin()

    print('生成以下对局：')
    for each in game.matchList:

        print(str(each))

    files = os.listdir()
    if 'score.tmp' in files:
        print('上轮对局异常结束，正在尝试恢复')
        warnings.warn("对局中断功能当前不可用",UserWarning)
        '''
        with open('score.tmp', 'rb') as f:
            game.score = pickle.load(f)
        for keys in game.score.score:
            print('%s:%s' % (keys, game.score.score[keys]))
        game.current = len(game.score.score[player[0]])
        '''


    if config.forceStartPosition != -1:
        game.start(config.forceStartPosition)
    else:
        game.start()

    # 写入分数
    # 暂时每副牌运行一次脚本，所以文件名取config.seeds[0]
    xlFile = pd.ExcelWriter('./%s.xlsx' % config.seeds[0])
    scoreSheet = pd.DataFrame(game.score.score)
    scoreSheet.to_excel(xlFile)
    xlFile.save()
    os.remove('score.tmp')
