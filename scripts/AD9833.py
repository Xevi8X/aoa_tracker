import spidev
import time

class AD9833:
    OSCILLATOR_FREQ = 25_000_000

    B28 = 1 << 13
    HLB = 1 << 12
    FSEL = 1 << 11
    PSEL = 1 << 10
    RESET = 1 << 8
    SLEEP1 = 1 << 7
    SLEEP12 = 1 << 6
    OPBITEN = 1 << 5
    DIV2 = 1 << 3
    MODE = 1 << 1

    def __init__(self, spi_bus=0, spi_device=0):
        self.spi = spidev.SpiDev()
        self.spi.open(spi_bus, spi_device)
        self.spi.max_speed_hz = 500000
        self.control_reg = self.B28
        self._init_device()

    def _init_device(self):
        self.reset()
        time.sleep(0.1)

    def _write_register(self, data):
        high_byte = (data >> 8) & 0xFF
        low_byte = data & 0xFF
        print(self.spi.xfer2([high_byte, low_byte]))

    def reset(self):
        self._write_register(self.control_reg | self.RESET)

    def power_down(self):
        self._write_register(self.control_reg | self.SLEEP1 | self.SLEEP12)

    def set_freq(self, freq_hz):
        freq_word = int((freq_hz * (1 << 28)) / self.OSCILLATOR_FREQ)
        lsb = freq_word & 0x3FFF
        msb = (freq_word >> 14) & 0x3FFF

        self._write_register(self.control_reg | self.B28)  # Enable 28-bit write
        self._write_register(0x4000 | lsb)                 # Write LSB to FREQ0
        self._write_register(0x4000 | 0x4000 | msb)        # Write MSB to FREQ0
        self._write_register(self.control_reg)             # Exit reset

    def set_waveform(self, waveform):
        """
        Set output waveform type.

        waveform: "sine", "triangle", or "square"
        """
        self.control_reg &= ~(self.MODE | self.OPBITEN | self.DIV2)

        if waveform == "sine":
            pass
        elif waveform == "triangle":
            self.control_reg |= self.MODE
        elif waveform == "square":
            self.control_reg |= self.OPBITEN | self.DIV2
        else:
            raise ValueError("Invalid waveform. Use 'sine', 'triangle', or 'square'.")

        self._write_register(self.control_reg)

    def close(self):
        self.spi.close()
