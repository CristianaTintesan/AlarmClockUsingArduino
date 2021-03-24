#include <DS3231.h> // am inclus libraria pentru a putea folsi modulul rtc si pentru a prelua ora curenta si data
#include <LiquidCrystal.h> // librarie pentru a afisa date pe lcd


DS3231  rtc(SDA, SCL);
LiquidCrystal lcd( 2,3, 4, 5, 6, 7);

#define BUZZER 9
#define SETALARMBUTTON 8
#define INCREMENTALARMHOUR 10
#define INCREMENTALARMMINUTES 11
#define RESETALARMBUTTON 13
#define STOPALARMBUTTON A7
#define DRECREMENTALARMHOUR A8
#define DECREMENTALARMMINUTES A10
#define VIEWSETALARMLED A2

int alarmHours = 0; // pentru a salva ora dorita pt alarma
int alarmMinutes = 0; // pentru a salva minutele dorite pt alarma

int alarmDay = 0;

int currentHour = 0; // salvam ora curenta
int currentMinutes = 0; // salvam minutele 
int currentSeconds = 0; // salvam secundele 
int currentDay = 0; // salvam ziua zurenta
int currentMonth = 0; // salvam luna curenta
int maxDay = 0; // salvam numarul maxim de zile din luna curenta

bool setAlarm = false; // indica daca avem o alarma setata
bool ring = true; 
int sensor = A5;  // senzorul de sunet este conectat la pinul A5




void setup() { 
  
 rtc.begin();
 lcd.begin(16,2);
 Serial.begin(9600);

 pinMode(BUZZER, OUTPUT); //buzzer

 pinMode(SETALARMBUTTON, INPUT); // butonul conectat la pinul 8 
 digitalWrite(SETALARMBUTTON, HIGH); // butonul trebuie sa fie apasat pt a putea seta alarma

 pinMode(INCREMENTALARMHOUR, INPUT); // butonul conectat la pinul 10 
 digitalWrite(INCREMENTALARMHOUR, HIGH); // buton apasat => se incrementeza alarmHour

 pinMode(INCREMENTALARMMINUTES, INPUT); // butonul conectat la pinul 11 
 digitalWrite(INCREMENTALARMMINUTES, HIGH); // buton apasat => se incrementeza alarmMinutes

 pinMode(RESETALARMBUTTON, INPUT); // butonul conectat la pinul 13 
 digitalWrite(RESETALARMBUTTON, HIGH); // buton apasat => se reseteaza alarma 

 pinMode(STOPALARMBUTTON, INPUT); // butonul conectat la punul A7
 digitalWrite(STOPALARMBUTTON, HIGH); // butonul pt oprirea alarmei

 pinMode(DRECREMENTALARMHOUR, INPUT); // butonul conectat la punul A8
 digitalWrite(DRECREMENTALARMHOUR, HIGH); // butonul pt decrementarea orei pt alarma

 pinMode(DECREMENTALARMMINUTES, INPUT); // butonul conectat la punul A10
 digitalWrite(DECREMENTALARMMINUTES, HIGH); // butonul pt decrementarea minutelor pt alarma

 pinMode(VIEWSETALARMLED, OUTPUT); // ledul se aprinde cand am setat o alrma si se stinge dupa ce a sunat
 pinMode(A3, OUTPUT); // ledul se aprinde daca senzorul de sunet a detectat oprirea alarmei prin batai din palme

 alarmDay = rtc.getTime().date; //initializam ziua in care vrem sa sune alarma cu data curenta 


}


