/***
       __   __  _______  __    _  ___   _  ___      _______  _______  ___   _
      |  |_|  ||       ||  |  | ||   | | ||   |    |       ||       ||   | | |
      |       ||   _   ||   |_| ||   |_| ||   |    |   _   ||       ||   |_| |
      |       ||  | |  ||       ||      _||   |    |  | |  ||       ||      _|
      |       ||  |_|  ||  _    ||     |_ |   |___ |  |_|  ||      _||     |_
      | ||_|| ||       || | |   ||    _  ||       ||       ||     |_ |    _  |
      |_|   |_||_______||_|  |__||___| |_||_______||_______||_______||___| |_|

      Cistercian Monk's cypher digital clock.

      Danjovic 2021 - danjovic@hotmail.com

      Versions:
      0.1   07/03/2021 - Basic Release

*/

//         _               _
//    _ __(_)_ _  ___ _  _| |_
//   | '_ \ | ' \/ _ \ || |  _|
//   | .__/_|_||_\___/\_,_|\__|
//   |_|

// Columns
#define _C1 13
#define _C2 12
#define _C3 11
#define _C4 10
#define _C5 9

// Rows
#define _R1 3
#define _R2 5
#define _R3 2
#define _R4 4
#define _R5 6
#define _R6 8
#define _R7 7

// Butons
#define BUTTON1 A2
#define BUTTON2 A3

#define ANODE 0
#define CATHODE 1

#define COMMON ANODE


//    _ _ _                 _
//   | (_) |__ _ _ __ _ _ _(_)___ ___
//   | | | '_ \ '_/ _` | '_| / -_|_-<
//   |_|_|_.__/_| \__,_|_| |_\___/__/
//


#include <avr/pgmspace.h>
#include "Wire.h"
#include "uRTCLib.h"
#include "RTClib.h"


//       _      __ _      _ _   _
//    __| |___ / _(_)_ _ (_) |_(_)___ _ _  ___
//   / _` / -_)  _| | ' \| |  _| / _ \ ' \(_-<
//   \__,_\___|_| |_|_||_|_|\__|_\___/_||_/__/
//

#define DEBUG 1

#define PULSE      4  // bit 4
#define RELEASE    2  // bit 2
#define PRESS      1  // bit 1
#define LONGPRESS  0  // bit 0

#define THRESHOLD  10     // 10  x ~4ms = ~ 40ms
#define TIMELONGPRESS 125 // 125 x ~4ms = ~ 0.5 seconds

#define DISPLAY_TIMEOUT_3SECONDS 732

enum {  DISPLY = 0,
        SETUP,
        BADFACE
     };

enum {  SHOW_TIME = 0,
        SHOW_DAY,
        SHOW_MONTH,
        SHOW_YEAR
     };

enum { SETUP_HOUR = 0,
       SETUP_MINUTE,
       SETUP_DAY,
       SETUP_MONTH,
       SETUP_YEAR
     };

uint8_t hora = 0,
        minuto = 0,
        dia = 1,
        mes = 1;

uint16_t ano = 2021;


//                   _            _
//    __ ___ _ _  __| |_ __ _ _ _| |_ ___
//   / _/ _ \ ' \(_-<  _/ _` | ' \  _(_-<
//   \__\___/_||_/__/\__\__,_|_||_\__/__/
//                          0     1     2     3     4     5     6     7     8     9
uint8_t bmpThousC0[10] = {0x00, 0x40, 0x10, 0x10, 0x40, 0x40, 0x70, 0x70, 0x70, 0x70 };
uint8_t bmpThousC1[10] = {0x00, 0x40, 0x10, 0x20, 0x20, 0x60, 0x00, 0x40, 0x10, 0x50 };

uint8_t bmpTensC0 [10] = {0x00, 0x01, 0x04, 0x04, 0x01, 0x01, 0x07, 0x07, 0x07, 0x07 };
uint8_t bmpTensC1 [10] = {0x00, 0x01, 0x04, 0x02, 0x02, 0x03, 0x00, 0x01, 0x04, 0x05 };

