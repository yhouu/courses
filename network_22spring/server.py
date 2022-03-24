import datetime
import os
import socket


# https://stackoverflow.com/questions/47726865/html-page-not-displaying-using-python-socket-programming
def read_html(file, c):
    f = open(file, "r")
    c.send(str.encode("HTTP/1.1 200 OK\n"))
    c.send(str.encode("Content-Type: text/html\n"))
    c.send(str.encode("\r\n"))
    for line in f.readlines():
        c.sendall(str.encode("" + line + "", "iso-8859-1"))
        line = f.read(1024)
    f.close()
    return


server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("0.0.0.0", 10090))
server.listen(5)

while True:
    client, address = server.accept()
    data = client.recv(65535)
    data_temp = data.decode().split()
    # https://kentakang.com/133
    request_method = data_temp[0]
    page = data_temp[1]
    if page == "/" or page == "/index.html":
        if request_method == "GET":
            filename = "./index.html"
            read_html(filename, client)
        elif request_method == "POST":
            form_data = data_temp[-1]
            user, pw = form_data.split("&")
            user = user.split("=")[1]
            if not os.path.isdir("./{}".format(user)):
                os.mkdir("./{}".format(user))
            # client.send(str.encode("HTTP/1.1 201 OK\n"))
            client.send(str.encode("HTTP/1.1 302 redirect\n"))
            client.send(str.encode("Location: {}\n".format("./storage.html")))
            client.send(
                str.encode(
                    "Set-Cookie: user={}; created_at={}; Max-Age=20;\n".format(
                        user, datetime.datetime.now().timestamp()
                    )
                )
            )
            client.send(
                str.encode(
                    "Set-Cookie: expire_at={}; Max-Age=20;\n".format(
                        datetime.datetime.now().timestamp() + 20
                    )
                )
            )
            client.send(str.encode("\r\n"))
    elif page == "/storage" or page == "/storage.html":
        if "Cookie:" in data_temp:
            if request_method == "GET":
                filename = "./storage.html"
                read_html(filename, client)
        else:
            client.send(str.encode("HTTP/1.1 302 redirect\n"))
            client.send(str.encode("Location: {}\n".format("./index.html")))
    elif page == "/cookie" or page == "/cookie.html":
        if "Cookie:" in data_temp:
            filename = "./cookie.html"
            read_html(filename, client)
        else:
            # https://developer.mozilla.org/ko/docs/Web/HTTP/Status/301
            client.send(str.encode("HTTP/1.1 302 redirect\n"))
            client.send(str.encode("Location: {}\n".format("./index.html")))
    client.close()
