import tkinter as tk
import tkinter.filedialog as filedialog
import socket
import subprocess
import threading
import os
from datetime import datetime
import tkinter.messagebox as messagebox

def select_program_path():
    path = filedialog.askopenfilename(filetypes=[("Executable Files", "*.exe")])
    if path:
        path_entry.delete(0, tk.END)
        path_entry.insert(tk.END, path)


def start_program():
    global sock, process
    global path_entry, ip_entry, port_entry, output_text
    
    # 获取输入框的内容
    path = path_entry.get()
    ip = ip_entry.get()
    port = int(port_entry.get())
    name = name_entry.get()

    # 禁用输入框
    path_entry.configure(state='disabled')
    ip_entry.configure(state='disabled')
    port_entry.configure(state='disabled')
    name_entry.configure(state='disabled')

    try:
        # 连接服务器
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((ip, port))
    except ConnectionError as e:
        messagebox.showerror("连接错误", str(e))
        # 恢复输入框状态 
        path_entry.configure(state='normal')
        ip_entry.configure(state='normal')
        port_entry.configure(state='normal')
        name_entry.configure(state='disabled')
        return
    
    # 启动子进程并定向标准输入输出
    try:
        process.kill()
    except NameError:
        pass
    process = subprocess.Popen(path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    
    def msg_handler():
        while True:
            # 接收服务器发来的数据
            data = sock.recv(1024).decode('utf-8')

            # 在文本框中显示记录
            timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
            output_text.insert(tk.END, timestamp + data + '\n')
            output_text.see(tk.END)
            #服务器消息处理
            if not data:
                break #与服务器连接断开
            if data == 'NAME WHAT':
                reply = name
                sock.send(reply.encode('utf-8'))
                # 在文本框中显示记录
                timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
                output_text.insert(tk.END, timestamp + reply + '\n')
                output_text.see(tk.END)
            else:
                # 将数据发送给子进程
                process.stdin.write(data.encode('utf-8'))
                process.stdin.write(b"\n")
                process.stdin.flush()
                # 接收子进程的输出
                reply = process.stdout.readline().decode('utf-8')
                # 发送输出给服务器
                sock.send(reply.encode('utf-8'))
                # 在文本框中显示记录
                timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
                output_text.insert(tk.END, timestamp + reply)
                output_text.see(tk.END)
                

    
    # 启动消息处理线程
    receive_thread = threading.Thread(target=msg_handler)
    receive_thread.start()

def quit_program():

    # 结束子进程
    process.terminate()
    # 关闭套接字
    sock.close()
    # 退出程序
    root.destroy()


# 创建主窗口
root = tk.Tk()
root.title("程序通讯")

# 创建框架
frame = tk.Frame(root)
frame.pack(padx=10, pady=10)

# 路径输入框
path_label = tk.Label(frame, text="程序路径：")
path_label.grid(row=0, column=0, sticky="w")

path_entry = tk.Entry(frame, width=40)
path_entry.grid(row=0, column=1, padx=5, pady=5)

# 选择文件按钮
select_button = tk.Button(frame, text="选择文件", command=select_program_path)
select_button.grid(row=0, column=2, padx=5)

# 程序名称输入框
name_label = tk.Label(frame, text="程序名称：")
name_label.grid(row=1, column=0, sticky="w")

name_entry = tk.Entry(frame, width=40)
name_entry.insert(tk.END, "CGA_01")  # 设置默认值
name_entry.grid(row=1, column=1, padx=5, pady=5)

# IP地址输入框
ip_label = tk.Label(frame, text="IP地址：")
ip_label.grid(row=2, column=0, sticky="w")

ip_entry = tk.Entry(frame, width=40)
ip_entry.insert(tk.END, "127.0.0.1")  # 设置默认值
ip_entry.grid(row=2, column=1, padx=5, pady=5)

# 端口输入框
port_label = tk.Label(frame, text="端口：")
port_label.grid(row=3, column=0, sticky="w")

port_entry = tk.Entry(frame, width=40)
port_entry.insert(tk.END, "37001")  # 设置默认值
port_entry.grid(row=3, column=1, padx=5, pady=5)

# 确定按钮
start_button = tk.Button(frame, text="连接", command=start_program)
start_button.grid(row=4, column=0, columnspan=2, pady=10)

# 输出文本框
output_text = tk.Text(root, height=10, width=50)
output_text.pack(padx=10, pady=10)

# 退出按钮
quit_button = tk.Button(root, text="退出", command=quit_program)
quit_button.pack(pady=10)

# 运行主循环
root.mainloop()