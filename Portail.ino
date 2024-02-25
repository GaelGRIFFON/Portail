#include "Global.h"
#include <EEPROM.h>

void setup() {
  Serial.begin(115200);
  delay(200);

  // Initialisation des paramètres stockés en EEPROM
  if (EEPROM.read(0)==255) { // si addresse 0 en EEPROM est vide, les données suivantes sont pré-inscrites
    EEPROM.update(0, OPENING_TIME/100);   // Temps d'ouverture
    EEPROM.update(1, CLOSING_TIME/100);   // Temps de fermeture
  }

  // Récupération des paramètres stockés en EEPROM
  OPENING_TIME = EEPROM.read(0) * 100;
  CLOSING_TIME = EEPROM.read(1) * 100;

  // Initialisation des composants
  screen.Setup();
  motor.Setup(pin_L_EN, pin_R_EN, pin_L_PWM, pin_R_PWM); // Moteur
  button1.Setup(pin_Button1); // Bouton poussoir
  endClosing.Setup(pin_endClosing); // Fin de course fermeture
  endOpening.Setup(pin_endOpening); // Fin de course ouverture
  cellClosing.Setup(pin_cellClosing, pin_ActiveCellClosing); // Cellule à la fermeture
  cellOpening.Setup(pin_cellOpening, pin_ActiveCellOpening); // Cellule à l'ouverture
  ACS1.Setup(pin_ACS1); // mesure de courant
}

