# Potts simulation model

There are three branches of the code:
  * *original_version* : Keeps the original version of the code
  * *restyling* : it's updated every time new code is written
  * *master* : it keeps the stable release of the code (in the sense that the code has been checked and gives realistic results)

## Running the stable code

To download the code clone or fork the project and type on a terminal:

    git clone https://leonardo_romor@bitbucket.org/leonardo_romor/potts-simulation.git

To compile the code in terminal use *make* or instead to compile and run, write *make run*.

### How to sync with the repository

  A simple git pull inside the cloned folder should to the job. In case of troubles contact me.

### !Hot! Features

* I have made a test directory so that when I finish the code refactoring and I have to check that numbers are still the same it will be an easy task.
* Changed the Makefile in the root directory. Now you can check that numbers are ok with the old version running make test, compile and run the code with make run or just compile with make
