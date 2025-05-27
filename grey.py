def grey(bit):
    if bit == 0:
        return []
    if bit == 1:
        return [[False], [True]]
    
    tmp = grey(bit - 1)
    return [[False] + i for i in tmp] + [[True] + i for i in reversed(tmp)]


if __name__ == "__main__":
    print(grey(3))