#!/bin/bash
xsecDataFile=test.dat
lam3dat=/Users/yush/no_constraint/updated_lam3.dat
sintdat=/Users/yush/no_constraint/updated_sint.dat
tanbdat=/Users/yush/no_constraint/updated_tb.dat

#lam3dat=/Users/yush/no_constraint/tmp_lam3.dat
#sintdat=/Users/yush/no_constraint/tmp_sint.dat
#tanbdat=/Users/yush/no_constraint/tmp_tb.dat


workdir=/Users/yush/2HDMa/MG5_aMC_v3_5_7/workdir_HAAahah
cd $workdir

echo "MH" "MA" "BR(H->AA)" "BR(A->ah)" "BR(a->chi+chi)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile

paste $lam3dat $sintdat $tanbdat | while read -r seth2 seth3 lam3 lam3err seth2_dummy seth3_dummy sint sinterr seth2_dummy1 seth3_dummy2 tb tberr; do 
    
    echo $seth2, $seth3, $lam3, $sint, $tb
    cp -p Cards/param_card_paper.dat Cards/param_card.dat
    file=Cards/param_card.dat
    sed -i "" -e "s/MH2/"$seth2"/g" $file
    sed -i "" -e "s/MH3/"$seth3"/g" $file
    sed -i "" -e "s/TANB/"$tb"/g" $file
    sed -i "" -e "s/SINP/"$sint"/g" $file
    sed -i "" -e "s/LAM3/"$lam3"/g" $file
    
    ./bin/generate_events -f
    banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
    echo $banner
    xsec=`grep -a Integrated $banner | awk '{print $6}'`
    echo $xsec
    decayH=`grep -a "2    36  36" $banner | awk '{print $1}'`
    echo $decayH
    decayA=`grep -a "2    25  55" $banner | awk '{print $1}'`
    echo $decayA
    decaya=`grep -A 2 "DECAY  55" $banner | grep -a "52  -52" | awk '{print $1}'`
    echo $decaya
    finalXSec=`awk -v a=$xsec -v b=$decayH -v c=$decayA -v d=$decaya 'BEGIN{print (a*b*c*c*d*d)}'`
    echo $finalXSec
    echo $seth2 $seth3 $decayH $decayA $decaya $xsec $finalXSec >> $xsecDataFile
    olddir=`ls -lrt Events | tail -1 | awk '{print $9}'`
    newdir=resonant_HAAah_13p6TeV_MA_${seth3}_MH_${seth2}_tanbeta${tb}_lambda3_${lam3}_sint${sint}
	echo $olddir $newdir
	mv Events/$olddir Events/$newdir
	olddir=`ls -lrt Events | grep -a run | tail -1 | awk '{print $9}'`
	mv Events/${olddir}/*txt Events/$newdir/.
	rm -rf Events/${olddir}
	
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
