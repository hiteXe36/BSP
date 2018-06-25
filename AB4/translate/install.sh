#!/bin/sh
devname="trans"
drvname="translate"
echo "Removing old module"
sudo rm /dev/${devname}0
sudo rm /dev/${devname}1
sudo rmmod ${drvname}.ko
echo "Rebuild Translate"
make clean
make
echo "Installing new module"
sudo insmod ${drvname}.ko
echo "Lookup majornumber"
major=$(grep "${devname}" /proc/devices | cut -d" " -f1)
echo "Majornumber: $major"
echo "Creating node"
sudo mknod /dev/${devname}0 c $major 0
sudo chmod 666 /dev/${devname}0
sudo mknod /dev/${devname}1 c $major 1
sudo chmod 666 /dev/${devname}1
