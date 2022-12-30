#define BLYNK_TEMPLATE_ID "TMPLmdrW21FE"
#define BLYNK_DEVICE_NAME "ASM2"
#define BLYNK_AUTH_TOKEN "nfyvEkawro8tFiG_FX73p4Na_bO49D6c"
// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
char ssid[] = "Greenwich-Student";
char pass[] = "12345678";
//
// Notice: replace your API key and database URL
#define API_KEY "AIzaSyDmH0oXihgxZVu0joDkK-B9O73uIn169Co"
#define DATABASE_URL "https://testing-96b7a-default-rtdb.asia-southeast1.firebasedatabase.app/"

// #  Use the NTP (Network Time Protocol) to get the time and date
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// # declare firebase database, authentication, and configuration objects
FirebaseData fbdo;
FirebaseAuth auth2;
FirebaseConfig config;
FirebaseJson json;
// # declare WiFiManager Object
WiFiManager wifiManager;
bool signupOK = false;

//define the pin of the photoresistor

//define the function to get the datetime
String getDatetime() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  String formattedTime = timeClient.getFormattedTime();
  return String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear) + " " + formattedTime;
}
/*
 *
 * All the resources for this project: http://randomnerdtutorials.com/
 * Modified by Rui Santos
 *
 * Created by FILIPEFLOP
 *
 */



#define SS_PIN D8
#define RST_PIN D0
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

void setup() {
  Serial.begin(115200);
  timeClient.begin();
  timeClient.setTimeOffset(25200);
  bool res = wifiManager.autoConnect("tanne");
  if (!res) Serial.println("Failed to connect");
  else Serial.println("Connected...yay ");
  Serial.println("Connecting to Wi-Fi");
  Serial.println("Connected with IP: ");
  // Assign the api key (required)
  config.api_key = API_KEY;
  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;
  /* 
Sign up on Firebase
In this lab, the database is opened for anonymous 
For a better security, you need to setup username and password
And fill it in the statement Firebase.signUp
*/
  if (Firebase.signUp(&config, &auth2, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  //Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth2);
  Firebase.reconnectWiFi(true);

  //////////////////////////
  pinMode(D2, OUTPUT);
  Blynk.begin(auth, ssid, pass);
  // Initiate a serial communication
  SPI.begin();         // Initiate  SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  Serial.println();
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

String prPath = "/RFID";
String timePath = "/RFID";
String databasePath = "/ASM2";
String parentPath;

void loop() {
  // Runs all Blynk stuff
  Blynk.run();
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  //A3 8E 22 B7
  if (content.substring(1) == "B4 8F 77 07")  //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    tone(D2, 2093, 100);
    delay(200);
    tone(D2, 2093, 100);
    delay(200);
    tone(D2, 2093, 100);
    delay(200);
    Blynk.logEvent("NOTIFY", "Tân hồ vừa quét thẻ");
    //FIRE BASE
    if (Firebase.ready() && signupOK) {
      String datetime = getDatetime();
      Serial.print("time: ");
      parentPath = databasePath + "/" + datetime;
      //set the JSON string
      json.set(timePath.c_str(), "TanHo");
      //send data to the real-time database
      if (Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
    //END FIRE BASE
    delay(3000);
  }
else if (content.substring(1) == "90 F3 38 20")  //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    tone(D2, 2093, 100);
    delay(200);
    tone(D2, 2093, 100);
    delay(200);
    tone(D2, 2093, 100);
    delay(200);
    Blynk.logEvent("NOTIFY", "Khoa vừa quét thẻ");
    //FIRE BASE
    if (Firebase.ready() && signupOK) {
      String datetime = getDatetime();
      Serial.print("time: ");
      parentPath = databasePath + "/" + datetime;
      //set the JSON string
      json.set(timePath.c_str(), "Khoa");
      //send data to the real-time database
      if (Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
    //END FIRE BASE
    delay(3000);
  }
  else {
    Serial.println(" Access denied");
    tone(D2, 100, 1000);
    delay(200);
    Blynk.logEvent("NOTIFY", "Một người lạ vừa quét thẻ");
    //FIRE BASE
    if (Firebase.ready() && signupOK) {
      String datetime = getDatetime();
      Serial.print("time: ");
      parentPath = databasePath + "/" + datetime;
      //set the JSON string
      json.set(timePath.c_str(), "Unknown");
      //send data to the real-time database
      if (Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
    //END FIRE BASE
    delay(3000);
  }
}