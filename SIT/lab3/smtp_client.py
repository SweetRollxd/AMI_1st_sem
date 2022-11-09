import base64
import socket
import ssl
import traceback
import getpass

from logger import FileLogger

log_filename = "smtp_3.log"
host = 'mail2.nstu.ru'
# host = 'smtp.freesmtpservers.com'
port = 587
# login = 'suxix.2018@stud.nstu.ru'
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
    """
    Собственное исключение, чтобы вызывать его при ответах сервера об ошибке
    """
    pass


class SMTPClient:
    """
    Класс SMTP клиента.
    Атрибуты класса:
    __logfile - объект класса FileLogger для логирования сообщений между клиентом и сервером
    """
    __logfile = FileLogger(log_filename)

    def __init__(self, server_host, server_port, login, password):
        self.login = login
        self.password = password
        self.client_sock = socket.socket()
        self.client_sock.settimeout(10)

        self.client_sock.connect((server_host, server_port))
        self.__logfile.write_log(f"Successfully connected to {server_host}:{server_port}")
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

        client_log = f"Client: {command}"
        self.__logfile.write_log(client_log)
        self.client_sock.send((command + "\r\n").encode())
        if no_response:
            return
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

    def send_letter(self, sender, recipients, subj, msg):

        try:
            self.__send_cmd("EHLO localhost")
            if self.use_tls is True:
                self.__send_cmd("STARTTLS")
                self.__create_ssl_socket()
                self.__send_cmd("EHLO localhost", secure=False)

                self.__send_cmd("AUTH LOGIN", secure=True)
                self.__send_cmd(base64.b64encode(self.login.encode()).decode(), secure=True)

                # TODO: отключить логирование только на момент передачи сообщение клиента о пароле. Ответ от сервера должен логироваться
                self.__logfile.change_active_state(False)
                self.__send_cmd(base64.b64encode(self.password.encode()).decode(), secure=True)
                self.__logfile.change_active_state(True)

            self.__send_cmd(f"MAIL FROM:{sender}", secure=self.use_tls)

            for recipient in recipients:
                self.__send_cmd(f"RCPT TO:{recipient}", secure=self.use_tls)

            self.__send_cmd("DATA", secure=self.use_tls)
            self.__send_cmd(f"FROM:{sender}\r\n" +
                            f"TO:{', '.join(recipients)}\r\n" +
                            f"SUBJECT:{subj}", secure=self.use_tls, no_response=True)
            self.__send_cmd(f"\n{msg}", secure=self.use_tls, no_response=True)
            self.__send_cmd(".", secure=self.use_tls)
            self.__send_cmd("QUIT", secure=self.use_tls)
            self.__logfile.write_log("Letter was sent successfully!")

            return 0
        except SMTPClientException as e:
            # TODO: обработка различных ответов от сервера об ошибке, чтобы говорить о них пользователю
            self.__logfile.write_log(f"SMTPClientException: {e}", msg_type="ERROR")
            return 1
        except TimeoutError:
            self.__logfile.write_log("SMTP command timeout", msg_type="ERROR")
            return 1
        except Exception as e:
            self.__logfile.write_log(f"Unexpected exception: {e}", msg_type="ERROR")
            print(traceback.format_exc())
            self.close()
            raise
            # exit()

    def close(self):
        print("Connection closed")
        self.__logfile.write_log("Connection closed\n___________________\n\n\n")
        self.client_sock.close()
        self.__logfile.close()


if __name__ == "__main__":
    print(f"Welcome to SMTP Client!\nYou are going to connect to this SMTP server: {host}:{port}")
    login = input("Enter login from the server: ")
    password = getpass.getpass()
    try:
        client = SMTPClient(host, port, login, password)
    except Exception as e:
        print("Unexpected exception caught:", e)
        print("Terminating...")
        exit(code=1)

    while True:
        print("You're writing a new letter.\nUse Ctrl-D or Ctrl-Z (Windows) to close SMTP client.")
        try:
            from_address = input("From: ")

            to_addresses = input("To (separated by commas): ")
            to_address_list = to_addresses.replace(" ", "").split(',')

            subject = input("Subject: ")

            print("Enter your letter body. To save the message enter 'EOF' line.")
            message = ""
            while True:
                line = input()
                if line == "EOF":
                    break

                # elif line != '' and line[0] == '.':
                #     message += "." + line + "\n"
                # else:
                message += line + "\n"

            status_code = client.send_letter(sender=from_address,
                                             recipients=to_address_list,
                                             subj=subject,
                                             msg=message)
            if status_code == 0:
                print("Hell yeah! Letter was sent.")
            else:
                print("This is very sad. Letter wasn't sent.")
        except EOFError:
            print("\nGoodbye!")
            break
        except Exception as e:
            print("Unexpected exception caught:", e)
            print("Terminating...")
            exit(code=1)
