#include <Wire.h>
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


const int coinPin = 0; 
const int ledPin = 13;
const int relayPin = 23;
const int resetAmountPin = 3;

const unsigned long SECOND = 1000;
const unsigned long HOUR = 3600*SECOND;

const unsigned long TIME_FOR_5_CENTS = HOUR/2;
//const unsigned long TIME_FOR_5_CENTS = 1000;  // testing

float totalAmount = 0.00;

// A coin has been inserted flag
volatile int coinsChange = 0;                  

bool keyboardEnabled = false;
elapsedMillis runTime = 0;
unsigned long timePaidFor = 0;

elapsedMillis revelationTime = 0;
unsigned int revelationIndex = 0;
//const unsigned int REVELATION_DURATION = 1000;  // testing
const unsigned int REVELATION_DURATION = 60000;

char* revelations[][2] = {
  { "Life is beautiful", "with donuts" },
  { "Homer says", "D'ohnut!!" },
  { "Keep typing,", "your donuts need you" },
  { "Sugar is good for", "your brain" },
  { "Working hard?", "or hardly working?" },
  { "Rocket League", "rocks!" },
  { "Feed your", "co-workers" },
  { "Vacation is for", "donuts" },
  { "Money is not all.", "Donuts are." },
  { "Saving for donuts", "is smart" },
  { "Keyboard + coins", "= donuts" },
  { "Go nuts", "for donuts" },
  { "The donut nut", "is not a nut!" },
  { "Is it time to", "get donuts yet?" },
  { "Donuts > abs", "" },
  { "Donut start", "this argument" },
  { "Donut stop", "believing" },
  { "Donut worry,", "be happy!" },
  { "Donut ever", "give up!" },
  { "But the holes", "are delicious" },
  { "Happiness with", "sprinkles on top" },
  { "Whatever sprinkles", "your donuts" },
  { "Donut kill", "your vibe, man!" },
  { "Life is short,", "eat more donuts" },
  { "Love. Life.", "Donuts." },
  { "Eat more", "hole foods" },
  { "Donut", "judge me" },
  { "Don't panic!", "There's more donuts" },
  { "Everything", "is donutsome" },
  { "Wake up. Eat donuts.", "Sleep. Repeat." }
};

const unsigned int revelationsNum = sizeof(revelations) / sizeof(revelations[0]);
void setup()
{
  //Start Serial Communication
  //Serial.begin(9600);
  Serial.begin(115200);                 

  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  // By default held high
  pinMode(resetAmountPin, INPUT_PULLUP);

  // Turn the LED off by making the voltage LOW
  digitalWrite(ledPin, LOW);

  // turn the RELAY off by making the voltage LOW
  // (keyboard is disconnected)
  digitalWrite(relayPin, LOW);

  lcd.begin(20,4);
  lcd.clear();

  randomSeed(analogRead(0));
  revelationIndex = random(revelationsNum);

  // wait for coin acceptor to start up
  lcd.setCursor(0, 0);
  lcd.print("Please wait...");
  delay(2000);
  lcd.clear();

  //If coinPin goes HIGH (a Pulse), call the coinInserted function
  //An attachInterrupt will always trigger, even if you are using delays
  attachInterrupt(coinPin, coinInserted, RISING);
}

//The function that is called every time it recieves a pulse
void coinInserted()    
{
  //Flag that there has been a coin inserted
  coinsChange = 1;                           
}

void loop()
{
  if (digitalRead(resetAmountPin) == LOW)
  {
    // The button is hit
    totalAmount = 0.0;
  }

  //Check if a coin has been Inserted
  if (coinsChange == 1)          
  { 
    // Unflag that a coin has been inserted
    coinsChange = 0;

    timePaidFor += TIME_FOR_5_CENTS;
    totalAmount += 0.05;

    // Print the Value of coins inserted
    //Serial.print("Credit: $");
    //Serial.println(totalAmount);
 
    if (keyboardEnabled == false)
    {
      // Enable keyboard
      lcd.clear();
      revelationTime = 0;
      keyboardEnabled = true;
      digitalWrite(ledPin, HIGH);
      digitalWrite(relayPin, HIGH);
      runTime = 0;
    }
  }

  if (runTime > timePaidFor)
  {
    // Time is over
    
    if (keyboardEnabled == true)
    {
      lcd.clear();  
    }
    
    keyboardEnabled = false;
    digitalWrite(ledPin, LOW);
    digitalWrite(relayPin, LOW);
    timePaidFor = 0;

    int i = 0;
    lcd.setCursor(0, i++);
    lcd.print("Jan! Want donuts?");
    lcd.setCursor(0, i++);
    lcd.print("Feed the keyboard!");
    lcd.setCursor(0, i++);
    lcd.print("Cost: $0.05/30 min");
    lcd.setCursor(0, i++);
    lcd.print("Donut Fund: $");
    lcd.print(totalAmount);
  }
  else
  {
    // Time is still running
    
    unsigned long timeLeft = (timePaidFor - runTime)/1000;
    unsigned int hours = timeLeft / 3600;
    unsigned int minutes = (timeLeft % 3600)/60;
    unsigned int seconds = timeLeft % 60;

    // Show how much time left
    lcd.setCursor(0, 0);

    char timeBuf[20];
    snprintf(timeBuf, sizeof(timeBuf), "Time left: %dh%dm%ds", hours, minutes, seconds);
    lcd.print(timeBuf);
    for (int i=strlen(timeBuf); i<20; i++) lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("Donut Fund: $");
    lcd.print(totalAmount);

    if (revelationTime > REVELATION_DURATION)
    {
      // Change revelation
      lcd.clear();
      revelationTime = 0;
      //revelationIndex++;
      //revelationIndex = revelationIndex % (revelationsNum);
      revelationIndex = random(revelationsNum);
    }
    lcd.setCursor(0, 2);
    lcd.print(revelations[revelationIndex][0]);
    lcd.setCursor(0, 3);
    lcd.print(revelations[revelationIndex][1]);
  }
}
