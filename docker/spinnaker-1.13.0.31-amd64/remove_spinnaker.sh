#!/bin/bash

MAJ_VER=1

MY_PROMPT='$ '
MY_YESNO_PROMPT='(y/n)$ '

echo "This is a script to assist with the removal of the Spinnaker SDK.";
echo "Would you like to continue and remove all the Spinnaker packages?";
echo -n "$MY_YESNO_PROMPT"
read confirm

if [ $confirm != "y" ] && [ $confirm != "Y" ] && [ $confirm != "yes" ] && [ $confirm != "Yes" ]
then
    exit 0
fi

echo

echo "Removing Spinnaker packages...";

sudo dpkg -r spinnaker-doc
sudo dpkg -r spinnaker
sudo dpkg -r spinupdate$MAJ_VER-dev
sudo dpkg -r spinupdate$MAJ_VER
sudo dpkg -r libspinvideo-c$MAJ_VER-dev
sudo dpkg -r libspinvideo-c$MAJ_VER
sudo dpkg -r libspinvideo$MAJ_VER-dev
sudo dpkg -r libspinvideo$MAJ_VER
sudo dpkg -r libspinnaker-c$MAJ_VER-dev
sudo dpkg -r libspinnaker-c$MAJ_VER
sudo dpkg -r spinview-qt$MAJ_VER-dev
sudo dpkg -r spinview-qt$MAJ_VER
sudo dpkg -r libspinnaker$MAJ_VER-dev
sudo dpkg -r libspinnaker$MAJ_VER
sudo dpkg -r libspinvideoencoder$MAJ_VER

echo "Removing rules file";

if [ -e "/etc/udev/rules.d/40-flir-spinnaker.rules" ]
then
    sudo rm /etc/udev/rules.d/40-flir-spinnaker.rules
fi

echo "Complete";

exit 0
