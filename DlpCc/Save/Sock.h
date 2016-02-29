/*****
* STM - Station de Travail Monétique
* API ON2 - Communication avec le serveur monétique ON/2 sur le S/88
* Couche bas niveau: communication sockets TCP/IP
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
*****
* Sock.h
*****/

#ifndef __SOCK_H__
#define __SOCK_H__

#include <winsock.h>

/*****
* Classe CSock
*   Note: Les timeouts sont exprimés en millisecondes
*****/

#ifdef __cplusplus

class CSock
{
// Construction/Destruction
public:
	CSock();
	~CSock();

// Operations
public:
	BOOL Open(LPCSTR lpszRemoteAddress, WORD wRemotePort);
	BOOL Close();
	BOOL Abort();
	BOOL Send(LPCSTR lpData, DWORD dwLenData);
	BOOL Receive(DWORD dwTimeout, LPSTR lpData, DWORD dwLenData);

// Implementation
protected:
	static void Startup();
	static void Cleanup();

protected:
	SOCKET	m_hSocket;

protected:
	static int	m_nInstances;
	static BOOL	m_bStartedUp;
};

#endif

#endif /* __SOCK_H__ */