//uint8_t bmpHundsC3[10] = {0x00, 0x40, 0x10, 0x40, 0x10, 0x50, 0x00, 0x40, 0x10, 0x50 }; // same as bmpThousC1
//uint8_t bmpHundsC4[10] = {0x00, 0x40, 0x10, 0x20, 0x20, 0x60, 0x70, 0x70, 0x70, 0x70 }; // same as bmpThousC0
//uint8_t bmpUnitsC3[10] = {0x00, 0x01, 0x04, 0x01, 0x04, 0x05, 0x00, 0x01, 0x04, 0x05 }; // same as bmpTensC1
//uint8_t bmpUnitsC4[10] = {0x00, 0x01, 0x04, 0x02, 0x02, 0x03, 0x07, 0x07, 0x07, 0x07 }; // same as bmpTensC0




//                 _      _    _
//   __ ____ _ _ _(_)__ _| |__| |___ ___
//   \ V / _` | '_| / _` | '_ \ / -_|_-<
//    \_/\__,_|_| |_\__,_|_.__/_\___/__/
//
DateTime now;
RTC_DS3231 rtc;

uint8_t modeButtonEvent, setButtonEvent;
uint8_t setupMode = SETUP_HOUR;
uint8_t displayMode = SHOW_TIME;
uint16_t displayTimeout = DISPLAY_TIMEOUT_3SECONDS;
volatile bool readyToGo = false;
volatile uint8_t operationMode = DISPLY;
volatile uint8_t pixMap[5];  // pixel map
/*
   pixpmap [0][1][2][3][4] columns
   bit0 [0] O  O  O  O  O
   bit1 [0] O  O  O  O  O
   bit2 [0] O  O  O  O  O
   bit3 [0] O  O  O  O  O
   bit4 [0] O  O  O  O  O
   bit5 [0] O  O  O  O  O
   bit6 [0] O  O  O  O  O
   bit7 [0] -  -  -  -  -
  rows
*/



//    _     _                         _
//   (_)_ _| |_ ___ _ _ _ _ _  _ _ __| |_ ___
//   | | ' \  _/ -_) '_| '_| || | '_ \  _(_-<
//   |_|_||_\__\___|_| |_|  \_,_| .__/\__/__/
//                              |_|

ISR (TIMER2_OVF_vect )  {  // Timer2 interrupt, every 128us
#define BLINK_ON_COUNTS   3125
#define BLINK_OFF_COUNTS  2343
#define BLINK_TOTAL BLINK_ON_COUNTS + BLINK_OFF_COUNTS

  static uint16_t blinkCounter = BLINK_TOTAL;
  static uint8_t tickCounter = 32;

  static uint8_t line = 0;
  static uint8_t column = 0;
  bool blinkActive;
  uint8_t temp;
  const uint8_t columnPin [5] = { _C1, _C2, _C3, _C4, _C5 };

  // process main task counter
  tickCounter--;
  if (tickCounter == 0) {
    tickCounter = 32;
    readyToGo = true;
  }

  // process blink if required
  if ( operationMode == SETUP ) {
    blinkCounter--;
    if (blinkCounter == 0 ) blinkCounter = BLINK_TOTAL;

    if (blinkCounter < BLINK_OFF_COUNTS)
      blinkActive = true;
    else
      blinkActive = false;
  } else
    blinkActive = true;  // always show display when not in setup mode

#if defined (COMMON) && COMMON == CATHODE
#define ACTIVE HIGH
#define UNACTIVE LOW
#endif

#if defined (COMMON) && COMMON == ANODE
#define ACTIVE LOW
#define UNACTIVE HIGH
#endif

  //lit rows
  temp = pixMap[column] & (1 << line);

  // set desired bit clear others
  if (temp & (1 << 0)) digitalWrite (_R1, UNACTIVE); else digitalWrite (_R1, ACTIVE);
  if (temp & (1 << 1)) digitalWrite (_R2, UNACTIVE); else digitalWrite (_R2, ACTIVE);
  if (temp & (1 << 2)) digitalWrite (_R3, UNACTIVE); else digitalWrite (_R3, ACTIVE);
  if (temp & (1 << 3)) digitalWrite (_R4, UNACTIVE); else digitalWrite (_R4, ACTIVE);
  if (temp & (1 << 4)) digitalWrite (_R5, UNACTIVE); else digitalWrite (_R5, ACTIVE);
  if (temp & (1 << 5)) digitalWrite (_R6, UNACTIVE); else digitalWrite (_R6, ACTIVE);
  if (temp & (1 << 6)) digitalWrite (_R7, UNACTIVE); else digitalWrite (_R7, ACTIVE);

  // enable column
  line ++;
  if (line > 7) {
    line = 0;
    // unset last column
    digitalWrite ( columnPin [column], UNACTIVE);
    // compute next column
    column++;
    if (column > 4) column = 0;
    // set next column
    if ( blinkActive ) digitalWrite ( columnPin [column], ACTIVE);
  }

} // end of ISR Timer2



