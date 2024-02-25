/*

  Classe de controle du moteur

*/

class Motor
{
  private:
  // var
  byte L_EN;
  byte R_EN;
  byte L_PWM;
  byte R_PWM;

  public:
  // Méthodes
  void Setup(byte pL_EN, byte pR_EN, byte pL_PWM, byte pR_PWM);
  void Open(byte speed);
  void Close(byte speed);
  void Stop();
};

// Définition des méthodes:
void Motor::Setup(byte pL_EN, byte pR_EN, byte pL_PWM, byte pR_PWM)
{
  L_EN = pL_EN;
  R_EN = pR_EN;
  L_PWM = pL_PWM;
  R_PWM = pR_PWM;
  
  pinMode(L_EN, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  
  digitalWrite(L_EN, LOW);
  digitalWrite(R_EN, LOW);
  digitalWrite(L_PWM, 0);
  digitalWrite(R_PWM, 0); 
}

void Motor::Open(byte speed)
{
  digitalWrite(R_EN, HIGH);  
  digitalWrite(L_EN, LOW);

  analogWrite(R_PWM, speed);
  analogWrite(L_PWM, 0);
}

void Motor::Close(byte speed)
{
  digitalWrite(R_EN, LOW);  
  digitalWrite(L_EN, HIGH);

  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, speed);
}

void Motor::Stop()
{
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 0);

  digitalWrite(R_EN, LOW);  
  digitalWrite(L_EN, LOW);
}