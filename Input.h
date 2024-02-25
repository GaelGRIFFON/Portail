/*

  Classe des gestion des boutons

*/

class Input
{
  private:
  // var
  byte pinInput;
  bool state;
  bool loopTrigger;
  long lastStateMillis;
  bool inversed;
  void onEvent();

  public:
  // Méthodes
  void Setup(byte pInput, bool inversed = false);
  void check();
  bool getState();
  bool isLoopTrigger();
};

// Définition des méthodes:
void Input::Setup(byte pInput, bool pinversed = false)
{
  lastStateMillis = 0;
  pinInput = pInput;
  loopTrigger = false;
  inversed = pinversed;

  pinMode(pinInput, INPUT_PULLUP);
}

void Input::check()
{
  // Lecture de l'état, avec gestion de l'inversion si besoin
  int readValue = digitalRead(pinInput);
  int currentState = inversed ? !readValue : readValue;

  // Réinitialisation du triger
  loopTrigger = false;
  
  if(currentState != state && (millis() - lastStateMillis) > 200){ // changement d'état & debounce
    // Triger levé sur la boucle en cours (sera réinitialisé à la prochaine loop)
    loopTrigger = true;

    // Mise à jour du state
    state = currentState;
    lastStateMillis = millis();
  }
}

bool Input::getState()
{
  return state;
}

bool Input::isLoopTrigger()
{
  return loopTrigger;
}
