#include "Motor.h"
#include "Input.h"
#include "CelluleControler.h"
#include "ACS.h"
#include "Screen.h"

/*
Déclaration des valeurs globales
*/

/*
Résumé attribution des PIN:
Boutton:
2

Fins de course:
3
4

Controle Moteur:
5
6
7
8

Cellules:
9
10
11
12

ACS:
A2
*/

/////////////////
// Différents statut du portail durant son mouvement
  char* statusText[] = {"FIRST_START", 
                        "OPENING_WAIT", 
                        "OPENING_INIT", 
                        "OPENING", 
                        "OPENING_E_STOP", 
                        "OPENING_E_SLOWCLOSE",
                        "CLOSING_WAIT",
                        "CLOSING_INIT",
                        "CLOSING", 
                        "CLOSING_E_STOP", 
                        "CLOSING_E_SLOWOPEN",
                        "SET_INIT",
                        "SET_OPENING",
                        "SET_CLOSING"
                        };
  
  const byte FIRST_START = 0;

  const byte OPENING_WAIT = 1;
  const byte OPENING_INIT = 2;
  const byte OPENING = 3;
  const byte OPENING_EMERGENCY_STOP = 4;
  const byte OPENING_EMERGENCY_SLOWCLOSE = 5;

  const byte CLOSING_WAIT = 6;
  const byte CLOSING_INIT = 7;
  const byte CLOSING = 8;
  const byte CLOSING_EMERGENCY_STOP = 9;
  const byte CLOSING_EMERGENCY_SLOWOPEN = 10;
  
  const byte SET_INIT = 11;
  const byte SET_OPENING = 12;
  const byte SET_CLOSING = 13;

  // Initialisation au démarrage sur OPEN_WAITING 
  byte STATUS = FIRST_START;
  byte lastSTATUS = STATUS;

  void changeStatus(byte newStatus){
    STATUS = newStatus;

    Serial.println(String(statusText[lastSTATUS]) + " --> " + String(statusText[STATUS]));
  }

  byte POSITION = 0; // Position en %

/////////////////
// Timings (ms)
  long OPENING_TIME = 14000; // Durée d'ouverture totale théorique
  long CLOSING_TIME = 16000;  // Durée de fermeture totale théorique
  long OPENING_LOWSPEED_TIME = 4000;  // Durée du ralentissement (le ralentissement démarre à OPEN_LAPSE - SLOW_LAPSE)
  long CLOSING_LOWSPEED_TIME = 4000;  // Durée du ralentissement (le ralentissement démarre à OPEN_LAPSE - SLOW_LAPSE)
  long ACCELERATE_TIME = 2000; // Durée de l'accélération
  long EMERGENCY_REMOVE_TIME = 1500; // Durée de mouvement inverse en cas d'EMERGENCY_STOP
  long TIMEOUT = 4000;  // Durée d'attente de l'atteinte de la fin de course

  // Timing des étapes
  long ELAPSED = 0; // Durée écoulée sur l'action en cours
  long TIMESTAMP = 0; // Timestamp (millis()) utilisé pour calculer le ELAPSED
  long relativeELAPSED = 0; // Durée relative qui sera calculée en prenant en compte la position du portail au début du mouvement (positionTimeLapse)
  long positionTimeLapse = 0; // TimeLapse correspondant à la position du portail en début de mouvement (initialisé au stepStart)


  void stepStart(){
    Serial.println("Step Start");
    TIMESTAMP = millis();
    ELAPSED = 0;
  }

  void stepRun(){
    lastSTATUS = STATUS;
    ELAPSED = millis() - TIMESTAMP;
  }

  void stepEnd(){
    ELAPSED = 0;
    
    Serial.println("Step End");
  }

/////////////////
// SECURITY
  byte SAFETY_LIMIT = 10; // Surcharge max acceptable

/////////////////
// Ecran
  Screen screen;

/////////////////
// MOTEUR
  const uint8_t pin_L_EN = 7; // D7
  const uint8_t pin_R_EN = 8; // D8
  const uint8_t pin_L_PWM = 5; // D5
  const uint8_t pin_R_PWM = 6; // D6

  Motor motor;

  // Vitesses
  byte HIGHSPEED = 100;
  byte LOWSPEED = 40;
  byte SPEED = 0;

/////////////////
// Boutons
  const uint8_t pin_Button1 = 2; // D2 Interrupt

  Input button1;

/////////////////
// Cellules
  const uint8_t pin_cellClosing = 9; // D9 - cellules au niveau des poteaux (passage véhicule/personne)
  const uint8_t pin_ActiveCellClosing = 10; // D10

  Cellule cellClosing;

  const uint8_t pin_cellOpening = 11; // cellules au niveau du rail du portail
  const uint8_t pin_ActiveCellOpening = 12;

  Cellule cellOpening;


/////////////////
// Fin de course
  const uint8_t pin_endClosing = 3; // D3
  const uint8_t pin_endOpening = 4; // D3

  Input endClosing;
  Input endOpening;

/////////////////
// ACS (mesure courant)
  const uint8_t pin_ACS1 = A2;

  ACS ACS1;
