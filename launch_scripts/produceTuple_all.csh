#!/bin/tcsh

setenv thisDir $PWD
set dir=$1
echo $thisDir
foreach dir($argv)
    echo $dir
    cd $thisDir/$dir
    foreach file(gentuple*.root)
    root -q -b ~/scripts/DMDrawer.C++\(\"$file\"\)
    end
    cd -
end
