#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <SPI.h>
#include <MFRC522.h>

/* ====== Broches NodeMCU v3 ====== */
// RC522 (SPI)
#define RC522_SS   D4   // SDA/SS du RC522 -> D8
#define RC522_RST  D3   // RST du RC522   -> D0
// SPI hardware : SCK=D5, MOSI=D7, MISO=D6

// LCD I2C
#define I2C_SDA    D2   // LCD SDA -> D2
#define I2C_SCL    D1   // LCD SCL -> D1

#define BUZZER_PIN D0   // Buzzer actif : IO -> D0, VCC -> 3V3, GND -> GND


/* ====== Modules ====== */
MFRC522 rfid(RC522_SS, RC522_RST);
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Adresse confirmée: 0x27

/* ====== Wi-Fi + API ====== */
const char* WIFI_SSID     = "Galaxy S20 FE 2875";
const char* WIFI_PASSWORD = "ggmv0478";
const char* SCRIPT_URL    = "https://script.google.com/macros/s/AKfycbzR5H095FDKNFVtOuRRn7qhGIAZdgYrJfCft-TL6HpD5rpbXyECvn0-V16zTdzt7ur8cA/exec";

/* ====== Helpers ====== */
String uidToHex(const byte* uid, byte size) {
  String s;
  for (byte i = 0; i < size; i++) { 
    if (uid[i] < 0x10) s += "0"; 
    s += String(uid[i], HEX); 
  }
  s.toUpperCase(); 
  return s;
}

void showIdle() {
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Scan a card...");
}

void showConnecting() {
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Connecting to");
  String ssid = WIFI_SSID; 
  if (ssid.length() > 16) ssid = ssid.substring(0,16);
  lcd.setCursor(0,1); 
  lcd.print(ssid);
}

void showConnected() {
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Connected OK");
  String ip = WiFi.localIP().toString(); 
  if (ip.length() > 16) ip = ip.substring(0,16);
  lcd.setCursor(0,1); 
  lcd.print(ip);
  delay(1200);
}

void showWifiFail() {
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("WiFi FAIL");
  lcd.setCursor(0,1); 
  lcd.print("Retrying...");
}

/* Connexion Wi-Fi (bloque jusqu’à succès au boot et force la reco si perdue) */
void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("WiFi: connecting to '%s'\n", WIFI_SSID);
  showConnecting();

  const unsigned long TIMEOUT = 15000;
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < TIMEOUT) { 
    delay(300); 
    Serial.print("."); 
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("WiFi OK  IP:%s  RSSI:%d dBm\n",
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
    showConnected();
  } else {
    Serial.println("WiFi FAIL (blocking until connected)");
    showWifiFail();
    while (WiFi.status() != WL_CONNECTED) {
      delay(2000);
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
    showConnected();
  }
}

/* ====== Arduino ====== */
void setup() {
  Serial.begin(115200);

  // LCD I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  // Wi-Fi
  wifiConnect();

  // RC522 (SPI: SCK=D5, MOSI=D7, MISO=D6)
  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_AntennaOn();
  Serial.println("RFID ready");

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // au repos


  showIdle();
}

void loop() {
  // 0) Vérifie Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WIFI] Offline → reconnexion...");
    showWifiFail();
    wifiConnect();
    showIdle();
    return;
  }

  // 1) Attend une carte
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // 2) UID lu
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Scanning");
  lcd.setCursor(0,1); 
  lcd.print("The card...");
  String uid = uidToHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println("========== RFID EVENT ==========");
  Serial.print  ("[RFID] UID = ");
  Serial.println(uid);

  // 3) Préparation de la requête
  String name = "UNKNOWN";
  String url  = String(SCRIPT_URL) + "?uid=" + uid;
  Serial.println("[HTTP] Préparation de la requête...");
  Serial.print  ("[HTTP] URL = ");
  Serial.println(url);

  // 4) Création du client HTTPS
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure(); // Ignore la vérification du certificat
  HTTPClient http;
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.setReuse(false);

  // 5) Lancement de la connexion
  Serial.println("[HTTP] Démarrage de la connexion...");
  bool beginOK = http.begin(*client, url);
  if (!beginOK) {
    Serial.println("[HTTP][ERROR] http.begin() a échoué !");
  } else {
    // 6) Envoi de la requête GET
    Serial.println("[HTTP] Envoi GET...");
    int code = http.GET();
    Serial.print("[HTTP] Code réponse = ");
    Serial.println(code);

    // 7) Lecture de la réponse
    if (code == HTTP_CODE_OK) {
      String body = http.getString();
      body.trim();
      Serial.print("[HTTP] Corps de réponse : '");
      Serial.print(body);
      Serial.println("'");

      if (body.length() > 0) {
        name = body;
        Serial.print("[GAS ] Nom reçu : ");
        Serial.println(name);
        Serial.println("[GAS ] Log ajouté dans Google Sheets (HTTP 200 OK)");
      } else {
        Serial.println("[GAS ] Réponse vide → name=UNKNOWN");
      }
    } else if (code > 0) {
      Serial.println("[HTTP][WARN] Réponse non-200 (log probable malgré tout)");
    } else {
      Serial.print("[HTTP][ERROR] Erreur de transport : ");
      Serial.println(code);
    }

    // === Feedback utilisateur selon résultat ===
    if (name == "UNKNOWN") {
      lcd.clear();
      lcd.setCursor(0,0); lcd.print("Badge unknown");
      lcd.setCursor(0,1); lcd.print("Access denied");

      // Buzzer : bip plus long (800 ms)
      digitalWrite(BUZZER_PIN, HIGH);
      delay(800);
      digitalWrite(BUZZER_PIN, LOW);

      Serial.println("[ACCESS] DENIED");
    } else {
      // LCD : Hello, Nom
      lcd.clear();
      lcd.setCursor(0,0); lcd.print("Hello,");
      if (name.length() > 16) name = name.substring(0,16);
      lcd.setCursor(0,1); lcd.print(name);

  // Buzzer : double bip court
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH); delay(120);
    digitalWrite(BUZZER_PIN, LOW);  delay(120);
  }
  Serial.println("[ACCESS] GRANTED");
}


    // 8) Fin HTTP
    http.end();
    Serial.println("[HTTP] Fermeture de la connexion");
  }

  // 10) Fin transaction RFID
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  Serial.println("[RFID] Halt + StopCrypto1");
  Serial.println("================================");

  delay(5000);
  showIdle();
}

