
#ifndef __RANDOM_SEQUENCE_H
#define __RANDOM_SEQUENCE_H

#include <random>

class RandomSequence{
    private:
        int * sequence;
        int N;

    public:
        RandomSequence(const int N);
        ~RandomSequence();

        void shuffle(std::default_random_engine & generator);
        void print();
        int * begin();
        int * end();
        int get(const int & i){return this->sequence[i];};

};

#endif
