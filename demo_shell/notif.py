import pynotify
from time import gmtime, strftime,sleep
pynotify.init("Basic")
# python notif.py
# ps aux | grep python
# kill -9 pid
while 1:

    now = strftime("%d %b %H:%M:%S", gmtime())
    n = pynotify.Notification("Time now", now)
    n.show()
    sleep(5)
