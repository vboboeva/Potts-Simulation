#!/usr/bin/perl -w

printf "Degree distribution:\n";
printf " power law decay k^-tau\n";
printf " exponential cutoff e^k/kappa\n\n";
for($tau = 1.; $tau < 3; $tau +=.5){
	system("./giant","1000000","$tau", "100");
}

printf "\nEnd!\n\n"