//    ___      _              ____
//   / __| ___| |_ _  _ _ __ / /\ \ 
//   \__ \/ -_)  _| || | '_ \ |  | |
//   |___/\___|\__|\_,_| .__/ |  | |
//                     |_|   \_\/_/
void setup() {


#if DEBUG >0
  // Serial port
  Serial.begin(9600);
#endif

  // Display pins
  pinMode (_C1, OUTPUT);
  pinMode (_C2, OUTPUT);
  pinMode (_C3, OUTPUT);
  pinMode (_C4, OUTPUT);
  pinMode (_C5, OUTPUT);

  pinMode (_R1, OUTPUT);
  pinMode (_R2, OUTPUT);
  pinMode (_R3, OUTPUT);
  pinMode (_R4, OUTPUT);
  pinMode (_R5, OUTPUT);
  pinMode (_R6, OUTPUT);
  pinMode (_R7, OUTPUT);

  // Buttons
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  // Initialize RTC
  if (rtc.begin()) {
    if (rtc.lostPower()) {
      operationMode = SETUP;
#if DEBUG == 1
      Serial.println("RTC Lost Power");
#endif
    } else {
      operationMode = DISPLY;
#if DEBUG == 1
      Serial.println("RTC OK");
#endif
    }
  } else {
    operationMode = BADFACE;
#if DEBUG == 1
    Serial.println("Couldn't find RTC");
#endif
  }

  // Setup Timer 2 interrupts (once at each 128us
  TCCR2A  = (  (0 << COM2A1 ) | // Normal mode count up to 255
               (0 << COM2A0 ) |
               (0 << COM2B1 ) |
               (0 << COM2B0 ) |
               (0 << WGM21  ) |
               (0 << WGM20  )
            ) ;

  TCCR2B = (   (0 << FOC2A  ) |
               (0 << FOC2B  ) |
               (0 << WGM22  ) |
               (0 << CS22   ) |  // prescaler 8
               (1 << CS21   ) |
               (0 << CS20   )
           ) ;

  TCNT2 = 0;                     // clear timer
  TIMSK2 = (  1 << 0   ) ;       // enable timer 2 interrupts
  sei();                         // enable global interrupts

} // setup()




//    _                   ____
//   | |   ___  ___ _ __ / /\ \ 
//   | |__/ _ \/ _ \ '_ \ |  | |
//   |____\___/\___/ .__/ |  | |
//                 |_|   \_\/_/
void loop() {
  // wait main temporization function
  while ( !readyToGo) ;
  readyToGo = false;

  // process buttons
  modeButtonEvent = button1Event();
  setButtonEvent = button2Event();

  // Get current time from RTC
  if ( rtc.begin() ) {
    now = rtc.now();
    hora = now.hour();
    minuto = now.minute();
    dia = now.day();
    mes = now.month();
    ano = now.year();
  } else {
    operationMode = BADFACE;
  }
  switch (operationMode) {
    case DISPLY:
      showTime();
      break;
    case SETUP:
      setupTime();
      break;
    case BADFACE:
    default:
      showBadFace();
  } // switch (op mode)
} // loop()



