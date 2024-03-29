/*

  Classe de controle de l'écran

*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

class Screen
{
  public:
  // Méthodes
  void Setup();
  void Display(char * pstatus, byte pposition, byte pmvtTimer, byte pPWM, float pCurrent1, float pCurrent2, bool pButton, bool pOpenEnd, bool pCloseEnd, bool pClosingCellEnable, bool pClosingCell, bool pOpeningCellEnable, bool pOpeningCell, bool pFlash);
};

// Définition des méthodes:
void Screen::Setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
}

void Screen::Display(char * pstatus, byte pposition, byte pmvtTimer, byte pPWM, float pCurrent1, float pCurrent2, bool pButton, bool pOpenEnd, bool pCloseEnd, bool pClosingCellEnable, bool pClosingCell, bool pOpeningCellEnable, bool pOpeningCell, bool pFlash)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Statut:
  display.setCursor(0, 0);
  display.print(pstatus);
  
  // PWM Moteur:
  display.setCursor(32, 8);
  display.print("PWM:" + String(pPWM));

  // Timer du Mvt:
  display.setCursor(104, 8);
  display.print(String(pmvtTimer) + "s");

  // Position:
  display.setCursor(104, 16);
  display.print(String(pposition) + "%");
  display.drawLine(0, 16, 0, 22, SSD1306_WHITE);
  display.drawLine(0, 22, 50, 22, SSD1306_WHITE);
  display.drawRect(50, 16, 50, 7, SSD1306_WHITE);
  display.fillRect(pposition/2, 18, 50, 3, SSD1306_WHITE);

  // Fin de course gauche (ouverture)
  if(pOpenEnd){
    display.fillCircle(2, 12, 2, SSD1306_WHITE); 
  }else{
    display.drawCircle(2, 12, 2, SSD1306_WHITE);
  }
  
  // Fin de course gauche (fermeture)
  if(pCloseEnd){
    display.fillCircle(97, 12, 2, SSD1306_WHITE); 
  }else{
    display.drawCircle(97, 12, 2, SSD1306_WHITE);
  }
  
  // Cellule Fermeture
  if(pClosingCellEnable){
    display.fillRect(9, 11, 3, 5, SSD1306_WHITE);
    display.fillRect(25, 11, 3, 5, SSD1306_WHITE);
    display.drawLine(11, 13, 25, 13, SSD1306_WHITE);
  }else{
    display.drawRect(9, 11, 3, 5, SSD1306_WHITE);
    display.drawRect(25, 11, 3, 5, SSD1306_WHITE);
  }

  if(pClosingCell){
    display.fillCircle(18, 13, 2, SSD1306_WHITE); 
  }

  // Flash
  if(pFlash){
    display.fillCircle(5, 28, 2, SSD1306_WHITE); 

    display.drawPixel(1, 28, SSD1306_WHITE);
    display.drawPixel(9, 28, SSD1306_WHITE);
    display.drawPixel(5, 24, SSD1306_WHITE);
    display.drawPixel(5, 31, SSD1306_WHITE);
    
    display.drawPixel(2, 24, SSD1306_WHITE);
    display.drawPixel(2, 31, SSD1306_WHITE);

    display.drawPixel(8, 24, SSD1306_WHITE);
    display.drawPixel(8, 31, SSD1306_WHITE);
  }else{
    display.drawCircle(5, 28, 2, SSD1306_WHITE); 
  }
    
  // Cellule Ouverture
  if(pOpeningCellEnable){
    display.fillRect(74, 11, 3, 5, SSD1306_WHITE);
    display.fillRect(90, 11, 3, 5, SSD1306_WHITE);
    display.drawLine(76, 13, 90, 13, SSD1306_WHITE);
  }else{
    display.drawRect(74, 11, 3, 5, SSD1306_WHITE);
    display.drawRect(90, 11, 3, 5, SSD1306_WHITE);
  }

  if(pOpeningCell){
    display.fillCircle(83, 13, 2, SSD1306_WHITE); 
  }
    
  // Courant:
  display.setCursor(18, 24);
  // Conversion en Ampère
  display.print(String(pCurrent1,3) + "A " + String(pCurrent2,3) + "A");

  // Bouton:
  display.setCursor(100, 24);
  display.print("Bt:" + String(pButton));

  display.display();      // Show initial text
}