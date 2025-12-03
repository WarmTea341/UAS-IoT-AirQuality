# Smart Indoor Air Quality Monitoring System 🍃

Proyek ini adalah sistem pemantauan kualitas udara cerdas berbasis IoT menggunakan **ESP32**. Sistem ini dapat mendeteksi suhu, gas berbahaya, dan debu, serta mengaktifkan kipas ventilasi secara otomatis jika udara buruk.

Sholeh Afandi Al Hakim (240605110162)

##  Fitur Utama
- **Monitoring Real-time:** Suhu, Kelembapan, Gas (CO/CO2), Debu.
- **Otomatisasi:** Kipas menyala otomatis jika nilai sensor melebihi batas.
- **IoT Integration:** Monitoring via Aplikasi Blynk / Telegram.
- **Dual Display:** Data tampil di layar OLED 0.96" dan Smartphone.

##  Komponen Hardware
- ESP32 Devkit V1
- Sensor MQ-135 (Gas)
- Sensor DHT11 (Suhu)
- Sensor GP2Y1010AU0F (Debu)
- Modul Relay & Kipas DC 5V
- OLED SSD1306


##  Cara Menjalankan
1. Buka file `IAQ_Monitor.ino` di Arduino IDE.
2. Install library: `Blynk`, `DHT`, `Adafruit_SSD1306`.
3. Masukkan Token Blynk dan WiFi Credential pada kode.
4. Upload ke ESP32.
