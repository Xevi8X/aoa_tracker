class Gray:
    def __init__(self, bit_size=3):
        self.max_value = (1 << bit_size)
        self.lut = self._gen_lut(bit_size)

    def dec_to_bin(self, n) -> list[bool]:
        return self.lut[n]

    def _gen_lut(self, bit_size):
        lut = []
        for i in range(1 << bit_size):
            gray = i ^ (i >> 1)
            lut.append([bool((gray >> j) & 1) for j in range(bit_size - 1, -1, -1)])
        return lut


if __name__ == "__main__":
    gray = Gray(3)
    for i in range(gray.max_value):
        print(f"{i:03b} -> {gray.dec_to_bin(i)}")