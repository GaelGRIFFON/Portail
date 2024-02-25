/*

  Classe des gestion des boutons


#include "Input.h" // Déjà définie dans Global

*/

class Cellule
{
  private:
  // var
  Input cellInput;
  byte pinActiveCell;
  bool enabled;
  bool activationInversed;

  public:
  // Méthodes
  void Setup(byte pCell, byte pActiveCell, bool pActivationInversed = true, bool pInputInversed = false);
  void Enable();
  void Disable();
  bool isEnable();
  void check();
  bool getState();
  bool isLoopTrigger();
};

// Définition des méthodes:
void Cellule::Setup(byte pCell, byte pActiveCell, bool pActivationInversed = true, bool pInputInversed = false)
{
  cellInput.Setup(pCell, pInputInversed); // Bouton poussoir

  activationInversed = pActivationInversed;

  pinActiveCell = pActiveCell;
  pinMode(pinActiveCell, OUTPUT);

  Disable();
}

void Cellule::Enable()  // Active la cellule
{
  enabled = HIGH;
  digitalWrite(pinActiveCell, activationInversed ? LOW : HIGH);
}

void Cellule::Disable() // Désactive la cellule
{
  enabled = LOW;
  digitalWrite(pinActiveCell, activationInversed ? HIGH : LOW);
}

bool Cellule::isEnable() // Désactive la cellule
{
  return enabled;
}

void Cellule::check()
{
  cellInput.check();
}

bool Cellule::getState()
{
  return cellInput.getState();
}

bool Cellule::isLoopTrigger()
{
  return cellInput.isLoopTrigger();
}


