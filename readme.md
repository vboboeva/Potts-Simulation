# Potts simulation model

There are three branches of the code:
  * *original_version* : Keeps the original version of the code
  * *restyling* : it's updated every time new code is written
  * *master* : it keeps the stable release of the code (in the sense that the code has been checked and gives realistic results)

## Running the stable code

To compile the code in terminal use *make* or instead to compile and run, write *make run*.
Do not break the directory tree to keep makefiles and scripts fully working.

##Folder structure

* *bench* : Keeps some useful benchmarks, more info in the readme inside the folder.
* *build* : Default directory where the binaries are going to be generated
* *include* : Default directory that keeps the "frontend" includes
* *src* : Default directory that keeps the source files.
* *lib* : Default directory that keeps all the .cpp and .h used in a generic source file.
* *tests* : Directory in which is possible to run some "sanity check" tests to match the new code results with the old one.
