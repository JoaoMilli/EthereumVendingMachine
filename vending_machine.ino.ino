#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "POCO M3";
const char* password = "esp32teste";

HTTPClient http;

String serverPath = "https://api-rinkeby.etherscan.io/api?module=account&action=txlist&address=0xAE2CeD8E05931EE4017Ec81a34C7537Bd89ACe1B&startblock=0&endblock=99999999&page=1&offset=1&sort=desc&apikey=DCFFPVIFMHX99SJ1XZTH5Q8AYJ77MA9Y17";
String currentHash;

#define SCREEN_WIDTH 128 // Largura da tela OLED em pixels
#define SCREEN_HEIGHT 64 // Altura da tela OLED em pixels
#define OLED_RESET   -1 // Pino reset(pino reset compartilhado)
#define SCREEN_ADDRESS 0x3C // Endereço da tela

#define CONFIRMA 2
#define CANCELA 15
#define CIMA 18
#define BAIXO 5

#define PRODUTOMENOR1 "Produto 1"
#define PRODUTOMENOR2 "Produto 2"

#define PRODUTOMAIOR1 "Produto 3"
#define PRODUTOMAIOR2 "Produto 4"

#define PRECOMENOR 1000000000000000
#define PRECOMAIOR 2000000000000000

#define INITIAL_SCREEN 0
#define PRICE_LIST 1
#define PAYMENT_CHECK 2
#define PRODUCT_LIST_LOWER 3
#define PRODUCT_LIST_BIGGER 4
#define PAYMENT_NOT_RECEIVED 5

#define MOTOR1 32
#define MOTOR2 33
#define MOTOR3 25
#define MOTOR4 26

int currentState;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void iniciaHTTP(){
  http.begin(serverPath);
}

String precoMaiorToString() {
  float ether = (float)PRECOMAIOR / 1000000000000000000;
  String toString = String(ether,3) + " ETH";

  return toString;
}

String precoMenorToString() {
  float ether = (float)PRECOMENOR / 1000000000000000000;
  String toString = String(ether,3) + " ETH";
  
  return toString;
}

void conectWifi(){

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(9,18);


  display.println("Conectando WIFI...");

  display.display();

  WiFi.begin(ssid, password);

  while (!wifiConected()) {
    delay(500);
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18,18);


  display.println("WIFI conectado");

  display.display();
}

boolean wifiConected(){
  return (WiFi.status() == WL_CONNECTED);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Alocação do display SSD1306 falhou!"));
    for (;;); // Loop para sempre
  }
  
  conectWifi();
  iniciaHTTP();

  currentHash = getLastTransactionHash();

  pinMode(CANCELA, INPUT_PULLUP);   // Botão Cancela
  pinMode(CONFIRMA, INPUT_PULLUP);   // Botão Confirma
  pinMode(CIMA, INPUT_PULLUP);   // Botão Cima
  pinMode(BAIXO, INPUT_PULLUP);   // Botão Baixo
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);  
  pinMode(MOTOR3, OUTPUT);  
  pinMode(MOTOR4, OUTPUT);  

  currentState = INITIAL_SCREEN;

  display.clearDisplay();
}


void loop() {

  switch(currentState){
    case INITIAL_SCREEN:
    introductionScreen();
    break;

    case PRICE_LIST:
    priceListScreen();
    break;

    case PAYMENT_CHECK:
    paymentCheckScreen();
    break;

    case PRODUCT_LIST_LOWER:
    ProductListScreen(PRODUTOMENOR1, PRODUTOMENOR2, precoMenorToString());
    break; 

    case PRODUCT_LIST_BIGGER:
    ProductListScreen(PRODUTOMAIOR1, PRODUTOMAIOR2, precoMaiorToString());
    break;

    case PAYMENT_NOT_RECEIVED:
    paymentNotReceived();
    break; 

    default:
    break;
  }
}

void printIntroductionScreen(){
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(36,0);


  display.println("Ethereum");

  display.setCursor(18,9);
  display.println("Vending Machine");

  display.setCursor(9,45);
  display.println("Pressione Confirma");

  display.setCursor(27,54);
  display.println("Para iniciar");
  display.display();
}

void introductionScreen() {
  printIntroductionScreen();

  while(1){
    if (digitalRead(CONFIRMA) == LOW){
      while(digitalRead(CONFIRMA) == LOW){
        delay(20);
      }
      currentState = PRICE_LIST;
      return;
    };
    delay(20);
  }
}

