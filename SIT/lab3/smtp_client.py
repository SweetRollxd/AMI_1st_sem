import base64
import socket
import ssl
import traceback

from logger import FileLogger

log_filename = "smtp_3.log"
host = 'mail2.nstu.ru'
# host = 'smtp.freesmtpservers.com'
port = 587
login = 'suxix.2018@corp.nstu.ru'
# password = ''

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
        self.client_sock.settimeout(10)

        self.client_sock.connect((server_host, server_port))
        self.__logfile.write_log(f"Connected to {server_host}:{server_port}")
        server_response = self.client_sock.recv(1024).decode('utf-8')
        server_log = f"Server: {server_response}"
        self.__logfile.write_log(server_log)

        self.use_tls = True if server_port == 587 else False

    def __create_ssl_socket(self):
        ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
        tmp_sock = self.client_sock
        self.client_sock = ssl_context.wrap_socket(sock=self.client_sock, server_hostname=host)
        tmp_sock.close()

    def __send_cmd(self, command, secure=False, no_response=False):
        # sock = self.client_sock if not secure else self.ssl_client_sock

        client_log = f"Client: {command}"
        self.__logfile.write_log(client_log)
        self.client_sock.send((command + "\r\n").encode())
        if no_response:
            return 0
        else:
            server_response = self.client_sock.recv(1024).decode('utf-8')
            status_code = server_response[0:3]

            # if b64decode_response is True:
            #     server_response = base64.b64decode(server_response[4:] + b'==').decode('utf-8')
            # else:
            #     server_response = server_response.decode('utf-8')

            server_log = f"Server: {server_response}"
            # print(server_log)
            self.__logfile.write_log(server_log)
            if status_code[0] not in ('2', '3'):
                raise SMTPClientException(
                    f"Error while sending command {command}.\nStatus code: {status_code}.\nResponse from server: {server_response}")
            return server_response

    def send_letter(self, from_addr, to_addr, subject, message):

        try:
            self.__send_cmd("EHLO localhost")
            if self.use_tls is True:
                self.__send_cmd("STARTTLS")
                self.__create_ssl_socket()
                self.__send_cmd("EHLO localhost", secure=False)
                self.__send_cmd("AUTH LOGIN", secure=True)
                self.__send_cmd(base64.b64encode(login.encode()).decode(), secure=True)
                password = input("Input your password:")
                self.__logfile.change_active_state(False)
                self.__send_cmd(base64.b64encode(password.encode()).decode(), secure=True)
                self.__logfile.change_active_state(True)

            self.__send_cmd(f"MAIL FROM:{from_addr}", secure=self.use_tls)
            self.__send_cmd(f"RCPT TO:{to_addr}", secure=self.use_tls)
            self.__send_cmd("DATA", secure=self.use_tls)
            self.__send_cmd(f"FROM:{from_addr}\r\n" +
                                f"TO:{to_addr}\r\n" +
                                f"SUBJECT:{subject}", secure=self.use_tls, no_response=True)
            self.__send_cmd(f"\n{message}", secure=self.use_tls, no_response=True)
            self.__send_cmd(".", secure=self.use_tls)
            self.__send_cmd("QUIT", secure=self.use_tls)

        except SMTPClientException as e:
            self.__logfile.write_log(f"SMTPClientException: {e}", msg_type="ERROR")
        except TimeoutError:
            self.__logfile.write_log("SMTP command timeout", msg_type="ERROR")
        except Exception as e:
            self.__logfile.write_log(f"Unexpected exception: {e}", msg_type="ERROR")
            print(traceback.format_exc())
            self.close()
            exit()
        return 'OK'

    def close(self):
        print("Connection closed")
        self.__logfile.write_log("Connection closed\n___________________\n\n\n")
        self.client_sock.close()
        self.__logfile.close()


client = SMTPClient(host, port)
print("Welcome to SMTP Client!")
from_address = input("From: ")
to_address = input("To: ")
subject = input("Subject: ")
print("Enter your letter body. To save the message type 'EOF' line.")
message = ""
while True:
    line = input()
    if line == "EOF":
        break

    message += line + "\n"

client.send_letter(from_addr=from_address,
                   to_addr=to_address,
                   subject=subject,
                   message=message)

# client.send_letter("b4@cn.ami.nstu.ru", "b10@cn.ami.nstu.ru", "lol", True)
