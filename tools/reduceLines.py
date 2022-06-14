
import sys

def main(inputfile: str, outputfile: str):
    with open(inputfile, 'r') as f, open(outputfile, 'w') as g:
        for line in f:
            if line.find(':0 ') != -1:
                g.write(line)

if __name__ == "__main__":
    inputfile = sys.argv[1]
    outputfile = sys.argv[2]
    main(inputfile, outputfile)
