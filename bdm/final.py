import time
import RPi.GPIO as GPIO
import os
import spidev #library for SPI
import threading

class calc_and_lighten():
    spi = spidev.SpiDev()
    spi.open(0,0)
    spi.max_speed_hz = 5000 #MAX_Hz of SPI
    ch = 0
    Vref=3.29476
    
    LedGpio=26
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)#BCM-mode
    GPIO.setup(LedGpio, GPIO.OUT)#for out
    GPIO.output(LedGpio, False)        
    
    def readAdc(self, channel):
        adc = self.spi.xfer2([1,((1<<3)+channel)<<4,0])#Din_3byte, 1:start, mode and channel, equalize the size to Dout
        data = ((adc[1]&3) << 8) + adc[2]#size of data is 10bits, 2bits(3) and 1byte
        return data
    
    def get_intensity(self):
        amount = self.readAdc(self.ch)
        return amount*self.Vref/1024 #voltage
        
    def lighten(self):
        onoff=False#led on or off
        
        pre=time.time()
        now=time.time()
        
        while now-pre<30:
            time.sleep(0.1)
            volt = self.get_intensity()
            
            if volt<0.07 and not onoff:
                GPIO.output(self.LedGpio, True)
                onoff = True
            elif volt>0.2 and onoff:
                GPIO.output(self.LedGpio, False)
                onoff = False
                
            now=time.time()

        GPIO.cleanup()
        self.spi.close()

class sub_process_capsend():
    def cap_and_send(self):#sub-process
        loss=0
        
        t0=time.time()
        t1=time.time()
        
        while t1-t0<30:
            time.sleep(max(0, 15-loss))
            pre=time.time()
            os.system("python3 capture.py")
            
            #socket-programming is needed
            
            loss=time.time()-pre
            
            t1=time.time()
        
if __name__=='__main__':
    #thread0 : capture and send by socket
    cl0=sub_process_capsend()
    thread0 = threading.Thread(target=cl0.cap_and_send)
    
    #thread1 : calculate the amount of light and lighten
    cl1=calc_and_lighten()
    thread1 = threading.Thread(target=cl1.lighten)
    
    thread0.start()
    thread1.start()
    thread0.join()
    thread1.join()
