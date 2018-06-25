
devname=trans
drvname=translate
echo ------alten entfernen
sudo rm /dev/${devname}0
sudo rm /dev/${devname}1
sudo rmmod ${drvname}.ko

echo -------rebuilden
make clean
make

echo -------insmod
sudo insmod ${drvname}.ko  #translate_bufsize=50 translate_shift=6

echo -------major finden
major=$(grep "${devname}"  /proc/devices | cut -d" " -f1)
echo $major

echo -----create node
sudo mknod /dev/${devname}0 c $major 0
sudo chmod 666 /dev/${devname}0
sudo mknod /dev/${devname}1 c $major 1
sudo chmod 666 /dev/${devname}1
