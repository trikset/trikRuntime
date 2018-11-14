import os
# Python imports are intelligent, so we can import every time we call a method


class script(object):
    def __init__(self):
        from PythonQt import QtCore
        self.files = {}
        self.waitTimer = QtCore.QTimer()
        self.waitTimer.setSingleShot(True)
        # quitTimer is used to quit AFTER all events n the loop were processed
        # but it is fine even without it
        self.quitTimer = QtCore.QTimer()
        self.quitTimer.setSingleShot(True)
        self.quitTimer.setInterval(0)
        self.waitLoop = QtCore.QEventLoop()
        self.quitTimer.connect("timeout()", self.waitLoop.quit)
        self.waitTimer.connect("timeout()", self.quitTimer.start)
        self.interruptionFlag = False

    def __del__(self):
        for filename in self.files:
            self._flushAndClose(self.files[filename])

    @staticmethod
    def _flushAndClose(userfile):
        # https://docs.python.org/2.7/library/os.html#os.fsync
        userfile.flush()
        os.fsync(userfile.fileno())
        userfile.close()

    @staticmethod
    def random(a, b):
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

    @staticmethod
    def system(cmnd):
        import shlex
        import subprocess
        subprocess.Popen(shlex.split(cmnd))

    @staticmethod
    def time():
        import time
        return time.time() * 1000

    def wait(self, ms):
          #waitTimer object is per instance and we have script's instance per thread
          #same about waitLoop
          self.waitTimer.setInterval(ms)
          self.waitTimer.start()
          self.waitLoop.exec()
          # This event loops is needed for processing events from other threads (i.e., script stopping)
          # Exception should be thrown after loop to be processed in main thread.
          if self.interruptionFlag:
              raise ValueError()

    def writeToFile(self, filename, text):
        if filename not in self.files:
            self.files[filename] = open(filename, 'w+')
        self.files[filename].write(text)

    def kill(self):
        self.interruptionFlag = True

script = script()  # singleton
