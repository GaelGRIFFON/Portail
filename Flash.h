/*

  Classe des gestion du flash

*/

class Flash
{
  private:
  // var
  byte pinFlash;
  bool state;
  long lastMillisState;
  int cycle;
  bool inversed;

  public:
  // Méthodes
  void Setup(byte pPinFlash, int pCycle = 500, bool pInversed = true);
  bool getState();
  void Off();
  void On();
};

// Définition des méthodes:
void Flash::Setup(byte pPinFlash, int pCycle = 500, bool pInversed = true)
{
  inversed = pInversed;
  cycle = pCycle;

  pinFlash = pPinFlash;
  pinMode(pinFlash, OUTPUT);

  Off();
}

bool Flash::getState() // Désactive la cellule
{
  return state;
}

void Flash::Off() // Désactive la cellule
{
  state = LOW;
  digitalWrite(pinFlash, inversed ? HIGH : LOW);
}

void Flash::On()
{
  if(millis() - lastMillisState > cycle) {
    lastMillisState = millis();   
       
    state = !state;

    digitalWrite(pinFlash, inversed ? !state : state);
  }
}