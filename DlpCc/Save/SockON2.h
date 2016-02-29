/*****
* STM - Station de Travail Monétique
* API ON2 - Communication avec le serveur monétique ON/2 sur le S/88
* Couche moyen niveau: messages ON/2 sur sockets TCP/IP
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
*****
* SockON2.h
*****/

#ifndef __SOCKON2_H__
#define __SOCKON2_H__

#include "Sock.h"

/*****
* Classe CSockON2
*   Note: Les timeouts sont exprimés en millisecondes
*****/

#ifdef __cplusplus

class CSockON2 : public CSock
{
// Construction/Destruction
public:
	CSockON2();
	~CSockON2();

// Operations
public:
	BOOL Send(LPCSTR lpData, DWORD dwLenData);
	BOOL Receive(DWORD dwTimeout, LPSTR lpData, DWORD dwLenData);

// Implementation
protected:
	BOOL Discard(DWORD dwLenData);

};

#endif

#endif /* __SOCKON2_H__ */
