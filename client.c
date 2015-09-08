/*
 * CLIENT UDP
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>


int checkSYN_ACK(char str[64]){
	
	char begin[7]="SYN-ACK";
	int i;
	for(i=0;i<3;i++){
		if(begin[i]!=str[i]){
			return 0;
		}
	}
	return 1;
}


int main (int argc, char* argv []){
	
	printf("CLIENT \n\n");
	//Recuperation des arguments
	char* ip_client = argv[1];
	printf("Addresse client: %s \n",ip_client);
	int port_client = atoi(argv[2]);
	printf("Port du client: %d \n",port_client);
	char* ip_serveur = argv[3];
	printf("Adresse du serveur: %s \n", ip_serveur);
	int port_serveur = 5000;
	printf("Port de controle du seveur: %d\n\n",port_serveur);

	//Creation socket client
	int socket_client = socket(AF_INET,SOCK_DGRAM,0);
	if(socket_client == -1){
		printf("Erreur Creation socket UDP 1 \n");
		return -1;
	}

	/* Réutilisation des sockets si plantage : pas de blocage */
	int reuse= 1 ;
	setsockopt(socket_client, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	  
	//Initialisation du sockaddr_in client:
	struct sockaddr_in client_addr;
	memset((char*)&client_addr,0,sizeof(client_addr) );
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(port_client);
	inet_aton(ip_client,(struct in_addr*)&client_addr.sin_addr);

	//Initialisation du sockaddr_in serveur de controle:
	int port_serveur1;
	struct sockaddr_in serveur_addr;
	memset((char*)&client_addr,0,sizeof(serveur_addr) );
	serveur_addr.sin_family = AF_INET;
	serveur_addr.sin_port = htons(port_serveur);
	inet_aton(ip_serveur,(struct in_addr*)&serveur_addr.sin_addr);

	//Bind: fait le lien entre socket client er sockaddr client
    if( bind(socket_client,  (struct sockaddr*) &client_addr, sizeof(struct sockaddr_in ))== -1) {
		printf("erreur creation bind \n");
		close(socket_client); 
		return -2;
	}
  
	socklen_t taille = sizeof(serveur_addr);
	char SYN[64] = "SYN";
	char buffer[64] ={0};
	char ACK[64]="ACK";
	/** PROTOCOLE DEBUT DE CONNEXION **/
	
	//Envoie au serveur un message type SYN
	if ( sendto( socket_client, &SYN, sizeof(SYN), 0, (struct sockaddr*) &serveur_addr, taille ) == -1 ){
		printf("Erreur envoie message \n");
		close(socket_client);
		return -4;
	}
	printf("Message SYN envoye au serveur: %s\n",SYN);
	
	//Attente d'un segment SYN-ACK envoyé depuis le serveur:
	while( !checkSYN_ACK(buffer)){
		if(  recvfrom(socket_client, &buffer, sizeof(buffer), 0, (struct sockaddr*) &serveur_addr, &taille) == -1){
			printf("Erreur reception message \n");
			close(socket_client);
			return -3;
		}else{
			printf("Flag recu: %s \n",buffer);
		}
	}
	printf("Message SYN-ACK Recu \n");
	
	//Envoie au serveur un message type ACK:
	if ( sendto( socket_client, &ACK, sizeof(ACK), 0, (struct sockaddr*) &serveur_addr, taille ) == -1 ){
		printf("Erreur envoie message \n");
		close(socket_client);
		return -4;
	}
	printf("Message ACK envoye au serveur \n\n");
	
	//Attente du numero de port de communication
	if(  recvfrom(socket_client, &buffer, sizeof(buffer), 0, (struct sockaddr*) &serveur_addr, &taille) == -1){
		printf("Erreur reception message \n");
		close(socket_client);
		return -3;
	}else{
		printf("Numéro de port recu: %s \n",buffer);
		port_serveur1=atoi(buffer);
	}
	
		//Initialisation du sockaddr_in serveur de communication:
	struct sockaddr_in serveur1_addr;
	memset((char*)&client_addr,0,sizeof(serveur1_addr) );
	serveur1_addr.sin_family = AF_INET;
	serveur1_addr.sin_port = htons(port_serveur1);
	inet_aton(ip_serveur,(struct in_addr*)&serveur1_addr.sin_addr);
	
		
	/** FIN PROCOLOLE DE CONNEXION **/
	
	
		
	
	while(1){
		printf("Saisir un message à envoyer sur le port %d du serveur \n",port_serveur1);
		scanf("%s",&buffer); 
		
		
		if ( sendto( socket_client, &buffer, sizeof(buffer), 0, (struct sockaddr*) &serveur1_addr, taille ) == -1 ){
			printf("Erreur envoie message \n");
			close(socket_client);
			return -4;
		}else{
			printf("Message envoyes: %s \n",buffer);
		}
		sleep(0.5);
		if(  recvfrom(socket_client, &buffer, sizeof(buffer), 0, (struct sockaddr*) &serveur1_addr, &taille) == -1){
			printf("Erreur reception message \n");
			close(socket_client);
			return -3;
		}else{
			printf("Le client a recu: %s \n",buffer);
		}
	}
	
}
