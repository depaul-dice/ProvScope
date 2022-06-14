'''
Copyright (c) 2021 Yuta Nakamura. All rights reserved.
'''

import argparse

class args:
    def __init__(self):
        parser = argparse.ArgumentParser(description = 'just parsing casually')
        # below are positional arguments
        parser.add_argument("file2parse", type=str, help= 'filename to parse')
        parser.add_argument("sysfile", nargs='?', type = str, default="", help='file to specify whether the function has a system calls')

        # below are specified options
        parser.add_argument("-s", "--removeSysfuncs", dest = 'removeSysfuncs', action='store_true',
                help="removes irrelevant (not related to syscalls) funccalls")
        parser.add_argument("--no-removeSysfuncs", dest = 'removeSysfuncs', action='store_false')
        parser.set_defaults(removeSysfuncs = True)

        parser.add_argument("-p", "--printArgs", action="store_true")

        _args = parser.parse_args()
        
        self.file2parse = _args.file2parse
        self.sysfile = _args.sysfile
        self.removeSysfuncs = _args.removeSysfuncs
        
        if _args.printArgs == True:
            self.__printArgs()
        
    def __printArgs(self) -> None:
        print("/////////////////////////printing arguments//////////////////////////////")
        print("file2parse: " + self.file2parse)
        print("sysfile: " + self.sysfile)
        print("removeSysfuncs: " + self.removeSysfuncs)
        print("/////////////////////////////////////////////////////////////////////////", end = '\n\n')

transFuncs = {
                "___errno_location": "__errno_location",
                "fstatat64": "fstatat",
                "__fxstat64": "__fxstat", # added for bwa
                "freopen64": "freopen",
                "lseek64": "lseek",
                "stat64": "stat",
                "open64": "open",
                "getrlimit64": "getrlimit", # added for minimap2
                "setrlimit64": "setrlimit", # added for minimap2
                "strchrnul": "__strchrnul", # added for chownv
                "__getdelim": "getdelim", # added for chownv
                "memrchr": "__memrchr", # added for dated
                "__posix_getopt": "getopt", # added for dated
                "clock_gettime": "__clock_gettime", # added for dated 
                "localtime_r": "__localtime_r",
                "mmap64": "__mmap",
                "munmap": "__munmap", # added for minimap2
                "strftime_l": "__strftime_l",
                "__strtol_internal": "strtol",
                "fwrite_unlocked": "fwrite",
                "fileno_unlocked": "fileno", # added for uniq
                "fflush_unlocked": "fflush", # added for uniq
                "fdopen": "__fdopen",
                "dup3": "__dup3",
                "mprotect": "__mprotect",
                "posix_fadvise64": "posix_fadvise",
                "fputs_unlocked": "fputs", # added for minimap2
                "fgetc_unlocked": "getc_unlocked",
                "fread_unlocked": "fread",
                "ferror_unlocked": "ferror",
                "_IO_feof_unlocked": "feof",
                "pthread_setcancelstate": "__pthread_setcancelstate", # added for minimap2
                "pthread_join": "__pthread_join", # added for minimap2
                "madvise": "__madvise", # added for bwa
        }

linefeed = '\n'
returnChar = '/'
