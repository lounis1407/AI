#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NB_LIGNES 10
#define NB_COLONNES 10
#define INFINI 10000

/*#define DEBUG*/

// TD1 IA BOUHADOUN Lounis

// Ajout de deux variables globales
long long noeuds_explores = 0;
clock_t debut_partie, fin_partie;

typedef struct pion_s
{
    int couleur;
    int valeur;
} Pion;

typedef struct dep {
    int ligne_de_depart;
    int ligne_de_arrive;
    int colonne_de_depart;
    int colonne_de_arrive;
} dep;

Pion *plateauDeJeu;

void f_affiche_plateau(Pion *plateau);
int f_convert_char2int(char c);
char f_convert_int2char(int i);
int f_max(Pion *ptj, int joueur, dep *mouvement, int profondeur);
int f_min(Pion *ptj, int joueur, dep *mouvement, int profondeur);
int f_max_alphaBeta(Pion *ptj, int joueur, dep *mouvement, int profondeur, int alpha, int beta);
int f_min_alphaBeta(Pion *ptj, int joueur, dep *mouvement, int profondeur, int alpha, int beta);
int f_convert_char2int(char c)
{
#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif
    if (c >= 'A' && c <= 'Z')
        return (int)(c - 'A');
    if (c >= 'a' && c <= 'z')
        return (int)(c - 'a');
    return -1;
}

char f_convert_int2char(int i)
{
#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

    return (char)i + 'A';
}

