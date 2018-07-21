/**
 * \file Principal.cpp
 * \brief Fichier de chargement pour le dictionnaire, avec interface de traduction
 * \author IFT-2008, Gabrielle Martin-Fortier
 * \version 0.1
 * \date juillet 2018
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Dictionnaire.h"

using namespace std;
using namespace TP3;

int main()
{

	try
	{
		string reponse = "";
		ifstream englishFrench;

		bool reponse_lue = false; //Booléen pour savoir que l'utilisateur a saisi une réponse

		//Tant que la réponse est vide ou non valide, on redemande.
		while (!reponse_lue)
		{
			cout << "Entrez le nom du fichier du dictionnaire Anglais-Francais : ";
			getline(cin, reponse);
			englishFrench.open(reponse.c_str());
			if (englishFrench) {
				cout << "Fichier '" << reponse << "' lu!" << endl;
				reponse_lue = true;
			}
			else {
				cout << "Fichier '" << reponse << "' introuvable! Veuillez entrer un nom de fichier, ou son chemin absolu." << endl;
				cin.clear();
				cin.ignore();
			}
		}

		//Initialisation d'un ditionnaire, avec le constructeur qui utilise un fstream
		Dictionnaire dictEnFr(englishFrench);
		englishFrench.close();

	    // Affichage du dictionnaire niveau par niveau
	    cout << dictEnFr << endl;

		vector<string> motsAnglais; //Vecteur qui contiendra les mots anglais de la phrase entrée

		//Lecture de la phrase en anglais
		cout << "Entrez un texte en anglais (pas de majuscules ou de ponctuation/caracteres speciaux" << endl;
		getline(cin, reponse);

		stringstream reponse_ss(reponse);
		string str_temp;

		//On ajoute un mot au vecteur de mots tant qu'on en trouve dans la phrase (séparateur = espace)
		while (reponse_ss >> str_temp)
		{
			motsAnglais.push_back(str_temp);
		}

		vector<string> motsFrancais; //Vecteur qui contiendra les mots traduits en français

		for (vector<string>::const_iterator i = motsAnglais.begin(); i != motsAnglais.end(); i++)
			// Itération dans les mots anglais de la phrase donnée
		{
            vector<string> traductionsPossibles;
            traductionsPossibles = dictEnFr.traduit(*i);
		//Si plusieurs traductions sont possibles, l'utilisateur doit choisir	
            if (traductionsPossibles.size() > 1)
            {
		std::string choix = "";
		bool inputOk = true;
		do
		{
			int n = 0;
			cout << "Il existe plusieurs traductions possibles pour le mot " << *i << ", vous devez choisir : " << endl;

			for (vector<string>::const_iterator it = traductionsPossibles.begin(); it != traductionsPossibles.end(); it++)
			{
			    cout << ++n << ". " << *it << endl;
			}

			getline(cin, choix);
			if !(0 < stoi(choix) && stoi(choix) <= traductionsPossibles.size()) //! avant la parenthese ok?
			{
				inputOk = false;
				cout << "Il faut entrer un entier correspondant à l'un des choix proposés" << endl;
			}
			
		} while (!inputOk); //A tester
		motsFrancais.push_back(traductionsPossibles[stoi(choix) - 1]);
            }
            else if (traductionsPossibles.size() == 1)
            {
                motsFrancais.push_back(traductionsPossibles[0]);
            }
		//Si le mot anglais n'eest pas trouvé dans le dictionnaire, on offre les mots les plus ressemblants
            else if (traductionsPossibles.size() == 0)
            {
		inputOk = true;
		do
		{
			std::string choixUtilisateur = "";
			cout << "Le mot " << *i << " n'existe pas, choisissez un mot parmi les suivants : " << endl;

			std::vector<std::string> choixMots = dictEnFr.suggereCorrections(*i);

			int n = 0;
			for (vector<string>::const_iterator it = choixMots.begin(); it != choixMots.end(); it++)
			{
			    cout << ++n << ". " << *it << endl;;
			}

			getline(cin, choixUtilisateur);
			
			if !(0 < stoi(choixUtilisateur) && stoi(choixUtilisateur) <= choixMots.size()) //! avant la parenthese ok?
			{
				inputOk = false;
				cout << "Il faut entrer un entier correspondant à l'un des choix proposés" << endl;
			}
			
		} while (!inputOk); //A tester
			motsAnglais.erase(i);
			motsAnglais.insert(i, choixMots[stoi(choixUtilisateur) - 1]);
			i--;
            }
		}

		stringstream phraseFrancais; // On crée un string contenant la phrase,
									 // À partir du vecteur de mots traduits.
		for (vector<string>::const_iterator i = motsFrancais.begin(); i != motsFrancais.end(); i++)
		{
			phraseFrancais << *i << " ";
		}

		cout << "La phrase en francais est :" << endl << phraseFrancais.str() << endl;

	}
	catch (exception & e)
	{
		cerr << e.what() << endl;
	}

	return 0;
}