void priceListScreen(){

  printPriceListScreen();

  while(1){

    if (digitalRead(CANCELA) == LOW){
      while(digitalRead(CANCELA) == LOW){
        delay(20);
      }
      currentState = INITIAL_SCREEN;
      return;
    };

    if (digitalRead(CONFIRMA) == LOW){
      while(digitalRead(CONFIRMA) == LOW){
        delay(20);
      }
      currentState = PAYMENT_CHECK;
      return;
    };
    delay(20);
  }
}

void printPriceListScreen() {

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(36,0);

  display.println("Produtos");

  display.setCursor(9,9);
  display.println("Produto");

  display.setCursor(85,9);
  display.println("Preco");

  display.setCursor(0,18);
  display.println(PRODUTOMENOR1);

  display.setCursor(72,18);
  display.println(precoMenorToString());

  display.setCursor(0,27);
  display.println(PRODUTOMENOR2);

  display.setCursor(72,27);
  display.println(precoMenorToString());

  display.setCursor(0,36);
  display.println(PRODUTOMAIOR1);

  display.setCursor(72,36);
  display.println(precoMaiorToString());

  display.setCursor(0,45);
  display.println(PRODUTOMAIOR2);

  display.setCursor(72,45);
  display.println(precoMaiorToString());

  display.setCursor(0,54);
  display.println("<-Canc");

  display.setCursor(88,54);
  display.println("Conf->");

  display.display();
}

void paymentCheckScreen(){

  iniciaHTTP(); 
  int timeStarted = millis();
  printPaymentCheckScreen();

  StaticJsonDocument<1024> doc;
  int httpResponseCode;
  String payload;

  while(1){

    if (digitalRead(CANCELA) == LOW){
      while(digitalRead(CANCELA) == LOW){
        delay(20);
      }
      currentState = PRICE_LIST;
      return;
    };

    if(!wifiConected()){
      printWifiDisconectScreen();
      delay(5000);
      conectWifi();
    }


    httpResponseCode = http.GET();
    // httpResponseCode = 1;
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      // payload = "{'status':'1','message':'OK','result':[{'blockNumber':'10744006','timeStamp':'1653577477','hash':'0x7e40d358fe32f049b165de9e4704742525ca044daac093f8addf06211d4b3df5','nonce':'17','blockHash':'0xa6986eea725cef34f9ae808dda7257b9daf201a19827b2697470150d121e10ef','transactionIndex':'18','from':'0x541436de7ea8537b6db06e354355f27deecc294f','to':'0xae2ced8e05931ee4017ec81a34c7537bd89ace1b','value':'2000000000000000','gas':'39496','gasPrice':'1500000018','isError':'0','txreceipt_status':'1','input':'0x','contractAddress':'','cumulativeGasUsed':'5186066','gasUsed':'26331','confirmations':'69141'}]}";

      DeserializationError err = deserializeJson(doc, payload);
      if (err) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.f_str());
      }

      // Serial.println("PAYLOAD: ");
      // Serial.println(payload);
      // Serial.println(atoll(doc["result"]));

      String hash = doc["result"][0]["hash"];

      Serial.println("HASH: ");
      Serial.println(hash);
      Serial.println("currentHASH: ");
      Serial.println(currentHash);

      if (!currentHash.equals(hash)){

        Serial.println("PASSOU");

        currentHash = hash;

        long long payment = doc["result"][0]["value"];
        String sender = doc["result"][0]["from"];

        float ether = (float)payment / 1000000000000000000;

        printPaymentReceived(ether, sender);
        delay(5000);

        if(payment == PRECOMENOR){
          currentState = PRODUCT_LIST_LOWER;
          return;
        } 
        else if (payment == PRECOMAIOR){
          currentState = PRODUCT_LIST_BIGGER;
          return;
        }
      }
      delay(200);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      delay(200);
    }
    if (millis() - timeStarted > 300000){
      currentState = PAYMENT_NOT_RECEIVED;
      return;
    }
  }
}

void printPaymentReceived(float ether, String sender){
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(24,9);
  display.println("Pagamento de");

  String payment = String(ether,3) + " ETH";

  display.setCursor(24,18);
  display.print(payment);

  display.setCursor(24,27);

  display.println("Recebido de");

  display.setCursor(24,36);

  String sender4digits = sender.substring(0,6) + "...";
  display.println(sender4digits);

  display.display();
}

void printPaymentCheckScreen() {

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.println("Realize o pagamento");
  display.println("para o endereco do");
  display.println("QR-CODE, o produto");
  display.println("sera liberado apos");
  display.println("confirmacao da");
  display.println("transacao");


  display.setCursor(0,54);
  display.println("<-Canc");

  display.display();
}

