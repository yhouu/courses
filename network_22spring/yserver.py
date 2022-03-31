
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
        print(data)
   
        if (page == "/" or page == "/index.html"):
            if request_method == "GET":
                htmladdr = "/index.html"
                read_html(htmladdr, client)
            

        elif (page=="/storage" or page=="/storage.html"):
            if request_method=="POST":
                user, pw=data[-1].split('&')
                user= user.split('=')[1]
                if not os.path.isdir("./{}".format(user)):
                    os.mkdir("./{}".format(user))
                htmladdr="/storage.html"
                read_html(htmladdr,client)
            elif request_method=="GET":
                filename=page.split("?submitted_file")[1].split("&submit=Submit")[0]
                upload(filename,c)



            if "Cookie:" in data:
                if request_method=="GET":
                    htmladdr="/storage.html"
                    read_html(htmladdr,client)
                else:
                    client.send(str.encode("HTTP/1.1 302 redirect\n"))
                    client.send(str.encode("Location: {}\n".format("./index.html")))

        elif(page=="/cookie" or page == "/cookie.html"):
            if "Cookie:" in data:
                htmladdr = "/cookie.html"
                read_html(htmladdr,client)
            
            else:
                client.send("403Forbidden".encode('utf-8'))
        
        client.close()

   

def read_html(html,c):
    f=open('.'+html,'rt', encoding='utf-8')
    htmlcode=f.read()
    c.send(('HTTP/1.1 200 OK\r\n\r\n'+htmlcode).encode('utf-8'))
    f.close()
    print(htmlcode)

def upload_file(file,c):
    f=open(file,'w')
    #하는중..
    f.close()






if __name__=="__main__":
    main()

