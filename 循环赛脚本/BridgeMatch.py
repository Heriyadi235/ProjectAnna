"""
匹配系统
扫描目录下exe文件，获得名称，参赛人数
生成每个对局

"""
import os
import pickle
import config
import pandas as pd

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

    def __init__(self, north, south, east, west, seed=000000, roundNum='1', dealer=NORTH, vulnerable=NONE):
        # 四方AI程序名(xxx.exe)
        self.northAIName = north
        self.southAIName = south
        self.eastAIName = east
        self.westAIName = west
        self.roundId = roundNum  # 局号
        self.dealer = dealer  # 发牌方
        self.vulnerable = vulnerable  # 局况
        self.seed = seed  # 发牌种子
        self.cmd = ''  # 本对局的执行命令
        self.info = "%s_%s_vs_%s_%s" % (self.roundId, self.northAIName, self.eastAIName, self.seed)  # 用来存点注释

    def __str__(self):
        self.info = "%s_%s_vs_%s_%s" % (self.roundId, self.northAIName, self.eastAIName, self.seed)  # 用来存点注释
        return self.info

    @property
    def execute(self):
        """
        准备对局，复制并重命名每个文件
        :return: 执行平台所需的参数
        """
        self.clear()
        os.system("powershell copy %s north.exe" % self.northAIName)
        os.system("powershell copy %s south.exe" % self.southAIName)
        os.system("powershell copy %s east.exe" % self.eastAIName)
        os.system("powershell copy %s west.exe" % self.westAIName)

        self.cmd = "4,0,1,2,3,%s,%s,4,1200,1,4,20,%s,%s" % (self.dealer, self.roundId, self.vulnerable, self.seed)
        pbnName = ''
        score = ''
        response = os.popen("BridgeInterface.exe %s" % self.cmd, 'r')  # 从管道获取平台输出
        for line in response.readlines():  # 获取输出信息
            print(line)
            if 'Error' in line:
                score = line.strip('\n')
            if 'pbn' in line:
                pbnName = line.strip('\n')
            if line[0] == 'N' or line[0] == 'E':
                score = line.strip('\n')
        # TODO：如何处理对局异常?
        # 修改pbn
        with open(pbnName, 'r') as f1, open(self.info + '.pbn', 'w') as f2:
            for line in f1:
                if '%NorthName%' in line:
                    line = line.replace('%NorthName%', self.northAIName)
                elif '%SouthName%' in line:
                    line = line.replace('%SouthName%', self.southAIName)
                elif '%EastName%' in line:
                    line = line.replace('%EastName%', self.eastAIName)
                elif '%WestName%' in line:
                    line = line.replace('%WestName%', self.westAIName)
                f2.write(line)
        os.remove(pbnName)

        # 计分
        if 'NS' in score:
            name1 = self.northAIName
            score1 = int(score[2:])
            name2 = self.eastAIName
            score2 = -int(score[2:])

        elif 'EW' in score:
            name1 = self.eastAIName
            score1 = int(score[2:])
            name2 = self.northAIName
            score2 = -int(score[2:])
        else:
            name1 = self.northAIName
            score1 = score
            name2 = self.eastAIName
            score2 = score

        return name1, score1, name2, score2

    @staticmethod
    def clear():
        """
        清空脚本产生的文件，包括删除目录下以north south east west 命名的程序文件
        :return:
        """
        try:
            os.remove('east.exe')
        except FileNotFoundError:
            pass
        else:
            pass
        try:
            os.remove('north.exe')
        except FileNotFoundError:
            pass
        else:
            pass
        try:
            os.remove('south.exe')
        except FileNotFoundError:
            pass
        try:
            os.remove('west.exe')
        except FileNotFoundError:
            pass
        else:
            pass
        return


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
            if name == key:
                self.score[key].append(value)
            elif name1 == key:
                self.score[key].append(value1)
            else:
                self.score[key].append(0)


class MatchData(object):
    """
    循环赛数据选项，包括生成比赛，保存，终端管理
    """

    def __init__(self, players, randseeds):
        """
        :param players: list with each name in str
        :param randseeds: list with each deal seed in str
        """
        self.total = 0  # 总计的比赛数
        self.current = 0  # 目前正在进行的比赛
        self.playerList = players  # 玩家列表(程序名)
        self.seedList = randseeds
        self.matchList = []  # 所有比赛数
        self.score = Score(players)

    def create_round_robin(self):
        """
        创建一场循环赛，每个对局信息保存于self.matchList中
        :return:
        """
        self.total = 0
        self.current = 0
        self.matchList = []  # 开始前先清理一下变量
        for idx, Player1 in enumerate(self.playerList):
            # 干脆直接嵌套个大循环把匹配做了
            if len(self.playerList) == 1:
                restPlayerList = self.playerList
            else:
                restPlayerList = self.playerList[idx + 1:]

            for Player2 in restPlayerList:
                # 进入这里之后就是player1和2了
                north = Player1
                south = Player1
                east = Player2
                west = Player2
                for seed in self.seedList:
                    match = BridgeTable(north, south, east, west, seed=seed)
                    match.roundId = str(len(self.matchList) + 1)
                    self.matchList.append(match)

                # 交换方向（为了清晰一些，交换前后就分开写了）
                north = Player2
                south = Player2
                east = Player1
                west = Player1
                for seed in self.seedList:
                    match = BridgeTable(north, south, east, west)
                    match.seed = seed
                    match.roundId = str(len(self.matchList) + 1)
                    self.matchList.append(match)

        self.total = len(self.matchList)

    def start(self, idx=0):
        """
        执行比赛, 默认从第一局开始，如果之前有异常退出，则从断点开始
        :return:
        """
        if idx != 0:
            matchList = self.matchList[idx:]
        else:
            matchList = self.matchList[self.current:]

        for eachMatch in matchList:

            print('%s=========================================' % eachMatch.roundId)
            print('     %s     ' % eachMatch.northAIName)
            print()
            print('%s          %s' % (eachMatch.westAIName, eachMatch.eastAIName))
            print()
            print('     %s     ' % eachMatch.southAIName)
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
    # 今年先不做UI了
    # 测试代码
    player = []
    files = os.listdir()
    try:
        player = config.player
    except NameError:
        for each in files:
            if ('.exe' in each) and each != 'BridgeInterface.exe':
                player.append(each)
    game = MatchData(player, config.seeds)

    print('选手程序列表：')
    for each in player:
        print(each)
    print('牌号：')
    for each in config.seeds:
        print(each)

    game.create_round_robin()

    print('生成以下对局：')
    for each in game.matchList:

        print(str(each))

    if 'score.tmp' in files:
        print('上轮对局异常结束，正在尝试恢复')
        with open('score.tmp', 'rb') as f:
            game.score = pickle.load(f)
        for keys in game.score.score:
            print('%s:%s' % (keys, game.score.score[keys]))
        game.current = len(game.score.score[player[0]])


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
