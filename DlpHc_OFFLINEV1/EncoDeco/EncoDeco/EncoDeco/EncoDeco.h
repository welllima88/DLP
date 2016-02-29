// Le bloc ifdef suivant est la façon standard de créer des macros qui facilitent l'exportation 
// à partir d'une DLL. Tous les fichiers contenus dans cette DLL sont compilés avec le symbole ENCODECO_EXPORTS
// défini sur la ligne de commande. Ce symbole ne doit pas être défini dans les projets
// qui utilisent cette DLL. De cette manière, les autres projets dont les fichiers sources comprennent ce fichier considèrent les fonctions 
// ENCODECO_API comme étant importées à partir d'une DLL, tandis que cette DLL considère les symboles
// définis avec cette macro comme étant exportés.
#ifdef ENCODECO_EXPORTS
#define ENCODECO_API __declspec(dllexport)
#else
#define ENCODECO_API __declspec(dllimport)
#endif

// Cette classe est exportée de EncoDeco.dll
class ENCODECO_API CEncoDeco {
public:
	CEncoDeco(void);
	// TODO : ajoutez ici vos méthodes.
	void Encode(void);
	void Decode(void);
};

extern ENCODECO_API int nEncoDeco;

ENCODECO_API int fnEncoDeco(void);
