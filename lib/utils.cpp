
#include <iostream>
#include <fstream>
#include <sstream>
#include "config.h"
#include "parameters_struct.h"
#include "utils.h"


unsigned int str2Int(const char* str, int h = 0){
    return !str[h] ? 5381 : (str2Int(str, h+1)*33) ^ str[h];
}

constexpr unsigned int str2int(const char* str, int h = 0){
    return !str[h] ? 5381 : (str2int(str, h+1)*33) ^ str[h];
}

void key_value_matcher(std::string key, std::string value, struct parameters & params){


    switch (str2Int(key.c_str())){
        case str2int("N"):
            params.N = std::stoi(value);
            break;
        case str2int("C"):
            params.C = std::stoi(value);
            break;
        case str2int("p"):
            params.p = std::stoi(value);
            break;
        case str2int("S"):
            params.S = std::stoi(value);
            break;
        case str2int("a"):
            params.a = (__fpv)std::stod(value);
            break;
        case str2int("U"):
            params.U = (__fpv)std::stod(value);
            break;
        case str2int("b1"):
            params.b1 = (__fpv)std::stod(value);
            break;
        case str2int("b2"):
            params.b2 = (__fpv)std::stod(value);
            break;
        case str2int("b3"):
            params.b3 = (__fpv)std::stod(value);
            break;
        case str2int("beta"):
            params.beta = (__fpv)std::stod(value);
            break;
        case str2int("w"):
            params.w = (__fpv)std::stod(value);
            break;
        case str2int("g"):
            params.g = (__fpv)std::stod(value);
            break;
        case str2int("tau"):
            params.tau = (__fpv)std::stod(value);
            break;
        case str2int("nupdates"):
            params.nupdates = std::stoi(value);
            break;
        case str2int("NumSet"):
            params.NumSet = std::stoi(value);
            break;
        case str2int("N_fact"):
            params.N_fact = std::stoi(value);
            break;
        case str2int("Num_fact"):
            params.Num_fact = std::stoi(value);
            break;
        case str2int("a_fact"):
            params.a_fact = (__fpv)std::stod(value);
            break;
        case str2int("eps"):
            params.eps = (__fpv)std::stod(value);
            break;
        case str2int("a_pf"):
            params.a_pf = (__fpv)std::stod(value);
            break;
        case str2int("fact_eigen_slope"):
            params.fact_eigen_slope = (__fpv)std::stod(value);
            break;
        case str2int("tstatus"):
            params.tstatus = std::stoi(value);
            break;
    }

}


void load_params(std::string filename, struct parameters & params){

    std::ifstream ifile;
    ifile.open(filename);


    int name;


    if (ifile.is_open()){

        std::string line;

        while( std::getline(ifile, line) ){
            std::istringstream is_line(line);
            std::string key;
            std::string value;
            if( std::getline(is_line, key, '=') ){
                if( std::getline(is_line, value) )
                key_value_matcher(key,value, params);
            }
        }

        ifile.close();

    }else{

        std::cerr << "\e[4;31mERROR\e[0m: Unable to open file" << std::endl;

    }



}

void print_params(struct parameters & params){

    std::cout << params.N <<std::endl;
    std::cout << params.C <<std::endl;
    std::cout << params.p <<std::endl;
    std::cout << params.S <<std::endl;
    std::cout << params.a <<std::endl;
    std::cout << params.U <<std::endl;
    std::cout << params.b1 <<std::endl;
    std::cout << params.b2 <<std::endl;
    std::cout << params.b3 <<std::endl;
    std::cout << params.beta <<std::endl;
    std::cout << params.w <<std::endl;
    std::cout << params.g <<std::endl;
    std::cout << params.tau <<std::endl;
    std::cout << params.nupdates <<std::endl;
    std::cout << params.NumSet <<std::endl;
    std::cout << params.N_fact <<std::endl;
    std::cout << params.Num_fact <<std::endl;
    std::cout << params.a_fact <<std::endl;
    std::cout << params.eps <<std::endl;
    std::cout << params.a_pf <<std::endl;
    std::cout << params.fact_eigen_slope <<std::endl;
    std::cout << params.tstatus <<std::endl;
}
