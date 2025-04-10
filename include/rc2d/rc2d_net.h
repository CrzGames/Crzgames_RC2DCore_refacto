#ifndef RC2D_NET_H
#define RC2D_NET_H

#ifndef __EMSCRIPTEN__

#ifndef RCENET_ENET_H
#include <rcenet/enet.h>
#endif

#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure représentant le contexte pour un client spécifique connecté au serveur.
 * Elle stocke les informations nécessaires à la gestion de la communication sécurisée avec le client.
 *
 * @typedef {struct} RC2D_NetServerContext
 * @property {ENetPeer*} peer - Pointeur vers le client ENet représentant la connexion réseau avec le client.
 */
typedef struct RC2D_NetServerContext {
    ENetPeer* clientPeer;
} RC2D_NetServerContext;

/**
 * Structure représentant un serveur ENet amélioré avec des capacités SSL/TLS pour une communication sécurisée.
 * Cette structure englobe les configurations et les états nécessaires à la gestion du serveur et des connexions clients sécurisées.
 *
 * @typedef {struct} RC2D_NetServer
 * @property {ENetHost*} host - Hôte pour le serveur, représentant le serveur ENet dans le réseau.
 * @property {RC2D_NetServerContext*} serverContext - Tableau dynamique pour la liste des clients, stockant un contexte par client connecté.
 */
typedef struct RC2D_NetServer {
    ENetHost* host;
    RC2D_NetServerContext* serverContext;
} RC2D_NetServer;

/**
 * Structure représentant un client ENet amélioré avec des capacités SSL/TLS pour une communication sécurisée avec un serveur.
 * Elle stocke les informations nécessaires à la gestion de la connexion client et à la communication sécurisée avec le serveur.
 *
 * @typedef {struct} RC2D_NetClient
 * @property {ENetHost*} host - Hôte pour le client, utilisé pour gérer la connexion réseau avec le serveur ENet.
 * @property {ENetPeer*} peer - Peer représentant la connexion au serveur, pour l'envoi et la réception de paquets réseau.
 */
typedef struct RC2D_NetClient {
    ENetHost* host;
    ENetPeer* serverPeer;
} RC2D_NetClient;

/**
 * Énumérations pour les types de paquets dans le réseau.
 * Ces types déterminent comment les paquets sont traités en termes de fiabilité et de séquencement.
 *
 * @typedef {enum} RC2D_NetPacketType
 * @property {RC2D_NET_PACKET_RELIABLE} - Paquet fiable nécessitant un accusé de réception.
 * @property {RC2D_NET_PACKET_UNSEQUENCED} - Paquet non séquencé sans garantie de livraison.
 * @property {RC2D_NET_PACKET_NO_ALLOCATE} - Paquet sans allocation dynamique, utilisant une mémoire tampon pré-allouée.
 * @property {RC2D_NET_PACKET_UNRELIABLE_FRAGMENT} - Fragment de paquet non fiable, permettant une plus grande taille de données au risque de perte.
 */
typedef enum RC2D_NetPacketType {
    RC2D_NET_PACKET_RELIABLE,
    RC2D_NET_PACKET_UNSEQUENCED,
    RC2D_NET_PACKET_NO_ALLOCATE,
    RC2D_NET_PACKET_UNRELIABLE_FRAGMENT
} RC2D_NetPacketType;

// Wrapper for RCENet
RC2D_NetServer* rc2d_net_createServer(const char* hostName, enet_uint16 port, size_t maxClients, size_t channelLimit, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth);
RC2D_NetClient* rc2d_net_createClientAndConnect(const char* hostName, enet_uint16 port, size_t channelCount, enet_uint32 data, size_t channelLimit, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth); 

void rc2d_net_disconnect(ENetPeer* peer);
void rc2d_net_forceDisconnect(ENetPeer* peer);

void rc2d_net_destroyClientOrServer(ENetHost* host);

ENetPacket* rc2d_net_createPacket(const void* data, size_t dataSize, RC2D_NetPacketType packetType);
int rc2d_net_sendPacket(ENetHost* host, ENetPeer* peer, ENetPacket* packet, enet_uint8 channelID, bool broadcast, bool immediate);
ENetPacket* rc2d_net_resizePacket(ENetPacket* packet, size_t dataSize);
void rc2d_net_destroyPacket(ENetPacket* packet);

#ifdef __cplusplus
}
#endif

#endif // __EMSCRIPTEN__
#endif // RC2D_NET_H