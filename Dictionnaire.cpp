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
Dictionnaire::Dictionnaire() : m_racine(nullptr), m_cpt(0) {}

//Destructeur.
Dictionnaire::~Dictionnaire()
{
    _auxDestructeur(m_racine);
}


/**
 * \fn Dictionnaire::Dictionnaire(std::ifstream &fichier)
 *
 * \pre Le fichier doit etre ouvert au prealable
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


/**
 * \fn void Dictionnaire::ajouteMot(const std::string& p_motOriginal, const std ::string& p_motTraduit)
 *
 * \brief Ajoute un mot ou une nouvelle traduction au dictionnaire
 *
 * \param[in] p_motOriginal Le mot en anglais a ajouter
 * \param[in] p_motTraduit Un mot en francais representant une traduction possible de p_motOriginal
 *
 * \post Le mot appartient maintenant au dictionnaire : si le mot anglais n'y etait pas deja, un nouveau noeud est cree. S'il y etait, le mot francais est ajoute a la liste de traductions possibles s'il n'y etait pas deja.
 */
void Dictionnaire::ajouteMot(const std::string& p_motOriginal, const std ::string& p_motTraduit)
{
    _auxAjouteMot(m_racine, p_motOriginal, p_motTraduit);
}


/**
 * \fn void Dictionnaire::supprimeMot(const std ::string& p_motOriginal)
 *
 * \brief Si le mot appartient au dictionnaire, on l'enlève et on équilibre. Sinon, on ne fait rien.
 *
 * \param[in] p_motOriginal Le mot en anglais a supprimer
 *
 * \exception logic_error si l'arbre est vide ou si le mot a supprimer ne s'y trouve pas
 *
 * \post Le mot ne se trouve plus dans l'arbre du dictionnaire
 */
void Dictionnaire::supprimeMot(const std ::string& p_motOriginal)
{
    if (estVide())
    {
        throw std::logic_error("Le dictionnaire est vide!");
    }

    _auxSupprimeMot(m_racine, p_motOriginal);
}


/**
 * \fn double Dictionnaire::similitude(const std ::string& p_mot1, const std ::string& p_mot2)
 *
 * \brief Quantifie la similitude entre 2 mots donnes en parametre
 *
 * \param[in] p_mot1 L'un des mots a comparer
 * \param[in] p_mot2 L'autre mot a comparer
 *
 * \return Un reel entre 0 et 1 quantifiant la similarité entre les 2 mots donnés. 1 représente le fait que les 2 mots sont identiques, 0 représente le fait que les 2 mots sont complètements différents
 */
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


/**
 * \fn std::vector<std::string> Dictionnaire::suggereCorrections(const std ::string& p_motMalEcrit)
 *
 * \brief Trouve les 5 meilleures suggestions de remplacement pour un mot introuvable dans le dictionnaire.
 *
 * \param[in] p_motMalEcrit Le mot pour lequel on doit trouver des suggestions
 *
 * \exception logic_error si le dictionnaire est vide.
 *
 * \return Un vecteur des 5 meilleures propositions selon la fonction similitude. S'il y a des egalites, les mots sont choisis par ordre alphabetique.
 */
std::vector<std::string> Dictionnaire::suggereCorrections(const std ::string& p_motMalEcrit)
{
    if (estVide())
    {
        throw std::logic_error("Le dictionnaire est vide!");
    }

    std::vector<paireSimilitudeMot> motsSimilitudes;
    _auxCalculerSimilitudes(p_motMalEcrit, m_racine, &motsSimilitudes);

    std::sort(motsSimilitudes.begin(), motsSimilitudes.end());

    std::vector<std::string> choixMots;
    int n = 0;
    std::vector<paireSimilitudeMot>::iterator it = motsSimilitudes.end();
    it--;
    for (it; n < 5; it--)
    {
        choixMots.push_back(it -> second);
        n++;
    }
    return choixMots;
}


/**
 * \fn std::vector<std::string> Dictionnaire::traduit(const std ::string& p_mot)
 *
 * \brief Trouve le mot anglais dans le dictionnaire et en donne les traductions francaises
 *
 * \param[in] p_mot Le mot a traduire
 *
 * \return Le vecteur contenant les traductions francaises de p_mot, un vecteur vide si le mot n'a pas ete trouve.
 */
