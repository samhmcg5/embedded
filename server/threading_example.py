import time
from threading import Thread
from collections import deque

# for passing global data
d = deque()

#worker function
def worker(secs):
    #print("Thread sleeps for %d seconds"%(secs) )
    d.append("Thread sleeps for %d seconds"%(secs))
    time.sleep(secs)
    #print("Thread %d woke up"%(secs))
    d.append("Thread %d woke up"%(secs))

def listener():
    while True:
        try:
            print(d.popleft())
        except IndexError:
            continue
    print("done")

# generate listener thread
t_listen = Thread(target=listener)
t_listen.start()

#generate worker threads
threads = []
for i in range(4):
    t = Thread(target=worker, args=(i+1,))
    t.start()
    threads.append(t)

