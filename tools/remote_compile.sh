#!/bin/sh

## define the remote server and package
server="lazar@openwrt2.onion.io"
remotePath="/home/lazar/OpenWRT-Buildroot/openwrt/dl"
package="i2c-exp-driver"

## upload project to openwrt2.onion.io 
localPath="../$package"


cmd="rsync -va --progress $localPath $server:$remotePath"
echo "$cmd"
eval "$cmd"


## create a tar from the file, run the compile
cmd="ssh $server \"cd $remotePath && tar -zcvf $package.tar.gz $package && cd .. && make package/feeds/onion/$package/compile V=99\""
echo "$cmd"
eval "$cmd"

