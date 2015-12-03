#!/bin/sh

# check for argument
if [ "$1" == "" ]
then
    echo "ERROR: expecting Omega hex code as argument!"
    echo "$0 <hex code>"
    exit
fi

localPath="rpcd/i2c-exp.sh"
remotePath="/usr/libexec/rpcd/i2c_exp"

cmd="rsync -va --progress $localPath root@omega-$1.local:$remotePath"
echo "$cmd"
eval "$cmd"

