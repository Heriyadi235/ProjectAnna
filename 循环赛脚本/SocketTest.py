
import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = "127.0.0.1"
port = 37000
server_socket.bind((host, port))
server_socket.listen(1)
client_socket, client_address = server_socket.accept() #这个是阻塞式的
while(1):
    data = client_socket.recv(1024).decode('utf-8')
    print(f"{data}")
    client_socket.send("OK".encode('utf-8'))
    