from gray import Gray

import RPi.GPIO as GPIO

class Switch:
    def __init__(self, gpios: list[int]):
        self.gpios = gpios
        self.bits = len(self.gpios)
        self.channels = pow(2, self.bits)
        self.gray = Gray(self.bits)

        self.current = 0
        self.outputs = self.gray.dec_to_bin(self.current)

        GPIO.setmode(GPIO.BCM)
        for i, gpio in enumerate(self.gpios):
            GPIO.setup(gpio, GPIO.OUT)
            self._set_gpio(i, self.outputs[i])

    def __del__(self):
        GPIO.cleanup(self.gpios)

    def update(self):
        self.current = (self.current + 1) % self.channels
        new_outputs = self.gray.dec_to_bin(self.current)
        for i in range(self.bits):
            if new_outputs[i] != self.outputs[i]:
                self._set_gpio(i, new_outputs[i])
        self.outputs = new_outputs

    def _set_gpio(self, gpio_index: int, value: bool):
        GPIO.output(self.gpios[gpio_index], GPIO.HIGH if value else GPIO.LOW)

if __name__ == "__main__":
    switch = Switch([1, 2, 3])
    for _ in range(10):
        print(f"Update")
        switch.update()
        print(f"Current state: {switch.current}")