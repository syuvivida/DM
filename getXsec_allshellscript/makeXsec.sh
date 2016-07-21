#!/bin/bash

for i in 600 800 1000 1200 1400 1700 2000 2500
do
    
    replaceName=$(cat param_card.dat |grep 'mzp')
     #echo $replaceName                                                               
     reducedName=${replaceName#*32}
     #echo $reducedName                                                               
     reducedName2=${reducedName%#*}
     #echo $reducedName2                                                              
     sed -i -e  's/'"$replaceName"'/32 '"$i"' #mzp /g' param_card.dat




    for j in 300 400 500 600 700 800
    do
     
     replaceName3=$(cat param_card.dat |grep 'mhn')
     #echo $replaceName3
     reducedName3=${replaceName3#*26}
     #echo $reducedName3
     reducedName23=${reducedName3%#*}
     #echo $reducedName23
     sed -i -e  's/'"$replaceName3"'/26 '"$j"' #mhn /g' param_card.dat

     replaceName4=$(cat param_card.dat |grep 'mhp')
     #echo $replaceName4
     reducedName4=${replaceName4#*27}
     #echo $reducedName4
     reducedName24=${reducedName4%#*}
     #echo $reducedName24
     sed -i -e  's/'"$replaceName4"'/27 '"$j"' #mhp /g' param_card.dat

     replaceName5=$(cat param_card.dat |grep 'ma0')
     #echo $replaceName5
     reducedName5=${replaceName5#*28}
     #echo $reducedName5
     reducedName25=${reducedName5%#*}
     #echo $reducedName25
     sed -i -e  's/'"$replaceName5"'/28 '"$j"' #ma0 /g' param_card.dat
     

     word=$(grep "DECAY  28" param_card.dat)
  #echo $word
     test=${word#*28}
  #echo $test2
     sed -i -e  's/'"$test"'/  Auto/g' param_card.dat
  #echo $word
     word2=$(grep "DECAY  25" param_card.dat)
     word3=$(grep "DECAY  32" param_card.dat)
     test2=${word2#*25}
     test3=${word3#*32}
  #sed -i -e  's/'"$test2"'/  Auto/g' param_card.dat
     sed -i -e  's/'"$test3"'/  Auto/g' param_card.dat
     cd ../
     ./bin/generate_events -f >& cout.txt
     out1=$(grep "Cross-section :" cout.txt)
     #echo $out1
     out2=${out1##*Cross-section :}
     #echo $out2
     out3=${out2% +-*}
     echo $out3
     cd Cards/
    done
done