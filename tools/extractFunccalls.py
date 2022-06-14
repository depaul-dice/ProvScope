'''
Copyright (c) 2021 Yuta Nakamura. All rights reserved.
'''

# this gets you the function that is in libc
import sys
from os.path import exists as file_exists
from funcList import musllvmLibcFuncList
from distutils.util import strtobool
from CONSTANTS import transFuncs, linefeed, returnChar, args

def getNonRetFuncs(inputfile: str) -> dict:
    stack = list() 
    nonRetFuncs = dict()
    begin = False

    with open(inputfile, 'r') as f:
        # whenever you are returning from the function, 
        # you pop the stack until you find the function name at the stack
        for index, line in enumerate(f): 
            if line.find('->') != -1:  # if it is going inside the function
                funcName = line[line.find('->') + 2: -1]
                if funcName in transFuncs:  # added this because of fstatat64
                    funcName = transFuncs[funcName] 
                if funcName == 'main': # if it is main start recording
                    begin = True
                if begin:
                    stack.append(funcName)
                    
            elif line.find('<-') != -1: # if it is going out from the function
                funcName = line[line.find('<-') + 2: -1] 
                if funcName in transFuncs:
                    funcName = transFuncs[funcName] 
                if begin:
                    while funcName != stack[-1]:
                        nonRetFuncs[stack.pop()] = 1
                        if len(stack) == 0:
                            print("this is funcname: " + funcName + ", " + str(index))
                    stack.pop()
                if funcName == 'main':
                    begin = False

            else:
                raise Exception("file format not correct")
    return nonRetFuncs

def outputParsedFuncTrace(inputfile: str, outputfile: str, nonRetFuncs: dict, sysFuncs = None):
    begin = False
    stack = list()
    pause = False
    pauseFunc = None 

    with open(inputfile, 'r') as f, open(outputfile, 'w') as g:
        for line in f:
            if line.find('->') != -1:
                funcName = line[line.find('->') + 2: -1]
                if funcName in transFuncs:
                    funcName = transFuncs[funcName]

                if funcName == 'main':
                    begin = True

                # that it has gone into the main function and has syscalls inside is the requirement for the function to be registered
                if begin and (sysFuncs == None or sysFuncs[funcName]): 
                    # if the function is in libc, stop and pause the recording
                    if funcName in musllvmLibcFuncList and not pause: 
                        if funcName not in nonRetFuncs: # strrchr fucked up in the past
                            # print("pausing at: " + funcName)
                            pause = True
                            pauseFunc = funcName
                            # assert sysFuncs[funcName] == True
                            g.write(funcName + linefeed) # I am writing here because it is already paused
                    if not pause:
                        if funcName not in nonRetFuncs:
                            stack.append(funcName)
                        # assert sysFuncs[funcName] == True
                        g.write(funcName + linefeed)

            elif line.find('<-') != -1: 
                funcName = line[line.find('<-') + 2: -1]
                
                if funcName in transFuncs:
                    funcName = transFuncs[funcName]
                if begin and (sysFuncs == None or sysFuncs[funcName]):
                    if pause and pauseFunc == funcName:
                        # print("pause stopping at: " + funcName)
                        pause = False
                    
                    if not pause: # this is not else by design
                        if funcName == 'main':
                            g.write(returnChar + funcName)
                        else:
                            g.write(returnChar + funcName + linefeed)
                        
                if funcName == 'main':
                    assert pause == False
                    begin = False
            else:
                raise Exception("file format not correct")

def getSysFuncs(sysfile: str):
    sysFuncs = {
            "__syscall_ret":False,
            "memmove":False,
            "__syscall_cp":False,
            "__syscall_cp_c": False,
            "__syscall_cp_asm": False,
            "sccp":False,
            "__clone": False,
            } 
    with open(sysfile, 'r') as f:
        for line in f:
            funcName, sys = line.split(':')
            '''
            if funcName == "chopt_init":
                print(funcName + ': ' + str(strtobool(sys.strip())))
            '''
            sysFuncs[funcName.strip()] = bool(strtobool(sys.strip())) 
    return sysFuncs

def outputNonRet(nonRetFuncs: dict, nonRetFile: str): 
    first = True
    with open(nonRetFile, 'w') as f:
        for key in nonRetFuncs:
            if first == False:
                f.write('\n')
            else:
                first = False

            f.write(key)

def main(inputfile: str, sysfile: str, removeSysfuncs: bool):
    outputfile = inputfile.replace(".out", ".ftr")
    outputfile = outputfile[outputfile.rfind('/') + 1: ] 
    nonRetFile = inputfile.replace(".out", ".nr")
    nonRetFile = nonRetFile[nonRetFile.rfind('/') + 1: ]
    
    print("outputFile: " + outputfile)
    print("nonRetFile: " + nonRetFile)
    # getting the dictionary of function that does not return

    nonRetFuncs = getNonRetFuncs(inputfile) # this is dictionary
    outputNonRet(nonRetFuncs, nonRetFile)
    
    if file_exists(sysfile) and removeSysfuncs:
        sysFuncs = getSysFuncs(sysfile)
        outputParsedFuncTrace(inputfile, outputfile, nonRetFuncs, sysFuncs)
    elif (not file_exists(sysfile)) and (not removeSysfuncs):
        outputParsedFuncTrace(inputfile, outputfile, nonRetFuncs)
    else:
        # this doesn't really match, so it is an error  
        print("sysfile: " + str(sysfile), file=sys.stderr)
        raise Exception("you have to either specify the legit sysfile or say that you don't need them because the cfgs don't cut those")

if __name__ == "__main__":
    Args = args()

    inputfile = Args.file2parse
    sysfile = Args.sysfile
    removeSysfuncs = Args.removeSysfuncs
    
    main(inputfile, sysfile, removeSysfuncs)
    
