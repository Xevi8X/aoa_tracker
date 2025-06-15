
import pigpio
import time

pi = pigpio.pi()
if not pi.connected:
    exit()

GPIO = 12        # GPIO 12 = physical pin 32
FREQ = 500500     # 50 kHz
DUTY = 500000    # 50% (range is 0–1,000,000)

pi.hardware_PWM(GPIO, FREQ, DUTY)

time.sleep(30)   # run PWM for 10 seconds

pi.hardware_PWM(GPIO, 0, 0)  # stop PWM
pi.stop()
