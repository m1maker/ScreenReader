#
# This file is part of the Screen Reader project.
#
# Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

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