std::vector<std::string> Dictionnaire::traduit(const std ::string& p_mot)
{
    const NoeudDictionnaire * recherche = _trouverNoeud(m_racine, p_mot);

    if (recherche == nullptr)
    {
        return std::vector<std::string>(0);
    }

    return recherche -> m_traductions;
}


/**
 * \fn bool Dictionnaire::appartient(const std::string &p_data)
 *
 * \brief Vérifie si le mot donné appartient au dictionnaire
 *
 * \param[in] p_data Le mot a chercher dans le dictionnaire
 *
 * \return true si le mot est dans le dictionnaire, false sinon
 */
bool Dictionnaire::appartient(const std::string &p_data)
{
    if (this -> estVide())
    {
        return false;
    }

    return Dictionnaire::_trouverNoeud(m_racine, p_data) != nullptr && !this -> estVide();
}


/**
 * \fn bool Dictionnaire::estVide() const
 *
 * \brief Vérifie si le dictionnaire est vide
 *
 * \return true si le dictionnaire est vide, false sinon
 */
bool Dictionnaire::estVide() const
{
    return (m_racine == nullptr);
}


/**
 * \fn Dictionnaire::NoeudDictionnaire * Dictionnaire::_trouverNoeud(Dictionnaire::NoeudDictionnaire* p_noeud, const std::string &p_data) const
 *
 * \pre Le noeud en argument ne doit pas etre nullptr
 *
 * \brief Trouve un noeud de facon recursive, en se basant sur le mot anglais
 *
 * \return Retourne le meme noeud si p_noeud contient le bon mot, le bon noeud enfant sinon et nullptr si on a atteint une feuille
 */
Dictionnaire::NoeudDictionnaire * Dictionnaire::_trouverNoeud(Dictionnaire::NoeudDictionnaire* p_noeud, const std::string &p_mot) const
{
    if (p_noeud -> m_mot == p_mot)
    {
        return p_noeud;
    }
    else if (p_mot < p_noeud -> m_mot)
    {
        if (p_noeud -> m_gauche == nullptr)
        {
            return nullptr;
        }
        else
        {
            return Dictionnaire::_trouverNoeud(p_noeud -> m_gauche, p_mot);
        }
    }
    else if (p_mot > p_noeud -> m_mot)
    {
        if (p_noeud -> m_droite == nullptr)
        {
            return nullptr;
        }
        else
        {
            return Dictionnaire::_trouverNoeud(p_noeud -> m_droite, p_mot);
        }
    }
}


/**
 * \fn int Dictionnaire::trouverHauteur(const Dictionnaire::NoeudDictionnaire* p_noeud, std::string p_cote) const
 *
 * \brief Trouve la hauteur d'un noeud enfant de p_noeud de facon recursive
 *
 * \return La hauteur du fils du cote p_cote de p_noeud
 */
int Dictionnaire::trouverHauteur(const Dictionnaire::NoeudDictionnaire* p_noeud, std::string p_cote) const
{
    Dictionnaire::NoeudDictionnaire* cote;

    if (p_noeud == nullptr)
    {
        return -1;
    }

    if (p_cote == "gauche")
    {
        cote = p_noeud -> m_gauche;
    }

    if (p_cote == "droite")
    {
        cote = p_noeud -> m_droite;
    }

    if (cote == nullptr)
    {
        return -1;
    }
    else
    {
        return cote -> m_hauteur;
    }
}


/**
 * \fn bool Dictionnaire::estBalance()
 *
 * \brief Determine si l'arbre est balance
 *
 * \return true si l'arbre est balance, false sinon
 */
bool Dictionnaire::estBalance()
{
    return _trouverDebalancement(m_racine) == nullptr;
}


Dictionnaire::NoeudDictionnaire ** Dictionnaire::_trouverDebalancement(Dictionnaire::NoeudDictionnaire* &p_noeud) const
{
    int gauche = trouverHauteur(p_noeud, "gauche");
    int droite = trouverHauteur(p_noeud, "droite");

    if (gauche - droite < -1 || gauche - droite > 1)
    {
        return &p_noeud;
    }

    if (gauche != -1)
    {
        Dictionnaire::NoeudDictionnaire** aGauche = Dictionnaire::_trouverDebalancement(p_noeud -> m_gauche);

        if (aGauche != nullptr)
        {
            return aGauche;
        }
    }

    if (droite != -1)
    {
        return Dictionnaire::_trouverDebalancement(p_noeud -> m_droite);
    }

    return nullptr;
}


