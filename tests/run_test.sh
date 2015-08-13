#!/bin/bash

echo -e "\nCHECKING OUTPUT FILES\n"

if diff old_code/pattern.dat new_code/patterns.dat $2>/dev/null; then
    echo -e "Pattern generation test: \e[4;32mPASSED\e[0m"
else
    echo -e "Pattern generation test: \e[4;31mNOT PASSED\e[0m"
fi

if diff old_code/init_states.dat new_code/init_states.dat $2>/dev/null; then
    echo -e "States initialization test: \e[4;32mPASSED\e[0m"
else
    echo -e "States initialization test: \e[4;31mNOT PASSED\e[0m"
fi

if diff old_code/init_connections2.dat new_code/init_connections.dat $2>/dev/null ; then
    echo -e "Connection initialization test: \e[4;32mPASSED\e[0m"
else
    echo -e "Connection initialization test: \e[4;31mNOT PASSED\e[0m"
fi

if diff old_code/init_J.dat new_code/init_J.dat $2>/dev/null ; then
    echo -e "J test: \e[4;32mPASSED\e[0m"
else
    echo -e "J test: \e[4;31mNOT PASSED\e[0m"
fi

if diff old_code/updated_states.dat new_code/updated_states.dat $2>/dev/null ; then
    echo -e "States update test: \e[4;32mPASSED\e[0m"
else
    echo -e "States update test: \e[4;31mNOT PASSED\e[0m"
fi

echo -e "\n"
