#!/bin/bash
echo -e "\nCHECKING OUTPUT FILES\n"

if diff old_code/pattern.dat new_code/patterns.dat ; then
    echo -e "Pattern generation test: \e[4;32mPASSED\e[0m\n"
else
    echo -e "Pattern generation test: \e[4;31mNOT PASSED\e[0m\n"
fi
