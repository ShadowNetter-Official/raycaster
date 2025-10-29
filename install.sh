#!/bin/bash

echo
echo "raytracer"
echo "by ShadowNetter"
echo
echo "cloning into repo..."
git clone https://github.com/ShadowNetter-Official/raytracer
cd raytracer
echo "done"
echo "installing..."
cd src
gcc -o raytracer main.c $(sdl2-config --cflags --libs) cs50.c -lcrypt -lm
sudo cp raytracer /bin/
echo
echo "done"
echo
echo "to uninstall do: "
echo "sudo rm /bin/raycaster"
echo
