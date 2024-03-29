/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>

void construire_message(char * message, char motif, int lg) {
	int i;
	for (i=0;i<lg;i++)
		message[i] = motif;
}

void afficher_message(char *message, int lg) {
	int i;
	printf("message construit : ");
	for (i=0;i<lg;i++)
		printf("%c", message[i]);
	printf("\n");
}

void source_udp(int port, char * host, int nb_message, int lg_M) { //PAS TOUCHE, CA MARCHE
	// Creation du socket local
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Echec de creation du socket \n");
		exit(1);
	}
	// Construction de l'adresse du socket destinataire
	struct hostent * hp;
	struct sockaddr_in adr_distant;
	int lg_adr_distant = sizeof(adr_distant);
	memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;
	adr_distant.sin_family = AF_INET ; // domaine Internet
	adr_distant.sin_port = port ; // n° de port

	if ((hp = gethostbyname(host)) == NULL) { 
		printf("erreur gethostbyname\n");
		exit(1);
	}
	memcpy((char*)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);
	
	// Emission du message au destinaire
	for (int i = 0; i < nb_message;i++) {
		// Creation message
		char * message = malloc(lg_M*sizeof(char));
		construire_message(message,(char)(i%26 + 97),lg_M);
		// printf("%c",(char)(i%26 + 97));
		// Envoi du message
		int temp;
		if (temp = sendto(sock, message, lg_M, 0, ((struct sockaddr *)&adr_distant), lg_adr_distant) == -1) {
			printf("Echec du sendto \n");
			exit(1);
		}
		free(message);
	}

	// Destruction du socket
	if (close(sock) == -1) {
		printf("Echec de destructon du socket \n");
		exit(1);
	}
	
}

void puit_udp(int port, int lg_M) { // PAS TOUCHE CA MARCHE DE OUF !!!!
	// Creation du socket local
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Echec de creation du socket \n");
		exit(1);
	}
	
	struct sockaddr_in adr_local ; // Adresse du socket local
	int lg_adr_local = sizeof(adr_local) ;

	// Construction de l’adresse du socket
	memset((char*)&adr_local, 0, sizeof(adr_local));
	adr_local.sin_family = AF_INET;
	adr_local.sin_port = htons(port);
	adr_local.sin_addr.s_addr = INADDR_ANY;

	// Association de l'adresse socket et représentation interne
	if (bind (sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1) {
		printf("échec du bind\n");
		exit(1);
	}

	// Réception du message
	char * pmesg = malloc(sizeof(char) * lg_M);
	int plg_adr_em = sizeof(struct sockaddr_in);
	struct sockaddr_in padr_em;

	while(1) {
		int temp;
		if ((temp = recvfrom(sock, pmesg, lg_M, 0, (struct sockaddr *)&padr_em, &plg_adr_em)) == -1) {
			printf("Echec de réception \n");
		}
		afficher_message(pmesg, lg_M);
	}

	// Destruction du socket
	if (close(sock) == -1) {
		printf("Echec de destructon du socket \n");
		exit(1);
	}
			
}

void source_tcp(int port, char * host, int nb_message, int lg_M) { // PAS TOUCHE CA MARCHE AUSSI !!! 
	// Creation du socket local
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Echec de creation du socket \n");
		exit(1);
	}
	// Construction de l'adresse du socket destinataire
	struct hostent * hp;
	struct sockaddr_in adr_distant;
	int lg_adr_distant = sizeof(adr_distant);
	memset((char *)& adr_distant, 0, sizeof(adr_distant)) ;
	adr_distant.sin_family = AF_INET ; // domaine Internet
	adr_distant.sin_port = htons(port) ; // n° de port

	if ((hp = gethostbyname(host)) == NULL) { 
		printf("erreur gethostbyname\n");
		exit(1);
	}
	memcpy((char*)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);

	// Demande de connexion
	if (connect(sock, ((struct sockaddr *)&adr_distant), lg_adr_distant) == -1) {
		printf("Echec de connexion \n");
		exit(1);
	}
	
	// Emission du message au destinaire
	for (int i = 0; i < nb_message;i++) {
		// Creation message
		char * message = malloc(lg_M*sizeof(char));
		construire_message(message,(char)(i%26 + 97),lg_M);
		// printf("%c",(char)(i%26 + 97));
		// Envoi du message
		int temp;
		if (temp = write(sock, message, lg_M) == -1) {
			printf("Echec du write \n");
			exit(1);
		}
		free(message);
	}

	// Destruction du socket
	if (close(sock) == -1) {
		printf("Echec de destructon du socket \n");
		exit(1);
	}

}

void puit_tcp(int port, int lg_M, int nb_max) {
	// Creation du socket local
	int sock, sock_bis;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Echec de creation du socket \n");
		exit(1);
	}
	
	struct sockaddr_in adr_local ; // Adresse du socket local
	int lg_adr_local = sizeof(adr_local) ;

	// Construction de l’adresse du socket
	memset((char*)&adr_local, 0, sizeof(adr_local));
	adr_local.sin_family = AF_INET;
	adr_local.sin_port = htons(port);
	adr_local.sin_addr.s_addr = INADDR_ANY;

	// Association de l'adresse socket et représentation interne
	if (bind (sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1) {
		printf("échec du bind\n");
		exit(1);
	}

	// Dimensionnement de la file de demandes de connexion avec la primitive listen
	if (listen(sock, nb_max) == -1) {
		printf("échec du listen\n");
		exit(1);
	}

	// Mise en état d’acceptation de demande de connexion avec la primitive accept
	int plg_adr_em = sizeof(struct sockaddr_in);
	struct sockaddr_in padr_em;
	if (sock_bis = accept(sock, ((struct sockaddr *)&padr_em), &plg_adr_em) == -1) {
		printf("échec d'acceptation\n");
		exit(1);
	}

	// Réception du message
	char * pmesg = malloc(sizeof(char) * lg_M);
	while(1) {
		int temp;
		printf("COUCOU");
		if ((temp = read(sock_bis, pmesg, lg_M)) == -1) {
			printf("Echec de réception \n");
		}
		afficher_message(pmesg, lg_M);
	}

	// Destruction du socket
	if (close(sock) == -1) {
		printf("Echec de destructon du socket \n");
		exit(1);
	}
	if (close(sock_bis) == -1) {
		printf("Echec de destructon du socket \n");
		exit(1);
	}
	
		
}

int main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int protocol = 1; // 0 = UDP, 1 = TCP
	int port = htons(atoi(argv[argc - 1]));
	char * host = argv[argc - 2];
	int lg_M = 30;
	while ((c = getopt(argc, argv, "upn:s")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-u][-n ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-u][-n ##]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;
		
		case 'u':
			protocol = 0;
			break;
		
		default:
			printf("usage: cmd [-p|-s][-u][-n ##]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-u][-n ##]\n");
		exit(1) ;
	}

	if (source == 1) {
		printf("on est dans la source\n");
		nb_message = 10;
		switch (protocol) {
			case  0:
				source_udp(port, host, nb_message, lg_M);
				break;
			case 1:
				printf("COUCOU2");
				source_tcp(port, host, nb_message, lg_M);
				break;
				
			default:
				printf("Coucou");
				break;
		}
	} else {
		printf("on est dans le puit\n");
		nb_message = 30;
		switch (protocol) {
			case 0:
				puit_udp(port, lg_M);
				break;

			case 1:
				puit_tcp(port, lg_M, nb_message);
				break;
				
			default:
				printf("Coucou");
				break;
		}
	}

	// A quoi sert cette section ???? 
	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");
	}
	return 0;
}

