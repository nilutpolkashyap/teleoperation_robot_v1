  #include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <M5Core2.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "ASUS_X00TD"
#define WIFI_PASSWORD "biscuitppp"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDCE8PFIpoTwsz8WZ4ZJEoo6IOSjXDUV68"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-firebase-e009f-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
float val = 0.0;
bool signupOK = false;
String direc;
//String direc_array[] = {};
int ch = 0;

void setup(){
  Serial.begin(115200);

  M5.begin();
  M5.Lcd.begin(); 

  M5.Lcd.fillScreen(YELLOW);

  M5.Lcd.setTextColor(BLACK); 
  M5.Lcd.setTextSize(5); 

  M5.Lcd.setCursor(50, 40); 
  M5.Lcd.print("ESP32");

  M5.Lcd.setCursor(140, 95); 
  M5.Lcd.print(" +");

  M5.Lcd.setCursor(30, 150); 
  M5.Lcd.setTextColor(BLACK); 
  M5.Lcd.setTextSize(5);  
  M5.Lcd.print("FIREBASE");

  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/light", count)){
      Serial.println("PASSED");
      Serial.println(count);
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    
    // Write an Float number on the database path test/float

    val = random(0,100);
    
    if (Firebase.RTDB.setFloat(&fbdo, "exercise/float", 0.01 + val)){
      Serial.println("PASSED");
      Serial.println(val);
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    String direc_array[6] = {"FR", "BK", "TL", "TR", "RL", "RR"};
    ch = random(0,6);
    direc =  direc_array[ch];     //F("Hello World!");

    if (Firebase.RTDB.setString(&fbdo, "exercise/command", direc)){
      Serial.println("PASSED");
      Serial.println(direc);
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    
  }
}
