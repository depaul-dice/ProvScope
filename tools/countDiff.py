import sys

def countDiff(inputfile: str) -> None:
    right = 0
    left = 0
    direction = -1 
    with open(inputfile, 'r') as f:
        for line in f:
            print((line[0] == '>' and direction != 0) or (line[0] == '<' and direction != 1))
            if line[0] == '>' and direction != 0:
                right += 1
                direction = 0
            elif line[0] == '>' and direction != 1:
                left += 1
                direction = 1
            else:
                print('came here')
    print(right)
    print(left)
    return max(right, left)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: python3 countDiff.py inputfile")
    else:
        inputfile = sys.argv[1]
        print(str(countDiff(inputfile)) + " differences found")
