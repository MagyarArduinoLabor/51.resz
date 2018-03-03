#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22   // DHT 22/DHT11 stb.

DHT dht(DHTPIN, DHTTYPE);


const char* ssid = "";
const char* password = "";

const char *GScriptId = "";

const int dataPostDelay = 60000;  // 15 minutes = 15 * 60 * 1000

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";

const int httpsPort =  443;
HTTPSRedirect client(httpsPort);

//URL
String url = String("/macros/s/") + GScriptId + "/exec?";

//A fingerprint ellenőrzés valamilyen okból kifolyólag nem működik, ezért kikommentelem ideiglenesen
//const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());


  Serial.print(String("Connecting to "));
  Serial.println(host);

  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

//1 - csatlakozva, 0 - nem csatlakozva
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    Serial.flush();
    return;
  }

  //fingerprint ellenőrzés
  //A fingerprint ellenőrzés valamilyen okból kifolyólag nem működik, ezért kikommentelem ideiglenesen
  /* if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  } */
}

void postData(float value1, float value2) {
  if (!client.connected()) {
    Serial.println("Connecting to client again...");
    client.connect(host, httpsPort);
  }
  //url összerakása
  String urlFinal = url + "&value1=" + String(value1) + "&value2=" + String(value2);
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();
  client.printRedir(urlFinal, host, googleRedirHost);
}

void loop()
{
  //mérések
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //kiküldés
  postData(h, t);

  delay (dataPostDelay);
}
