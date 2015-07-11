#!/bin/bash     
if [[ -e $1 ]]; then
    sed -i -e '/version/s/3.0/1.0/' $1
    sed -i '/cite/d' $1
    sed -i '/<mgrwt>/,/<\/mgrwt>/d' $1 
fi
