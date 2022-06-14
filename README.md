# HOW TO USE #
Two programs exist in this repo. One is to parse the function trace and the other is to find the divergence/convergence of the two executions.

## HOW TO PARSE THE RAW FUNCTION TRACE ##
## HOW TO FIND THE DIVERGENCE/CONVERGENCE ##
## HOW TO FIND THE NUMBER OF PATHS THAT ALIGNS WITH FUNCTION TRACES ##

# OBJECTIVE OF THE PROGRAM #

- main: to get the divergence/convergence of the two executions 
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
# NOTE #
- In the experiment, since it was a hassle to deal with many functions that glibc deals with, the executables are compiled with libc in musllvm. The experiment dataset uses coreutils. 

