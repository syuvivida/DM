#!/bin/bash
xsecDataFile=test.dat


lambda3=(0.1 0.5 1 2 3 3.6 3.8 4 5 6)

echo "lambda3" "WH" "WH/MH" "WA" "WA/MA" "Wa" "WHc" "Xsec" "Xsec*3000/fb" >> $xsecDataFile
for l3 in "${lambda3[@]}"
do
    setl3=$l3
    echo $setl3
	
    cp -p Cards/param_card_paper.dat Cards/param_card.dat
    file=Cards/param_card.dat
    sed -i "" -e "s/LAM3/"$setl3"/g" $file
    ./bin/generate_events -f
    banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
    echo $banner
    xsec=`grep -a Integrated $banner | awk '{print $6}'`
    decayH=`grep -a "DECAY  35" $banner | awk '{print $3}'`
#    echo $decayH
    fracH=`awk -v a=$decayH -v b=500 'BEGIN{print (a /b)}'`
    decayA=`grep -a "DECAY  36" $banner | awk '{print $3}'`
    fracA=`awk -v a=$decayA -v b=230 'BEGIN{print (a /b)}'`
    decaya=`grep -a "DECAY  55" $banner | awk '{print $3}'`
    decayHc=`grep -a "DECAY  37" $banner | awk '{print $3}'`
    neve=`awk -v a=$xsec -v b=3000000 'BEGIN{print (a*b)}'`
    echo $setl3 $decayH $fracH $decayA $fracA $decaya $decayHc $xsec $neve >> $xsecDataFile
done

updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
