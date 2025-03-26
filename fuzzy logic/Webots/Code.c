/*
 * Auteur :        Lounis Bouhadoun
 */

#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <webots/motor.h>
#include <math.h>
#include <stdio.h>

// =======================================================================
//                         Constantes et paramètres
// =======================================================================

#define DELAI_SIMULATION    32      
#define FACTEUR_VITESSE     0.6f   
#define VIT_MAX             4.0f    // Vitesse “avance” maximale
#define VIT_MIN             0.0f    // Vitesse nulle (arrêt)

#define SEUIL_RAPPROCHE     111     // Seuil “proche”
#define SEUIL_ELOIGNE       76      // Seuil “loin”

// =======================================================================
//           Fonctions d’appartenance floue
// =======================================================================

/**
 * \brief Calcule le degré d’appartenance à “proche” pour une valeur capteur.
 *
 * \param valeurCapteur La valeur brute lue par le capteur de distance.
 * \return Un flottant entre 0 et 1 représentant l’appartenance au terme “proche”.
 */
static float calculAppartenanceProche(float valeurCapteur) {
  if (valeurCapteur >= SEUIL_RAPPROCHE) {
    // Totalement proche
    return 1.0f;
  } else if (valeurCapteur > SEUIL_ELOIGNE && valeurCapteur < SEUIL_RAPPROCHE) {
    // Zone intermédiaire : calcul linéaire
    return (valeurCapteur - SEUIL_ELOIGNE) / (float)(SEUIL_RAPPROCHE - SEUIL_ELOIGNE);
  } else {
    // Trop bas : pas proche
    return 0.0f;
  }
}

/**
 * \brief Calcule le degré d’appartenance à “loin” pour une valeur capteur.
 *
 * \param valeurCapteur La valeur brute lue par le capteur de distance.
 * \return Un flottant entre 0 et 1 représentant l’appartenance au terme “loin”.
 */
static float calculAppartenanceLoin(float valeurCapteur) {
  if (valeurCapteur <= SEUIL_ELOIGNE) {
    // Totalement loin
    return 1.0f;
  } else if (valeurCapteur > SEUIL_ELOIGNE && valeurCapteur < SEUIL_RAPPROCHE) {
    // Zone intermédiaire : calcul linéaire
    return (SEUIL_RAPPROCHE - valeurCapteur) / (float)(SEUIL_RAPPROCHE - SEUIL_ELOIGNE);
  } else {
    // Trop élevé : pas loin
    return 0.0f;
  }
}

// =======================================================================
//                          Règles floues
// =======================================================================
// Chaque règle calcule deux valeurs d’activation
// en se basant sur les degrés d’appartenance de l’entrée “gauche” et de l’entrée “droite”.

/**
 * \brief Règle 1 : Si (gauche est proche) ET (droite est loin)
 *        alors : (gauche avance) ET (droite arrêt).
 *
 * \param entreeGauche Valeur capteur floue “gauche”.
 * \param entreeDroite Valeur capteur floue “droite”.
 * \param mGauche      (Sortie) degré d’activation de la composante “gauche” de cette règle.
 * \param mDroite      (Sortie) degré d’activation de la composante “droite” de cette règle.
 */
static void regleGaucheProcheDroiteLoin(float entreeGauche, float entreeDroite, float *mGauche, float *mDroite) {
  float degProcheGauche = calculAppartenanceProche(entreeGauche);
  float degLoinDroite   = calculAppartenanceLoin(entreeDroite);

  // Le degré d’activation de la règle est le minimum des deux
  float valeurActivation = fminf(degProcheGauche, degLoinDroite);

  // On met la même valeur pour les deux sorties
  *mGauche = valeurActivation;
  *mDroite = valeurActivation;
}

/**
 * \brief Règle 2 : Si (gauche est loin) ET (droite est proche)
 *        alors : (gauche arrêt) ET (droite avance).
 */
static void regleGaucheLoinDroiteProche(float entreeGauche, float entreeDroite, float *mGauche, float *mDroite) {
  float degLoinGauche   = calculAppartenanceLoin(entreeGauche);
  float degProcheDroite = calculAppartenanceProche(entreeDroite);

  float valeurActivation = fminf(degLoinGauche, degProcheDroite);

  *mGauche = valeurActivation;
  *mDroite = valeurActivation;
}

/**
 * \brief Règle 3 : Si (gauche est proche) ET (droite est proche)
 *        alors : (gauche arrêt) ET (droite avance).
 */
static void regleGaucheProcheDroiteProche(float entreeGauche, float entreeDroite, float *mGauche, float *mDroite) {
  float degProcheGauche = calculAppartenanceProche(entreeGauche);
  float degProcheDroite = calculAppartenanceProche(entreeDroite);

  float valeurActivation = fminf(degProcheGauche, degProcheDroite);

  *mGauche = valeurActivation;
  *mDroite = valeurActivation;
}

/**
 * \brief Règle 4 : Si (gauche est loin) ET (droite est loin)
 *        alors : (gauche avance) ET (droite arrêt).
 */
static void regleGaucheLoinDroiteLoin(float entreeGauche, float entreeDroite, float *mGauche, float *mDroite) {
  float degLoinGauche = calculAppartenanceLoin(entreeGauche);
  float degLoinDroite = calculAppartenanceLoin(entreeDroite);

  float valeurActivation = fminf(degLoinGauche, degLoinDroite);

  *mGauche = valeurActivation;
  *mDroite = valeurActivation;
}

