/*****
* STM - Station de Travail Mon�tique
* API TPE - Communication Titus
* Couche bas niveau: ligne s�rie et messages Titus
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - F�vrier 1998
*****
* Titus.h
*****/

#ifndef __TITUS_H__
#define __TITUS_H__

/*****
* Classe CTitus
*   Note: Les timeouts sont exprim�s en millisecondes
*****/

#ifdef __cplusplus

class CTitus
{
// Construction/Destruction
public:
	CTitus();
	~CTitus();

// Operations
public:
	BOOL Open(LPCSTR lpszPortName);
	void Close();
	BOOL SendMessage(LPCSTR lpszMessage);
	BOOL ReadMessage(DWORD dwTimeout, LPSTR lpszBuffer, DWORD dwBufferSize);

// Implementation
protected:
	BOOL ParsePortSettings(LPCSTR lpszPortSettings, LPDCB lpDCB);
	void PurgeAndClearError();
	void ClearError();
	BOOL SetReadTimeout(DWORD dwTimeout = 0);
	BOOL SendChar(char cChar)
		{ return SendBuffer(&cChar, 1); }
	BOOL SendBuffer(LPCVOID lpBuffer, DWORD dwLenBuffer);
	BOOL ReadChar(LPCH lpcChar, LPBOOL lpbTimeoutError = NULL);

protected:
	HANDLE	m_hTitus;
};

#endif

#endif /* __TITUS_H__ */
