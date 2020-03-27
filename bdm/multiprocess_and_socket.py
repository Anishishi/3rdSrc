import RPi.GPIO as GPIO
import time
import threading

class test:
    num=0
    num+=1
    
    def p(self):
        print(self.num)
        
    def increment(self):
        self.num+=1
        self.p()
        
def turnoff():
    t = test()
    t.p()
    t.increment()
    t.p()
    
    LedGpio=26
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)#BCM-mode
    GPIO.setup(LedGpio, GPIO.OUT)#for out
    GPIO.output(LedGpio, False)
    
    GPIO.output(LedGpio, True)
    time.sleep(2)
    
    GPIO.cleanup()

class ttt():
    sum = 0

    def sl1(self):
        for i in range(10):
            sum+=1
            print(sum)
        
    def sl2(self):
        for i in range(10):
            sum+=1
            print(sum)
        
def threadtest():
    cl = ttt()
    t1 = threading.Thread(target=cl.sl1)
    t2 = threading.Thread(target=cl.sl2)
    pre = time.time()
    t1.start()
    t2.start()
    t1.join()
    t2.join()
    print(time.time()-pre)
    
if __name__=='__main__':
    threadtest()