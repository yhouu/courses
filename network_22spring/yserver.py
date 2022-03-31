
import datetime
import socket
import os
from threading import Thread

IP="127.0.0.1"
PORT=10090
ADDR=(IP, PORT)
SIZE=1024
FORMAT="utf-8"
SERVER_DATA_PATH="server_data"

def main():
    server=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(ADDR)
    server.listen()

    while True:
        client,address=server.accept()
        print("Accepted")
        data=client.recv(65535)

        data=data.decode().split()
        request_method=data[0]
        page=data[1]
        print('receive >>'+data)
   
        if (page == "/" or page == "/index.html"):
            if request_method == "GET":
                filename = "/index.html"
                read_html(filename, client)
            elif request_method=="POST":
                user, pw=data[-1].split('&')
                user= user.split('=')[1]
                if not os.path.isdir("./{}".format(user)):
                    os.mkdir("./{})".format(user))
                filename="/storage.html"
                read_html(filename,client)

        elif (page=="/storage" or page=="/storage.html"):
            if "Cookie:" in data:
                if request_method=="GET":
                    filename="/storage.html"
                    read_html(filename,client)
                else:
                    client.send(str.encode("HTTP/1.1 302 redirect\n"))
                    client.send(str.encode("Location: {}\n".format("./index.html")))

        elif(page=="/cookie" or page == "/cookie.html"):
            if "Cookie:" in data:
                filename = "/cookie.html"
                read_html(filename,client)
            
            else:
                client.send("403Forbidden".encode('utf-8'))
        
        client.close()

   

def read_html(file,c):
    f=open('.'+file,'rt', encoding='utf-8')
    html=f.read()
    c.send(('HTTP/1.1 200 OK\r\n\r\n'+html).encode('utf-8'))
    f.close()
    print(html)









if __name__=="__main__":
    main()

