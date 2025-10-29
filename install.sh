#!/bin/bash

echo
echo "raycaster"
echo "by ShadowNetter"
echo
echo "cloning into repo..."
git clone https://github.com/ShadowNetter-Official/raycaster
cd raycaster
echo "done"
echo "installing..."
cd src
gcc -o raycaster main.c $(sdl2-config --cflags --libs) cs50.c -lcrypt -lm
sudo cp raycaster /bin/
echo
echo "done"
echo
echo "to uninstall do: "
echo "sudo rm /bin/raycaster"
echo
