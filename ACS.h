/*

Classe Lecteurs de courant

*/

class ACS
{
  private:
  // var
  byte pinACS;
  int lastValue;

  public:
  // Méthodes
  void Setup(byte pACS);
  float getValue(bool getAbs = true);
};

// Définition des méthodes:
void ACS::Setup(byte pACS)
{
  lastValue = 0;
  pinACS = pACS;

  pinMode(pinACS, INPUT);
}

// Lire et moyenner
float ACS::getValue(bool getAbs = true) {
  const byte SAMPLE = 10;   // num of readings
  int ACS = 0;
  for( uint8_t i = 0; i < SAMPLE; i++ ){
    ACS += analogRead(pinACS); /* NANO : ~100 microseconds par 6xlectures */
  }

  float mesure = ACS/SAMPLE;
  float voltage = mesure*5/1023.0;
  float lastValue = (voltage-2.5)/(30/2.5);

  return getAbs ? abs(lastValue) : lastValue;
}
