import base64
import socket
import ssl
import traceback

from logger import FileLogger

log_filename = "smtp_3.log"
# host = 'mail2.nstu.ru'
host = 'smtp.gmx.com'
port = 587
login = 'suxix.2018@corp.nstu.ru'
password = 'lol'

"""
Разработайте клиентское приложение для отправки текстовых сообщений по протоколу SMTP с учетом следующих требований:
    1. все команды и данные должен вводить пользователь (адреса получателя и отправителя, текст сообщения);  
    2. подключение к почтовому серверу реализовать на основе сокетов;
    3. приложение должно формировать строки команд в соответствии с протоколом SMTP, выводить их на экран и 
        отправлять на сервер. Ответы сервера также должны выводиться на экран. 
    4. весь процесс почтовой сессии должен сохраняться в файле журнала smtp_Х.log
"""


class SMTPClientException(Exception):
    pass


class SMTPClient:
    __logfile = FileLogger(log_filename)

    def __init__(self, server_host, server_port):
        self.client_sock = socket.socket()
        # self.client_sock.settimeout(10)

        self.client_sock.connect((server_host, server_port))
        self.__logfile.write_log(f"Connected to {server_host}:{server_port}")
        server_response = self.client_sock.recv(1024).decode('utf-8')
        server_log = f"Server: {server_response}"
        print(server_log)
        self.__logfile.write_log(server_log)

    def __send_cmd(self, command):
        client_log = f"Client: {command}"
        print(client_log)
        self.__logfile.write_log(client_log)
        self.client_sock.send((command + "\r\n").encode())

        server_response = self.client_sock.recv(1024).decode('utf-8')
        response_code = server_response[0:3]
        server_log = f"Server: {server_response}"
        print(server_log)
        self.__logfile.write_log(server_log)
        if response_code[0] != '2':
            raise SMTPClientException(f"Error while sending command {command}. Response from server: {server_response}")
        return server_response

    def __send_cmd_ssl(self, command):
        client_log = f"Client: {command}"
        print(client_log)
        self.__logfile.write_log(client_log)
        self.ssl_client_sock.send((command + "\r\n").encode())

        server_response = base64.b64decode(self.ssl_client_sock.recv(1024) + b'==').decode('utf-8')
        response_code = server_response[0:3]
        server_log = f"Server: {server_response}"
        print(server_log)
        self.__logfile.write_log(server_log)
        if response_code[0] not in ('2', '3'):
            raise SMTPClientException(f"Error while sending command {command}. Response from server: {server_response}")
        return server_response

    def send_letter(self, from_addr, to_addr, message, start_tls=False):
        """
        Отправка письма

        :param from_addr:
        :param to_addr:
        :param message:
        :param start_tls:
        :return:
        """
        try:
            self.__send_cmd("EHLO localhost")
            if start_tls is True:
                self.__send_cmd("STARTTLS")
                # self.__send_cmd("AUTH GSSAPI")
                context = ssl.SSLContext(ssl.PROTOCOL_SSLv23)
                self.ssl_client_sock = context.wrap_socket(sock=self.client_sock,  server_hostname=host)
                self.__send_cmd_ssl("EHLO localhost")
                self.__send_cmd_ssl("AUTH LOGIN")
                # self.__send_cmd("EHLO")
                self.__send_cmd_ssl(base64.b64encode(login.encode()).decode())

            # self.__send_cmd(f"MAIL FROM:{from_addr}")
            # self.__send_cmd(f"RCPT TO:{to_addr}")
            # self.__send_cmd("DATA")
        except SMTPClientException as e:
            print("SMTPClientException:", e)
        except TimeoutError:
            print("SMTP command timeout")
        except Exception as e:
            print("Unexpected exception:", e)
            print(traceback.format_exc())
            self.close()
            exit()
        return 'OK'

    def close(self):
        print("Connection closed")
        self.__logfile.write_log("Connection closed\n___________________\n\n\n")
        self.client_sock.close()
        self.__logfile.close()



# client = SMTPClient(host, port)

client = SMTPClient(host, port)
# client.send_letter("suxix.2018@stud.nstu.ru", "sukharik0720@gmail.com", "lol")
client.send_letter("b4@cn.ami.nstu.ru", "b10@cn.ami.nstu.ru", "lol", True)
client.close()
# client.


"""
print("CLIENT: MAIL FROM:b4@cn.ami.nstu.ru CRLF")
client.send('MAIL FROM:b4@cn.ami.nstu.ru \r\n'.encode())
data = client.recv(1024)
print("Server: %s" % data.decode('utf-8'))

print("CLIENT: RCPT TO:b10@cn.ami.nstu.ru CRLF")
client.send('RCPT TO:b10@cn.ami.nstu.ru \r\n'.encode())
data = client.recv(1024)
print("Server: %s" % data.decode('utf-8'))

print("CLIENT: DATA CRLF")
client.send('DATA \r\n'.encode())
data = client.recv(1024)^
print("Server: %s" % data.decode('utf-8'))
message = 'FROM: b4@cn.ami.nstu.ru\r\n' + \
    'TO: b10@cn.ami.nstu.ru\r\n' + \
    'SUBJECT: Test message\r\n'

print("CLIENT: %s CRLF" % message)
client.send(message.encode())

print("CLIENT: ")
client.send(''.encode())

print("CLIENT: This is a test message! CRLF")
client.send('This is a test message!\r\n'.encode())

print("CLIENT: . CRLF")
client.send('.\r\n'.encode())
data = client.recv(1024)
print("Server: %s" % data.decode('utf-8'))

print("CLIENT: QUIT CRLF")
client.send('QUIT\r\n'.encode())
data = client.recv(1024)
print("Server: %s" % data.decode('utf-8'))

client.close()
"""
