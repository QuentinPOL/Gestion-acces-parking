#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

// Le numéro de broche CS du shield Ethernet
//const int pinCS = 4;
// Adresse MAC : 00:AA:BB:CC:DE:02
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
// L'adresse IP que prendra le shield
IPAddress ip;
// L'adresse IP du serveur auquel se connecter
IPAddress server(192, 168, 65, 210);
// Le numéro de port du serveur
const int port = 1234;
// L'objet qui nous servira à la communication
EthernetClient client;
// Définir la taille maximale du tampon JSON
const int jsonBufferSize = 100;
DynamicJsonDocument doc(jsonBufferSize);
// Définir la taille maximale du tampon JSON
char json[jsonBufferSize];
// Définir la taille maximale du tampon JSON
char json2[jsonBufferSize];
// Statut Barriere
bool statutBarriere = false;

void setup() {
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
 
  // Vérifier si le client est connecté au serveur
  if (client.connected()) {
      // Lire les données JSON depuis le serveur
      String json2 = client.readString();
      Serial.println("Message : ");
      Serial.println(json2);

      // Analyser le message JSON reçu
      DeserializationError error = deserializeJson(doc, json2);
      if (error) {
        Serial.print("Erreur d'analyse JSON : ");
        Serial.println(error.f_str());
      } else {
        // Vérifier si la demande d'ouverture a été reçue
        const char* demande = doc["DemandeOpenBarriere"];
        if (demande && strcmp(demande, "DemandeOUVERTURE") == 0) {
          // La demande d'ouverture a été reçue
          statutBarriere = true;
          Serial.println("Demande d'ouverture de la barrière reçue.");
          doc.clear();
          
          // Création de l'objet JSON
          // Réinitialiser manuellement l'objet JSON en le recréant
          JsonObject root = doc.to<JsonObject>();
          root["statutBarriere"] = "Ouverte";
      
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

          delay(10000); // Attendre 10 secondes que la voiture passe
          doc.clear();

         if (client.connected()) {
          // Création de l'objet JSON
          // Réinitialiser manuellement l'objet JSON en le recréant
          JsonObject root = doc.to<JsonObject>();
          root["statutBarriere"] = "Fermer";
      
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

          statutBarriere = false;
         }
         else{
          // Tenter de se reconnecter au serveur
          if (client.connect(server, port)) {
            Serial.println("Connecté au serveur !");
          } else {
            Serial.println("Échec de la connexion au serveur.");
          } 
         }
        }
      }
  } else {
    // Tenter de se reconnecter au serveur
    if (client.connect(server, port)) {
      Serial.println("Connecté au serveur !");
    } else {
      Serial.println("Échec de la connexion au serveur.");
    }
  }
}