Pion *f_init_plateau()
{
    int i, j;
    Pion *plateau = NULL;

#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

    plateau = (Pion *)malloc(NB_LIGNES * NB_COLONNES * sizeof(Pion));
    if (plateau == NULL)
    {
        printf("error: unable to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NB_LIGNES; i++)
    {
        for (j = 0; j < NB_COLONNES; j++)
        {
            plateau[i * NB_COLONNES + j].couleur = 0;
            plateau[i * NB_COLONNES + j].valeur = 0;
        }
    }

    plateau[9 * NB_COLONNES + 5].couleur = 1;
    plateau[9 * NB_COLONNES + 5].valeur = 1;

    plateau[9 * NB_COLONNES + 6].couleur = 1;
    plateau[9 * NB_COLONNES + 6].valeur = 2;

    plateau[9 * NB_COLONNES + 7].couleur = 1;
    plateau[9 * NB_COLONNES + 7].valeur = 3;

    plateau[9 * NB_COLONNES + 8].couleur = 1;
    plateau[9 * NB_COLONNES + 8].valeur = 2;

    plateau[9 * NB_COLONNES + 9].couleur = 1;
    plateau[9 * NB_COLONNES + 9].valeur = 1;

    plateau[8 * NB_COLONNES + 0].couleur = 1;
    plateau[8 * NB_COLONNES + 0].valeur = 1;

    plateau[8 * NB_COLONNES + 1].couleur = 1;
    plateau[8 * NB_COLONNES + 1].valeur = 3;

    plateau[8 * NB_COLONNES + 2].couleur = 1;
    plateau[8 * NB_COLONNES + 2].valeur = 3;

    plateau[8 * NB_COLONNES + 3].couleur = 1;
    plateau[8 * NB_COLONNES + 3].valeur = 1;

    plateau[8 * NB_COLONNES + 6].couleur = 1;
    plateau[8 * NB_COLONNES + 6].valeur = 1;

    plateau[8 * NB_COLONNES + 7].couleur = 1;
    plateau[8 * NB_COLONNES + 7].valeur = 1;

    plateau[8 * NB_COLONNES + 8].couleur = 1;
    plateau[8 * NB_COLONNES + 8].valeur = 1;

    plateau[7 * NB_COLONNES + 1].couleur = 1;
    plateau[7 * NB_COLONNES + 1].valeur = 1;

    plateau[7 * NB_COLONNES + 2].couleur = 1;
    plateau[7 * NB_COLONNES + 2].valeur = 1;

    plateau[2 * NB_COLONNES + 7].couleur = -1;
    plateau[2 * NB_COLONNES + 7].valeur = 1;

    plateau[2 * NB_COLONNES + 8].couleur = -1;
    plateau[2 * NB_COLONNES + 8].valeur = 1;

    plateau[1 * NB_COLONNES + 1].couleur = -1;
    plateau[1 * NB_COLONNES + 1].valeur = 1;

    plateau[1 * NB_COLONNES + 2].couleur = -1;
    plateau[1 * NB_COLONNES + 2].valeur = 1;

    plateau[1 * NB_COLONNES + 3].couleur = -1;
    plateau[1 * NB_COLONNES + 3].valeur = 1;

    plateau[1 * NB_COLONNES + 6].couleur = -1;
    plateau[1 * NB_COLONNES + 6].valeur = 1;

    plateau[1 * NB_COLONNES + 7].couleur = -1;
    plateau[1 * NB_COLONNES + 7].valeur = 3;

    plateau[1 * NB_COLONNES + 8].couleur = -1;
    plateau[1 * NB_COLONNES + 8].valeur = 3;

    plateau[1 * NB_COLONNES + 9].couleur = -1;
    plateau[1 * NB_COLONNES + 9].valeur = 1;

    plateau[0 * NB_COLONNES + 0].couleur = -1;
    plateau[0 * NB_COLONNES + 0].valeur = 1;

    plateau[0 * NB_COLONNES + 1].couleur = -1;
    plateau[0 * NB_COLONNES + 1].valeur = 2;

    plateau[0 * NB_COLONNES + 2].couleur = -1;
    plateau[0 * NB_COLONNES + 2].valeur = 3;

    plateau[0 * NB_COLONNES + 3].couleur = -1;
    plateau[0 * NB_COLONNES + 3].valeur = 2;

    plateau[0 * NB_COLONNES + 4].couleur = -1;
    plateau[0 * NB_COLONNES + 4].valeur = 1;

#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif

    return plateau;
}

void f_affiche_plateau(Pion *plateau)
{
    int i, j, k;

#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

    printf("\n    ");
    for (k = 0; k < NB_COLONNES; k++)
        printf("%2c ", f_convert_int2char(k));
    printf("\n    ");
    for (k = 0; k < NB_COLONNES; k++)
        printf("-- ");
    printf("\n");
    for (i = NB_LIGNES - 1; i >= 0; i--)
    {
        printf("%2d ", i);
        for (j = 0; j < NB_COLONNES; j++)
        {
            printf("|");
            switch (plateau[i * NB_COLONNES + j].couleur)
            {
            case -1:
                printf("%do", plateau[i * NB_COLONNES + j].valeur);
                break;
            case 1:
                printf("%dx", plateau[i * NB_COLONNES + j].valeur);
                break;
            default:
                printf("  ");
            }
        }
        printf("|\n    ");
        for (k = 0; k < NB_COLONNES; k++)
            printf("-- ");
        printf("\n");
    }
    printf("    ");

#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
}

int f_gagnant()
{
    int i, j, somme1 = 0, somme2 = 0;

#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

    // Quelqu'un est-il arrive sur la ligne de l'autre
    for (i = 0; i < NB_COLONNES; i++)
    {
        if (plateauDeJeu[i].couleur == 1)
            return 1;
        if (plateauDeJeu[(NB_LIGNES - 1) * NB_COLONNES + i].couleur == -1)
            return -1;
    }

    // taille des armees
    for (i = 0; i < NB_LIGNES; i++)
    {
        for (j = 0; j < NB_COLONNES; j++)
        {
            if (plateauDeJeu[i * NB_COLONNES + j].couleur == 1)
                somme1++;
            if (plateauDeJeu[i * NB_COLONNES + j].couleur == -1)
                somme2++;
        }
    }
    if (somme1 == 0)
        return -1;
    if (somme2 == 0)
        return 1;

#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
    return 0;
}

/**
 * Prend comme argument la ligne et la colonne de la case
 *  pour laquelle la bataille a lieu
 * Renvoie le couleur du gagnant
 * */
int f_bataille(int l, int c)
{
    int i, j, mini, maxi, minj, maxj;
    int somme = 0;

#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif
    mini = l - 1 < 0 ? 0 : l - 1;
    maxi = l + 1 > NB_LIGNES - 1 ? NB_LIGNES - 1 : l + 1;
    minj = c - 1 < 0 ? 0 : c - 1;
    maxj = c + 1 > NB_COLONNES - 1 ? NB_COLONNES - 1 : c + 1;

    for (i = mini; i <= maxi; i++)
    {
        for (j = minj; j <= maxj; j++)
        {
            somme += plateauDeJeu[i * NB_COLONNES + j].couleur * plateauDeJeu[i * NB_COLONNES + j].valeur;
        }
    }
    somme -= plateauDeJeu[l * NB_COLONNES + c].couleur * plateauDeJeu[l * NB_COLONNES + c].valeur;

#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
    if (somme < 0)
        return -1;
    if (somme > 0)
        return 1;

    return plateauDeJeu[l * NB_COLONNES + c].couleur;
}

/**
 * Prend la ligne et colonne de la case d'origine
 *  et la ligne et colonne de la case de destination
 * Renvoie 1 en cas d'erreur
 * Renvoie 0 sinon
 * */
int f_test_mouvement(Pion *plateau, int l1, int c1, int l2, int c2, int couleur)
{
#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
    printf("de (%d,%d) vers (%d,%d)\n", l1, c1, l2, c2);
#endif
    /* Erreur, hors du plateau */
    if (l1 < 0 || l1 >= NB_LIGNES || l2 < 0 || l2 >= NB_LIGNES ||
        c1 < 0 || c1 >= NB_COLONNES || c2 < 0 || c2 >= NB_COLONNES)
        return 1;
    /* Erreur, il n'y a pas de pion a deplacer ou le pion n'appartient pas au joueur*/
    if (plateau[l1 * NB_COLONNES + c1].valeur == 0 || plateau[l1 * NB_COLONNES + c1].couleur != couleur)
        return 1;
    /* Erreur, tentative de tir fratricide */
    if (plateau[l2 * NB_COLONNES + c2].couleur == plateau[l1 * NB_COLONNES + c1].couleur)
        return 1;

    if (l1 - l2 > 1 || l2 - l1 > 1 || c1 - c2 > 1 || c2 - c1 > 1 || (l1 == l2 && c1 == c2))
        return 1;
#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
    return 0;
}

/**
 * Prend la ligne et colonne de la case d'origine
 *  et la ligne et colonne de la case de destination
 *  et effectue le trantement de l'operation demandÃ©e
 * Renvoie 1 en cas d'erreur
 * Renvoie 0 sinon
 * */
int f_bouge_piece(Pion *plateau, int l1, int c1, int l2, int c2, int couleur)
{
    int gagnant = 0;

#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

    if (f_test_mouvement(plateau, l1, c1, l2, c2, couleur) != 0)
        return 1;

    /* Cas ou il n'y a personne a l'arrivee */
    if (plateau[l2 * NB_COLONNES + c2].valeur == 0)
    {
        plateau[l2 * NB_COLONNES + c2].couleur = plateau[l1 * NB_COLONNES + c1].couleur;
        plateau[l2 * NB_COLONNES + c2].valeur = plateau[l1 * NB_COLONNES + c1].valeur;
        plateau[l1 * NB_COLONNES + c1].couleur = 0;
        plateau[l1 * NB_COLONNES + c1].valeur = 0;
    }
    else
    {
        gagnant = f_bataille(l2, c2);
        /* victoire */
        if (gagnant == couleur)
        {
            plateau[l2 * NB_COLONNES + c2].couleur = plateau[l1 * NB_COLONNES + c1].couleur;
            plateau[l2 * NB_COLONNES + c2].valeur = plateau[l1 * NB_COLONNES + c1].valeur;
            plateau[l1 * NB_COLONNES + c1].couleur = 0;
            plateau[l1 * NB_COLONNES + c1].valeur = 0;
        }
        /* defaite */
        else if (gagnant != 0)
        {
            plateau[l1 * NB_COLONNES + c1].couleur = 0;
            plateau[l1 * NB_COLONNES + c1].valeur = 0;
        }
    }

#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
    return 0;
}

// Calcul du nombre de pions sur le plateau du joueur
int f_nbPions(Pion *jeu, int joueur)
{
    int nbPion = 0;
    int i, j;
    for (i = 0; i < NB_COLONNES; ++i)
    {
        for (j = 0; j < NB_LIGNES; ++j)
        {
            if (jeu[i * NB_COLONNES + j].couleur == joueur)
            {
                ++nbPion;
            }
        }
    }
    return nbPion;
}

// Calcul de la valeur de tous les pions du joueur
int f_valeur(Pion *jeu, int joueur)
{
    int i, j;
    int valeur = 0;
    for (i = 0; i < NB_COLONNES; ++i)
    {
        for (j = 0; j < NB_LIGNES; ++j)
        {
            if (jeu[i * NB_COLONNES + j].couleur == joueur)
            {
                valeur += jeu[i * NB_COLONNES + j].valeur;
            }
        }
    }
    return valeur;
}

/**
 * Test simple pour un f_eval quelconque
*
* int f_eval(Pion *jeu, int joueur)
{
    // Initialise la graine de l'alÃƒÂ©atoire une seule fois (souvent dans la fonction main)

    srand(time(NULL));

    // Retourne un entier alÃƒÂ©atoire entre 1 et 50
    int random_value = (rand() % 50) + 1;
    return random_value;
}
 * */



//
//
// Fonction d'évaluation améliorée avec récompense accrue pour l'avancement vers la ligne de l'adversaire
int f_eval(Pion *jeu, int joueur)
{
    int score = 0;
    int opponent = -joueur;
    int i, j;

    // Poids pour les différents critères
    const int material_weight = 100;
    const int advancement_weight = 50; // Augmentation du poids pour l'avancement
    const int threat_weight = 50;
    const int mobility_weight = 10;
    const int center_control_weight = 3;

    for (i = 0; i < NB_LIGNES; i++)
    {
        for (j = 0; j < NB_COLONNES; j++)
        {
            Pion p = jeu[i * NB_COLONNES + j];

            if (p.couleur == joueur)
            {
                // Valeur matérielle du pion
                score += p.valeur * material_weight;

                // Bonus pour l'avancement vers le camp adverse
                int advancement = (joueur == 1) ? (i) : (NB_LIGNES - 1 - i);
                if (advancement > 0)
                {
                    score += advancement_weight * (1 + advancement); // Récompense linéaire
                    score += (advancement * advancement_weight);    // Récompense quadratique
                }

                // Contrôle du centre
                if (i >= NB_LIGNES / 3 && i <= 2 * NB_LIGNES / 3 &&
                    j >= NB_COLONNES / 3 && j <= 2 * NB_COLONNES / 3)
                {
                    score += center_control_weight;
                }

                // Mobilité du pion
                int mobility = 0;
                for (int di = -1; di <= 1; di++)
                {
                    for (int dj = -1; dj <= 1; dj++)
                    {
                        if (di == 0 && dj == 0)
                            continue;
                        int new_i = i + di;
                        int new_j = j + dj;
                        if (new_i >= 0 && new_i < NB_LIGNES &&
                            new_j >= 0 && new_j < NB_COLONNES)
                        {
                            if (f_test_mouvement(jeu, i, j, new_i, new_j, joueur) == 0)
                            {
                                mobility++;
                            }
                        }
                    }
                }
                score += mobility * mobility_weight;
            }
            else if (p.couleur == opponent)
            {
                // Valeur matérielle du pion adverse
                score -= p.valeur * material_weight;

                // Pénalité pour l'avancement de l'adversaire
                int advancement = (opponent == 1) ? (i) : (NB_LIGNES - 1 - i);
                if (advancement > 0)
                {
                    score -= advancement_weight * (1 + advancement);
                    score -= (advancement * advancement_weight);
                }

                // Contrôle du centre par l'adversaire
                if (i >= NB_LIGNES / 3 && i <= 2 * NB_LIGNES / 3 &&
                    j >= NB_COLONNES / 3 && j <= 2 * NB_COLONNES / 3)
                {
                    score -= center_control_weight;
                }

                // Mobilité du pion adverse
                int mobility = 0;
                for (int di = -1; di <= 1; di++)
                {
                    for (int dj = -1; dj <= 1; dj++)
                    {
                        if (di == 0 && dj == 0)
                            continue;
                        int new_i = i + di;
                        int new_j = j + dj;
                        if (new_i >= 0 && new_i < NB_LIGNES &&
                            new_j >= 0 && new_j < NB_COLONNES)
                        {
                            if (f_test_mouvement(jeu, i, j, new_i, new_j, opponent) == 0)
                            {
                                mobility++;
                            }
                        }
                    }
                }
                score -= mobility * mobility_weight;
            }
        }
    }

    return score;
}

// copie du plateau
void f_copie_plateau(Pion *source, Pion *destination)
{
    int i, j;
    for (i = 0; i < NB_LIGNES; i++)
    {
        for (j = 0; j < NB_COLONNES; j++)
        {
            destination[i * NB_COLONNES + j].couleur = source[i * NB_COLONNES + j].couleur;
            destination[i * NB_COLONNES + j].valeur = source[i * NB_COLONNES + j].valeur;
        }
    }
}

// mise a zero du plateau
Pion *f_raz_plateau()
{
    Pion *jeu = NULL;
    int i, j;
    jeu = (Pion *)malloc(NB_LIGNES * NB_COLONNES * sizeof(Pion));
    for (i = 0; i < NB_LIGNES; i++)
    {
        for (j = 0; j < NB_COLONNES; j++)
        {
            jeu[i * NB_COLONNES + j].couleur = 0;
            jeu[i * NB_COLONNES + j].valeur = 0;
        }
    }
    return jeu;
}

// Fonction min trouve le minimum des noeuds fils
int f_min(Pion *ptj, int joueur, dep *mouvement, int profondeur)
{
    if (profondeur == 0)
    {
        return f_eval(ptj, joueur);
    }

    int value = INFINI;
    dep meilleur_mouvement;
    noeuds_explores++; // Compte le nombre de nœuds explorés


    for (int lig = 0; lig < NB_LIGNES; lig++)
    {
        for (int col = 0; col < NB_COLONNES; col++)
        {
            if (ptj[lig * NB_COLONNES + col].couleur == joueur)
            {
                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {
                        if (i != 0 && j != 0)
                        {
                            if (f_test_mouvement(ptj, lig, col, lig + i, col + j, joueur) == 0)
                            {
                                Pion *copie = f_init_plateau();
                                f_copie_plateau(ptj, copie);

                                if (f_bouge_piece(copie, lig, col, lig + i, col + j, joueur) == 0)
                                {
                                    int resultat = f_max(copie, -joueur, mouvement, profondeur - 1);

                                    if (resultat < value)
                                    {
                                        value = resultat;
                                        meilleur_mouvement.colonne_de_arrive = col + j;
                                        meilleur_mouvement.colonne_de_depart = col;
                                        meilleur_mouvement.ligne_de_arrive = lig + i;
                                        meilleur_mouvement.ligne_de_depart = lig;
                                    }
                                }
                                free(copie);
                            }
                        }
                    }
                }
            }
        }
    }

    if (profondeur == 3)
    {
        *mouvement = meilleur_mouvement;
    }
    return value;
}

int f_max(Pion *ptj, int joueur, dep *mouvement, int profondeur)
{

    if (profondeur == 0)
    {
        return f_eval(ptj, joueur);
    }

    int value = -INFINI;
    dep meilleur_mouvement;
    noeuds_explores++; // Compte le nombre de nœuds explorés


    for (int lig = 0; lig < NB_LIGNES; lig++)
    {
        for (int col = 0; col < NB_COLONNES; col++)
        {
            if (ptj[lig * NB_COLONNES + col].couleur == joueur)
            {
                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {

                        if (i != 0 && j != 0)
                        {

                            if (f_test_mouvement(ptj, lig, col, lig + i, col + j, joueur) == 0)
                            {
                                Pion *copie = f_init_plateau();
                                f_copie_plateau(ptj, copie);

                                if (f_bouge_piece(copie, lig, col, lig + i, col + j, joueur) == 0)
                                {
                                    int resultat = f_min(copie, -joueur, mouvement, profondeur - 1);
                                    if (resultat > value)
                                    {
                                        value = resultat;
                                        meilleur_mouvement.colonne_de_arrive = col + j;
                                        meilleur_mouvement.colonne_de_depart = col;
                                        meilleur_mouvement.ligne_de_arrive = lig + i;
                                        meilleur_mouvement.ligne_de_depart = lig;
                                    }
                                }
                                free(copie);
                            }
                        }
                    }
                }
            }
        }
    }

    if (profondeur == 3)
    {
        *mouvement = meilleur_mouvement;
    }
    return value;
}

