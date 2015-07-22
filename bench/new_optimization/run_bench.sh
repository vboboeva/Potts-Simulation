cd new_code
echo -e "NEW CODE"
./main.x | grep "TOTAL UPDATE"
cd ..
echo -e "OLD CODE"
cd old_code
./master.x | grep "TOTAL UPDATE"
