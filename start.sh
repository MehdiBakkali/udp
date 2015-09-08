#!/bin/bash

#Nettoyage 
echo Nettoyage
rm *.o
rm serveur 
rm client

#Compilation 
echo Compilation
gcc client.c -o client 
gcc serveur.c -o serveur

#lancement 

gnome-terminal -e "./serveur 5500"
./client 127.0.0.1 5501 127.0.0.2 5000