// =======================================================================
//                    Fonction de défuzzification
// =======================================================================
/**
 * \brief Combine les règles floues pour déterminer la vitesse des moteurs.
 *
 * \param entreeGauche  La valeur “gauche” calculée à partir des capteurs.
 * \param entreeDroite  La valeur “droite” calculée à partir des capteurs.
 * \param moteurGauche  Identifiant du moteur gauche.
 * \param moteurDroite  Identifiant du moteur droit.
 */
static void calculVitesseFloue(float entreeGauche, float entreeDroite,
                               WbDeviceTag moteurGauche, WbDeviceTag moteurDroite) {
  // Degrés d’activation pour chaque règle
  float mG1, mD1, mG2, mD2, mG3, mD3, mG4, mD4;
  mG1 = mD1 = mG2 = mD2 = mG3 = mD3 = mG4 = mD4 = 0.0f;

  // Activation de chaque règle
  regleGaucheProcheDroiteLoin(entreeGauche,  entreeDroite, &mG1, &mD1);
  regleGaucheLoinDroiteProche(entreeGauche,  entreeDroite, &mG2, &mD2);
  regleGaucheProcheDroiteProche(entreeGauche, entreeDroite, &mG3, &mD3);
  regleGaucheLoinDroiteLoin(entreeGauche,    entreeDroite, &mG4, &mD4);

  // Sommes des activations pour gauche et droite
  float sommeGauche = mG1 + mG2 + mG3 + mG4;
  float sommeDroite = mD1 + mD2 + mD3 + mD4;

  // Évite la division par zéro
  if (sommeGauche < 0.0001f) sommeGauche = 1.0f;
  if (sommeDroite < 0.0001f) sommeDroite = 1.0f;

  // Détermine la vitesse finale pour le moteur gauche
  // - Règles 1,2,3 : gauche avance => VIT_MAX
  // - Règle 4      : gauche arrêt  => VIT_MIN
  float vitesseGauche = (
      mG1 * VIT_MAX +
      mG2 * VIT_MAX +
      mG3 * VIT_MAX +
      mG4 * VIT_MIN
  ) / sommeGauche;

  // Détermine la vitesse finale pour le moteur droit
  // - Règle 4      : droite avance => VIT_MAX
  // - Règles 1,2,3 : droite arrêt  => VIT_MIN
  float vitesseDroite = (
      mD1 * VIT_MIN +
      mD2 * VIT_MIN +
      mD3 * VIT_MIN +
      mD4 * VIT_MAX
  ) / sommeDroite;

  // Application des vitesses calculées aux moteurs
  wb_motor_set_velocity(moteurGauche,  vitesseGauche);
  wb_motor_set_velocity(moteurDroite, vitesseDroite);
}

// =======================================================================
//                         Programme principal
// =======================================================================

int main(int argc, char **argv) {
  //
  wb_robot_init();

  // ---------------------------------------------------------------------
  //            Récupération et configuration des capteurs
  // ---------------------------------------------------------------------
  WbDeviceTag capteurs[8];          // Tableau pour les 8 capteurs de distance
  char nomsCapteurs[8][4] = {
    "ps0", "ps1", "ps2", "ps3",
    "ps4", "ps5", "ps6", "ps7"
  };

  for (int i = 0; i < 8; i++) {
    capteurs[i] = wb_robot_get_device(nomsCapteurs[i]);
    wb_distance_sensor_enable(capteurs[i], DELAI_SIMULATION);
  }

  // ---------------------------------------------------------------------
  //            Récupération et configuration des moteurs
  // ---------------------------------------------------------------------
  WbDeviceTag moteurGauche  = wb_robot_get_device("left wheel motor");
  WbDeviceTag moteurDroite  = wb_robot_get_device("right wheel motor");

  // On fixe la position à l’infini pour contrôler la vitesse
  wb_motor_set_position(moteurGauche,  INFINITY);
  wb_motor_set_position(moteurDroite, INFINITY);

  // ---------------------------------------------------------------------
  // ---------------------------------------------------------------------
  while (wb_robot_step(DELAI_SIMULATION) != -1) {
    // Lecture brute des capteurs
    float capteur0 = wb_distance_sensor_get_value(capteurs[0]);
    float capteur1 = wb_distance_sensor_get_value(capteurs[1]);
    float capteur2 = wb_distance_sensor_get_value(capteurs[2]);
    // 
    float capteur5 = wb_distance_sensor_get_value(capteurs[5]) * 0.7f;
    float capteur6 = wb_distance_sensor_get_value(capteurs[6]);
    float capteur7 = wb_distance_sensor_get_value(capteurs[7]);

    // -------------------------------------------------------------------
    //        Construction des entrées floues “gauche” et “droite”
    // -------------------------------------------------------------------
    //  - “gauche” : on prend le max parmi capteurs ps5, ps6, ps7
    //  - “droite” : on prend le max parmi capteurs ps0, ps1, ps2
    float entreeGauche = fmaxf(capteur7, fmaxf(capteur6, capteur5));
    float entreeDroite = fmaxf(capteur0, fmaxf(capteur1, capteur2));

    // Déclenche la défuzzification
    calculVitesseFloue(entreeGauche, entreeDroite, moteurGauche, moteurDroite);

    //
    printf("Itération en cours...\n");
  }

  // Nettoyage et fin du programme
  wb_robot_cleanup();
  return 0;
}