void loop() { 

  initLCD();
  while(digitalRead(8) == LOW) // butonul conentat la pinul 8 trebuie tinut apasat pt a seta ora si minutul la care vrem sa sune alarma
  {
    lcd.setCursor(0,1);
    lcd.print("Alarm: "); // Mesajul Alarm apare pe lcd atunci cand butonul 8 este apasat
    lcd.setCursor(5,1);
    
    alarm();
    
    lcd.setCursor(7,1);
    
    alarmFormat();
    
    delay(500);
    setAlarm = true; // punem setAlarm pt true, deoarece avem o alarma setata

  }
  
  if ( digitalRead (RESETALARMBUTTON) == LOW ) // daca apasam butonul 13 => alarma se sterge
  {
    deleteAlarm();
  }

  currentDay = rtc.getTime().date; // obtinem ziua curenta, preluand datele de la rtc
  currentMonth = rtc.getTime().mon; // obtinem luna curenta, preluand datele de la rtc

  if ( currentMonth == 1 || currentMonth == 3 || currentMonth == 5 || currentMonth == 7 || currentMonth == 8 || currentMonth == 10 || currentMonth == 12) // determinam nr de zile dintr-o luna
  {
    maxDay = 31;
  }
  else if ( currentMonth == 2) 
  {
    maxDay = 28;
  }else
  {
    maxDay = 30;
  }


  while (digitalRead(DRECREMENTALARMHOUR) == LOW) // daca apasam butonoul conectat la pinul A8, inseamna ca dorim sa setam ziua la care vrem sa programam alarma
  {
    setAlarmDay();
    
  }

  currentHour = rtc.getTime().hour; // salvam ora curenta in var currentHour apeland functia getTime din libraria DS3231
  currentMinutes = rtc.getTime().min; // salvam minutele in var currentMinutes apeland functia getTime din libraria DS3231 
  verifySetAlarm();

  tenSecoundsBeforeAlarm();

  itIsAlarmTime();

}

void initLCD()
{

  lcd.setCursor(0,0);
  lcd.print("Time:  "); // Afisam pr lcd mesajul Time:
  lcd.print(rtc.getTimeStr()); // Afisam in continuare pe lcd ora curenta 
  
}

void alarm()
{

    if(digitalRead(INCREMENTALARMMINUTES) == LOW)
    {
     alarmMinutes++; // incrementam minutele alarmei pana ajungem la ora dorita
    } 
    else if (digitalRead(INCREMENTALARMHOUR) == LOW)
    {
      alarmHours++; // incrementam ora alarmei pana ajungem la ora dorita
    }
    else if (digitalRead(DRECREMENTALARMHOUR) == LOW && alarmHours != 0)
    {
      alarmHours--; // decrementam ora alarmei pana ajungem la ora dorita
    }
    else if (digitalRead(DECREMENTALARMMINUTES) == LOW && alarmMinutes != 0)
    {
      alarmMinutes--; // decrementam minutele alarmei pana ajungem la ora dorita
    }
}

void alarmFormat()
{

    if(alarmHours < 10) // if folosit pentru a afisa pe lcd ora alarmei in formatul hh:mm , chiar daca 
    {
      lcd.print("0"); // minutul setat este mai mic decat 10 ( caz in care ar ocupa doar o pozitie)
      lcd.print(alarmHours);
    }
    else
    {
      lcd.print(alarmHours); // daca minutul alarmei este > 10, nu afisam 0 in fata 
    }
    
    lcd.print(":");
    
    if (alarmMinutes < 10) // procedam la fel si pentru ora, pentru a respecta formatul hh:mm
    {
      lcd.print("0");
      lcd.print(alarmMinutes);
    }
    else
    {
      lcd.print(alarmMinutes);
    }
 
    if(alarmMinutes > 59) //  daca minutele setate pt alarma depasesc 59
    {
      alarmHours++; // incrementam ora
      alarmMinutes = 0; // punem minutul pe 0
    }   
       
    if(alarmHours > 23) // daca ora setata pt alerma depaseste 23 
    {
      alarmHours = 0; // o resetam
    }
  
}

void deleteAlarm()
{
   if (setAlarm == true)
   { // prima data verificam daca avem o alrma setata
     alarmHours = 0; // resetam valorile alarmei
     alarmMinutes = 0;
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Alarm calceld!!"); // afisam un mesaj pe lcd
     delay(1000); // producem o intarziere pt a putea vizualiza mesajul
     lcd.clear();
     digitalWrite(VIEWSETALARMLED, LOW); // stingem ledul care indica ca avem o alarma setata
     setAlarm = false; // punem variabila setAlarm pe false doarece am anulat alarma
     alarmDay = rtc.getTime().date; // actualizam ziua in care vrem sa sune alarma cu ziua curenta
  
   }
}

void setAlarmDay()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(rtc.getDateStr()); // Afisam data curenta pe lcd
    lcd.setCursor(0,1);
    lcd.print("Set Day: ");
    lcd.setCursor(9,1);
    //alarmDay va avea initial valoarea din ziua curenta
    if ( digitalRead(DECREMENTALARMMINUTES) == LOW) // prin apasarea butonului se incrementeaza ziua la care dorim sa sune alarma
    {
       alarmDay++;
    }

    if ( alarmDay > maxDay ){ // verificam sa nu depasim nr maxim de zile dintr-o luna
      alarmDay = rtc.getTime().date ;
    }
    
    lcd.print(alarmDay); // Afisam ziua la care am programat alarma
    delay(1000); // producem o intarziere pt a vizualiza datele noi pe cld
    lcd.clear();
}