int f_min_alphaBeta(Pion *ptj, int joueur, dep *mouvement, int profondeur, int alpha, int beta)
{

    noeuds_explores++; // Compte le nombre de nœuds explorés
    if (profondeur == 0)
    {
        return f_eval(ptj, joueur);
    }

    int value = INFINI;
    dep meilleur_mouvement;

    for (int lig = 0; lig < NB_LIGNES; lig++)
    {
        for (int col = 0; col < NB_COLONNES; col++)
        {
            if (ptj[lig * NB_COLONNES + col].couleur == joueur)
            {
                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {
                        if (i != 0 && j != 0)
                        {
                            if (f_test_mouvement(ptj, lig, col, lig + i, col + j, joueur) == 0)
                            {
                                Pion *copie = f_init_plateau();
                                f_copie_plateau(ptj, copie);

                                if (f_bouge_piece(copie, lig, col, lig + i, col + j, joueur) == 0)
                                {
                                    int resultat = f_max_alphaBeta(copie, -joueur, mouvement, profondeur - 1, alpha, beta);

                                    if (resultat < value)
                                    {
                                        value = resultat;
                                        meilleur_mouvement.colonne_de_arrive = col + j;
                                        meilleur_mouvement.colonne_de_depart = col;
                                        meilleur_mouvement.ligne_de_arrive = lig + i;
                                        meilleur_mouvement.ligne_de_depart = lig;
                                    }
                                    if (alpha >= beta)
                                    {
                                        beta = value;
                                        free(copie);
                                        return value; // Coupure Beta
                                    }
                                }
                                free(copie);
                            }
                        }
                    }
                }
            }
        }
    }

    if (profondeur == 3)
    {
        *mouvement = meilleur_mouvement;
    }

    return value;
}

