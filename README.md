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

## References
The code for this compiler was inspired by the **Joos compiler code**.  
Additionally, parts of the code generation was modeled off of the **reference compiler**.  
This(http://stackoverflow.com/questions/10826744/semicolon-insertion-ala-google-go-with-flex) stackoverflow question/answer was referenced to support insertion of semicolons.
