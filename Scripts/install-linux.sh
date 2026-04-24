#!/bin/sh

if [ "$(id -u)" -ne 0 ]; then
  echo "Err: This script needs root privileges to correctly grant permissions and install program files to the system. Please, either use 'su root' to elevate privileges and run this script again or use 'sudo $0' to invoke this script as root."
  exit 1
fi

cd Install/Linux/
echo "Step: Copying needed files..."
cp -av . /
cd ../../
echo "Step: Applying changes..."
udevadm control --reload-rules && udevadm trigger

echo "Ok: All done!"
