#include <mictcp.h>
#include <api/mictcp_core.h>
#include <limits.h>
#define MAXLOSS 10


// tableau de 1 socket destination et 1 socket source??
int id = 0;
mic_tcp_sock tabSocket[2];

// les seq
int PA,PE;

// compteur pour transmission rate
int compteur = 0;
int nbTransmission = 0;


/* On peut utiliser une liste circulaire pour faire un fenetrage continu ?????? sans segmenter
// Liste circulaire pour le fenetrage
typedef struct Liste_circulaire {
    char success;
    Liste_circulaire * suiv;
}Liste_circulaire;

typedef struct Fenetre {
    int countFailure;
    Liste_circulaire * debut;
}Fenetre;

// Declaration variable globale pour le fenetrage
Fenetre * debut;


// Fonctions pour le fenetrage

// Initialisation fenetre de taille 100
int init_fenetre(Fenetre * pf){
    pf->debut = (Liste_circulaire *)malloc(sizeof(Liste_circulaire));
    Liste_circulaire * suiveur = pf->debut;
    // Creation de 100 cellules
    for (int i = 0 ; i<99;i++){
        suiveur->succes = 1;
        suiveur->suiv = (Liste_circulaire *)malloc(sizeof(Liste_circulaire));
        suiveur = suiveur->suiv;
    };
    free(suiveur);
    return 0;
};

// Retour du compte d'echec de transmission
int moyenne(Fenetre *pf){
    Liste_circulaire * suiveur = pf->debut;
    if(suiveur->success == 0){
                    IP_send(pdu,addresse);pf->countFailure += 1; 
    }
    suiveur = suiveur->suiv;
    while (suiveur != pf->debut){
        if(suiveur->success == 0){
            pf->countFailure += 1; 
        };
        suiveur = suiveur->suiv;
    }
    free(suiveur);
    return pf->countFailure;
};

// Ajout etat transmission
int update(Liste_circulaire * suiveur, int resultat){
    suiveur-            IP_send(pdu,addresse);

// Libération de la memoire
int delete(Fenetre * pf){
    Lsite_circulaire * suiveur = pf->debut;
    Liste_circulaire * toDel = suiveur;
    while(suiveur != pf->debut){
        toDel  = suiveur;
        suiveur = suiveur->suiv;
        free(toDel)
    };
    free(suiveur);
    free(pf);
    return 0;
}
*
*/



/*
 * Permet de créer un socket entre l’application et MIC-TCP
 * Retourne le descripteur du socket ou bien -1 en cas d'erreur
 */
int mic_tcp_socket(start_mode sm)
{
   int result = -1;
   printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
   result = initialize_components(sm); /* Appel obligatoire */
   set_loss_rate(1);

    // On range le nouveau socket crée dans la  variable globale
    tabSocket[id].fd = id;
    result = id;
    id++;
    return result;
};


/*
 * Permet d’attribuer une adresse à un socket.
 * Retourne 0 si succès, et -1 en cas d’échec
 */
 // On suppose que la structure d'une adresse de socket est déjà initialisée avant le bind
int mic_tcp_bind(int socket, mic_tcp_sock_addr addr)
{
    int res = -1;
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    tabSocket[socket].addr = addr;
    res = 0;
    return res;
}

// Pas besoin de coder ceci
/*
 * Met le socket en état d'acceptation de connexions
 * Retourne 0 si succès, -1 si erreur
 */
int mic_tcp_accept(int socket, mic_tcp_sock_addr* addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    return 0;
}


/*
 * Permet de réclamer l’établissement d’une connexion
 * Retourne 0 si la connexion est établie, et -1 en cas d’échec
 */
// Permet seulement de communiquer l'adresse du socket distant
int mic_tcp_connect(int socket, mic_tcp_sock_addr addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    return 0;
}

// Askip on utilise pas le champs source on le laisse tel quel
/*
 * Permet de réclamer l’envoi d’une donnée applicative
 * Retourne la taille des données envoyées, et -1 en cas d'erreur
 */
