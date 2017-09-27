import time
from threading import Thread

def worker(secs):
    print("Thread sleeps for %d seconds"%(secs) )
    time.sleep(secs)
    print("Thread %d woke up"%(secs))

threads = []
for i in range(5):
    t = Thread(target=worker, args=(i,))
    t.start()
    threads.append(t)

for t in threads:
    t.join()

print("completed")