void loop() {
  // Lecture des entrées digitales (avec déclenchement des trigger sur la loop en cours ou réinitialisation des trigger)
  button1.check();
  endClosing.check();
  endOpening.check();
  cellClosing.check();
  cellOpening.check();

  // Actualisation de l'écran
  screen.Display(statusText[STATUS], POSITION, ELAPSED/1000, SPEED, ACS1.getValue(), 2.123, button1.getState(), endClosing.getState(), endOpening.getState(), cellClosing.isEnable(), cellClosing.getState(), cellOpening.isEnable(), cellOpening.getState());
  
  ////////////////////////
  // Actions selon status:
  switch(STATUS){
    case FIRST_START:
    
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();
      }

      /////////////////////
      // Actions
      stepRun();

      motor.Stop();
      
      /////////////////////
      // Conditions de sortie
      
      // Attente de 5 sec pour laisser le temps de passer en mode SETTING
      if(ELAPSED > 5000){ 
        changeStatus(OPENING_WAIT);
      }

      // Si le bouton est appuyé dans les 5 secondes
      if(button1.getState() == HIGH && button1.isLoopTrigger()){
        changeStatus(SET_INIT);
      }

      /////////////////////
      // Actions de Sortie
      if(STATUS != FIRST_START){
        stepEnd();
      }
    break;
    
    case SET_INIT:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();
        
        // Activation des cellules
        cellClosing.Enable();
      }

      /////////////////////
      // Actions
        stepRun();

        // Allumage du flash
        // flash.on();

        // Position fixée à 50 pendant le réglage en attendant d'atteindre la fin de course
        POSITION = 50;

        // Calcul de la vitesse:
        SPEED = LOWSPEED;
        
        motor.Close(SPEED);

      /////////////////////
      // Conditions de sortie

      if(ELAPSED > 30000){ // MAX 30 Secondes 
        changeStatus(CLOSING_WAIT);
      }
      
      // Fin de courses à la fermeture:
      if(endClosing.getState() == HIGH && endClosing.isLoopTrigger()){
        POSITION = 0;
        changeStatus(SET_OPENING);
      }
      
      /////////////////////
      // Actions de Sortie
      if(STATUS != SET_INIT){
        stepEnd();
        
        // Désactivation des cellules
        cellClosing.Disable();
      }
    break;

    
    case SET_OPENING:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();
        
        // Activation des cellules
        cellOpening.Enable();
      }

      // Mise à jour du temps relatif
      relativeELAPSED = ELAPSED; // En mode SET, on a pas de position relative, on part systématiquement de 0

      /////////////////////
      // Actions
        stepRun();

        // Etant donné qu'on ne connait pas la durée, on ne peut pas savoir la position
        POSITION = 0; 

        // Calcul de la vitesse:
        SPEED = 0;
        
        if (ELAPSED < ACCELERATE_TIME){
          SPEED = HIGHSPEED * ELAPSED / ACCELERATE_TIME;
        }else{
          SPEED = HIGHSPEED;
        }

        motor.Open(SPEED);

      /////////////////////
      // Conditions de sortie
            
      // Si détection obstacle
      if(cellOpening.getState() == HIGH && cellOpening.isLoopTrigger()){
        changeStatus(FIRST_START);
      }

      // Si détection obstacle par surintensitée
      if(ACS1.getValue() >= SAFETY_LIMIT){
        changeStatus(FIRST_START);
      }       
      
      // Fin de courses à l'ouverture:
      if(endOpening.getState() == HIGH && endOpening.isLoopTrigger()){
        POSITION = 100;
        OPENING_TIME = ELAPSED;
        EEPROM.update(0, OPENING_TIME/100);   // Temps d'ouverture inscrit dans l'EEPROM
        changeStatus(SET_CLOSING);
      }
      
      /////////////////////
      // Actions de Sortie
      if(STATUS != SET_OPENING){
        stepEnd();

        // Désactivation des cellules
        cellOpening.Disable();
      }
    break;

    
    case SET_CLOSING:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();
        
        // Activation des cellules
        cellClosing.Enable();
      }

      // Mise à jour du temps relatif
      relativeELAPSED = ELAPSED; // En mode SET, on a pas de position relative, on part systématiquement de 0

      /////////////////////
      // Actions
        stepRun();

        // Etant donné qu'on ne connait pas la durée, on ne peut pas savoir la position, mais on sait qu'on part de 100
        POSITION = 100; 

        // Calcul de la vitesse:
        SPEED = 0;
        
        if (ELAPSED < ACCELERATE_TIME){
          SPEED = HIGHSPEED * ELAPSED / ACCELERATE_TIME;
        }else{
          SPEED = HIGHSPEED;
        }

        motor.Close(SPEED);

      /////////////////////
      // Conditions de sortie
            
      // Si détection obstacle
      if(cellClosing.getState() == HIGH && cellClosing.isLoopTrigger()){
        changeStatus(FIRST_START);
      }

      // Si détection obstacle par surintensitée
      if(ACS1.getValue() >= SAFETY_LIMIT){
        changeStatus(FIRST_START);
      }
      
      // Fin de courses à l'ouverture:
      if(endClosing.getState() == HIGH && endClosing.isLoopTrigger()){
        POSITION = 0;
        CLOSING_TIME = ELAPSED;
        EEPROM.update(1, CLOSING_TIME/100);   // Temps de fermeture inscrit dans l'EEPROM 
        changeStatus(OPENING_WAIT);
      }
      
      /////////////////////
      // Actions de Sortie
      if(STATUS != SET_CLOSING){
        stepEnd();

        // Désactivation des cellules
        cellClosing.Disable();
      }
    break;


    case OPENING_WAIT:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();

      }

      /////////////////////
      // Actions
      stepRun();

        // Moteur à l'arrêt
        motor.Stop();
                
        // Désactivation du Flash
        //flash.Off();
      
      /////////////////////
      // Conditions de sortie
      
      // Si le bouton est appuyé: 
      if(button1.getState() == HIGH && button1.isLoopTrigger()){
        changeStatus(OPENING_INIT);
      }

      /////////////////////
      // Actions de Sortie
      if(STATUS != OPENING_WAIT){
        stepEnd();
      }
    break;

    case OPENING_INIT:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();

        // Activation des cellules
        cellOpening.Enable();
      }

      /////////////////////
      // Actions
      stepRun();

        // Activation du Flash
        //flash.On();
      
      /////////////////////
      // Conditions de sortie
           
      // Si le bouton est appuyé, abandon du mouvement: 
      if(button1.getState() == HIGH && button1.isLoopTrigger()){
        cellOpening.Disable();
        changeStatus(OPENING_WAIT);
      }

      // Attente de 1 sec de flash avant de démarrer le moteur (OPENING) ET qu'il n'y ai pas d'obastacle
      if(ELAPSED > 1000 && cellOpening.getState() == LOW){ 
        changeStatus(OPENING);
      }

      // Attente de 10 sec de flash avant d'abandonner (présence d'obstacle persistant)
      if(ELAPSED > 10000){ 
        cellOpening.Disable();
        changeStatus(OPENING_WAIT);
      }

      /////////////////////
      // Actions de Sortie
      if(STATUS != OPENING_INIT){
        stepEnd();
      }
    break;
    
    case OPENING:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();

        // Initialisation du temps de mouvement relatif par rapport à la position initiale du portail
        positionTimeLapse = OPENING_TIME * POSITION / 100;
      }

      // Mise à jour du temps relatif
      relativeELAPSED = ELAPSED + positionTimeLapse;

      /////////////////////
      // Actions
        stepRun();

        // Mise à jour de la position théorique
        POSITION = min(relativeELAPSED * 100 / OPENING_TIME, 100); 

        // Calcul de la vitesse:
        SPEED = 0;
        
        if(relativeELAPSED < (OPENING_TIME - OPENING_LOWSPEED_TIME)){ // Si on est avant la période de vitesse basse avant fin de mouvement
          if (ELAPSED < ACCELERATE_TIME){ //Période d’accélération (on prend comme référence le début du mouvement, peut importe la position, donc par rapport à ELAPSED et non par rapport à relativeELAPSED)
            SPEED = HIGHSPEED * ELAPSED / ACCELERATE_TIME;
          }else{
            SPEED = HIGHSPEED;
          }
        }else if(relativeELAPSED >= (OPENING_TIME - OPENING_LOWSPEED_TIME) && relativeELAPSED < OPENING_TIME){ // Période de descelleration
          SPEED = (HIGHSPEED - LOWSPEED) * (OPENING_TIME - relativeELAPSED) / OPENING_LOWSPEED_TIME + LOWSPEED;
        }
        else if(relativeELAPSED >= OPENING_TIME && relativeELAPSED < OPENING_TIME + TIMEOUT){ // Période vitesse basse (durant le TIMEOUT)
          SPEED = LOWSPEED;
        }
        else{ // Dépassement du TIMEOUT
          SPEED = 0;
          changeStatus(CLOSING_WAIT);
        }

        motor.Open(SPEED);

      /////////////////////
      // Conditions de sortie
      
      // Si le bouton est appuyé: 
      if(button1.getState() == HIGH && button1.isLoopTrigger()){
        changeStatus(CLOSING_WAIT);
      }
      
      // Si détection obstacle
      if(cellOpening.getState() == HIGH && cellOpening.isLoopTrigger()){
        changeStatus(OPENING_EMERGENCY_STOP);
      }

      // Si détection obstacle par surintensitée
      if(ACS1.getValue() >= SAFETY_LIMIT){
        changeStatus(OPENING_EMERGENCY_STOP);
      }
      
      // Fin de courses à l'ouverture:
      if(endOpening.getState() == HIGH && endOpening.isLoopTrigger()){
        POSITION = 100;
        changeStatus(CLOSING_WAIT);
      }
      
      /////////////////////
      // Actions de Sortie
      if(STATUS != OPENING){
        stepEnd();

        // Désactivation des cellules
        cellOpening.Disable();
      }
    break;

    case OPENING_EMERGENCY_STOP:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();
      }

      /////////////////////
      // Actions
      stepRun();

        // Moteur à l'arrêt
        motor.Stop();
              
      /////////////////////
      // Conditions de sortie
      
      // Attente de 1 sec redémarrer le moteur (SLOWCLOSE)
      if(ELAPSED > 1000){ 
        changeStatus(OPENING_EMERGENCY_SLOWCLOSE);
      }

      /////////////////////
      // Actions de Sortie
      if(STATUS != OPENING_EMERGENCY_STOP){
        stepEnd();
      }
    break;
        
    case OPENING_EMERGENCY_SLOWCLOSE:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();

        // Initialisation du temps de mouvement relatif par rapport à la position initiale du portail
        positionTimeLapse = CLOSING_TIME * (100 - POSITION) / 100;
        
        // Activation des cellules
        cellClosing.Enable();
      }

      // Mise à jour du temps relatif
      relativeELAPSED = ELAPSED + positionTimeLapse;

      /////////////////////
      // Actions
        stepRun();

        // Mise à jour de la position théorique
        POSITION = max(100 - (relativeELAPSED * 100 / CLOSING_TIME), 0);

        // Calcul de la vitesse:
        SPEED = LOWSPEED;
        
        motor.Close(SPEED);

      /////////////////////
      // Conditions de sortie

      if(ELAPSED > EMERGENCY_REMOVE_TIME){ 
        changeStatus(CLOSING_WAIT);
      }

      // Si détection obstacle par surintensitée
      if(ACS1.getValue() >= SAFETY_LIMIT){
        changeStatus(CLOSING_WAIT);
      }
      
      // Fin de courses à la fermeture:
      if(endClosing.getState() == HIGH && endClosing.isLoopTrigger()){
        POSITION = 0;
        changeStatus(OPENING_WAIT);
      }
      
      /////////////////////
      // Actions de Sortie
      if(STATUS != OPENING_EMERGENCY_SLOWCLOSE){
        stepEnd();
        
        // Désactivation des cellules
        cellClosing.Disable();
      }
    break;

    case CLOSING_WAIT:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();
      }

      /////////////////////
      // Actions
      stepRun();

        // Moteur à l'arrêt
        motor.Stop();
              
        // Désactivation du Flash
        //flash.Off();


      /////////////////////
      // Conditions de sortie
      
      // Si le bouton est appuyé: 
      if(button1.getState() == HIGH && button1.isLoopTrigger()){
        changeStatus(CLOSING_INIT);
      }

      /////////////////////
      // Actions de Sortie
      if(STATUS != CLOSING_WAIT){
        stepEnd();
      }
    break;

    case CLOSING_INIT:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();
        
        // Activation des cellules
        cellClosing.Enable();
      }

      /////////////////////
      // Actions
      stepRun();

        // Activation du Flash
        // flash.on();
     
      /////////////////////
      // Conditions de sortie
      
      // Si le bouton est appuyé, abandon du mouvement: 
      if(button1.getState() == HIGH && button1.isLoopTrigger()){
        cellClosing.Disable();
        changeStatus(CLOSING_WAIT);
      }

      // Attente de 1 sec de flash avant de démarrer le moteur (CLOSING) ET qu'il n'y ai pas d'obastacle
      if(ELAPSED > 1000 && cellClosing.getState() == LOW){ 
        changeStatus(CLOSING);
      }

      // Attente de 10 sec de flash avant d'abandonner (présence d'obstacle persistant)
      if(ELAPSED > 10000){ 
        cellClosing.Disable();
        changeStatus(CLOSING_WAIT);
      }

      /////////////////////
      // Actions de Sortie
      if(STATUS != CLOSING_INIT){
        stepEnd();
      }
    break;
    
    case CLOSING:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();

        // Initialisation du temps de mouvement relatif par rapport à la position initiale du portail
        positionTimeLapse = CLOSING_TIME * (100 - POSITION) / 100;

      }

      // Mise à jour du temps relatif
      relativeELAPSED = ELAPSED + positionTimeLapse;

      /////////////////////
      // Actions
        stepRun();
      
        // Mise à jour de la position théorique
        POSITION = max(100 - (relativeELAPSED * 100 / CLOSING_TIME), 0); 

        // Calcul de la vitesse:
        SPEED = 0;
        
        if(relativeELAPSED < (CLOSING_TIME - CLOSING_LOWSPEED_TIME)){ // Si on est avant la période de vitesse basse avant fin de mouvement
          if (ELAPSED < ACCELERATE_TIME){ //Période d’accélération (on prend comme référence le début du mouvement, peut importe la position, donc par rapport à ELAPSED et non par rapport à relativeELAPSED)
            SPEED = HIGHSPEED * ELAPSED / ACCELERATE_TIME;
          }else{
            SPEED = HIGHSPEED;
          }
        }else if(relativeELAPSED >= (CLOSING_TIME - CLOSING_LOWSPEED_TIME) && relativeELAPSED < CLOSING_TIME){ // Période de descelleration
          SPEED = (HIGHSPEED - LOWSPEED) * (CLOSING_TIME - relativeELAPSED) / CLOSING_LOWSPEED_TIME + LOWSPEED;
        }
        else if(relativeELAPSED >= CLOSING_TIME && relativeELAPSED < CLOSING_TIME + TIMEOUT){ // Période vitesse basse (durant le TIMEOUT)
          SPEED = LOWSPEED;
        }
        else{ // Dépassement du TIMEOUT
          SPEED = 0;
          changeStatus(OPENING_WAIT);
        }

      motor.Close(SPEED);
      
      /////////////////////
      // Conditions de sortie
      
      // Si le bouton est appuyé: 
      if(button1.getState() == HIGH && button1.isLoopTrigger()){
        changeStatus(OPENING_WAIT);
      }
      
      // Si détection obstacle
      if(cellClosing.getState() == HIGH && cellClosing.isLoopTrigger()){
        changeStatus(CLOSING_EMERGENCY_STOP);
      }

      // Si détection obstacle par surintensitée
      if(ACS1.getValue() >= SAFETY_LIMIT){
        changeStatus(CLOSING_EMERGENCY_STOP);
      }
   
      // Fin de courses à la fermeture:
      if(endClosing.getState() == HIGH && endClosing.isLoopTrigger()){
        POSITION = 0;
        changeStatus(OPENING_WAIT);
      }

      /////////////////////
      // Actions de Sortie
      if(STATUS != CLOSING){
        stepEnd();
        
        // Désactivation des cellules
        cellClosing.Disable();
      }
    break;

    case CLOSING_EMERGENCY_STOP:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();
      }

      /////////////////////
      // Actions
      stepRun();

        // Moteur à l'arrêt
        motor.Stop();
              
      /////////////////////
      // Conditions de sortie
      
      // Attente de 1 sec redémarrer le moteur (OPENING)
      if(ELAPSED > 1000){ 
        changeStatus(CLOSING_EMERGENCY_SLOWOPEN);
      }

      /////////////////////
      // Actions de Sortie
      if(STATUS != CLOSING_EMERGENCY_STOP){
        stepEnd();
      }
    break;
        
    case CLOSING_EMERGENCY_SLOWOPEN:
      /////////////////////
      // Actions d'entrée
      if(lastSTATUS != STATUS){
        stepStart();

        // Initialisation du temps de mouvement relatif par rapport à la position initiale du portail
        positionTimeLapse = OPENING_TIME * POSITION / 100;
        
        // Activation des cellules
        cellOpening.Enable();
      }

      // Mise à jour du temps relatif
      relativeELAPSED = ELAPSED + positionTimeLapse;

      /////////////////////
      // Actions
        stepRun();

        // Mise à jour de la position théorique
        POSITION = min(relativeELAPSED * 100 / OPENING_TIME, 100); 

        // Calcul de la vitesse:
        SPEED = LOWSPEED;
        
        motor.Open(SPEED);

      /////////////////////
      // Conditions de sortie

      if(ELAPSED > EMERGENCY_REMOVE_TIME){ 
        changeStatus(OPENING_WAIT);
      }

      // Si détection obstacle par surintensitée
      if(ACS1.getValue() >= SAFETY_LIMIT){
        changeStatus(OPENING_WAIT);
      }

      // Fin de courses à l'ouverture:
      if(endOpening.getState() == HIGH && endOpening.isLoopTrigger()){
        POSITION = 100;
        changeStatus(CLOSING_WAIT);
      }
      
      /////////////////////
      // Actions de Sortie
      if(STATUS != CLOSING_EMERGENCY_SLOWOPEN){
        stepEnd();
        
        // Désactivation des cellules
        cellOpening.Disable();
      }
    break;
  }
}
