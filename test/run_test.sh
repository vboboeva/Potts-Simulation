#!/bin/bash
echo -e "\nCHECKING OUTPUT FILES\n"

if diff old_code/pattern.dat new_code/patterns.dat ; then
    echo -e "Pattern generation test: \e[4;32mPASSED\e[0m"
else
    echo -e "Pattern generation test: \e[4;31mNOT PASSED\e[0m"
fi


if diff old_code/init_states.dat new_code/init_states.dat ; then
    echo -e "States initialization test: \e[4;32mPASSED\e[0m"
else
    echo -e "States initialization test: \e[4;31mNOT PASSED\e[0m"
fi

if diff old_code/init_connections.dat new_code/init_connections.dat ; then
    echo -e "Connection initialization test: \e[4;32mPASSED\e[0m"
else
    echo -e "Connection initialization test: \e[4;31mNOT PASSED\e[0m"
fi


echo -e "\n"
