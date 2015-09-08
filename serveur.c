/*
 * SERVEUR UDP
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>


int checkSYN(char str[64]){
	
	char begin[3]="SYN";
	int i;
	for(i=0;i<3;i++){
		if(begin[i]!=str[i]){
			return 0;
		}
	}
	return 1;
}

int checkACK(char str[64]){
	
	char begin[3]="ACK";
	int i;
	for(i=0;i<3;i++){
		if(begin[i]!=str[i]){
			return 0;
		}
	}
	return 1;
}




int main (int argc, char* argv []){

	printf("SERVEUR \n\n");
	
	
	int port_serveur_controle = 5000;
	printf("Port de controle serveur: %d\n",port_serveur_controle);	
	int port_serveur1 = 5500;


	/* Création des sockets */
		//Socket de controle
	int socket_serveur_controle = socket(AF_INET,SOCK_DGRAM,0);
	if(socket_serveur_controle == -1){
		printf("Erreur Creation socket UDP \n");
		return -1;
	}
		//Socket de data
	int socket_serveur1 = socket(AF_INET,SOCK_DGRAM,0);
	if(socket_serveur1 == -1){
		printf("Erreur Creation socket UDP \n");
		return -1;
	}


	/* Réutilisation des sockets si plantage : pas de blocage */
	int reuse_controle= 1 ;
	setsockopt(socket_serveur_controle, SOL_SOCKET, SO_REUSEADDR, &reuse_controle, sizeof(reuse_controle));
	int reuse1= 1 ;
	setsockopt(socket_serveur1, SOL_SOCKET, SO_REUSEADDR, &reuse1, sizeof(reuse1));

	// Initialisation des adresses du serveur et du client:
	struct sockaddr_in serveur_addr_controle;
	struct sockaddr_in client_addr1;
	struct sockaddr_in serveur1_addr;

    //Remise a zero des adresses
	memset((char*)&serveur_addr_controle, 0, sizeof(serveur_addr_controle));
	memset((char*)&client_addr1, 0, sizeof(client_addr1));
	memset((char*)&serveur1_addr, 0, sizeof(serveur1_addr));

    //Remplissage des champs:
		//Adresse controle
	serveur_addr_controle.sin_family=AF_INET;
	serveur_addr_controle.sin_port=htons(port_serveur_controle);
	serveur_addr_controle.sin_addr.s_addr= htonl(INADDR_ANY);
		//Adresse data
	serveur1_addr.sin_family=AF_INET;
	serveur1_addr.sin_addr.s_addr= htonl(INADDR_ANY);


  //Utilisation de la fonction bind: lien entre socket serveur et sockaddr serveur
	if( bind(socket_serveur_controle,  (struct sockaddr*) &serveur_addr_controle, sizeof(struct sockaddr_in )) == -1) {
		printf("erreur creation bind serveur\n");
		close(socket_serveur_controle); 
		return -2;
	}
	socklen_t taille= sizeof(client_addr1);	
	char buffer[2048] = {0};
	char SYN_ACK[64] = "SYN-ACK";
	/** PROTOCOLE DE DEBUT DE CONNECTION **/

	//BOUCLE INFINIE D'ECOUTE:
	
	while(1){

			//Attente d'un segment depuis le client contenant SYN:
			while( !checkSYN(buffer) ){
				if(  recvfrom(socket_serveur_controle, &buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr1, &taille) == -1){
					printf("Erreur reception message \n");
					close(socket_serveur_controle);
					return -3;
				}else{
					printf("Message recu: %s \n",buffer);
				}
			}
			
			printf("Message SYN Recu \n");
			
			//Envoi vers le client d'un message SYN-ACK:
			if ( sendto( socket_serveur_controle, &SYN_ACK, sizeof(SYN_ACK), 0, (struct sockaddr*) &client_addr1, taille ) == -1 ){
				printf("Erreur envoie message \n");
				close(socket_serveur_controle);
				return -4;
			}
			printf("Message SYN-ACK envoye au client\n");
			
			//Attente d'un segment depuis le client contenant ACK:
			while( !checkACK(buffer) ){
							
				if(  recvfrom(socket_serveur_controle, &buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr1, &taille) == -1){
					printf("Erreur reception message \n");
					close(socket_serveur_controle);
					return -3;
				}
			}
			printf("Message ACK Recu \n\n");
			
			//Incrémentation du port au niveau du serveur. Le numéro du port est (5500 + Numéro client)
			port_serveur1 ++;
			
			/******* CODE CRITIQUE: CETTE PARTIE DU CODE NE DOIT ETRE PAS ETRE EXECUTE POUR LE PREMIER CLIENT	*******/
			
			if(port_serveur1 != 5501){
				socket_serveur1 = socket(AF_INET,SOCK_DGRAM,0);
				if(socket_serveur1 == -1){
					printf("Erreur Creation socket UDP \n");
					return -1;
				}
			}
			/****** FIN CODE CRITIQUE *******/

			//Attribution d'un nouveau numero de port coté serveur:
			serveur1_addr.sin_port=htons(port_serveur1);

			
			//Envoi vers le client du numero de port de communication:
			sprintf(buffer, "%d",port_serveur1);
			if (sendto( socket_serveur_controle, &buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr1, taille ) == -1){
				printf("Erreur envoie message \n");
				close(socket_serveur_controle);
				return -4;
			}
			printf("Numero du port de communication %d envoye au client\n",port_serveur1);
			
			int a = fork();
			
			if( a>0){
				// Prossesus Pere
				sleep(0.5);
				close(socket_serveur1);
				printf("socket fermee dans parent \n\n");
			}else{
				// Processus Fils
				
				/** FIN PROTOCOLE DEBUT DE CONNEXION **/
	
				//Fermeture de la socket de connexion
				close(socket_serveur_controle);
				//Utilisation de la fonction bind: lien entre socket serveur de communication et sockaddr serveur de communication
				if( bind(socket_serveur1,  (struct sockaddr*) &serveur1_addr, sizeof(struct sockaddr_in )) == -1) {
					printf("erreur creation bind serveur data\n");
					printf("%d \n",ntohs(serveur1_addr.sin_port));
					close(socket_serveur1); 
					return -2;
				}
				
				printf("Connexion Etablie sur le port %d du serveur\n",port_serveur1);
		
				
				
				/** DEBUT COMMUNICATION **/
				
					
				while(1){
					
					if(  recvfrom(socket_serveur1, &buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr1, &taille) == -1){
						printf("Erreur reception message \n");
						close(socket_serveur1);
						return -3;
					}else{
						printf("Le serveur a recu: %s \n",buffer);
					}
					sleep(0.5);
					if ( sendto( socket_serveur1, &buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr1, taille ) == -1 ){
						printf("Erreur envoie message \n");
						close(socket_serveur1);
						return -4;
					}else{
						printf("Message envoyes: %s \n",buffer);
					}
				}
				
			}
		
		
			
	}
	

}





