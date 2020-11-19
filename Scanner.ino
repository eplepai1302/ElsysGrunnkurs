 
#include <SPI.h>    //Legger til bilbioteker så jeg kan bruke funksjoner senere i koden (:
#include <MFRC522.h> //-||-

#include <Stepper.h> //-||-


//deffinerer steps per rotation. Altså bruker en motor som har forskjellige "steps" når den roterer
const int stepsPerRevolution = 768;

Stepper myStepper = Stepper(stepsPerRevolution, 7, 9, 8, 10); //deffinerer hvilke pinns motoren skal ha

#define SS_PIN 53   // en annen måte å definere hvilke pins som skal hete hva, i stedet for å si int ss_pin 53.
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance. (Dette er med å deffinerer rfid scanneren)

bool isLocked = false;

int greenPin = 13; //Disse pinnene blir brukt for testing på breadboard
int redPin = 12;
int bluePin = 11;


#define checkPin 4

String Active = "";
String Access= "";

String Master = "02 E3 7C 08"; //Sett hvilken nøkkel som skal være master her

void setup() {
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  //setter farten til motoren til 30RPM
  myStepper.setSpeed(30);
  
  pinMode(greenPin, OUTPUT); //deffinerer hvilke pins som skal være output og input
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(checkPin, OUTPUT);
}
void loop() {
  
  // Look for new cards. Koden går ikke videre med mindre den finner et kort
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) //gjør koden til kortet/brikken leselig på formen (** ** ** **) i HEX
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();

  //setter "active" til det siste kortet lest
  Active = content.substring(1);


if (isLocked != true){ // ser om låsen er åpen
  
  Access = content.substring(1); //setter det aktive kortet som Access, altså det kortet som kan åpne låsen
  Serial.println("Is now locked");
  digitalWrite(bluePin, HIGH);
  delay(500);
  digitalWrite(bluePin, LOW);
  digitalWrite(checkPin, HIGH);
  myStepper.step(stepsPerRevolution); //beveger motoren i låst possisjon
  isLocked = true;
  delay(500);
  digitalWrite(checkPin, LOW);
}

else if (isLocked = true){ // ser om låsen er låst og om det er et kort der
  
  if (Active == Access || Active == Master) //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    isLocked = false;
    digitalWrite(greenPin, HIGH);
    delay(500);
    digitalWrite(greenPin, LOW);
    digitalWrite(checkPin, HIGH);
    myStepper.step(-stepsPerRevolution); //Setter motoren tilbake til startposisjon
    delay(500);
    digitalWrite(checkPin, LOW);
  }

 else if (Active != Access){
    Serial.println(" Access denied");
    digitalWrite(redPin, HIGH);
    delay(500);
    digitalWrite(redPin, LOW);
    delay(500);
  }
  else{
    Serial.print("Registrerer ikke Active eller Access");
  }
}
else {
  Serial.print("Her gikk det galt");
}
}