void verifySetAlarm()
{
  if ( (alarmHours != 0 | alarmMinutes != 0) && setAlarm == true) // verificam daca avem o alarma setata
  {
   digitalWrite(VIEWSETALARMLED, HIGH); // in caz afirmativ, aprindem ledul
   ring = true;
  }
}

void tenSecoundsBeforeAlarm()
{
  if (currentHour == alarmHours && currentMinutes == alarmMinutes - 1 && ring == true && currentDay == alarmDay ) //  daca ne aflam cu 1 minut inainte de ora setata pt alarma
  {
    currentSeconds = rtc.getTime().sec; // salvam secunda curenta
  
    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 50 ) // cand mai sunt 10 secunde pana la alarma
    {
     tone(BUZZER, 200); // pornim sunetul care se va intensifica la fiecare secunda
     delay(1000); // inatziem cu o secunda pentru a se auzi sunetul 
     noTone(BUZZER); // oprim sunetul curent, pentru a porni sunetu urmartor, la secunda 51
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 51 ) // cand mai sunt 9 secunde pana la alarma
    {
     tone(BUZZER, 250); // crestem frecventa sunetului
     delay(1000);
     noTone(BUZZER);
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 52 )
    {
     tone(BUZZER, 300);
     delay(1000);
     noTone(BUZZER);
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 53 )
    {
     tone(BUZZER, 350);
     delay(1000);
     noTone(BUZZER);
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 54 )
    {
     tone(BUZZER, 400);
     delay(1000);
     noTone(BUZZER);
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 55 )
    {
     tone(BUZZER, 450);
     delay(1000);
     noTone(BUZZER);
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 56 )
    {
     tone(BUZZER, 500);
     delay(1000);
     noTone(BUZZER);
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 57 )
    {
     tone(BUZZER, 550);
     delay(1000);
     noTone(BUZZER);
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 58 )
    {
     tone(BUZZER, 600);
     delay(1000);
     noTone(BUZZER);
    }

    if (currentHour == alarmHours && currentMinutes == alarmMinutes -1 && currentSeconds == 59 )
    {
     tone(BUZZER, 200);
     delay(1000);
     noTone(BUZZER);
    }
 }
}

void itIsAlarmTime()
{
  if (currentHour == alarmHours && currentMinutes == alarmMinutes  && ring == true && currentDay == alarmDay) // cand ora setata pe alarma devine agala cu ora curenta
  {
    
    tone(BUZZER,200); // buzzerul suna la o freceventa joasa ( am setat frecventa 100 deoarce 
                 // pe langa butonul de stop, alarma se poate opri cand bat din palme
                 // si pentru ca senzorul de sunet sa nu detecteze sunetul produs de buzzer, am setat frecventa 100 
                 
    setAlarm = false; // alarma a sunat => setam var setAlarm pe false
    digitalWrite(VIEWSETALARMLED, LOW);            //=> stingem ledul care indica ca avem setata o alarma
    
    long sum = 0;
    for ( int i=0; i<300; i++)
    {
      sum = sum + analogRead(sensor);
    }

    sum = sum / 300; // calculam media valorilor citite de senzorul de sunet
  
    if ( sum > 55) // daca senzorul de sunet detecteaza sunetul produs de bataile din palme 
    {
      ring = false;  // setam ring pe false pentru a intra in if ul urmator care opreste sunetul alarmei
      digitalWrite(A3, HIGH); // aprindem ledul 
   
    }else{
     digitalWrite(A3, LOW); // ledul ramane stins 
    }
  
    if ( ring == false ) // daca senzorul de sunet a detecatat sunetul produs de bataile din palme 
    {
      noTone(BUZZER); // oprim alarma
      alarmHours = 0; // resetam valorile pt ora si minut 
      alarmMinutes = 0;
      delay(10);
      digitalWrite(A3, LOW);
    }

    if ( digitalRead(STOPALARMBUTTON) == LOW) // daca am apasam butonul care opreste alarma
    { 
      resetAlarmValues();
    }
    lcd.clear();
  }
}

// oprim alarma si resetam valorile
void resetAlarmValues()
{
   ring = false; 
   alarmHours = 0;
   alarmMinutes = 0;
   noTone(BUZZER);
   delay(10);
}
