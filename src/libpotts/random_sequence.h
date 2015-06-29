#include <random>
#include <algorithm>
#include <iostream>
#include <math.h>


#ifndef __RANDOM_SEQUENCE
#define __RANDOM_SEQUENCE

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

#endif