//     __              _   _
//    / _|_  _ _ _  __| |_(_)___ _ _  ___
//   |  _| || | ' \/ _|  _| / _ \ ' \(_-<
//   |_|  \_,_|_||_\__|\__|_\___/_||_/__/
//
void  showBadFace (void) {
  if (rtc.begin()) {
    operationMode = DISPLY;
#if DEBUG == 1
    Serial.println("RTC OK");
#endif
  } else {
    pixMap[0] = 0x20;
    pixMap[1] = 0x12;
    pixMap[2] = 0x10;
    pixMap[3] = 0x12;
    pixMap[4] = 0x20;
  }

} //showBadFace

void showTime(void) {
  if (modeButtonEvent & (1 << LONGPRESS)) {
    operationMode = SETUP;
#if DEBUG == 1
    Serial.println("enter setup mode");
#endif
  }
  if (setButtonEvent & (1 << PULSE /*RELEASE*/ )  )  {
#if DEBUG == 1
    Serial.println("Change display mode");
#endif
    displayMode++;
    displayTimeout = DISPLAY_TIMEOUT_3SECONDS;
    if (displayMode > SHOW_YEAR) displayMode = SHOW_TIME;
  }

  switch (displayMode) {
    case SHOW_TIME:
      putMonkDigit( hora * 100 + minuto );
      setupMode = SETUP_HOUR; // prepare for enter setup mode
      break;

    case SHOW_DAY:
      showDayOfMonth ( dia );
      setupMode = SETUP_DAY; // prepare for enter setup mode
      break;

    case SHOW_MONTH:
      showMonth ( mes );
      setupMode = SETUP_MONTH; // prepare for enter setup mode
      break;

    case SHOW_YEAR:
      putMonkDigit( ano );
      setupMode = SETUP_YEAR; // prepare for enter setup mode
      break;

  } // switch

  // Show the time again after a small timeout
  if (displayTimeout > 0 )
    displayTimeout--;
  else
    displayMode = SHOW_TIME;


} // showTime


void setupTime(void) {

  // check for end of setup
  if (modeButtonEvent & (1 << LONGPRESS) ) {
    displayTimeout = DISPLAY_TIMEOUT_3SECONDS;
    operationMode = DISPLY;
    modeButtonEvent = 0;
#if DEBUG == 1
    Serial.println("return to display");
#endif
  }

  //cycle between setup modes
  if (modeButtonEvent & (1 << PULSE /*PRESS*/ ) ) {
    setupMode++;
    if (setupMode > SETUP_YEAR ) setupMode = SETUP_HOUR;
#if DEBUG == 1
    Serial.println("Cycle setup mode");
#endif    
  }


  switch (setupMode) {

    case SETUP_HOUR:
      displayMode = SHOW_TIME; // prepare for return to  display mode
      if (setButtonEvent & (1 << PRESS) ) {
        hora++;
        if (hora > 24) hora = 0;
        rtc.adjust ( DateTime(ano, mes, dia, hora, minuto, 0) );

#if DEBUG == 1
        Serial.print("hora="); Serial.println(hora);
#endif
      }

      if  (setButtonEvent & (1 << LONGPRESS) ) {  // shortcut to get easy to set hours
        if (hora < 12)
          hora = 12;
        else
          hora = 0;
        rtc.adjust ( DateTime(ano, mes, dia, hora, minuto, 0) );
#if DEBUG == 1
        Serial.print("hora="); Serial.println(hora);
#endif
      }
      putMonkDigit (100 * hora) ;
      break;

    case SETUP_MINUTE:
      displayMode = SHOW_TIME; // prepare for return to  display mode
      if (setButtonEvent & (1 << PRESS) ) {
        minuto++;
        if (minuto > 59) minuto = 0;
        rtc.adjust ( DateTime(ano, mes, dia, hora, minuto, 0) );

#if DEBUG == 1
        Serial.print("min="); Serial.println(minuto);
#endif
      }

      if  (setButtonEvent & (1 << LONGPRESS) ) {  // shortcut to get easy to set minutes
        if (minuto < 30)
          minuto = 30;
        else
          minuto = 0;
        rtc.adjust ( DateTime(ano, mes, dia, hora, minuto, 0) );
#if DEBUG == 1
        Serial.print("min="); Serial.println(minuto);
#endif
      }

      putMonkDigit (minuto) ;
      break;


    case SETUP_DAY:
      displayMode = SHOW_DAY; // prepare for return to  display mode
      if (setButtonEvent & (1 << PRESS) ) {
        dia++;
        if (dia > 31) dia = 1;
        rtc.adjust ( DateTime(ano, mes, dia, hora, minuto, 0) );
#if DEBUG == 1
        Serial.print("dia="); Serial.println(dia);
#endif
      }
      showDayOfMonth(dia);
      break;


    case SETUP_MONTH:
      displayMode = SHOW_MONTH; // prepare for return to  display mode
      if (setButtonEvent & (1 << PRESS) ) {
        mes++;
        if (mes > 12) mes = 1;
        rtc.adjust ( DateTime(ano, mes, dia, hora, minuto, 0) );

#if DEBUG == 1
        Serial.print("mes="); Serial.println(mes);
#endif
      }
      showMonth(mes);
      break;


    case SETUP_YEAR:
      displayMode = SHOW_YEAR; // prepare for return to  display mode
      if (setButtonEvent & (1 << PRESS) ) {
        ano++;
        if (ano > 2099) ano = 2000;
        rtc.adjust ( DateTime(ano, mes, dia, hora, minuto, 0) );
#if DEBUG == 1
        Serial.print("ano="); Serial.println(ano);
#endif
      }
      if  (setButtonEvent & (1 << LONGPRESS) ) {
        ano = 2021 ;
        rtc.adjust ( DateTime(ano, mes, dia, hora, minuto, 0) );
#if DEBUG == 1
        Serial.print("ano="); Serial.println(ano);
#endif
      }
      putMonkDigit (ano) ;
      break;

    default:
      operationMode = DISPLY;
      break;

  } // switch (setup mode)




} // setupTime

