#!/bin/bash
sintDataFile=sint.dat
lam3DataFile=lam3.dat
tanbDataFile=tb.dat

mh2=(470 500 550 600 650 700 750 800)
mh3=(230 250 300 325 350 400)                                                                                             
#mh2=(500)                                                                      #mh3=(230 250)


echo "MH" "MA" "Fitted_sint" "sint_error" >> $sintDataFile
echo "MH" "MA" "Fitted_lam3" "lam3_error" >> $lam3DataFile
echo "MH" "MA" "Fitted_tanb" "tanb_error" >> $tanbDataFile

for h2 in "${mh2[@]}"
do
    seth2=$h2
    for h3 in "${mh3[@]}"
    do
        seth3=$h3
	if ((h3>=h2/2))
	then
	    break
	fi
	echo $seth2, $seth3
	./minimize_3d $seth2, $seth3 > Log
	tail -n 3 Log | head -n 1 | awk -v mH=$seth2 -v mA=$seth3 '{print mH, mA, $3, $5}' >> $sintDataFile
	tail -n 2 Log | head -n 1 | awk -v mH=$seth2 -v mA=$seth3 '{print mH, mA, $3, $5}' >> $lam3DataFile
	tail -n 1 Log | head -n 1 | awk -v mH=$seth2 -v mA=$seth3 '{print mH, mA, $3, $5}' >> $tanbDataFile
    done
done

updateFile=updated_${sintDataFile}
column -t $sintDataFile > $updateFile

updateFile=updated_${lam3DataFile}
column -t $lam3DataFile > $updateFile

updateFile=updated_${tanbDataFile}
column -t $tanbDataFile > $updateFile
