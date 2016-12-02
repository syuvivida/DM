#!/bin/tcsh

setenv thisDir $PWD
set dir=$1
echo $thisDir
foreach dir($argv)
    echo $dir
    cd $thisDir/$dir
    ls gen*root | awk '{if($5<1000)print "rm -rf "$9}' | bash
    foreach file(gentuple*.root)
    root -q -b ~/scripts/DMDrawer.C++\(\"$file\"\)
    end
    cd -
end