SensDebalanement Dictionnaire::sensDebalancement(Dictionnaire::NoeudDictionnaire* p_noeud)
{

    int gauche = trouverHauteur(p_noeud, "gauche");
    int droite = trouverHauteur(p_noeud, "droite");
    int gaucheGauche;
    int gaucheDroite;
    int droiteDroite;
    int droiteGauche;

    if (gauche != -1)
    {
        gaucheGauche = trouverHauteur(p_noeud -> m_gauche, "gauche");
        gaucheDroite = trouverHauteur(p_noeud -> m_gauche, "droite");
    }

    if (droite != -1)
    {
        droiteDroite = trouverHauteur(p_noeud -> m_droite, "droite");
        droiteGauche = trouverHauteur(p_noeud -> m_droite, "gauche");
    }

    //Debalancement a gauche
    if (gauche - droite > 1)
    {
        if (gaucheGauche - gaucheDroite <= 0)
        {
            return GD;
        }
        else if (gaucheGauche - gaucheDroite > 0)
        {
            return GG;
        }
    }

    //Debalancement a droite
    if (gauche - droite < -1)
    {
        if (droiteGauche - droiteDroite < 0)
        {
            return DD;
        }
        else if (droiteGauche - droiteDroite >= 0)
        {
            return DG;
        }
    }

    throw std::logic_error("Il n'y a pas de debalancement");
}


void Dictionnaire::_auxAjouteMot(Dictionnaire::NoeudDictionnaire* &p_noeud, const std::string &p_mot, const std::string &p_traduction)
{
    if (p_noeud == nullptr)
    {
        p_noeud = new NoeudDictionnaire(p_mot, nullptr, nullptr, 0);
        p_noeud -> m_traductions.push_back(p_traduction);
        m_cpt++;
    }
    else if (p_mot < p_noeud -> m_mot)
    {
        _auxAjouteMot(p_noeud -> m_gauche, p_mot, p_traduction);
        ajusterHauteur(p_noeud);
    }
    else if (p_mot > p_noeud -> m_mot)
    {
        _auxAjouteMot(p_noeud -> m_droite, p_mot, p_traduction);
        ajusterHauteur(p_noeud);
    }
    else
    {
        if (!_contient(p_mot, p_noeud -> m_traductions))
        {
            p_noeud -> m_traductions.push_back(p_traduction);
        }
    }

    _balancerDictionnaire(p_noeud);

    if (estVide())
    {
        m_racine = p_noeud;
    }

}

void Dictionnaire::_auxSupprimeMot(NoeudDictionnaire * &p_noeud, const std::string &p_mot)
{
    //NoeudDictionnaire * noeudMot = _trouverNoeud(m_racine, p_mot);

    if (p_noeud == nullptr)
    {
        throw std::logic_error("Tentative de suppression d'un mot inexistant!");
    }

    if (p_mot < p_noeud -> m_mot)
    {
        _auxSupprimeMot(p_noeud -> m_gauche, p_mot);
    }
    else if (p_mot > p_noeud -> m_mot)
    {
        _auxSupprimeMot(p_noeud -> m_droite, p_mot);
    }
    else if (p_noeud -> m_gauche != nullptr && p_noeud -> m_droite != nullptr) //Cas dans lequel on a trouve le mot
    {
        NoeudDictionnaire * noeudIntermediaire = p_noeud -> m_droite;
        NoeudDictionnaire * noeudParent = p_noeud;

        while (noeudIntermediaire -> m_gauche != nullptr)
        {
            noeudParent = noeudIntermediaire;
            noeudIntermediaire = noeudIntermediaire -> m_gauche;
        }

        p_noeud -> m_mot = noeudIntermediaire -> m_mot;
        p_noeud -> m_traductions = noeudIntermediaire -> m_traductions;
        _auxSupprimeMot(noeudParent -> m_gauche, noeudIntermediaire -> m_mot);
    }
    else if (p_noeud -> m_gauche != nullptr || p_noeud -> m_hauteur == 0)
    {
        NoeudDictionnaire * aSupprimer = p_noeud;
        p_noeud = p_noeud -> m_gauche;
        delete aSupprimer;
        --m_cpt;
    }
    else if (p_noeud -> m_droite != nullptr)
    {
        NoeudDictionnaire * aSupprimer = p_noeud;
        p_noeud = p_noeud -> m_droite;
        delete aSupprimer;
        --m_cpt;
    }

    _balancerDictionnaire(p_noeud);
}

