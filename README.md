# COMP 520 GoLite Project

This compiler was designed and implemented by Keith Strickling (McGill ID 260674699).  
For questions, please contact keith.strickling@mail.mcgill.ca.  

## Milestone 1 Instructions
Build with `./build.sh`.  
Run for an individual file with `./run.sh <path_to_golite_file>`.  
Or, to run against all valid/invalid files in `./programs/*`, run with `./test.sh`.  
The pretty-printed output is saved in `./src/output`. **It is essential that this directory exists to avoid segmentation faults.**

## Milestone 2 Instructions
Build with `./build.sh`.  
Run for an individual file with `./run.sh <path_to_golite_file> [-dumpsymtab, -pptype]`.  
Output files are saved in `./src/output`. **It is essential that this directory exists to avoid segmentation faults.**

## Milestone 3 Instructions
Same as Milestone 2.  

## Milestone 4 Instructions
Build with `./build.sh`.  
Run for a single file with `./run.sh <path_to_golite_file> [-h, -v, -pp, -onthefly, -dumpsymtab]`.  
The flag `-onthefly` is pretty useful for testing! It allows you to compile code typed directly into the terminal. In addition, I've added a flag `-reqmain`. If set, the symbol phase will check for a function called `main` with no parameters and no return type. If it does not find such a function, it will report an error and stop compilation. I added this so that my compiler is more inline with the Go compiler, which requires that a main function is declared. However, I noticed that some of the past test programs used in grading do not include a main function. In case some of the final test cases used do not contain a main function, I created this flag such that the check for a main function is performed only if the flag is set. By default, `run.sh` does not pass this flag. If you'd like to require a main function, just open up `run.sh` and replace `$7` with `-reqmain`. **Please note:** If the main function is not required, and the GoLite program does not declare a main function, then the compiled C++ code my compiler produces will throw an error during compilation, since C++, like Go, requires a main function.   
All the other flags are intuitive (run with the `-h` flag for more info!).  
On a final note, when compiling the C++ code my compiler produces, you may get some warnings due to string literals ("warning: conversion from string literal to 'char \*' is deprecated"). The code will still run without issue though! :)  
Output files are saved in `./src/output`. **It is essential that this directory exists to avoid segmentation faults.**

## Final Submission
Between **Milestone 4** and this submission, I fixed the following:
- naming conflicts with things imported by namespace _std_
- bug pertaining to printing out default values for structs
- fixed code generation of array assignment (now uses _memcpy_)
- fixed code generation of break statements in if statements -- the break statements are now generated as _goto_ statements
- a scoping issue that occurred in the following scenario
    int x = 3;
    {
        int x = x;
        println(x); // x should be 3, but it was undefined due to C++ behavior
    }
- printing of booleans, runes, and floats
- passing of arrays
- slice assignment

The only thing that I did not fix is **passing of slices**, which broke when I fixed slice assignment. Unfortunately, it doesn't seem to be easy to support both slice assignment by reference and slice passing by value in C++. So, I just support slice assignment by reference. My slices are also passed by reference instead of by value, which means that the function a slice is passed to can modify the original slice. This is not how Go behaves, but it's the best I can do for now.

## References
The code for this compiler was inspired by the **Joos compiler code**.  
Additionally, parts of the code generation was modeled off of the **reference compiler**.  
This(http://stackoverflow.com/questions/10826744/semicolon-insertion-ala-google-go-with-flex) stackoverflow question/answer was referenced to support insertion of semicolons.
