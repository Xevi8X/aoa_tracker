import RPi.GPIO as GPIO

class PWM:
    def __init__(self, pin, freq):
        self.pin = pin
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.pin, GPIO.OUT)
        self.pwm = GPIO.PWM(pin, freq)
    
    def __del__(self):
        GPIO.cleanup(self.pin)

    def start(self, duty_cycle):
        self.pwm.start(duty_cycle)

if __name__ == "__main__":
    pwm = PWM(12, 5000000)
    try:
        pwm.start(50)
        while True:
            pass
    except KeyboardInterrupt:
        pass
    finally:
        del pwm
