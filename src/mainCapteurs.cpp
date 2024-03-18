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
#include <PubSubClient.h>  // Ajoutez cette ligne pour la gestion MQTT

#include <DHT.h>
#define DHTPIN D4
// Definit le type de capteur utilise
#define DHTTYPE DHT11

const char* ssid = "Pixel Coline";           // Remplacez par le nom de votre réseau WiFi
const char* password = "Raclette"; // Remplacez par le mot de passe de votre réseau WiFi
const char* mqttServer = "maqiatto.com"; // Remplacez par l'adresse IP ou le nom du broker MQTT
const int mqttPort = 1883;  // Port MQTT par défaut

const char* mqttUsername = "colineauber@yahoo.fr";  // Si nécessaire, sinon commentez cette ligne
const char* mqttPassword = "plante";  // Si nécessaire, sinon commentez cette ligne

const char* mqttTopic = "colineauber@yahoo.fr/plante";  // Remplacez par le topic MQTT que vous souhaitez utiliser
const char* mqttH = "colineauber@yahoo.fr/capteur/humidite";
const char* mqttT = "colineauber@yahoo.fr/capteur/temperature";
const char* mqttA = "colineauber@yahoo.fr/alerte";

const int dry = 595; // value for dry sensor
const int wet = 239; // value for wet sensor

boolean alerte = false;

DHT dht(DHTPIN, DHTTYPE); //température

//const int relais_pompe = D5; // // le relais est connecté à la broche 2 de la carte Adruino

WiFiClient espClient;
PubSubClient client(espClient);

int ledPin = 5;

void callback(char* topic, byte* payload, unsigned int length) {
  // Traitement des messages MQTT entrants
  Serial.print("Message reçu sur le topic: ");
  Serial.println(topic);

  Serial.print("Contenu du message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Fonction de reconnexion au broker MQTT
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



  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(ledPin, HIGH);
  Serial.println();

  Serial.println("Wi-Fi Connecté Succès !");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);  // Définir la fonction de rappel pour le traitement des messages entrants

  dht.begin();

  //pinMode(relais_pompe, OUTPUT);
}

void loop() {
  //digitalWrite(relais_pompe, HIGH);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

int sensorVal = analogRead(A0);

/*

int percentageHumididy = map(sensorVal, wet, dry, 100, 0); 
Serial.println(sensorVal);
Serial.print(percentageHumididy);
Serial.println("%");

*/

const float temperature = dht.readTemperature();
Serial.println("Temperature = " + String(dht.readTemperature())+" °C");
// TODO : temporaire, à enlever quand on aura le bon capteur
int percentageHumididy = dht.readHumidity();
Serial.println("Humidite = " + String(percentageHumididy)+" %");

/*
if(percentageHumididy <= 30 && alerte == false){
  (relais_pompe, HIGH);
  delay(5000); //5s
  digitalWrite(relais_pompe, LOW);
}
*/

if(percentageHumididy <= 15 && alerte == true){
  String message = "Alerte! La temperature est de: " + String(temperature) + "°C l'arrosage automatique a été annulé mais l'humidité est trop basse, elle est actuellement à: " + String(percentageHumididy) + "%";
  client.publish(mqttA, message.c_str());
}

if(percentageHumididy > 70){
  String message = "Alerte! Le taux d'humidité est trop élevé, il est actuellement à : " + String(percentageHumididy) + "%";
  client.publish(mqttA, message.c_str());
}

if(temperature > 32 || temperature <= 0){
  alerte = true;
}

if(temperature < 32 || temperature > 0){
  alerte = false;
}

// Envoyer une donnée au broker MQTT
client.publish(mqttTopic, "Hello, MQTT!");
client.publish(mqttH, String(percentageHumididy).c_str());
client.publish(mqttT, String(temperature).c_str());
client.publish(mqttA, String(alerte).c_str());

}