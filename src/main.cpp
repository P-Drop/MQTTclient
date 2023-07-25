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

WiFiClient clienteWiFi;
PubSubClient clienteMQTT(clienteWiFi);

void callback(char *topico, byte *cargaUtil, unsigned int longitudDeDatos){
  Serial.print("Se ha recibido el mensaje en: ");
  Serial.println(topico);
  char accion = '0';
  Serial.print("Contenido: ");
  for(unsigned int i = 0; i < longitudDeDatos; i++){
    accion = (char)cargaUtil[i];
    Serial.print(accion);
  }
  if(accion == '1'){
    digitalWrite(RELE, LOW);
  }
  else if(accion == '0'){
    digitalWrite(RELE, HIGH);
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
    clienteMQTT.subscribe("lab/practicas1");
  }
  digitalWrite(LEDV, HIGH);
}

// Variable contador
  int contador = 0;

void loop() {

  clienteMQTT.loop();

  if(millis() - ultimaVez > TIEMPO_DE_ESPERA){
    sprintf(mensaje, "Contador: %d", contador);

    Serial.println("\nPublicando mensaje...");
    clienteMQTT.publish("lab/practicas2", mensaje);
    Serial.println("OK");
    contador++;
    ultimaVez = millis();
  }
  delay(10);
}