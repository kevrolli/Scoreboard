#include <arduino.h>
#include <Wire.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <TM1637Display.h>

int ersteZahl = 99;
int zweiteZahl = 99;
int dritteZahl = 99;
int Platzhalter = 0;

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Keypad pins verbunden mit den I2C-Expander Pins P0-P6
byte rowPins[ROWS] = {13, 12, 11, 10}; // verbindet die Reihen Pins vom Keypad
byte colPins[COLS] = {9, 8, 7}; // verbindet die Kolonen Pins vom Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

//4 Digits 7 Segment einrichten
#define Spieler1CLK  0    //Spieler 1 Display CLK verbunden mit UNO pin 0
#define Spieler1DIO  1    //Spieler 1 Display DIO verbunden mit UNO pin 1
#define Spieler2CLK  5    //Spieler 2 Display CLK verbunden mit UNO pin 5
#define Spieler2DIO  6    //Spieler 2 Display DIO verbunden mit UNO pin 6

TM1637Display Spieler1Display = TM1637Display(Spieler1CLK, Spieler1DIO);    //Bibliothek für Spieler 1 Display
TM1637Display Spieler2Display = TM1637Display(Spieler2CLK, Spieler2DIO);    //Bibliothek für Spieler 2 Display


// Drehknopf einrichten
#define DrehknopfCLK 2    //Drehknopf CLK verbunden mit UNO interrupt 2
#define DrehknopfDT  3    //Drehknopf CLK verbunden mit UNO interrupt 3
#define DrehknopfSW  4    //Drehknopf CLK verbunden mit UNO interrupt 4

// LED Anzeige Spieler
#define Spielerled1 A1    //Grüne LED verbunden mit uno A1
#define Spielerled2 A2    //Grüne LED verbunden mit uno A2

// verschidene Variablen definieren
int Spielstart = 1;               //1= Display startet menü: 0= Spiel wird gestartet
int Spielstartstand = 101;        //Spiel startet immer bei 101
int Spieler1Stand = 0;            //Speicher von Spieler 1 aktuellem Stand
int Spieler2Stand = 0;            //Speicher von Spieler 2 aktuellem Stand
int Spielzug = 1;                 //Spieler 1 startet als erstes
int Spielreset = 0;               //0= nein, 1= Disply neustart
int VerlassenJa = 2;              //Neustarten vom Spiel
int VerlassenNein = 2;            //Nicht neustarten vom Spiel

//Interrupt routine auf Pin2 (wenn Interruppt 0) läuft wenn Drehknopf CLK Pin Wert verändert
void Drehknopferkennt() {
  delay(10); // delay für Debouncing vom drehknopf

  if (Spielreset == 1) { //Wenn man in Spiel Reset Menü ist kontrolliert der Drehknopf den Ja/Nein Text
    if (digitalRead(DrehknopfCLK)) {
      if (digitalRead(DrehknopfDT)) {
        VerlassenNein = 1;
        VerlassenJa = 0;

      }
      if (!digitalRead(DrehknopfDT)) {
        VerlassenJa = 1;
        VerlassenNein = 0;
      }
    }
  }

  if (Spielreset == 0) { // Nicht im Spielreset Menü
    if (digitalRead(DrehknopfCLK)) {
      if (digitalRead(DrehknopfDT)) {
        if (Spielstart == 1) {
          if (Spielstartstand > 101) {
            Spielstartstand = Spielstartstand - 100;
          }
        }
      }
      if (!digitalRead(DrehknopfDT)) {
        if (Spielstart == 1) {
          if (Spielstartstand < 9901) {
            Spielstartstand = Spielstartstand + 100;
          }
        }
      }
    }
  }
}

void setup() {
  pinMode (Spielerled1, OUTPUT);
  pinMode (Spielerled2, OUTPUT);
  pinMode (DrehknopfSW, INPUT);
  digitalWrite(DrehknopfSW, INPUT_PULLUP); //Drehknopf verwendet internen PullUp Wiederstand
  lcd.begin (16, 2); //verwendetes LCD ist 16x2 gross



  lcd.setBacklight(HIGH);

  // 7Segment Display Helligkeit
  ////Spieler1Display.setBrightness(0x09);
  ////Spieler2Display.setBrightness(0x09);

  // Textanzeige beim aufstarten der Box
  for (int x = 0; x < 3; x++) {
    lcd.init();                      // initialize the lcd

    // Print a message to the LCD.
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Diplomarbeit");
    lcd.setCursor(0, 1);
    lcd.print("von Kevin Rolli");
    delay(1000); //für 1 Sekunde
    lcd.clear();
    delay(10); // 0,5 Sekunden
  }

  // Das Interrupt zum Drehknopf hinzufügen
  attachInterrupt (0, Drehknopferkennt, CHANGE); //Interrupt 0 ist immer mit dem Pin 2 vom Uno verbunden
}

