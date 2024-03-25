/**
 * 

#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* ssid="lois";         // "Votre Nom_du_routeur"
const char* password = "loisgarcion34";   // "Votre Mot_de_passe_du_routeur"

int ledPin = 5;

// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(115200);

   pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);

  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.print("Connexion Wi-Fi à ");
  Serial.println( ssid );

  WiFi.begin(ssid,password);

  Serial.println();
  Serial.print("Connecter");

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");        
  }

  digitalWrite( ledPin , HIGH);
  Serial.println();

  Serial.println("Wi-Fi Connecté Succès !");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World!");
  delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

**/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>  // Bibliothèque pour la gestion MQTT

#include <DHT.h>
#define DHTPIN D4
// Définition du type de capteur utilisé
#define DHTTYPE DHT11

const char* ssid = "Pixel Coline";           // Nom du réseau WiFi
const char* password = "Raclette";           // Mot de passe du réseau WiFi
const char* mqttServer = "maqiatto.com";     // Adresse IP ou nom du broker MQTT
const int mqttPort = 1883;                   // Port MQTT par défaut

const char* mqttUsername = "colineauber@yahoo.fr";  // Nom d'utilisateur MQTT
const char* mqttPassword = "plante";                 // Mot de passe MQTT

const char* mqttTopic = "colineauber@yahoo.fr/plante";   // Topic MQTT pour les données des plantes
const char* mqttH = "colineauber@yahoo.fr/capteur/humidite"; // Topic MQTT pour l'humidité
const char* mqttT = "colineauber@yahoo.fr/capteur/temperature"; // Topic MQTT pour la température

const int dry = 750; // Valeur pour le capteur sec
const int wet = 300; // Valeur pour le capteur mouillé

boolean alerte = false; // Variable pour indiquer une alerte de température

DHT dht(DHTPIN, DHTTYPE); // Instance du capteur de température et d'humidité

WiFiClient espClient; // Client WiFi
PubSubClient client(espClient); // Client MQTT

int ledPin = 5; // Broche pour la LED

// Fonction pour le traitement des messages MQTT entrants
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur le topic: ");
  Serial.println(topic);

  Serial.print("Contenu du message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Fonction pour reconnecter au broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT...");
    
    if (client.connect("ESP8266Client", mqttUsername, mqttPassword)) {
      Serial.println("Connecté au broker MQTT");
      client.subscribe(mqttTopic);  // S'abonner au topic MQTT
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayez dans 5 secondes");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println();
  Serial.print("Connexion Wi-Fi à ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecter");

  // Attente de connexion au réseau WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(ledPin, HIGH);
  Serial.println();

  Serial.println("Wi-Fi Connecté Succès !");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP());

  // Configuration du client MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Initialisation du capteur DHT
  dht.begin();

  //pinMode(relais_pompe, OUTPUT);
}

void loop() {
  // Vérification de la connexion au broker MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lecture de la valeur du capteur d'humidité
  int sensorVal = analogRead(A0);
  int percentageHumididy = map(sensorVal, wet, dry, 100, 0); 
  Serial.println(sensorVal);
  Serial.print(percentageHumididy);
  Serial.println("%");

  // Lecture de la température
  const float temperature = dht.readTemperature();
  Serial.println("Temperature = " + String(dht.readTemperature())+" °C");
  Serial.println("Humidite = " + String(dht.readHumidity())+" %");

  // Vérification des seuils de température pour l'alerte
  if(temperature > 32 || temperature <= 0){
    alerte = true;
  }

  if(temperature < 32 || temperature > 0){
    alerte = false;
  }

  // Envoi des données au broker MQTT
  client.publish(mqttTopic, "Hello, MQTT!");
  client.publish(mqttH, String(percentageHumididy).c_str());
  client.publish(mqttT, String(temperature).c_str());

  delay(3000);
}
