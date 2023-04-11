
# HOW TO USE EACH PROGRAM #
1. `cntNumLines.py`: In the raw function call trace that is given to you (from Intel Pin), you can count the number of lines from the entry of the function `main` to the same function

2. `convertDot.py`: This is for the visualization. What the program in the repo LLVM analyses creates is the .txt file. In order to visualize using *graphviz*, you can feed the *.txt* file to output *.dot* files

3. `convertDotDirectory.py`: This a tool for the laziness at one level higher than `convertDot.py`. If you want to feed the whole directory to create *.dot* files for the whole directory, use this program.

4. `countDiff.py`: This is a tool automating counting how many differences exist in the output of linux command *diff*. 

5. `extractFunccalls.py`: Explained in the README of the parent directory. 

6. `FuncAnalysis`: This is a PinTool directory. If you untar it under pin/source/tools and make the file and run with shared object and the binary file *pin*.
