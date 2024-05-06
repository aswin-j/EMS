
#include<WiFi.h>
#include<Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Kerala Vision FTTH"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyCzzB_xHaeuRLOsx0aWZRgaRYOMgivzMeU"
#define DATABASE_URL "https://environment-monitoring-s-19cfd-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define LED1_PIN 12
#define LED2_PIN 14
#define LDR_PIN 36

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int ldrData = 0;
float voltage =0.0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("connectinf to wifi");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("."); delay(300);
  }
  Serial.println();
  Serial.print("connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("signup ok");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n",config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
}

void loop() {
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    //.......stores sensor data to a RTDB
    ldrData = analogRead(LDR_PIN);
    voltage = (float)LDR_PIN/1000;
    if(Firebase.RTDB.setInt(&fbdo, "Sensor/ldr_data", ldrData))
    {
      Serial.println();Serial.print(ldrData);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println("(" + fbdo.dataType() +")");
    }
    else
    {
      Serial.println("FAILED:" + fbdo.errorReason());
    }
    if(Firebase.RTDB.setFloat(&fbdo, "Sensor/voltage", voltage))
    {
      Serial.println();Serial.print(voltage);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println("(" + fbdo.dataType() +")");
    }
    else
    {
      Serial.println("FAILED:" + fbdo.errorReason());
    }
   }
    
  }
  
 
