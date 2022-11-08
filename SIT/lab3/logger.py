from datetime import datetime as dt


def current_time():
    return dt.now().strftime('%Y-%m-%d %H:%M:%S')


class FileLogger:
    def __init__(self, filename):
        self.logfile = open(filename, 'a')

    def write_log(self, message):
        self.logfile.write(f'{current_time()}: {message}\n')

    def close(self):
        self.logfile.close()