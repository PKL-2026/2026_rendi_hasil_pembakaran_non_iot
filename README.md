\# 🚗 Rancang Bangun Alat Uji Emisi Kendaraan Motor Bakar Menggunakan Modul Sensor MQ Series (Non-IoT)



\## 📖 Deskripsi



Proyek ini merupakan prototipe alat uji emisi kendaraan bermotor berbasis \*\*ESP32\*\* dengan memanfaatkan sensor gas \*\*MQ Series\*\* untuk menganalisis respons gas hasil pembakaran kendaraan.



Sistem bekerja secara \*\*Non-IoT\*\*, sehingga seluruh proses pembacaan sensor, pengolahan data, dan klasifikasi dilakukan secara lokal tanpa koneksi internet.



Saat ini proyek menggunakan dua sensor aktif, yaitu \*\*MQ-2\*\* dan \*\*MQ-135\*\*, sedangkan \*\*MQ-7\*\* masih dalam tahap pengembangan.



\---



\## 🎯 Tujuan



\- Merancang prototipe alat uji emisi kendaraan bermotor.

\- Mendeteksi perubahan respons gas hasil pembakaran kendaraan.

\- Mengklasifikasikan kondisi emisi berdasarkan perubahan nilai sensor.

\- Menampilkan hasil pengukuran secara real-time pada LCD dan Serial Monitor.



\---



\## ✨ Fitur



\- ESP32 sebagai mikrokontroler utama.

\- Sensor MQ-2.

\- Sensor MQ-135.

\- LCD 16x2 I2C.

\- Kalibrasi baseline otomatis.

\- Proses pemanasan sensor (Warm-up).

\- Pengukuran respons gas secara real-time.

\- Klasifikasi kondisi emisi.

\- Tampilan hasil pada LCD.

\- Monitoring melalui Serial Monitor.



\---



\## 🚧 Status Pengembangan



| Komponen | Status |

|----------|--------|

| ESP32 | ✅ Selesai |

| LCD 16x2 I2C | ✅ Selesai |

| Sensor MQ-2 | ✅ Digunakan |

| Sensor MQ-135 | ✅ Digunakan |

| Sensor MQ-7 | ⏳ Dalam Pengembangan |

| Pengujian Emisi Kendaraan | 🔄 Berlangsung |



\---



\## 🛠 Perangkat Keras



\- ESP32 DevKit

\- Expansion Board ESP32

\- Sensor MQ-2

\- Sensor MQ-135

\- LCD 16x2 I2C

\- Modul I2C

\- Kabel Jumper

\- Kabel Micro USB

\- Box Proyek



\---



\## 💻 Perangkat Lunak



\- Arduino IDE

\- ESP32 Board Package

\- Library Wire

\- Library LiquidCrystal\_I2C



\---



\## ⚙️ Cara Kerja Sistem



1\. ESP32 dinyalakan.

2\. Sensor melakukan proses pemanasan (Warm-up).

3\. Sistem mengambil nilai baseline pada udara bersih.

4\. Sampel gas dari hasil pembakaran kendaraan diberikan ke sensor.

5\. ESP32 membaca nilai analog dari sensor.

6\. Sistem menghitung persentase perubahan terhadap baseline.

7\. Nilai respons diklasifikasikan menjadi:

&#x20;  - 🟢 Sehat

&#x20;  - 🟡 Sedang

&#x20;  - 🟠 Tinggi

&#x20;  - 🔴 Bahaya

8\. Hasil ditampilkan pada LCD dan Serial Monitor.



\---



\## 📊 Kategori Respons Gas



| Persentase Respons | Kategori |

|-------------------:|----------|

| < 10% | 🟢 Sehat |

| 10% – < 30% | 🟡 Sedang |

| 30% – < 60% | 🟠 Tinggi |

| ≥ 60% | 🔴 Bahaya |



Kategori akhir ditentukan berdasarkan \*\*nilai respons terbesar\*\* antara sensor MQ-2 dan MQ-135.



\---



\## 📁 Struktur Proyek



```text

.

├── Arduino/

│   └── sketch\_GAS\_FINAL.ino

│

├── Dokumentasi/

│   ├── Diagram Blok

│   ├── Flowchart

│   ├── Pengujian

│   └── Datasheet

│

├── Gambar/

│   ├── Rangkaian

│   ├── Prototype

│   └── LCD

│

└── README.md

```



\---



\## 🔄 Pengembangan Selanjutnya



\- Menambahkan sensor MQ-7 untuk mendeteksi gas Karbon Monoksida (CO).

\- Menambahkan penyimpanan data (Data Logging).

\- Meningkatkan akurasi klasifikasi.

\- Pengembangan versi IoT.



\---



\## 👨‍💻 Pengembang



\*\*Rendi\*\*



Proyek Akhir 2026



\---



\## 📄 Lisensi



Proyek ini dikembangkan untuk keperluan pendidikan, penelitian, dan pengembangan prototipe.

