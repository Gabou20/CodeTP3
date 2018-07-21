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
    double differenceLongueurs = abs((int) p_mot1.length() - p_mot2.length());// La fonction compare les deux mots lettre a lettre, et additionne 1 au total pour chaque lettre non identique.
    double distanceDuDebut = differenceLongueurs;                             // De plus, a difference de longueur est consideree (+1 pour chaque lettre de difference).
    double distanceDeLaFin = differenceLongueurs;                             // Les deux mots sont compares du debut et de la fin, et le total le plus bas est conserve.

    if (p_mot1.length() != 0 && p_mot2.length() != 0)
    {
        for (int i = 0; i < std::min(p_mot1.length(), p_mot2.length()); i++)
        {
            if (p_mot1[i] != p_mot2[i])
            {
                distanceDuDebut += 1;
            }
        }

        std::string mot1(p_mot1);
        std::reverse(mot1.begin(), mot1.end());
        std::string mot2(p_mot1);
        std::reverse(mot2.begin(), mot2.end());
        for (int i = 0; i < p_mot1.length(); i++)
        {
            if (p_mot1[i] != p_mot2[i])
            {
                distanceDeLaFin += 1;
            }
        }
    }

    double distance = std::min(distanceDuDebut, distanceDeLaFin);
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

    std::vector<PaireSimilitudeMot> motsSimilitudes;
    _auxCalculerSimilitudes(p_motMalEcrit, m_racine, &motsSimilitudes);

    std::sort(motsSimilitudes.begin(), motsSimilitudes.end());

    std::vector<std::string> choixMots;
    int n = std::min(motsSimilitudes.size(), 5);
    while (n-- < 0)
    {
        choixMots.push_back(motsSimilitudes[n] -> second);
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
    return Dictionnaire::_trouverNoeud(m_racine, p_data) != nullptr; //&& !estVide()?
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
 * \brief Trouve un noeud de facon recursive, en se basant sur le mot anglais
 *
 * \pre Le noeud en argument ne doit pas etre nullptr
 *
 * \param[in] p_noeud Le noeud a partir duquel on effectue la recherche
 * \param[in] p_mot Le mot qu'on cherche
 *
 * \return Retourne le meme noeud si p_noeud contient le bon mot, le bon noeud enfant sinon et nullptr si on a atteint une feuille
 */
Dictionnaire::NoeudDictionnaire * Dictionnaire::_trouverNoeud(NoeudDictionnaire* p_noeud, const std::string &p_mot) const
{
    if (p_noeud == nullptr)
    {
        return nullptr;
    }
    if (p_noeud -> m_mot == p_mot)
    {
        return p_noeud;
    }
    else if (p_mot < p_noeud -> m_mot)
    {
        return _trouverNoeud(p_noeud -> m_gauche, p_mot);
    }
    else if (p_mot > p_noeud -> m_mot)
    {
        return _trouverNoeud(p_noeud -> m_droite, p_mot);
    }
}


/**
 * \fn int Dictionnaire::trouverHauteurGauche(const Dictionnaire::NoeudDictionnaire* p_noeud) const
 *
 * \brief Trouve la hauteur du fils gauche d'un noeud
 *
 * \pre p_noeud n'est pas nullptr
 *
 * \param[in] p_noeud Le noeud dont on veut la hauteur du fils gauche
 *
 * \return La hauteur du fils gauche de p_noeud
 */
int Dictionnaire::trouverHauteurGauche(const Dictionnaire::NoeudDictionnaire* p_noeud) const
{
    return p_noeud -> m_gauche == nullptr ? -1 : p_noeud -> m_gauche -> m_hauteur;
}


/**
 * \fn int Dictionnaire::trouverHauteurDroite(const Dictionnaire::NoeudDictionnaire* p_noeud) const
 *
 * \brief Trouve la hauteur du fils droit d'un noeud
 *
 * \pre p_noeud n'est pas null
 *
 * \param[in] p_noeud Le noeud dont on veut calculer la hauteur du fils droit
 *
 * \return La hauteur du fils droit de p_noeud
 */
int Dictionnaire::trouverHauteurDroite(const Dictionnaire::NoeudDictionnaire* p_noeud) const
{
    return p_noeud -> m_droite == nullptr ? -1 : p_noeud -> m_droite -> m_hauteur;
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


/**
 * \fn Dictionnaire::NoeudDictionnaire ** Dictionnaire::_trouverDebalancement(Dictionnaire::NoeudDictionnaire* &p_noeud) const
 *
 * \brief Trouve un noeud critique (qui est debalance) s"il y en a au moins un
 *
 * \param[in] p_noeud Le noeud a partir duquel on effectue la recherche
 *
 * \return Le pointeur d'un noeud qui est debalance, nullptr s'il n'y en a pas
 */
Dictionnaire::NoeudDictionnaire ** Dictionnaire::_trouverDebalancement(Dictionnaire::NoeudDictionnaire* &p_noeud) const
{
    int gauche = trouverHauteurGauche(p_noeud);
    int droite = trouverHauteurDroite(p_noeud);

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


/**
 * \fn SensDebalancement Dictionnaire::sensDebalancement(Dictionnaire::NoeudDictionnaire* p_noeud)
 *
 * \brief Determine le sens du debalancement d'un noeud critique
 *
 * \param[in] p_noeud Le noeud dont on veut trouver le sens de debalancement
 *
 * \exception logic_error si le noeud passe en argument ne debalance pas
 *
 * \return Le sens du debalancement, soit l'une des 4 possibilites suivantes : GG, DD, GD et DG
 */
SensDebalancement Dictionnaire::sensDebalancement(Dictionnaire::NoeudDictionnaire* p_noeud)
{

    int gauche = trouverHauteurGauche(p_noeud);
    int droite = trouverHauteurDroite(p_noeud);
    int gaucheGauche;
    int gaucheDroite;
    int droiteDroite;
    int droiteGauche;

    if (gauche != -1)
    {
        gaucheGauche = trouverHauteurGauche(p_noeud -> m_gauche);
        gaucheDroite = trouverHauteurDroite(p_noeud -> m_gauche);
    }

    if (droite != -1)
    {
        droiteDroite = trouverHauteurDroite(p_noeud -> m_droite);
        droiteGauche = trouverHauteurGauche(p_noeud -> m_droite);
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


/**
 * \fn void Dictionnaire::_auxAjouteMot(Dictionnaire::NoeudDictionnaire* &p_noeud, const std::string &p_mot, const std::string &p_traduction)
 *
 * \brief Ajoute un mot en cherchant le bon endroit de facon recursive. Si le mot anglais est deja present, le mot francais est ajoute a la liste de traductions possibles (s'il n'etait pas deja la), et rebalance l'arbre apres l'ajout
 *
 * \param[in] p_noeud Le noeud a partir duquel on cherche l'endroit ou il faudrait inserer le mot
 * \param[in] p_mot Le mot anglais a trouver/inserer
 * \param[in] p_traduction La traduction francaise de p_mot a ajouter
 *
 * \post Le mot et sa traduction se trouvent maintenant dans le dictionnaire, qui est toujours balance
 */
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
}


/**
 * \fn void Dictionnaire::_auxSupprimeMot(NoeudDictionnaire * &p_noeud, const std::string &p_mot)
 *
 * \brief Supprime un mot (son noeud) en cherchant le bon noeud de facon recursive, puis rebalance l'arbre apres la suppression.
 *
 * \param[in] p_noeud Le noeud a partir duquel on cherche le mot a supprimer
 * \param[in] p_mot Le mot anglais a supprimer du dictionnaire
 *
 * \post Le noeud qui contenait le mot p_mot n'existe plus dans l'arbre du dictionnaire, et ce dernier est toujours balance
 */
void Dictionnaire::_auxSupprimeMot(NoeudDictionnaire * &p_noeud, const std::string &p_mot)
{
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
        if (m_cpt == 1)
        {
            m_racine = nullptr; //Si le noeud qui est supprime ici etait le seul noeud de l'arbre, il faut mettre la racine a NULL
        }
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


/**
 * \fn void Dictionnaire::_balancerDictionnaire(NoeudDictionnaire* &p_noeud)
 *
 * \brief Fonction qui balance le dictionnaire de facon recursive.
 *
 * \param[in] p_noeud Le noeud a partir duquel on balance le dictionnaire
 *
 * \post L'arbre du dictionnaire est balance
 */
void Dictionnaire::_balancerDictionnaire(NoeudDictionnaire* &p_noeud)
{
    if (estVide()) return;
    if (estBalance()) return;

    Dictionnaire::NoeudDictionnaire ** noeudCritique = _trouverDebalancement(p_noeud);
    if (noeudCritique == nullptr) return;

    SensDebalancement sens = sensDebalancement(*noeudCritique);

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


/**
 * \fn void Dictionnaire::ajusterHauteur(Dictionnaire::NoeudDictionnaire* &p_noeud)
 *
 * \brief Fonction qui ajuste la hauteur d'un noeud en ajoutant 1 a la hauteur maximale entre ses deux fils
 *
 * \param[in] p_noeud Le noeud duquel il faut ajuster la hauteur
 *
 * \post La valeur de l'attribut m_hauteur de p_noeud est ajustee
 */
void Dictionnaire::ajusterHauteur(Dictionnaire::NoeudDictionnaire* &p_noeud)
{
    p_noeud -> m_hauteur = 1 + std::max(trouverHauteurGauche(p_noeud), trouverHauteurDroite(p_noeud));
}


/**
 * \fn void Dictionnaire::zigZigGauche(Dictionnaire::NoeudDictionnaire* &p_noeud)
 *
 * \brief Effectue une rotation de type zigZigGauche (une seule rotation) sur le noeud critique p_noeud
 *
 * \param[in] p_noeud Le noeud critique pour lequel la rotation doit etre effectuee
 *
 * \post Le noeud p_noeud est balance
 */
void Dictionnaire::zigZigGauche(Dictionnaire::NoeudDictionnaire* &p_noeud)
{
    Dictionnaire::NoeudDictionnaire* ancienGauche = p_noeud -> m_gauche;
    p_noeud -> m_gauche = ancienGauche -> m_droite;
    ancienGauche -> m_droite = p_noeud;
    ajusterHauteur(p_noeud);
    ajusterHauteur(ancienGauche);
    p_noeud = ancienGauche;
}


/**
 * \fn void Dictionnaire::zigZigDroite(Dictionnaire::NoeudDictionnaire* &p_noeudCritique)
 *
 * \brief Effectue une rotation de type zigZigDroit (une seule rotation) sur le noeud critique p_noeud
 *
 * \param[in] p_noeud Le noeud critique pour lequel la rotation doit etre effectuee
 *
 * \post Le noeud p_noeud est balance
 */
void Dictionnaire::zigZigDroite(Dictionnaire::NoeudDictionnaire* &p_noeudCritique)
{
    Dictionnaire::NoeudDictionnaire * sousCritique = p_noeudCritique -> m_droite;
    p_noeudCritique -> m_droite = sousCritique -> m_gauche;
    sousCritique -> m_gauche = p_noeudCritique;
    ajusterHauteur(p_noeudCritique);
    ajusterHauteur(sousCritique);
    p_noeudCritique = sousCritique;
}


/**
 * \fn void Dictionnaire::zigZagGauche(Dictionnaire::NoeudDictionnaire* &p_noeud)
 *
 * \brief Effectue une rotation de type zigZagGauche (deux rotations) sur le noeud critique p_noeud
 *
 * \param[in] p_noeud Le noeud critique pour lequel la rotation doit etre effectuee
 *
 * \post Le noeud p_noeud est balance
 */
void Dictionnaire::zigZagGauche(Dictionnaire::NoeudDictionnaire* &p_noeud)
{
    zigZigDroite(p_noeud -> m_gauche);
    zigZigGauche(p_noeud);

}


/**
 * \fn void Dictionnaire::zigZagDroite(Dictionnaire::NoeudDictionnaire* &p_noeud)
 *
 * \brief Effectue une rotation de type zigZagDroit (deux rotations) sur le noeud critique p_noeud
 *
 * \param[in] p_noeud Le noeud critique pour lequel la rotation doit etre effectuee
 *
 * \post Le noeud p_noeud est balance
 */
void Dictionnaire::zigZagDroite(Dictionnaire::NoeudDictionnaire* &p_noeud)
{
    zigZigGauche(p_noeud -> m_droite);
    zigZigDroite(p_noeud);
}


/**
 * \fn bool Dictionnaire::_contient(const std::string &p_mot, const std::vector<std::string> &p_liste) const
 *
 * \brief Verifie si un mot est present dans une liste
 *
 * \param[in] p_mot Le mot a chercher dans la liste
 * \param[in] p_liste La liste dans laquelle le mot est cherche
 *
 * \return true si p_mot est dans p_liste, false sinon
 */
bool Dictionnaire::_contient(const std::string &p_mot, const std::vector<std::string> &p_liste) const
{
    for (size_t i = 0; i < p_liste.size(); i++)
    {
        if (p_liste[i] == p_mot)
        {
            return true;
        }
    }
    return false;
}


/**
 * \fn void Dictionnaire::_auxCalculerSimilitudes(const std::string &p_motMalEcrit, const Dictionnaire::NoeudDictionnaire * p_noeud, std::vector<paireSimilitudeMot> * p_motsSimilitudes)
 *
 * \brief Cree un vecteur de paires contenant chaque mot du dictionnaire ainsi que son indice de similitude par rapport a un mot. Cette fonction est recursive.
 *
 * \param[in] p_motMalEcrit Le mot avec lequel il faut comparer les mots du dictionnaire
 * \param[in] p_noeud Le noeud a partir duquel on calcule les indices de similitudes
 * \param[in] p_motsSimilitudes Le vecteur dans lequel il faut inserer les paires indice-mot
 *
 * \post Le vecteur p_motsSimilitudes contient tous les mots du dictionnaire avec leur indice de similitude
 */
void Dictionnaire::_auxCalculerSimilitudes(const std::string &p_motMalEcrit, const Dictionnaire::NoeudDictionnaire * p_noeud, std::vector<PaireSimilitudeMot> * p_motsSimilitudes)
{
    if (p_noeud != nullptr)
    {
        _auxCalculerSimilitudes(p_motMalEcrit, p_noeud -> m_gauche, p_motsSimilitudes);

        PaireSimilitudeMot paire;
        paire.first = similitude(p_noeud -> m_mot, p_motMalEcrit);
        paire.second = p_noeud -> m_mot;
        p_motsSimilitudes -> push_back(paire);

        _auxCalculerSimilitudes(p_motMalEcrit, p_noeud -> m_droite, p_motsSimilitudes);
    }

}


/**
 * \fn void Dictionnaire::_auxDestructeur(NoeudDictionnaire * p_noeud)
 *
 * \brief Detruit les noeuds du dictionnaire de facon recursive
 *
 * \param[in] p_noeud Le noeud a partir duquel on effectue la suppression
 *
 * \post p_noeud et tous ses descendants ont ete detruits
 */
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
