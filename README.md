📡 MQTT Integration with Tasmota & ATmega via Serial Communication
🧰 Project Overview
Repositori ini berisi panduan untuk menghubungkan ESP dengan firmware Tasmota dan mikrokontroler ATmega menggunakan komunikasi Serial dan protokol MQTT. Sistem ini bergantung pada konfigurasi deep switch dan komunikasi serial untuk melakukan upload program serta mengatur komunikasi antar perangkat.

⚙️ Hardware Setup
DIP Switch Configuration:
Fungsi	Switch DIP
Upload Program Tasmota (ESP)	Switch 5,6,7 ON (lainnya OFF)
Upload Program ATmega	Switch 3,4 ON
Komunikasi Serial3	Switch 1,2,3,4 ON + TX3 RX3

📥 Upload Program
1. Upload Firmware Tasmota ke ESP
Atur DIP switch 5, 6, 7 ke posisi ON, sisanya OFF.

Hubungkan ESP ke PC menggunakan USB to TTL.

Upload firmware Tasmota menggunakan tool seperti Tasmotizer atau browser-based uploader di link ini:
👉 Upload Tasmota via Web

2. Upload Program ke ATmega
Atur DIP switch 3 dan 4 ON.

Upload program ATmega seperti biasa menggunakan ISP/Serial sesuai kebutuhan.

🌐 Setup Tasmota (ESP) via HP & MQTT
1. Sambungkan ESP ke WiFi HP
Nyalakan ESP. Tasmota akan membuat Access Point.

Koneksikan HP ke WiFi tersebut.

Buka browser dan masuk ke: 192.168.4.1

Pilih WiFi lokal dan masukkan password-nya.

2. Cek IP Address ESP
Gunakan router atau aplikasi seperti Fing untuk melihat alamat IP dari ESP yang terhubung.

3. Konfigurasi Perangkat Tasmota
Buka browser, masuk ke IP ESP.

Pilih Configuration → Configure Module.

Pilih:

Module Type: Generic (0)

Klik Save.

4. Set Pin untuk RX dan TX
Masuk ke Configuration → Configure Module lagi.

Atur:

GPIO1 → Serial Out (TX)

GPIO3 → Serial In (RX)

Klik Save.

5. Konfigurasi MQTT
Masuk ke Configuration → Configure MQTT.

Isi:

Host: IP Broker MQTT (misalnya: 192.168.1.100)

Port: 1883

Topic: sesuaikan sesuai kebutuhan

Klik Save

🧪 Pengujian via Console Tasmota
Masuk ke halaman Console Tasmota dari antarmuka web.

Kirimkan perintah berikut:
SerialSend5 <TOPIK_MQTT>
Contoh: SerialSend5 {"data":"hello from esp"}

Perintah ini akan mengirimkan string melalui Serial3 ke ATmega.
Pastikan komunikasi baudrate antara Tasmota dan ATmega cocok (default 9600).
Periksa koneksi fisik TX dan RX (TX ESP ke RX ATmega dan sebaliknya).

Jika menggunakan resistor pembagi tegangan untuk TX ESP → RX ATmega, pastikan aman 3.3V ↔ 5V.