void loop() {

  char Nummergedrueckt = keypad.getKey(); //wenn Zahl gedrückt wird der Wert aufgenommen
  if (Nummergedrueckt != NO_KEY) { //Wenn Zahl gedrückt dann wird sie überprüft
    switch (Nummergedrueckt) {

      case '1':
        checknumber(1);
        break;

      case '2':
        checknumber(2);
        break;

      case '3':
        checknumber(3);
        break;

      case '4':
        checknumber(4);
        break;

      case '5':
        checknumber(5);
        break;

      case '6':
        checknumber(6);
        break;

      case '7':
        checknumber(7);
        break;

      case '8':
        checknumber(8);
        break;

      case '9':
        checknumber(9);
        break;

      case '0':
        checknumber(0);
        break;

      case '*':
        loeschen();
        break;

      case '#':
        berechnen();
        break;

    }
  }

  if (!(digitalRead(DrehknopfSW))) { //wird ausgeführt wenn Drehknopf gedrückt wird
    delay(250); // Für debouncing vom Knopf
    if (Spielstart == 0) { //Spiel hat gestartet
      if (Spielreset == 0) { //Resetfunktion ist nicht aktiv
        Spielreset = 1; //Display reset screen
      }
      if (Spielreset == 1 && VerlassenNein == 1) { //Spiel ist im Resetmodus und Nein wird ausgewählt
        Spielreset = 0; // Spielreset wird auf 0 gesetzt
        VerlassenNein = 2; //Reset Nein
        VerlassenJa = 2;
      }
      if (Spielreset == 1 && VerlassenJa == 1) {
        softReset();
      }
    }
    if (Spielstart == 1) { //Spiel ist bei der Auswähl des Startscores
      Spielstart = 0; //Spiel startet
    }
  }

  if (Spielstart == 1) { //LED Anzeige Spieler 1+2 leuchten
    digitalWrite(Spielerled1, HIGH); //Led von Spieler 1 leuchtet
    digitalWrite(Spielerled2, HIGH); //Led von Spieler 2 leuchtet


    lcd.setCursor(0, 0);
    lcd.print("Start Score");
    lcd.setCursor(0, 1);
    lcd.print(Spielstartstand);
    lcd.print("     ");

    //7Segmente zeigen gleichen Start Score an

    Spieler1Display.setBrightness(1);
    Spieler2Display.setBrightness(1);
    Spieler1Display.showNumberDec(Spieler1Stand, false, 4, 0);
    Spieler2Display.showNumberDec(Spieler2Stand, false, 4, 0);
    Spieler1Stand = Spielstartstand;
    Spieler2Stand = Spielstartstand;


  }

  if (Spielstart == 0) {
    if (Spielreset == 0) { //nicht in Resetmodus
      if (Spielzug == 1) { //Spieler 1 ist am Zug
        digitalWrite(Spielerled1, HIGH);
        digitalWrite(Spielerled2, LOW);
        lcd.setCursor(0, 0);
        lcd.print("Spieler 1:        ");
        lcd.setCursor (0, 1);
        lcd.print(Spieler1Stand);
        lcd.print("  Minus  ");
      }
      else { //Spieler 2 ist am Zug
        digitalWrite(Spielerled2, HIGH);
        digitalWrite(Spielerled1, LOW);
        lcd.setCursor(0, 0);
        lcd.print("Spieler 2:        ");
        lcd.setCursor (0, 1);
        lcd.print(Spieler2Stand);
        lcd.print("  Minus  ");
      }
    }
  }

  if (Spielreset == 1) {
    if (VerlassenNein == 1) { //Spiel verlassen Nein auf dem Bildschirm
      lcd.setCursor (0, 0);
      delay (1000);
      lcd.clear();
      lcd.print("Spiel verlassen?    ");
      lcd.setCursor (0, 1);
      lcd.print ("Nein!       ");
    } else {
      if (VerlassenJa == 1) { // Spiel verlassen Ja auf dem Bildschirm
        lcd.setCursor(0, 0);
        lcd.print("Spiel verlassen?   ");
        lcd.setCursor(0, 1);
        lcd.print("             Ja!");
      } else {
        if (VerlassenJa == 2) { //Ja und Nein wird angezeigt
          lcd.setCursor(0, 0);
          lcd.print("Spiel verlassen?     ");
          lcd.setCursor(0, 1);
          lcd.print ("Nein!        Ja!");
        }
      } delay(50);
    }
  }
}




void softReset () {
  asm volatile ("  jmp 0");   //.......
}


void checknumber (int x) {
  if (ersteZahl == 99) {
    ersteZahl = x;
    String displaygehalt = String(ersteZahl);
    lcd.setCursor (13, 1);
    lcd.print (displaygehalt);
  } else {
    if (zweiteZahl == 99) {
      zweiteZahl = x;
      String displaygehalt = (String(ersteZahl) + String(zweiteZahl));
      lcd.setCursor (13, 1);
      lcd.print(displaygehalt);
    } else {
      dritteZahl = x;
      String displaygehalt = (String(ersteZahl) + String(zweiteZahl) + String(dritteZahl));
      lcd.setCursor(13, 1);
      lcd.print(displaygehalt);

    }
  }
}

