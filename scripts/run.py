import time

from AD9833 import AD9833
from switch import Switch


if __name__ == "__main__":
    gen = AD9833()
    switch = Switch([21, 20, 16])
    try:
        gen.set_waveform("triangle")
        gen.set_freq(500000)

        while True:
            switch.update()
            time.sleep(0.1)

    finally:
        gen.close()
