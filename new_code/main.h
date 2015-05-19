#include <iostream>
#include <random>
#include <time.h>


//pattern_generation.cpp
#ifndef __PATTERN_GENERATION
#define __PATTERN_GENERATION

class PatternGen{
    private:
        std::default_random_engine * gen; //Generator of random values

    public:
        PatternGen();
        PatternGen(std::default_random_engine & generator);
        set_random_generator(std::default_random_engine & generator);

};

#endif
