#!/bin/bash

echo "This is a simple score shell script for you to find out problems in your program"
echo "--------------------------------------------------------------------------------"

L=1
R=118
for ((i = $L; i <= $R; i = i + 1))
do
    ../bin/myscheme << EOF > scm.out
    $(cat ./data/$i.in)
    (exit)
EOF
    sed '$d' scm.out > scm_cleaned.out
    mv scm_cleaned.out scm.out
    sed 's/scm> //' scm.out > scm_cleaned.out
    mv scm_cleaned.out scm.out
    diff -b scm.out ./data/$i.out > diff_output.txt
    if [ $? -ne 0 ]; then
        echo -e "\033[33mWrong answer in TEST\033[0m" $i
        cat scm.out
        # echo "---------------------------"
        # echo ""
        # exit 1
    fi
done

L_EXTRA=1
R_EXTRA=7
for ((i = $L_EXTRA; i <= $R_EXTRA; i = i + 1))
do
    ../bin/myscheme << EOF > scm.out
    $(cat ./more-tests/$i.in)
    (exit)
EOF
    sed '$d' scm.out > scm_cleaned.out
    mv scm_cleaned.out scm.out
    sed 's/scm> //' scm.out > scm_cleaned.out
    mv scm_cleaned.out scm.out
    diff -b scm.out ./more-tests/$i.out > diff_output.txt
    if [ $? -ne 0 ]; then
        echo -e "\033[33mWrong answer in EXTRA TEST\033[0m" $i
        cat scm.out
        # echo "---------------------------"
        # echo ""
        # exit 1
    fi
done