uint8_t button1Event(void) {

  static uint8_t buttonCounter = 0, lastButtonSample = !digitalRead(BUTTON1);
  uint8_t i, buttonEvent, thisButtonSample;


  //  else {
  thisButtonSample = !digitalRead(BUTTON1);

  i = 0;
  if (lastButtonSample) i |= 2; // i = 0 0 0 0 0 0 l 0  last
  if (thisButtonSample) i |= 1; // i = 0 0 0 0 0 0 l t  last and this

  lastButtonSample = thisButtonSample;

  switch (i) {
    case 0: // button stayed released
      buttonEvent = 0;
      break;

    case 1: // button press
      buttonEvent = (1 << PRESS);
      break;

    case 2: // button release
      buttonEvent = (1 << RELEASE);
      if ( (buttonCounter>THRESHOLD)  && (buttonCounter<TIMELONGPRESS ) )  buttonEvent = (1<< PULSE);      
      buttonCounter = 0;
      break;

    case 3: // button stayed pressed
      if (buttonCounter < TIMELONGPRESS) {
        buttonCounter++;
        if (buttonCounter == TIMELONGPRESS )
          buttonEvent = (1 << LONGPRESS);
        else
          buttonEvent = 0;
      }  else buttonEvent = 0;
  } // switch
  // } // else
  return buttonEvent;
} // end of function


//

uint8_t button2Event(void) {

  static uint8_t buttonCounter = 0, lastButtonSample = !digitalRead(BUTTON2);
  uint8_t i, buttonEvent, thisButtonSample;


  //  else {
  thisButtonSample = !digitalRead(BUTTON2);

  i = 0;
  if (lastButtonSample) i |= 2; // i = 0 0 0 0 0 0 l 0  last
  if (thisButtonSample) i |= 1; // i = 0 0 0 0 0 0 l t  last and this

  lastButtonSample = thisButtonSample;

  switch (i) {
    case 0: // button stayed released
      buttonEvent = 0;
      break;

    case 1: // button press
      buttonEvent = (1 << PRESS);
      break;

    case 2: // button release
      buttonEvent = (1 << RELEASE);
      if ( (buttonCounter>THRESHOLD)  && (buttonCounter<TIMELONGPRESS ) )  buttonEvent = (1<< PULSE);      
      buttonCounter = 0;
      break;

    case 3: // button stayed pressed
      if (buttonCounter < TIMELONGPRESS) {
        buttonCounter++;
        if (buttonCounter == TIMELONGPRESS )
          buttonEvent = (1 << LONGPRESS);
        else
          buttonEvent = 0;
      } else buttonEvent = 0;
  } // switch
  // } // else
  return buttonEvent;
} // end of function



