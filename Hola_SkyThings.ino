#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h >
#include <WiFiManager.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
// Instancia a la clase Ticker
Ticker ticker;
 
// Pin estado de conexion
byte pinLed = 16;
// Interruptores
 
int interruptor1 = 5;

void parpadeoLed(){
  // Cambiar de estado el interruptor1
  byte estado = digitalRead(pinLed);
  digitalWrite(pinLed, !estado);
}

SoftwareSerial ss (15,4);

const char *mqtt_server = "ieiot.com.mx";
const int mqtt_port = 1883;
const char *mqtt_user = "holaskythings@hotmail.com";
const char *mqtt_pass = "holasky";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[25];

long tsend = 0;
long ttsend = 10000;
//*****************************
//*** DECLARACION FUNCIONES ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() {
  Serial.begin(115200);
  tsend=millis();
  pinMode(pinLed, OUTPUT);
  pinMode(interruptor1, OUTPUT);
 
 
   // Empezamos el temporizador que hará parpadear el interruptor1
  ticker.attach(0.2, parpadeoLed);
 // Creamos una instancia de la clase WiFiManager
  WiFiManager wifiManager;
 
  // Descomentar para resetear configuración
  wifiManager.resetSettings();
 
  // Cremos AP y portal cautivo y comprobamos si
  // se establece la conexión
  if(!wifiManager.autoConnect("Multicontacto IoT")){
    Serial.println("Fallo en la conexión (timeout)");
    ESP.reset();
    delay(1000);
  }
 
  Serial.println("Ya estás conectado");
 
  // Eliminamos el temporizador
  ticker.detach();
  
  // Apagamos el Led
  digitalWrite(pinLed, HIGH);
  
  Serial.begin(115200);
  randomSeed(micros());
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
     
  if (!client.connected()) {
    reconnect();
  }
client.loop(); 

}

//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);

  if ( incoming == "ON") {
    
    
    digitalWrite(interruptor1, HIGH);
  
  }
    
  if ( incoming == "OFF") {
    
    
    digitalWrite(interruptor1, LOW);
     
  }
 
}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "Hola sky things";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      client.subscribe("ESP-P1/luz");
      
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");

      delay(500);
    }
  }
}
