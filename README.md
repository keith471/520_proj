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
Same as Milestone 2

## Milestone 4 Instructions
- I properly check that indices used in array accesses are not out of bounds for any accesses that occur within a function. However, for accesses that occur in a top level declaration, I do not perform the check check as C++ requires that function calls appear within functions, so I cannot simply add a call to GOLITE_CHECK_BOUNDS at the top level.

## Sources
The code for this compiler has been modeled off of the Joos compiler code.
Additionally, parts of the code generation was modeled off of the reference compiler.

