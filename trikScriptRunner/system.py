import os
# Python imports are intelligent, so we can import every time we call a method


class script(object):
    def __init__(self):
        self.files = {}

    def __del__(self):
        for filename in self.files:
            self._flushAndClose(self.files[filename])

    @staticmethod
    def _flushAndClose(userfile):
        # https://docs.python.org/2.7/library/os.html#os.fsync
        userfile.flush()
        os.fsync(userfile.fileno())
        userfile.close()

    def random(self, a, b):
        # https://docs.python.org/2.7/library/random.html#random.randint
        import random
        return random.randint(a, b)

    def readAll(self, filename):
        if filename in self.files:
            userfile = self.files[filename]
            userfile.seek(0)
            return userfile.read()
        else:
            with open(filename) as userfile:
                return userfile.read()

    def removeFile(self, filename):
        if filename in self.files:
            self._flushAndClose(self.files.pop(filename))
        os.remove(filename)

    def system(self, cmnd):
        # https://docs.python.org/2.7/library/subprocess.html#popen-constructor
        import shlex
        import subprocess
        subprocess.Popen(shlex.split(cmnd))

    def time(self):
        # https://docs.python.org/2.7/library/time.html#time.time
        import time
        return time.time() * 1000

    def wait(self, ms):
        # https://docs.python.org/2.7/library/time.html#time.sleep
        import time
        time.sleep(ms / 1000.0)

    def writeToFile(self, filename, text):
        if filename not in self.files:
            self.files[filename] = open(filename, 'w+')
        self.files[filename].write(text)

script = script()  # singleton
