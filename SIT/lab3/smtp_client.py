import base64
import socket
import ssl
import traceback

from logger import FileLogger

log_filename = "smtp_3.log"
host = 'mail2.nstu.ru'
# host = 'smtp.gmx.com'
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

    def __init__(self, server_host, server_port, use_tls=False):
        self.client_sock = socket.socket()
        self.client_sock.settimeout(10)

        self.client_sock.connect((server_host, server_port))
        self.__logfile.write_log(f"Connected to {server_host}:{server_port}")
        server_response = self.client_sock.recv(1024).decode('utf-8')
        server_log = f"Server: {server_response}"
        self.__logfile.write_log(server_log)

        self.use_tls = use_tls
        # self.use_tls = use_tls
        # if self.use_tls:
        #     ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS)
        #     self.ssl_client_sock = ssl_context.wrap_socket(sock=self.client_sock, server_hostname=host)
        #     self.__logfile.write_log("SSL socket created")

    def __send_cmd(self, command):
        client_log = f"Client: {command}"
        # print(client_log)
        self.__logfile.write_log(client_log)
        self.client_sock.send((command + "\r\n").encode())

        server_response = self.client_sock.recv(1024).decode('utf-8')
        status_code = server_response[0:3]
        server_log = f"Server: {server_response}"
        # print(server_log)
        self.__logfile.write_log(server_log)
        if status_code[0] not in ('2', '3'):
            raise SMTPClientException(
                f"Error while sending command {command}. Status code: {status_code} Response from server: {server_response}")
        return server_response

    def __send_cmd_ssl(self, command, b64encode_cmd=False, no_response=False):
        if b64encode_cmd is True:
            command = base64.b64encode(command.encode()) + b'\r\n'
        else:
            command = (command + "\r\n").encode()
        client_log = f"Client: {command}"
        self.__logfile.write_log(client_log)
        self.ssl_client_sock.send(command)
        if no_response:
            return 0
        else:
            server_response = self.ssl_client_sock.recv(1024).decode('utf-8')
            # server_response = self.ssl_client_sock.recv(1024)
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
            if self.use_tls is True:
                self.__send_cmd("STARTTLS")
                ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS)
                self.ssl_client_sock = ssl_context.wrap_socket(sock=self.client_sock, server_hostname=host)
                self.__send_cmd_ssl("EHLO localhost")
                self.__send_cmd_ssl("AUTH LOGIN")
                self.__send_cmd_ssl(login, b64encode_cmd=True)
                print("Input your password:")
                password = input()
                self.__logfile.change_active_state(False)
                self.__send_cmd_ssl(password, b64encode_cmd=True)
                self.__logfile.change_active_state(True)
                self.__send_cmd_ssl(f"MAIL FROM:{from_addr}")
                self.__send_cmd_ssl(f"RCPT TO:{to_addr}")
                self.__send_cmd_ssl("DATA")
                self.__send_cmd_ssl(f"FROM:{from_addr}\r\n" +
                                    f"TO:{to_addr}\r\n" +
                                    f"SUBJECT:{subject}", no_response=True)
                self.__send_cmd_ssl(f"\n{message}", no_response=True)
                self.__send_cmd_ssl(".")
                self.__send_cmd_ssl("QUIT")

            # self.__send_cmd(f"MAIL FROM:{from_addr}")
            # self.__send_cmd(f"RCPT TO:{to_addr}")
            # self.__send_cmd("DATA")
        except SMTPClientException as e:
            self.__logfile.write_log(f"SMTPClientException: {e}", msg_type="ERROR")
        except TimeoutError:
            self.__logfile.write_log("SMTP command timeout", msg_type="ERROR")
            # print()
        except Exception as e:
            # print("Unexpected exception:", e)
            self.__logfile.write_log(f"Unexpected exception: {e}", msg_type="ERROR")
            print(traceback.format_exc())
            self.close()
            exit()
        return 'OK'

    def close(self):
        print("Connection closed")
        self.__logfile.write_log("Connection closed\n___________________\n\n\n")
        self.client_sock.close()
        self.ssl_client_sock.close()
        self.__logfile.close()


client = SMTPClient(host, port, True)
client.send_letter(from_addr="suxix.2018@stud.nstu.ru", to_addr="sukharik0720@gmail.com", subject="SMTP test message",
                   message="Some text")
# client.send_letter("b4@cn.ami.nstu.ru", "b10@cn.ami.nstu.ru", "lol", True)
client.close()