void Dictionnaire::_balancerDictionnaire(NoeudDictionnaire* &p_noeud)
{
    if (estVide()) return;
    if (estBalance()) return;

    Dictionnaire::NoeudDictionnaire ** noeudCritique = _trouverDebalancement(p_noeud);
    if (noeudCritique == nullptr) return;

    SensDebalanement sens = sensDebalancement(*noeudCritique);

    if (sens == GG)
    {
        zigZigGauche(*noeudCritique);
    }
    else if (sens == DD)
    {
        zigZigDroite(*noeudCritique);
    }
    else if (sens == GD)
    {
        zigZagGauche(*noeudCritique);
    }
    else if (sens == DG)
    {
        zigZagDroite(*noeudCritique);
    }
    else
    {
        ajusterHauteur(*noeudCritique);
    }
}

void Dictionnaire::ajusterHauteur(Dictionnaire::NoeudDictionnaire* &p_noeud)
{
    p_noeud -> m_hauteur = 1 + std::max(trouverHauteur(p_noeud, "gauche"), trouverHauteur(p_noeud, "droite"));
}

void Dictionnaire::zigZigGauche(Dictionnaire::NoeudDictionnaire* &p_noeud)
{
    Dictionnaire::NoeudDictionnaire* ancienGauche = p_noeud -> m_gauche;
    p_noeud -> m_gauche = ancienGauche -> m_droite;
    ancienGauche -> m_droite = p_noeud;
    ajusterHauteur(p_noeud);
    ajusterHauteur(ancienGauche);
    p_noeud = ancienGauche;
}

void Dictionnaire::zigZigDroite(Dictionnaire::NoeudDictionnaire* &p_noeudCritique)
{
    Dictionnaire::NoeudDictionnaire * sousCritique = p_noeudCritique -> m_droite;
    p_noeudCritique -> m_droite = sousCritique -> m_gauche;
    sousCritique -> m_gauche = p_noeudCritique;
    ajusterHauteur(p_noeudCritique);
    ajusterHauteur(sousCritique);
    p_noeudCritique = sousCritique;
}

void Dictionnaire::zigZagGauche(Dictionnaire::NoeudDictionnaire* &p_noeud)
{
    zigZigDroite(p_noeud -> m_gauche);
    zigZigGauche(p_noeud);

}

void Dictionnaire::zigZagDroite(Dictionnaire::NoeudDictionnaire* &p_noeud)
{
    zigZigGauche(p_noeud -> m_droite);
    zigZigDroite(p_noeud);
}

bool Dictionnaire::_contient(const std::string &p_mot, const std::vector<std::string> &p_traductions) const
{
    for (size_t i = 0; i < p_traductions.size(); i++)
    {
        if (p_traductions[i] == p_mot)
        {
            return true;
        }
    }
    return false;
}


void Dictionnaire::_auxCalculerSimilitudes(const std::string &p_motMalEcrit, const Dictionnaire::NoeudDictionnaire * p_noeud, std::vector<paireSimilitudeMot> * p_motsSimilitudes)
{
    if (p_noeud != nullptr)
    {
        _auxCalculerSimilitudes(p_motMalEcrit, p_noeud -> m_gauche, p_motsSimilitudes);

        paireSimilitudeMot paire;
        paire.first = similitude(p_noeud -> m_mot, p_motMalEcrit);
        paire.second = p_noeud -> m_mot;
        p_motsSimilitudes -> push_back(paire);

        _auxCalculerSimilitudes(p_motMalEcrit, p_noeud -> m_droite, p_motsSimilitudes);
    }

}


void Dictionnaire::_auxDestructeur(NoeudDictionnaire * p_noeud)
{
    if (p_noeud != nullptr)
    {
        _auxDestructeur(p_noeud -> m_gauche);
        _auxDestructeur(p_noeud -> m_droite);
    }
    delete p_noeud;
}
  
}//Fin du namespace
