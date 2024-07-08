import socket
import threading
from datetime import datetime
from RemoteBridgePlayer import RemoteBridgePlayer
from RemoteBridgePlayer import RemoteBridgeTeam


class BridgeGameServer(object):
    def __init__(self,host:str,port:int) -> None:
        # 构造函数，创建一个套接字然后开始监听
        self.server_socket_player = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.host = host
        self.port = port
        self.server_socket_player.bind((host, port))
        self.server_socket_player.listen(1)

        self.player_list = []
        self.team_list = []

        timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
        print(f"{timestamp} 服务器启动，等待选手连接，正在监听端口 {port}...")
        #等待远程玩家连接
        thread = threading.Thread(target=self.wait_remote_client, args=())
        thread.start()
        
    
    def player_ready(self):
        #所有选手连接完毕后执行，
        timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
        print(f"{timestamp} 正在创建队伍")
        self.player_list.sort(key=lambda x:x.name)
        for i in range(0,len(self.player_list),2):
            self.team_list.append(RemoteBridgeTeam(self.player_list[i],self.player_list[i+1]))
            timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
            print(f"{timestamp} 创建队伍：{self.team_list[-1]}")
        return self.team_list
    
    # 等待远端玩家连接线程
    def wait_remote_client(self):
        while(True):
            client_socket, client_address = self.server_socket_player.accept() #这个是阻塞式的
            player = RemoteBridgePlayer(client_socket, client_address)
            self.player_list.append(player)
            print(f"{self.player_list[-1]}...")

if __name__ == '__main__':
    host = 'localhost'
    port = 37001
    server = BridgeGameServer(host,port)
    timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
    input(f"{timestamp} 等待玩家连接，回车键开始对局\n")

