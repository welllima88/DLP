// Le bloc ifdef suivant est la fa�on standard de cr�er des macros qui facilitent l'exportation 
// � partir d'une DLL. Tous les fichiers contenus dans cette DLL sont compil�s avec le symbole ENCODECO_EXPORTS
// d�fini sur la ligne de commande. Ce symbole ne doit pas �tre d�fini dans les projets
// qui utilisent cette DLL. De cette mani�re, les autres projets dont les fichiers sources comprennent ce fichier consid�rent les fonctions 
// ENCODECO_API comme �tant import�es � partir d'une DLL, tandis que cette DLL consid�re les symboles
// d�finis avec cette macro comme �tant export�s.
#ifdef ENCODECO_EXPORTS
#define ENCODECO_API __declspec(dllexport)
#else
#define ENCODECO_API __declspec(dllimport)
#endif

// Cette classe est export�e de EncoDeco.dll
class ENCODECO_API CEncoDeco {
public:
	CEncoDeco(void);
	// TODO�: ajoutez ici vos m�thodes.
	void Encode(void);
	void Decode(void);
};

extern ENCODECO_API int nEncoDeco;

ENCODECO_API int fnEncoDeco(void);