int f_max_alphaBeta(Pion *ptj, int joueur, dep *mouvement, int profondeur, int alpha, int beta)
{
    noeuds_explores++; // Compte le nombre de nœuds explorés
    if (profondeur == 0)
    {
        return f_eval(ptj, joueur);
    }

    int value = -INFINI;
    dep meilleur_mouvement;

    for (int lig = 0; lig < NB_LIGNES; lig++)
    {
        for (int col = 0; col < NB_COLONNES; col++)
        {
            if (ptj[lig * NB_COLONNES + col].couleur == joueur)
            {
                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {
                        if (i != 0 && j != 0)
                        {
                            if (f_test_mouvement(ptj, lig, col, lig + i, col + j, joueur) == 0)
                            {
                                Pion *copie = f_init_plateau();
                                f_copie_plateau(ptj, copie);

                                if (f_bouge_piece(copie, lig, col, lig + i, col + j, joueur) == 0)
                                {
                                    int resultat = f_min_alphaBeta(copie, -joueur, mouvement, profondeur - 1, alpha, beta);

                                    if (resultat > value)
                                    {
                                        value = resultat;
                                        meilleur_mouvement.colonne_de_arrive = col + j;
                                        meilleur_mouvement.colonne_de_depart = col;
                                        meilleur_mouvement.ligne_de_arrive = lig + i;
                                        meilleur_mouvement.ligne_de_depart = lig;
                                    }

                                    if (alpha >= beta)
                                    {
                                        alpha = value;
                                        free(copie);
                                        return value; // Coupure Alpha
                                    }
                                }
                                free(copie);
                            }
                        }
                    }
                }
            }
        }
    }

    if (profondeur == 3)
    {
        *mouvement = meilleur_mouvement;
    }

    return value;
}

