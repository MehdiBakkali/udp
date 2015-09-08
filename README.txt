	########################################
	#     Programme Client/serveur UDP     #
	########################################


#Plusieurs clients peuvent se connecter au serveur (utilisation du fork)

#Pour lancer le script start.sh, il faut lui attribuer les droits 
chmod u+x script.sh

#Puis lancer le script avec 
./start.sh
Cela ouvre automatiquement une autre fenetre pour le serveur 

#Pour lancer un deuxième client (dans un nouveau terminal)
./client 127.0.0.1 5501 127.0.0.2 6666
6666 est le nouveau port de connexion 
