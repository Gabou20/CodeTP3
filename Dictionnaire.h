/**
 * \file Dictionnaire.h
 * \brief Ce fichier contient l'interface d'un dictionnaire.
 * \author IFT-2008, Étudiant(e)
 * \version 0.1
 * \date juillet 2018
 *
 */


#ifndef DICO_H_
#define DICO_H_

#include <iostream>
#include <fstream> // pour les fichiers
#include <string>
#include <vector>
#include <queue>

namespace TP3
{

/**
 * \typedef enum SensDebalancement
 * \brief Ce type est utilise pour identifier la rotation a effectuer
 */
typedef enum
{
    GG, DD, GD, DG
} SensDebalancement;


/**
 * \typedef paireSimilitude
 * \brief Ce type est utilise pour stocker les mots et leur indice de similitude dans un vecteur lors de la recherche des meilleurs candidats pour remplacer un mot mal ecrit.
 */
typedef std::pair<double, std::string> PaireSimilitudeMot;


/**
 * \class Dictionnaire
 * \brief Classe utilisee pour representer un dictionnaire de mots anglais et leurs traductions en francais. Chaque dictionnaire est constitue d'un arbre AVL, implemente par chainage.
 */
class Dictionnaire
{
public:

	Dictionnaire();

	Dictionnaire(std::ifstream &p_fichier);

	~Dictionnaire();

	void ajouteMot(const std::string& p_motOriginal, const std::string& p_motTraduit);

	void supprimeMot(const std::string& p_motOriginal);

	double similitude(const std::string& p_mot1, const std::string& p_mot2);

	std::vector<std::string> suggereCorrections(const std::string& p_motMalEcrit);

	std::vector<std::string> traduit(const std::string& p_mot);

	bool appartient(const std::string &p_data);

	bool estVide() const;

    friend std::ostream& operator<<(std::ostream& out, const Dictionnaire& d)
    {
  	  if (d.m_racine != 0)
  	  {
  		  std::queue<NoeudDictionnaire*> file;
  		  std::queue<std::string> fileNiveau;

  		  NoeudDictionnaire * noeudDicotemp;
  		  std::string niveauTemp;

  		  int hg = 0, hd = 0;

  		  file.push(d.m_racine);
  		  fileNiveau.push("1");
  		  while (!file.empty())
  		  {
  			  noeudDicotemp = file.front();
  			  niveauTemp = fileNiveau.front();
  			  out << noeudDicotemp->m_mot;
  			  if (noeudDicotemp->m_gauche == 0) hg = -1; else hg = noeudDicotemp->m_gauche->m_hauteur;
  			  if (noeudDicotemp->m_droite == 0) hd = -1; else hd = noeudDicotemp->m_droite->m_hauteur;
  			  out << ", " << hg - hd;
  			  out << ", " << niveauTemp;
  			  out << std::endl;
  			  file.pop();
  			  fileNiveau.pop();
  			  if (noeudDicotemp->m_gauche != 0)
  			  {
  				  file.push(noeudDicotemp->m_gauche);
  				  fileNiveau.push(niveauTemp + ".1");
  			  }
  			  if (noeudDicotemp->m_droite != 0)
  			  {
  				  file.push(noeudDicotemp->m_droite);
  				  fileNiveau.push(niveauTemp + ".2");
  			  }
  		  }
  	  }
  	  return out;
    }


private:

	// Classe interne représentant un noeud dans l'arbre AVL constituant le dictionnaire de traduction.
	class NoeudDictionnaire
	{
	public:

		std::string m_mot;						// Un mot (en anglais)

		std::vector<std::string> m_traductions;	// Les différentes traductions possibles en français du mot en anglais
												// Par exemple, la liste française { "contempler", "envisager" et "prévoir" }
												// pourrait servir de traduction du mot anglais "contemplate".

	    NoeudDictionnaire *m_gauche, *m_droite;		// Les enfants du noeud

	    int m_hauteur;							// La hauteur de ce noeud (afin de maintenir l'équilibre de l'arbre AVL)

		// Vous pouvez ajouter ici un contructeur de NoeudDictionnaire
		NoeudDictionnaire(std::string p_mot, NoeudDictionnaire * p_gauche, NoeudDictionnaire * p_droite, int p_hauteur) :
                m_mot(p_mot),
                m_traductions(0),
                m_gauche(p_gauche),
                m_droite(p_droite),
                m_hauteur(p_hauteur)
        {};
	};
    
	NoeudDictionnaire *m_racine;		// La racine de l'arbre des mots

    int m_cpt;						// Le nombre de mots dans le dictionnaire

	NoeudDictionnaire * _trouverNoeud(NoeudDictionnaire* p_noeud, const std::string &p_mot) const;

    bool estBalance();

    NoeudDictionnaire ** _trouverDebalancement(NoeudDictionnaire* &p_noeud) const;

    void _auxAjouteMot(NoeudDictionnaire* &p_noeud, const std::string &p_mot, const std::string &p_traduction);

    void _auxSupprimeMot(NoeudDictionnaire * &p_noeud, const std::string &p_mot);

    void _balancerDictionnaire(NoeudDictionnaire* &p_noeud);

    int trouverHauteurGauche(const NoeudDictionnaire* p_noeud) const;

    int trouverHauteurDroite(const NoeudDictionnaire* p_noeud) const;

    void ajusterHauteur(NoeudDictionnaire* &p_noeud);

    void zigZigGauche(NoeudDictionnaire* &p_noeud);

    void zigZigDroite(NoeudDictionnaire* &p_noeud);

    void zigZagGauche(NoeudDictionnaire* &p_noeud);

    void zigZagDroite(NoeudDictionnaire* &p_noeud);

    bool _contient(const std::string &p_mot, const std::vector<std::string> &p_liste) const;

    SensDebalancement sensDebalancement(NoeudDictionnaire* p_noeud);

    void _auxCalculerSimilitudes(const std::string &p_motMalEcrit, const NoeudDictionnaire * p_noeud, std::vector<PaireSimilitudeMot> * p_motsSimilitudes);

    void _auxDestructeur(NoeudDictionnaire * p_noeud);
};
    
}

#endif /* DICO_H_ */