/**
 * Calcule et joue le meilleur coup
 * */
// pour lancer f_max dÃ©comenter la ligne d'en dessous et commenter la suivante
void f_IA(int joueur)
{
#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

    dep mouvement;

    int valeur = 0;
    int prof = 3;
    // valeur = f_max(plateauDeJeu, joueur, &mouvement, prof);
    valeur = f_max_alphaBeta(plateauDeJeu, joueur, &mouvement, 3, -INFINI, INFINI);
    int ld = mouvement.ligne_de_depart;
    int cd = mouvement.colonne_de_depart;
    int la = mouvement.ligne_de_arrive;
    int ca = mouvement.colonne_de_arrive;

    // Affichage du coup jouÃ©
    printf("Joueur %c joue : %c%d vers %c%d\n", joueur == 1 ? 'x' : 'o', f_convert_int2char(cd), ld, f_convert_int2char(ca), la);
    f_bouge_piece(plateauDeJeu, ld, cd, la, ca, joueur);

#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
}

/**
 * Demande le choix du joueur humain et calcule le coup demandÃ©
 * */
void f_humain(int joueur)
{
    char c1, c2;
    char buffer[32];
    int l1, l2;

#ifdef DEBUG
    printf("dbg: entering %s %d\n", __FUNCTION__, __LINE__);
#endif

    printf("joueur ");
    switch (joueur)
    {
    case -1:
        printf("o ");
        break;
    case 1:
        printf("x ");
        break;
    default:
        printf("inconnu ");
    }
    printf("joue:\n");
    while (1)
    {
        fgets(buffer, 32, stdin);
        if (sscanf(buffer, "%c%i%c%i\n", &c1, &l1, &c2, &l2) == 4)
        {
            if (f_bouge_piece(plateauDeJeu, l1, f_convert_char2int(c1), l2, f_convert_char2int(c2), joueur) == 0)
                break;
        }
        fflush(stdin);
        printf("mauvais choix\n");
    }

#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif
}