String getLastTransactionHash() {

  // iniciaHTTP(); 

  StaticJsonDocument<1024> doc;
  int httpResponseCode;
  String payload;

  while(1){
    httpResponseCode = http.GET();
    // httpResponseCode = 1;
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      // payload = "{'status':'1','message':'OK','result':[{'blockNumber':'10744006','timeStamp':'1653577477','hash':'0x7e48g358fx32f049b165de9e4704742525ca044daac093f8addf06211d4b3df5','nonce':'17','blockHash':'0xa6986eea725cef34f9ae808dda4567b9daf201a19827b2697470150d984e10gc','transactionIndex':'18','from':'0x541436de7ea8537b6db06e354355f27deecc294f','to':'0xae2ced8e05931ee4017ec81a34c7537bd89ace1b','value':'1000000000000000','gas':'39496','gasPrice':'1500000018','isError':'0','txreceipt_status':'1','input':'0x','contractAddress':'','cumulativeGasUsed':'5186066','gasUsed':'26331','confirmations':'69141'}]}";

      DeserializationError err = deserializeJson(doc, payload);
      if (err) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.f_str());
      }
      String hash = doc["result"][0]["hash"];
      return hash;
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      delay(500);
    }
  }
}

void paymentNotReceived(){
  printpaymentNotReceived();
  delay(5000);
  currentState = INITIAL_SCREEN;
  return;
}

void printWifiDisconectScreen(){
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(14,18);


  display.println("WIFI desconectado");

  display.setCursor(22,36);

  display.println("Reconectando...");

  display.display();

}

void printpaymentNotReceived(){
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(24,18);


  display.println("Pagamento nao");

  display.setCursor(28,36);

  display.println("Identificado");

  display.display();
}

void ProductListScreen(String prod1, String prod2, String price){
  short option = 1;

  printProductListScreen(option,prod1,prod2,price);

  while(1){

    if (digitalRead(CIMA) == LOW){
      while(digitalRead(CIMA) == LOW){
        delay(20);
      }
      option = 1;
      printProductListScreen(option,prod1,prod2,price);
    };

    if (digitalRead(BAIXO) == LOW){
      while(digitalRead(BAIXO) == LOW){
        delay(20);
      }
      option = 2;
      printProductListScreen(option,prod1,prod2,price);
    };
    delay(20);

    if (digitalRead(CONFIRMA) == LOW){
      while(digitalRead(CONFIRMA) == LOW){
        delay(20);
      }
      printProductReleaseScreen(option,price);

      switch(option) {
        case 1:
          if(price == precoMenorToString()) {
            digitalWrite(MOTOR1, HIGH);
            delay(2000);
            digitalWrite(MOTOR1, LOW);
          } 
          else if (price == precoMaiorToString()) {
            digitalWrite(MOTOR2, HIGH);
            delay(2000);
            digitalWrite(MOTOR2, LOW);
          }
          break;
        case 2:
          if(price == precoMenorToString()) {
            digitalWrite(MOTOR3, HIGH);
            delay(2000);
            digitalWrite(MOTOR3, LOW);
          } 
          else if (price == precoMaiorToString()) {
            digitalWrite(MOTOR4, HIGH);
            delay(2000);
            digitalWrite(MOTOR4, LOW);
          }
          break;
        default:
          break;
      }
      
      currentState = INITIAL_SCREEN;
      return;
    };
    delay(20);
  }
}

void printProductListScreen(short option, String prod1, String prod2, String price){
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(36,0);

  display.println("Produtos");

  display.setCursor(9,9);
  display.println("Produto");

  display.setCursor(85,9);
  display.println("Preco");

  if (option == 1) {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(0,27);
    display.println(prod1);

    display.setCursor(72,27);
    display.println(price);

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,36);
    display.println(prod2);

    display.setCursor(72,36);
    display.println(price);


  } else if (option == 2) {
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,27);
    display.println(prod1);

    display.setCursor(72,27);
    display.println(price);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(0,36);
    display.println(prod2);

    display.setCursor(72,36);
    display.println(price);
  }

  display.display();
}

void productReleaseScreen(short option, String price){
  printProductReleaseScreen(option,price);
  delay(5000);
}

void printProductReleaseScreen(short option, String price){

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(9,18);

  display.println("Liberando produto:");

  display.setCursor(36,27);

  if (option == 1){
    if (price == precoMenorToString()){
      display.println(PRODUTOMENOR1);
    }
    else if (price == precoMaiorToString()){
      display.println(PRODUTOMAIOR1);
    }
  } 
  else if (option == 2){
    if (price == precoMenorToString()){
      display.println(PRODUTOMENOR2);
    }
    else if (price == precoMaiorToString()){
      display.println(PRODUTOMAIOR2);
    }
  }
  display.display();
}
