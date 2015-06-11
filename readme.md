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

## Features


Right now I have cleaned up the code of the pattern generation and rewrote it in a more object oriented way so it's easier to read and to reuse, this is a one-time task per simulation and so I will not stress the efficiency in this part of the code right now. Anyway this work had to be done to simplify and reorganize the code.

The code now simply generate a pattern with defaults parameters and write optionally to a file. The output is consistent with the original version of the code. Now I will start working on the dynamics of the simulation.

Now a generator is an abstract object that can be initialized in this way:

```c++
//Random seed init
std::default_random_engine generator;
generator.seed(12345);

PatternGen pgen(
           600, //N
           10, //p
           3, //S
           0.25, //a
           11.0, //beta
           50, //N_fact
           200, //Num_fact
           0.25, //a_fact
           1e-6, //eps
           3e-6, //a_pf
           3e-6 //fact_eigen_slope
           );

pgen.set_random_generator(generator);
pgen.generate();
pgen.eval_stats();
pgen.save_pattern_to_file("patterns.dat");

```

also in the code are kept  as generally true the following equations of the old const_potts.h:

Num_u = N
Num_p = p
Num_s = S

### !Hot! Features

New class definitions:



Generate an array of integers between 0 and N-1, you can shuffle this random sequence, print it on terminal or retrieve the pointer to the sequence.
```c++
class RandomSequence{
    private:
        int * sequence;
        int N;

    public:
        RandomSequence(const int N);
        ~RandomSequence();

        void shuffle(std::default_random_engine & generator);
        void save_sequence_to_file();
        void print();
        int * begin();
        int * end();

};
```


PUnit class defines a Potts Unit. Object has been since the updates of the dynamics are asynchronous and because of that the overhead is not an issue.
```c++
class PUnit{
    private:
        double * state; //Array that keeps the value of the states of the current Potts unit
        double * cdata; //Array that keeps for each array all the data necessary to update the unit.
        double * r; //Input!?
        int S; //Number of states
        int C; //Number of connections
    public:
        ~PUnit();
        PUnit(const int S, const int C);
        void init(const double beta, const double U);
};
```
PNetwork is a class that handles the potts units. And provide useful methods to handle the dynamics and initialization process but a lot of the definitions has not be implemented yet.
```c++
class PNetwork{
    private:
        PUnit ** network;
        int N; //Number of units
        int S; //Number of states per unit
        int C; // Number of connections per unit
        double beta;
        double U;

        int * cm; //Connection matrix

    public:
        PNetwork(const int N, const int S, const int C, const double beta, const double U);
        ~PNetwork();
        void Init_Units();
        void ConnectUnits();
        void start();

        void print_cm();

};
```