int main(int argv, char *argc[])
{
    int fin = 0, mode = 0, ret, joueur = 1;
    printf("1 humain vs IA\n2 humain vs humain\n3 IA vs IA\n");
    scanf("%d", &mode);
    getchar(); // Pour consommer le retour chariot aprÃ¨s scanf

    debut_partie = clock();


    plateauDeJeu = f_init_plateau();
    while (!fin)
    {
        f_affiche_plateau(plateauDeJeu);
        if (mode == 1)
        {
            if (joueur > 0)
                f_humain(joueur);
            else
                f_IA(joueur);
        }
        else if (mode == 2)
        {
            f_humain(joueur);
        }
        else
        {
            f_IA(joueur);
        }

        if ((ret = f_gagnant()) != 0)
        {
            switch (ret)
            {
            case 1:
                f_affiche_plateau(plateauDeJeu);
                printf("joueur x gagne!\n");
                fin = 1;
                break;
            case -1:
                f_affiche_plateau(plateauDeJeu);
                printf("joueur o gagne!\n");
                fin = 1;
                break;
            }
        }
        joueur = -joueur;
    }

    // Arrête le chronomètre et calcule le temps écoulé
    fin_partie = clock();
    double temps_partie = (double)(fin_partie - debut_partie) / CLOCKS_PER_SEC;


    // Affiche les statistiques
    printf("\nStatistiques de la partie :\n");
    printf("Nombre de nœuds explorés : %lld\n", noeuds_explores);
    printf("Temps total de la partie : %.2f secondes\n", temps_partie);

    return 0;




#ifdef DEBUG
    printf("dbg: exiting %s %d\n", __FUNCTION__, __LINE__);
#endif

    return 0;
}