void setPixel (uint8_t x, uint8_t y) {
  if (x <= 5) {
    pixMap[x] |= ( 1 << (y & 0x07)); // limit y range
  }
}

void clearPixel (uint8_t x, uint8_t y) {
  if (x <= 5) {
    pixMap[x] &= ~( 1 << (y & 0x07)); // limit y range
  }
}

void clearDisplay (void) {
  uint8_t i;
  for (i = 0; i < 5; i++)
    pixMap[i] = 0;
}

void putMonkDigit(uint16_t number) {
  uint16_t n = number;
  uint8_t thous = 0, hunds = 0, tens = 0, units = 0;

  if (n < 10000) {

    while (n >= 1000) { // process thousands
      thous++;
      n = n - 1000;
    }
    while (n >= 100) { // process hundreds
      hunds++;
      n = n - 100;
    }
    while (n >= 10) { // process tens
      tens++;
      n = n - 10;
    }
    units = n;  // process units

    // update pixmap
    pixMap[0] = bmpThousC0 [thous] + bmpTensC0 [tens];
    pixMap[1] = bmpThousC1 [thous] + bmpTensC1 [tens];
    pixMap[2] = 0x7f;
    pixMap[3] = bmpThousC1 [hunds] + bmpTensC1 [units];
    pixMap[4] = bmpThousC0 [hunds] + bmpTensC0 [units];
  } // if

}

void showDayOfMonth(uint8_t dia) {
  uint8_t i, j;
  uint8_t d = dia;
  clearDisplay();

  i = 0; j = 0;
  //  if (d <= 31) { // limit to 31 days
  if (d <= 35) { // limit to 31 days               // TODO REMOVE
    while (d > 7) {
      pixMap[i] = 0xff; // fill entire column
      i++;
      d = d - 7;
    } // while

    while (d > 0) {  // fill in last row, if necessary
      j <<= 1; // shit in a "1" bit into j
      j |= 1;
      pixMap[i] = j;
      d--;
    }
  } //if
}


const PROGMEM uint8_t monthPattern [12][3] = {
  { 0x01, 0x00, 0x00 },
  { 0x01, 0x01, 0x00 },
  { 0x01, 0x01, 0x01 },
  { 0x05, 0x01, 0x01 },
  { 0x05, 0x05, 0x01 },
  { 0x05, 0x05, 0x05 },
  { 0x15, 0x05, 0x05 },
  { 0x15, 0x15, 0x05 },
  { 0x15, 0x15, 0x15 },
  { 0x55, 0x15, 0x15 },
  { 0x55, 0x55, 0x15 },
  { 0x55, 0x55, 0x55 }
};


void showMonth (uint8_t m) {
  clearDisplay();
  if ( (m > 0) && (m <= 12)) {
    pixMap[0] = pgm_read_byte_near(&monthPattern[m - 1][0]);
    pixMap[2] = pgm_read_byte_near(&monthPattern[m - 1][1]);
    pixMap[4] = pgm_read_byte_near(&monthPattern[m - 1][2]);
  } // if
}

#if DEBUG == 2
void printPixMap (void) {
  uint8_t i, j;
  uint8_t mask = 0x01;

  for (i = 0; i < 7; i++) {
    for (j = 0; j < 5; j++) {
      if (pixMap[j] & mask) Serial.print ("O"); else Serial.print (".");
    }
    mask <<= 1;
    Serial.println();
  }
}
#endif
