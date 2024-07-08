import warnings

class RemoteBridgePlayer(object):
    def __init__(self, client_socket = None, client_address = None):
        self.socket = client_socket
        self.address = client_address
        self.name = self.upload('NAME WHAT')

    def  __str__(self):
        return f"Bridge AI: {self.name} on {self.address}"
    
    def upload(self, command: str):
        reply = ''
        if(self.socket != None):
            self.socket.send(command.encode('utf-8'))
            reply = self.socket.recv(1024).decode('utf-8') #阻塞式等待回复
            #self.socket.send("OK NAME".encode('utf-8'))
        return reply
    
class RemoteBridgeTeam(object):
    def __init__(self,player_1:RemoteBridgePlayer,player_2:RemoteBridgePlayer,) -> None:
        self.player_1 = player_1
        self.player_2 = player_2
        if(player_1.name[0:2]!=player_2.name[0:2]):
            warnings.warn("Player name not match",UserWarning)
        self.name = player_1.name[0:3]

    def  __str__(self):
        return f"Bridge Team: {self.name}"
            