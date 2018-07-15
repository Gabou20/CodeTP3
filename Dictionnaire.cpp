/**
 * \file Dictionnaire.cpp
 * \brief Ce fichier contient une implantation des méthodes de la classe Dictionnaire
 * \author IFT-2008, Étudiant(e)
 * \version 0.1
 * \date juillet 2018
 *
 */

#include "Dictionnaire.h"
#include <algorithm>

// Limite du nombre de suggestions
#define LIMITE_SUGGESTIONS 5

namespace TP3
{
//Constructeur
Dictionnaire::Dictionnaire() : m_racine(0), m_cpt(0) {}

//Destructeur.
Dictionnaire::~Dictionnaire() {} //Itere pour supprimer les noeuds

/**
 * \fn Dictionnaire::Dictionnaire(std::ifstream &fichier)
 *
 * \param[in] fichier Le fichier à partir duquel on construit le dictionnaire
 */
Dictionnaire::Dictionnaire(std::ifstream &fichier): m_racine(nullptr), m_cpt(0)
{
    if (fichier)
    {
        for( std::string ligneDico; getline( fichier, ligneDico); )
        {
            if (ligneDico[0] != '#') //Élimine les lignes d'en-tête
            {
                // Le mot anglais est avant la tabulation (\t).
                std::string motAnglais = ligneDico.substr(0,ligneDico.find_first_of('\t'));

                // Le reste (définition) est après la tabulation (\t).
                std::string motTraduit = ligneDico.substr(motAnglais.length()+1, ligneDico.length()-1);

                //On élimine tout ce qui est entre crochets [] (possibilité de 2 ou plus)
                std::size_t pos = motTraduit.find_first_of('[');
                while (pos!=std::string::npos)
                {
                    std::size_t longueur_crochet = motTraduit.find_first_of(']')-pos+1;
                    motTraduit.replace(pos, longueur_crochet, "");
                    pos = motTraduit.find_first_of('[');
                }

                //On élimine tout ce qui est entre deux parenthèses () (possibilité de 2 ou plus)
                pos = motTraduit.find_first_of('(');
                while (pos!=std::string::npos)
                {
                    std::size_t longueur_crochet = motTraduit.find_first_of(')')-pos+1;
                    motTraduit.replace(pos, longueur_crochet, "");
                    pos = motTraduit.find_first_of('(');
                }

                //Position d'un tilde, s'il y a lieu
                std::size_t posT = motTraduit.find_first_of('~');

                //Position d'un tilde, s'il y a lieu
                std::size_t posD = motTraduit.find_first_of(':');

                if (posD < posT)
                {
                    //Quand le ':' est avant le '~', le mot français précède le ':'
                    motTraduit = motTraduit.substr(0, posD);
                }

                else
                {
                    //Quand le ':' est après le '~', le mot français suit le ':'
                    if (posT < posD)
                    {
                        motTraduit = motTraduit.substr(posD, motTraduit.find_first_of("([,;\n", posD));
                    }
                    else
                    {
                        //Quand il n'y a ni ':' ni '~', on extrait simplement ce qu'il y a avant un caractère de limite
                        motTraduit = motTraduit.substr(0, motTraduit.find_first_of("([,;\n"));
                    }
                }

                //On ajoute le mot au dictionnaire
                ajouteMot(motAnglais, motTraduit);
                //std::cout<<motAnglais << " - " << motTraduit<<std::endl;
            }
        }
    }
}

//Ajouter un mot au dictionnaire et l'une de ses traductions en équilibrant l'arbre AVL
void Dictionnaire::ajouteMot(const std ::string& p_motOriginal, const std ::string& p_motTraduit) {}

//Supprimer un mot et équilibrer l'arbre AVL
//Si le mot appartient au dictionnaire, on l'enlève et on équilibre. Sinon, on ne fait rien.
//Exception	logic_error si l'arbre est vide
//Exception	logic_error si le mot n'appartient pas au dictionnaire
void Dictionnaire::supprimeMot(const std ::string& p_motOriginal) {}

//Quantifier la similitude entre 2 mots (dans le dictionnaire ou pas)
//Ici, 1 représente le fait que les 2 mots sont identiques, 0 représente le fait que les 2 mots sont complètements différents
//On retourne une valeur entre 0 et 1 quantifiant la similarité entre les 2 mots donnés
//Vous pouvez utiliser par exemple la distance de Levenshtein, mais ce n'est pas obligatoire !
double Dictionnaire::similitude(const std ::string& p_mot1, const std ::string& p_mot2)
{
    float differenceLongueurs = abs((int) p_mot1.length() - p_mot2.length());// La fonction compare les deux mots lettre a lettre, et additionne 1 au total pour chaque lettre non identique.
    float distanceDuDebut = differenceLongueurs;                             // De plus, a difference de longueur est consideree (+1 pour chaque lettre de difference).
    float distanceDeLaFin = differenceLongueurs;                             // Les deux mots sont compares du debut et de la fin, et le total le plus bas est conserve.

    if (p_mot1.length() != 0 && p_mot2.length() != 0)
    {
        for (int i = 0; i < p_mot1.length(); i++)
        {
            if (p_mot1[i] != p_mot2[i])
            {
                distanceDuDebut += 1;
            }
        }

        std::string mot1(p_mot1);
        std::reverse(mot1.begin(), mot1.begin() + mot1.length());
        std::string mot2(p_mot1);
        std::reverse(mot2.begin(), mot2.begin() + mot2.length());
        for (int i = 0; i < p_mot1.length(); i++)
        {
            if (p_mot1[i] != p_mot2[i])
            {
                distanceDeLaFin += 1;
            }
        }
    }

    float distance = std::min(distanceDuDebut, distanceDeLaFin);
    distance = (distance/std::max(p_mot1.length(), p_mot2.length()));

    if (distance == 0)                          // Ici la distance calculee est ramenee entre 0 et 1.
    {
        return 1;
    }
    else if (distance == 1)
    {
        return 0;
    }
    else
    {
        return 1 - distance;
    }

}

//Suggère des corrections pour le mot motMalEcrit sous forme d'une liste de mots, dans un vector, à partir du dictionnaire
//S'il y a suffisament de mots, on redonne 5 corrections possibles au mot donné. Sinon, on en donne le plus possible
//Exception	logic_error si le dictionnaire est vide
std::vector<std::string> Dictionnaire::suggereCorrections(const std ::string& p_motMalEcrit)
{

}

//Trouver les traductions possibles d'un mot
//Si le mot appartient au dictionnaire, on retourne le vecteur des traductions du mot donné.
//Sinon, on retourne un vecteur vide
std::vector<std::string> Dictionnaire::traduit(const std ::string& p_mot)
{
    const NoeudDictionnaire * recherche = trouverNoeud(m_racine, p_mot);

    if (recherche != nullptr)
    {
        return std::vector<std::string>(0);
    }

    return recherche -> m_traductions;
}

//Vérifier si le mot donné appartient au dictionnaire
//On retourne true si le mot est dans le dictionnaire. Sinon, on retourne false.
bool Dictionnaire::appartient(const std::string &p_data)
{
    if (this -> estVide())
    {
        return false;
    }

    return Dictionnaire::trouverNoeud(m_racine, p_data) != nullptr && !this -> estVide();
}

//Vérifier si le dictionnaire est vide
bool Dictionnaire::estVide() const
{
    return (m_racine == nullptr);
}

//retourne le meme noeud si p_noeud contient le bon mot, le bon noeud enfant sinon et nullptr si on a atteint une feuille
    //Le noeud en argument ne doit pas etre nullptr
const NoeudDictionnaire * Dictionnaire::trouverNoeud(const Dictionnaire::NoeudDictionnaire* p_noeud, const std::string &p_data) const
{
    if (p_noeud -> m_mot == p_data)
    {
        return p_noeud;
    }
    else if (p_data < p_noeud -> m_mot)
    {
        if (p_noeud -> m_gauche == nullptr)
        {
            return nullptr;
        }
        else
        {
            return Dictionnaire::trouverNoeud(p_noeud -> m_gauche, p_data);
        }
    }
    else if (p_data > p_noeud -> m_mot)
    {
        if (p_noeud -> m_droite == nullptr)
        {
            return nullptr;
        }
        else
        {
            return Dictionnaire::trouverNoeud(p_noeud -> m_droite, p_data);
        }
    }
}

	// Complétez ici l'implémentation des autres méthodes demandées ainsi que vos méthodes privées.
  
}//Fin du namespace