void loeschen() {//Wenn falsche Zahl gedrückt wurde kann man sie durch die *-Taste löschen
  if (dritteZahl != 99) {
    String displaygehalt = (String(ersteZahl) + String(zweiteZahl));
    lcd.setCursor(13, 1);
    lcd.print("   ");
    lcd.setCursor(13, 1);
    lcd.print(displaygehalt);
    dritteZahl = 99;
  } else {

    if (zweiteZahl != 99) {
      String displaygehalt = String(ersteZahl);

      lcd.setCursor(13, 1);
      lcd.print("   ");
      lcd.setCursor(13, 1);
      lcd.print(displaygehalt);
      zweiteZahl = 99;
    } else {

      if (ersteZahl != 99) {
        String displaygehalt = "";
        lcd.setCursor(13, 1);
        lcd.print("   ");
        lcd.setCursor(13, 1);
        lcd.print(displaygehalt);
        ersteZahl = 99;
      }
    }
  }
}

void berechnen() {//Berechnen des Aktuellen Spielstandes und wechsel der Spieler wenn sie #-Taste gedrückt wurde
  Spieler1Display.setBrightness(1);
  Spieler2Display.setBrightness(1);

  if (Spielzug == 1) {
    if (dritteZahl == 99 && zweiteZahl == 99 && ersteZahl != 99) {
      Platzhalter = ersteZahl;
      if (Spieler1Stand - Platzhalter >= 0) {
        Spieler1Stand = Spieler1Stand - Platzhalter;
      }
      Spielzug = 2;
      Spieler1Display.showNumberDec(Spieler1Stand, false, 4, 0);
    }
    if (zweiteZahl != 99 && dritteZahl == 99) {
      Platzhalter = (ersteZahl * 10) + zweiteZahl;
      if (Spieler1Stand - Platzhalter >= 0) {
        Spieler1Stand = Spieler1Stand - Platzhalter;
      }
      Spielzug = 2;
      Spieler1Display.showNumberDec(Spieler1Stand, false, 4, 0);
    }
    if (dritteZahl != 99) {
      Platzhalter = (ersteZahl * 100) + (zweiteZahl * 10) + dritteZahl;
      if (Spieler1Stand - Platzhalter >= 0) {
        Spieler1Stand = Spieler1Stand - Platzhalter;
      }
      Spielzug = 2;
      Spieler1Display.showNumberDec(Spieler1Stand, false, 4, 0);
    }
  } else {
    if (dritteZahl == 99 && zweiteZahl == 99 && ersteZahl != 99) {
      Platzhalter = ersteZahl;
      if (Spieler2Stand - Platzhalter >= 0) {
        Spieler2Stand = Spieler2Stand - Platzhalter;
      }
      Spielzug = 1;
      Spieler2Display.showNumberDec(Spieler2Stand, false, 4, 0);
    }
    if (zweiteZahl != 99 && dritteZahl == 99) {
      Platzhalter = (ersteZahl * 10) + zweiteZahl;
      if (Spieler2Stand - Platzhalter >= 0) {
        Spieler2Stand = Spieler2Stand - Platzhalter;
      }
      Spielzug = 1;
      Spieler2Display.showNumberDec(Spieler2Stand, false, 4, 0);
    }
    if (dritteZahl != 99) {
      Platzhalter = (ersteZahl * 100) + (zweiteZahl * 10) + dritteZahl;
      if (Spieler2Stand - Platzhalter >= 0) {
        Spieler2Stand = Spieler2Stand - Platzhalter;
      }
      Spielzug = 1;
      Spieler2Display.showNumberDec(Spieler2Stand, false, 4, 0);
    }
  }
  lcd.clear();
  
  if (Spieler1Stand == 0)
    for (int x = 0; x < 3; x++) {
      lcd.init();                      // initialize the lcd
      lcd.setCursor(0, 0);
      lcd.print("Spieler 1");
      lcd.setCursor(0, 1);
      lcd.print("hat gewonnen!");
      delay(1000); //für 1 Sekunde
      lcd.clear();
      delay(10); // 0,5 Sekunden
    }


  if (Spieler2Stand == 0)              //Anzeige dass Spieler 2 gewonnen hat
    for (int x = 0; x < 3; x++) {
      lcd.init();                      //LCD Display initialisieren
      lcd.setCursor(0, 0);             //Cursor auf Zeile 0 Position 0 setzen
      lcd.print("Spieler 2");          //Die Anzeige "Spieler 2" wiedergeben
      lcd.setCursor(0, 1);             //Cursor auf Zeile 0 Position 0 setzen
      lcd.print("hat gewonnen!");      //Die Anzeige "hat gewonnen" wiedergeben
      delay(1000);                     //Verzögerung von 1 Sekunde
      lcd.clear();                     //Display Anzeige löschen
      delay(10);                       //Verzögerung von 0,1 Sekunden
    }
  resetnumbers();
  lcd.setCursor(13, 1);
  lcd.print("   ");
}

void resetnumbers() //Alle Zahlen zurücksetzen
{
  ersteZahl = 99;
  zweiteZahl = 99;
  dritteZahl = 99;
  Platzhalter = 0;
}
