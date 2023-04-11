# HOW TO USE #
Two programs exist in this repo. One is to parse the function trace and the other is to find the divergence/convergence of the two executions.

## HOW TO PARSE THE RAW FUNCTION TRACE ##
Below is the command to use
```
python3 extractFunccalls.py file2parse sysfile
```
sysfile is the file which specifies which function leads to system calls or not. 
When the function does not lead to system calls, those function calls are omitted. 
These sysfiles are created at LLVM analyses directory which is another repository.

The command above creates two types of files, \*.ftr and \*.nr. \*.ftr files are the function call traces that are parsed and \*.nr files are the list of functions which we could not detect returning. While using Intel Pin to create a function call trace, some functions do not return using the return calls; instead they use jumps to be back at the instruction pointer of the caller function. In order to create a hierarchical trace accurately, we need to know which functions do not return preliminarily.
 
## HOW TO FIND THE DIVERGENCE/CONVERGENCE ##
To find the points of divergence/convergence, the following command works. You first need to make though!
```
./src/provScope -c funcList.txt nrfile parsedCFG ftrfile1 ftrfile2
```
Notice that you only need one nonreturn function files because they ftrfiles should be from the same executables, and so is parsedCFGs which are created from LLVManalyses. If you need to run multiple times and becomes tedious, you can prepare a txt file with one line above (let's say the file name is input.txt) and you can use the following command to do the same.
```
./src/provScope -f input.txt
```

# OBJECTIVE OF THE PROGRAM #

- provScope: to get the divergence/convergence of the two executions 
- countPaths: to get the number of paths that would align the function trace

# WHAT IS NECESSARY AS INPUTS #

- files that specify which function are clibrary functions
- function trace files that are parsed preliminary via python program which exists in this repo
- files that specify which function in the execution does not return in the pin program
- cfg files created from llvmanalysis

# PROCEDURE (at very high level of course) #
## PRELIMINARY STEPS ##
The objective of the preliminary steps are to parse the function traces so that it will only have function calls and returns which could lead to system calls

1. Go through the function trace and find which functions do not return (by using stack)
2. Go through the function trace again and only write the function/return calls that are relevant to the output file. Relevant calls are function calls that user program has called (the caller of the function is not clib).
    - In case the function call user program has called does not return, then we need to go one step down until we find the function that returns.
3. Use the output file as the input file of the actual steps. This file is used as function trace.
     
## ACTUAL STEPS ##
1. reads cfgs from the directory specified as an input
2. reads clibrary used
3. reads 2 function traces as flat traces
4. changes flat traces into hierarchical traces and updates hash to the traces
5. compares two hierarchical traces

# POSSIBLE USAGE OF THE PROGRAM #

1. When you have a program which has been simplified for the portability issues and want to check if the execution in the original environment and the execution in the other environment are the same. If they are different, HOW they are different

2. When you have two execution with different inputs which caused the difference in the outcome of the provenance, you want to use the program to specify the reason of the program execution divergence with a systematic approach

# DRAWBACKS OF THIS PROGRAM #
1. One needs to prepare so much before being able to run this program, and likely to face many dependency issues that would take a lot of time to solve

# EXPERIMENTS #
1. (HiPC 2022 paper) General informations on program specification, before and after the simplifications of the graphs <- this is done in LLVManalyses

2. (HiPC 2022 paper) Modules with correct/incorrect reconvergence observed by SP: We have differenciated two simplified function traces using the linux command `diff` first. Then as a comparison, we find points of divergence/convergence and check if `diff` command yielded point of divergence/convergence correctly regarding the function hierarchy manually.

3. (HiPC 2022 paper) \# of differential locations: this experiment is created counting the points of divergences comparing 2 function traces with the same executables.

4. (HiPC 2022 paper) Tracing Overhead over Normal Execution: This is the pinTool overhead and it is uploaded on tools directory. 

5. (HiPC 2022 paper) Reduction in Program Specification: this is from LLVManalyses repo.  

6. (HiPC 2022 paper) Reduction in PIN Function Traces: this is created through using `extractFunccalls.py` in tools directory. 
