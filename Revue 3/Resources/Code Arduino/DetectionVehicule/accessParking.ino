#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

// Le numéro de broche CS du shield Ethernet
//const int pinCS = 4;
// Adresse MAC : C0:A8:40:1:55:3F
byte mac[] = { 0xC0, 0xA8, 0x40, 0x01, 0x55, 0x3F };
// L'adresse IP que prendra le shield
IPAddress ip;
// L'adresse IP du serveur auquel se connecter
IPAddress server(192, 168, 65, 210);
// Le numéro de port du serveur
const int port = 1234;
// L'objet qui nous servira à la communication
EthernetClient client;
// Définir la taille maximale du tampon JSON
const int jsonBufferSize = 200;
DynamicJsonDocument doc(jsonBufferSize);
char json[jsonBufferSize];

// Définition des pin/broches Leds
const int brocheLedOuvert = 6; // Ouverte barriere
const int brocheLedFermer = 5; // Fermeturre barriere

// Définition des pin/broches capteur ultra son
const int trigPin = 2; // Envoie
const int echoPin = 3; // Réception
const int trigPin2 = 9; // Envoie
const int echoPin2 = 8; // Réception

void setup() {
   // Définition des Leds en sortie
  pinMode(brocheLedOuvert, OUTPUT);
  pinMode(brocheLedFermer, OUTPUT);

  digitalWrite(brocheLedFermer, HIGH);
  
  // Définir les sorties et entrés du capteur ultra son
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT_PULLUP);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT_PULLUP);
  
  // Démarrer la communication série
  Serial.begin(9600);
  
  // Démarrer la connexion Ethernet avec DHCP
  if (Ethernet.begin(mac)) {
    Serial.println("Shield Ethernet initialisé.");
    Serial.print("Adresse IP : ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("Échec de l'initialisation du shield Ethernet.");
  }

 // Se connecter au serveur
  if (client.connect(server, port)) {
    Serial.println("Connecté au serveur !");
  } else {
    Serial.println("Échec de la connexion au serveur.");
  }
  
   // Lire les données JSON depuis le serveur
  String json = client.readString();
  Serial.println(json);
  
  delay(1000);
  
  // Initialiser la bibliothèque Ethernet avec le numéro de broche CS
 //Ethernet.init(pinCS);
 
  // Lire l'adresse MAC et l'afficher sur le moniteur série
  //byte macAddress[6];
  //Ethernet.maintain();
  //Ethernet.begin(macAddress);
  //Serial.print("Adresse MAC : ");
  //for (int i = 0; i < 6; i++) {
    //Serial.print(macAddress[i], HEX);
    //if (i < 5) {
      //Serial.print(":");
    //}
  //}
  //Serial.println();
}

void loop() {
 doc.clear();

 // Effacez le trigPin en le définissant sur LOW
 digitalWrite(trigPin, LOW);
 digitalWrite(trigPin2, LOW);
 delayMicroseconds(2);

 // Déclenchez le capteur en réglant le trigPin haut pendant 10 microsecondes
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);

 // Lit l'echoPin, pulseIn() renvoie la durée (longueur de l'impulsion) en microsecondes
 long time = pulseIn(echoPin, HIGH);

 // On va donc calculer la distance de l'impulsion en centimetre(cm)
 float distance = time * 0.034 / 2;

 // Ensuite on l'affiche
 Serial.print("Distance : ");
 Serial.print(distance);
 Serial.print(" cm\n");

 // Déclenchez le capteur en réglant le trigPin2 haut pendant 10 microsecondes
 digitalWrite(trigPin2, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin2, LOW);

 // Lit l'echoPin2, pulseIn() renvoie la durée (longueur de l'impulsion) en microsecondes
 long time2 = pulseIn(echoPin2, HIGH);

 // On va donc calculer la distance de l'impulsion en centimetre(cm)
 float distance2 = time2 * 0.034 / 2;

 // Ensuite on l'affiche
 Serial.print("Distance 2 : ");
 Serial.print(distance2);
 Serial.print(" cm\n");

 if (distance <= 5 && distance2 <= 5) 
 {
  if (client.connected())
  {
    // Création de l'objet JSON
    // Réinitialiser manuellement l'objet JSON en le recréant
    JsonObject root = doc.to<JsonObject>();
    root["InfoVeh"] = "VehiculeDetecter";

    // Vérifier la taille du document JSON
    size_t jsonLength = measureJson(doc);
    Serial.print("TAILLE BUFFER : ");
    Serial.print(jsonLength);
    Serial.print("\n");
    
    if (jsonLength > jsonBufferSize) {
      Serial.println("Erreur : le tampon JSON est trop petit !");
    }

    // Sérialisation de l'objet JSON
    serializeJson(doc, json);

    client.print(json);
    Serial.print("Envoi au serveur :");
    Serial.print(json);
    Serial.print("\n");

    digitalWrite(brocheLedOuvert, HIGH);
    digitalWrite(brocheLedFermer, LOW);
    delay(5000);
    digitalWrite(brocheLedOuvert, LOW);
    digitalWrite(brocheLedFermer, HIGH);
  }
  else
  {
    // Tenter de se reconnecter au serveur
    if (client.connect(server, port)) {
      Serial.println("Connecté au serveur !");
    } else {
      Serial.println("Échec de la connexion au serveur.");
    }
  }
 }

 delay(1000); // On va attendre 1 seconde afin de ne pas spam
}
