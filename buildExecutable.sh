#/bin/bash

source /opt/exorintos-2.x.x/2.x.x/environment-setup-aarch64-poky-linux;CFLAGS="";LDFLAGS=""

$CC  batteryLevel.c -o batteryLevel

#echo "press Enter for scp to target, (or Ctrl-c)"; read dummy
sshpass -p "Exor123@" scp  batteryLevel admin@10.1.35.21:/mnt/data/batteryLevel
