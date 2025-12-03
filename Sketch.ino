// ==========================================
//       KONFIGURASI BLYNK (ISI INI DULU!)
// ==========================================
// Copy 3 baris ini dari Dashboard Blynk (Device Info):
#define BLYNK_TEMPLATE_ID "TMPL6vDwhBRax"
#define BLYNK_TEMPLATE_NAME "Air Purifier"
#define BLYNK_AUTH_TOKEN "EMwsViDDD4Xk0cx3cMWqLQZ5O3TukaI4"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Masukkan Nama WiFi dan Passwordmu
char ssid[] = "tes";
char pass[] = "";

// ==========================================
//       SETTING LOGIKA RELAY
// ==========================================
#define RELAY_NYALA  LOW
#define RELAY_MATI   HIGH

// ==========================================
//       KONFIGURASI AMBANG BATAS
// ==========================================
float batasDebuMg   = 0.20;  
int batasGas        = 2200;  
int batasSuhu       = 32;    
float teganganNol   = 0.50; 

// ==========================================
//       DEFINISI PIN
// ==========================================
#define PIN_VO_DEBU  39  
#define PIN_LED_DEBU 14  
#define PIN_MQ135    35  
#define PIN_RELAY    26  // Pastikan ini pin yang benar (tadi kamu pakai 26)
#define DHTPIN       4   

DHT dht(DHTPIN, DHT11);
Adafruit_SSD1306 display(128, 64, &Wire, -1);
BlynkTimer timer; // Kita pakai timer, bukan delay()

// Variabel Global
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

// Fungsi ini akan dipanggil setiap 1 detik oleh Timer
void sendSensorData() {
  // --- 1. BACA SENSOR DEBU ---
  digitalWrite(PIN_LED_DEBU, LOW);
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(PIN_VO_DEBU);
  delayMicroseconds(deltaTime);
  digitalWrite(PIN_LED_DEBU, HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured * (3.3 / 4095.0);
  if (calcVoltage > teganganNol) {
    dustDensity = 0.17 * (calcVoltage - teganganNol);
  } else {
    dustDensity = 0.00;
  }

  // --- 2. BACA SENSOR LAIN ---
  int rawGas = analogRead(PIN_MQ135);
  float t = dht.readTemperature();
  if (isnan(t)) t = 0; 

  // Debug Serial
  Serial.print("D:"); Serial.print(dustDensity);
  Serial.print("| G:"); Serial.print(rawGas);
  Serial.print("| T:"); Serial.println(t);

  // --- 3. LOGIKA KONTROL ---
  bool bahayaTerdeteksi = false;
  String penyebab = "-";
  String statusUtama = "AMAN";

  if (dustDensity > batasDebuMg) {
    bahayaTerdeteksi = true;
    penyebab = "DEBU";
    statusUtama = "BAHAYA";
  } 
  else if (rawGas > batasGas) {
    bahayaTerdeteksi = true;
    penyebab = "ASAP";
    statusUtama = "BAHAYA";
  }
  else if (t > batasSuhu) {
    bahayaTerdeteksi = true;
    penyebab = "PANAS";
    statusUtama = "WARNING";
  }

  // --- 4. EKSEKUSI RELAY & KIRIM KE BLYNK ---
  if (bahayaTerdeteksi == true) {
    digitalWrite(PIN_RELAY, RELAY_NYALA); 
    Blynk.virtualWrite(V4, 1); // Kirim status NYALA ke Widget LED/Button di HP
  } else {
    digitalWrite(PIN_RELAY, RELAY_MATI); 
    Blynk.virtualWrite(V4, 0); // Kirim status MATI
  }

  // --- 5. KIRIM DATA SENSOR KE BLYNK ---
  Blynk.virtualWrite(V0, dustDensity); // Kirim Debu
  Blynk.virtualWrite(V1, rawGas);      // Kirim Gas
  Blynk.virtualWrite(V2, t);           // Kirim Suhu
  Blynk.virtualWrite(V3, statusUtama); // Kirim Status Teks

  // --- 6. UPDATE OLED ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("D:"); display.print(dustDensity, 2);
  display.print(" G:"); display.print(rawGas);
  display.setCursor(90, 0); 
  display.print(t, 0); display.print("C");

  display.setCursor(0, 20);
  display.setTextSize(2);
  display.println(statusUtama);

  display.setTextSize(1);
  display.setCursor(0, 45);
  if (bahayaTerdeteksi) {
    display.print("Sebab: "); display.println(penyebab);
    display.setCursor(0, 55); 
    display.println(">> FAN ON <<");
  } else {
    display.println("Kualitas Udara");
    display.setCursor(0, 55);
    display.println("Bagus / Normal");
  }
  display.display();
}

void setup() {
  Serial.begin(115200);

  // Setup Pin
  pinMode(PIN_LED_DEBU, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_MQ135, INPUT);
  analogSetAttenuation(ADC_11db);

  // Posisi Awal Mati
  digitalWrite(PIN_RELAY, RELAY_MATI); 

  // Setup Sensor & OLED
  dht.begin();
  Wire.begin(21, 22); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED Error"));
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println("Connecting WiFi...");
  display.display();

  // --- KONEKSI KE BLYNK ---
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup Timer: Jalankan fungsi 'sendSensorData' setiap 1000ms (1 detik)
  timer.setInterval(1000L, sendSensorData);

  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("ONLINE BLYNK!");
  display.display();
  delay(2000);
}

void loop() {
  // Loop harus bersih, cuma boleh ada ini:
  Blynk.run();
  timer.run();
}
