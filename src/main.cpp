#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define NOMBRE_RED "printer"
#define PASSWORD_RED "78115293"
#define SERVIDOR_MQTT "192.168.0.115"
#define PUERTO 1883
#define USUARIO_MQTT ""
#define PASSWORD_MQTT ""
#define TIEMPO_DE_ESPERA 3000

#define LEDV D1
#define LEDR D2
#define RELE D3


char mensaje[500] = "";
int ultimaVez = millis();

boolean rele_state = false;

WiFiClient clienteWiFi;
PubSubClient clienteMQTT(clienteWiFi);

void callback(char *topico, byte *cargaUtil, unsigned int longitudDeDatos){
  Serial.print("Se ha recibido el mensaje en: ");
  Serial.println(topico);
  String accion = "";
  Serial.print("Contenido: ");
  for(unsigned int i = 0; i < longitudDeDatos; i++){
    accion += (char)cargaUtil[i];
    Serial.println(accion);
  }
  if(accion == "ON"){
    digitalWrite(RELE, LOW);
    rele_state = true;
  }
  else if(accion == "OFF"){
    digitalWrite(RELE, HIGH);
    rele_state = false;
  }
  else{
    Serial.print("Accion no definida");
  }
  Serial.println();
}

void setup() {
// Iniciar puerto serie a 115200 baudios
  Serial.begin(115200);

// Definimos pines de los componentes
pinMode(LEDV, OUTPUT);
pinMode(LEDR, OUTPUT);
pinMode(RELE, OUTPUT);

// Y declaramos el estado inicial de los componentes
digitalWrite(LEDV, LOW);
digitalWrite(LEDR, LOW);
digitalWrite(RELE, HIGH);

// Conectar a WiFi
  WiFi.begin(NOMBRE_RED, PASSWORD_RED);
  Serial.print("\nConectando a WiFi...");
  while(WiFi.status() != WL_CONNECTED){
    digitalWrite(LEDR, HIGH);
    delay(250);
    Serial.print(".");
    digitalWrite(LEDR, LOW);
    delay(250);
  }
  Serial.println("\nConectado");
  digitalWrite(LEDR, HIGH);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

// Conectar a servidor MQTT
  clienteMQTT.setServer(SERVIDOR_MQTT, PUERTO);
  clienteMQTT.setCallback(callback);
  Serial.println("\nConectando a servidor MQTT...");
  while(!clienteMQTT.connected()){
    digitalWrite(LEDV, HIGH);
    if(clienteMQTT.connect("ClienteESP8266", NULL, NULL)){
      Serial.println("Conectado");
    } 
    else {
      Serial.print("Error conectando. Estado: ");
      Serial.println(clienteMQTT.state());
      delay(2000);
    }
    delay(250);
    digitalWrite(LEDV, LOW);
    delay(250);
    clienteMQTT.subscribe("lab/rele");
  }
  digitalWrite(LEDV, HIGH);
}


void loop() {

  clienteMQTT.loop();

  if(millis() - ultimaVez > TIEMPO_DE_ESPERA){
    
    sprintf(mensaje, rele_state ? "on" : "off");

    Serial.println("\nPublicando mensaje...");
    clienteMQTT.publish("lab/sensor", mensaje);
    Serial.println("OK");
    ultimaVez = millis();
  }
  delay(10);
}