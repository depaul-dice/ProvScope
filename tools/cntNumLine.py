import sys

def cntNumLines(inputfile: str) -> dict:
    cnt = 0
    begin = False

    with open(inputfile, 'r') as f:
        for line in f: 
            if line.find('->') != -1:  # if it is going inside the function
                funcName = line[line.find('->') + 2: -1]
                if funcName == 'main': # if it is main start recording
                    begin = True
                if begin:
                    cnt += 1
                    
            elif line.find('<-') != -1: # if it is going out from the function
                funcName = line[line.find('<-') + 2: -1] 
                if begin:
                    cnt += 1
                if funcName == 'main':
                    begin = False

            else:
                raise Exception("file format not correct")
    return cnt 

def main():
    if len(sys.argv) != 2:
        print('usage: python3 .py inputfile')
        sys.exit(1)
    inputfile = sys.argv[1]
    cnt = cntNumLines(inputfile)
    print('number of lines: ' + str(cnt))

if __name__ == "__main__":
    main()
