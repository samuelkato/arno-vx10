/*
 * este código é baseado no exemplo de http server wifi disponível na biblioteca ESP8266 do arduino
 * deve exibir o ip para acesso via serial ao se conectar com sucesso no ssid informado
 * espera um botão na porta 0, que usa pullup interno
 * a saída, por default na porta 14, deve ser ligada a um circuito rf compatível com o esperado pelo ventilador
 * no meu caso, usei um controle cujo chip principal havia queimado.
*/

//trocar para biblioteca adequada WIFI caso ESP32
#include <ESP8266WiFi.h>


#ifndef STASSID
#define STASSID "NomeWifi"
#define STAPSK  "senha"
#endif

int btnLuz = 0;
int pinoRF = 14;
int i,j;

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiServer server(80);


void fn4(){
  digitalWrite(pinoRF, LOW);
  delayMicroseconds(378);
  digitalWrite(pinoRF, HIGH);
  delayMicroseconds(5500);
}

void fn3(){
  digitalWrite(pinoRF, LOW);
  delayMicroseconds(1378);
  digitalWrite(pinoRF, HIGH);
  delayMicroseconds(4500);
}

void fn8(){
  for(i=0;i<8;i++){
    digitalWrite(pinoRF, LOW);
    delayMicroseconds(1000);
    digitalWrite(pinoRF, HIGH);
    delayMicroseconds(1000);
  }
}

void fn1(int n){
  for(i=0;i<n;i++){
    digitalWrite(pinoRF, LOW);
    delayMicroseconds(378);
    digitalWrite(pinoRF, HIGH);
    delayMicroseconds(1622);
  }
}

void fn2(int n){
  for(i=0;i<n;i++){
    digitalWrite(pinoRF, LOW);
    delayMicroseconds(1500);
    digitalWrite(pinoRF, HIGH);
    delayMicroseconds(500);
  }
}

void ligaDesligaLuz(){
  for(j=0;j<5;j++){
    fn3();
    fn8();
    fn2(3);
    fn1(1);
    fn2(4);
    fn1(1);
    fn2(1);
    fn1(1);
    fn2(1);
    fn1(1);
    fn2(1);
    fn1(1);
  }
}

void ligaDesligaVento(){
  for(j=0;j<5;j++){
    fn4();
    fn8();
    fn2(3);
    fn1(1);
    fn2(5);
    
    fn1(1);
    fn2(1);
    fn1(1);
    fn2(1);
    fn1(1);
    fn2(1);
  }
}

void menoVento(){
  for(j=0;j<5;j++){
    fn4();
    fn8();
    fn2(3);
    fn1(1);
    fn2(8);
    fn1(3);
  }
}

void maisVento(){
  for(j=0;j<5;j++){
    fn3();
    fn8();
    fn2(3);
    fn1(1);
    fn2(4);
    fn1(4);
    fn2(3);
  }
}

void dormir(){
  for(j=0;j<5;j++){
    fn4();
    fn8();
    fn2(3);
    fn1(1);
    fn2(4);
    fn1(1);
    fn2(2);
    fn1(2);
    fn2(2);
  }
}

void ventExaust(){
  for(j=0;j<5;j++){
    fn3();
    fn8();
    fn2(3);
    fn1(1);
    fn2(4);
    fn1(2);
    fn2(2);
    fn1(2);
    fn2(1);
  }
}

void tempo(){
  for(j=0;j<5;j++){
    fn4();
    fn8();
    fn2(3);
    fn1(1);
    fn2(4);
    fn1(2);
  }
}

void setup() {
  Serial.begin(115200);

  // prepare LED
  pinMode(pinoRF, OUTPUT);
  
  pinMode(btnLuz, INPUT_PULLUP);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  digitalWrite(pinoRF, LOW);
  while(!digitalRead(btnLuz)){//caso botao na porta zero seja pressionado
    ligaDesligaLuz();
  }
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(1000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // verifica se a url chamada deve ativar alguma ação
  if (req.indexOf(F("/gpio/0")) != -1) {
    ligaDesligaLuz();
  } else if (req.indexOf(F("/gpio/1")) != -1) {
    ligaDesligaVento();
  } else if (req.indexOf(F("/gpio/2")) != -1) {
    menoVento();
  } else if (req.indexOf(F("/gpio/3")) != -1) {
    maisVento();
  } else if (req.indexOf(F("/gpio/4")) != -1) {
    dormir();
  } else if (req.indexOf(F("/gpio/5")) != -1) {
    ventExaust();
  } else if (req.indexOf(F("/gpio/6")) != -1) {
    tempo();
  } else {
    Serial.println(F("invalid request"));
  }

  while (client.available()) {
    client.read();
  }

  //header + html enviado para o cliente
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n"));
  client.print(F("<br><br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/0'><h1>Luz</h1></a><br/><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/1'><h1>Vento</h1></a><br><br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/2'><h1>- Vento</h1></a><br><br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/3'><h1>+ Vento</h1></a><br><br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/4'><h1>Dormir</h1></a><br><br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/5'><h1>Ventilador Exaustor</h1></a><br><br><a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/6'><h1>Timer</h1></a></html>"));

  Serial.println(F("Disconnecting from client"));
  
}
