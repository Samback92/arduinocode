#include <WiFiS3.h>
#include "WiFiClient.h"
#include <ArduinoHttpClient.h>
#include "settings.h"
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASSWORD;

char serverAdress[] = "192.168.1.17";

int port = 8080;


int status = WL_IDLE_STATUS;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAdress, port);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Ansluter till wifi");
  WiFi.begin(ssid, pass);
  
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(500);
  }
  
  if (WiFi.status() == WL_CONNECTED) {


  dht.begin();
  Serial.println("Startar sensorn");

  printWifiStatus();
  
  } else {
    Serial.println("EJ ANSLUTEN! ");
  }

}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Skapa JSON-sträng för POST-data
  String postData = "{\"temperature\": " + String(t) + ", \"humidity\": " + String(h) + "}\n";

  Serial.print("JSON som skickas: ");
  Serial.println(postData);

  // Kontrollera att klienten är ansluten
  if (client.connect(serverAdress, port)) {
    Serial.println("Ansluten till servern");
    client.beginRequest();
    client.post("/post-data");

    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", postData.length());

    client.beginBody();
    client.print(postData);
    client.endRequest();

    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    client.stop();
  } else {
    Serial.println("Kunde inte ansluta till servern!");

    printWifiStatus();

  }
  delay(1200000);    //20 minuter

}

/* -------------------------------------------------------------------------- */
void printWifiStatus() {
/* -------------------------------------------------------------------------- */
  // Skriv ut SSID som kortet är anslutet till:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Skriv ut kortets IP adress:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
  
}

