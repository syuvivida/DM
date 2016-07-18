#! /bin/bash

for((i=1;i<=48;i++))
do
    if ((i < 10))
    then
        cd Events/run_0${i}
        cp run_0${i}_tag_1_banner.txt ../../Bannerfile
        cd ../../
    else
         cd Events/run_${i}
         cp run_${i}_tag_1_banner.txt ../../Bannerfile
         cd ../../
    fi
done