int mic_tcp_send (int mic_sock, char* mesg, int mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    mic_tcp_pdu pdu;
    mic_tcp_sock_addr addresse = tabSocket[mic_sock].addr;
    int size;

    // On suppose mic_sock est destination
 
    // modification header
    //pdu.header.source_port = htons(src.addr.port);
    pdu.header.dest_port = addresse.port;
    pdu.header.seq_num = PE;
    pdu.header.ack = 0;
    pdu.header.ack_num =0;
    pdu.header.syn = 0;
    pdu.header.fin = 0;

    pdu.payload.data = mesg;
    pdu.payload.size = mesg_size;
    size = IP_send(pdu,addresse);
    PE = (PE++)%2;

    mic_tcp_pdu ack;
    mic_tcp_sock_addr addr;
    int recvResult;

    // Tant que on reçoit pas l'ACK avec le bon on force pour la v2
    // On accepte la non reception d'ack ou du mauvais ack si perte acceptable
    while(0){
        recvResult = IP_recv(&ack,&addr,50);
        nbTransmission = (++nbTransmission)%100;
        compteur = nbTransmission == 0 ? 0 : compteur;
        if (recvResult>0){
            if(ack.header.ack == 1 && ack.header.ack_num == PE){
                // sucess
                break;
            }else{
                // mauvais ACK num
                compteur++;
                if(compteur<MAXLOSS){
                    break;
                }else{
                    IP_send(pdu,addresse);
                    compteur = 0; // OU compteur--;
                };
            };
        }else{
            // timer expiré
            compteur++;
            if(compteur<MAXLOSS){
                break;
            }else{
                IP_send(pdu,addresse);
                compteur = 0; // OU compteur--;
            };
        };
    };
    return size;
}

/*
 * Permet à l’application réceptrice de réclamer la récupération d’une donnée
 * stockée dans les buffers de réception du socket
 * Retourne le nombre d’octets lu ou bien -1 en cas d’erreur
 * NB : cette fonction fait appel à la fonction app_buffer_get()
 */
 // On trouve dans le buffer la payload
int mic_tcp_recv (int socket, char* mesg, int max_mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    
    mic_tcp_payload payload;
    // Je ne comprends pas pq on fait ca et pas l'inverse
    payload.data = mesg;
    payload.size = max_mesg_size;
    int effective_size;
    effective_size = app_buffer_get(payload);
    // why not here?
    // mesg = payload.data;
    if (effective_size < 0){
        return -1;
    }else{
        return effective_size;
    };
}

/*
 * Permet de réclamer la destruction d’un socket.
 * Engendre la fermeture de la connexion suivant le modèle de TCP.
 * Retourne 0 si tout se passe bien et -1 en cas d'erreur
 */
int mic_tcp_close (int socket)
{
    printf("[MIC-TCP] Appel de la fonction :  "); printf(__FUNCTION__); printf("\n");
    return 0;
}

/*
 * Traitement d’un PDU MIC-TCP reçu (mise à jour des numéros de séquence
 * et d'acquittement, etc.) puis insère les données utiles du PDU dans
 * le buffer de réception du socket. Cette fonction utilise la fonction
 * app_buffer_put().
 */
void process_received_PDU(mic_tcp_pdu pdu, mic_tcp_sock_addr addr)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    mic_tcp_pdu ack;
    pdu.header.dest_port = addr.port;
    pdu.header.ack = 1;
    pdu.header.syn = 0;
    pdu.header.fin = 0;
    ack.payload.size = 0;
    if (pdu.header.seq_num == PA){
        app_buffer_put(pdu.payload);
        PA = (PA++)%2;
        ack.header.ack_num = PA;
        if(IP_send(ack,addr)<0){
            exit(-1);
        }
    }else{
        ack.header.ack_num = PA;
        if(IP_send(ack,addr)<0){
            exit(-1);
        }
    }
}
