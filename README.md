# 🚗 Rancang Bangun Alat Uji Emisi Kendaraan Motor Bakar Menggunakan Modul Sensor MQ Series (Non-IoT)

## 📖 Deskripsi

Proyek ini merupakan prototipe alat uji emisi kendaraan bermotor berbasis **ESP32** dengan memanfaatkan sensor gas **MQ Series** untuk menganalisis respons gas hasil pembakaran kendaraan.

Sistem bekerja secara **Non-IoT**, sehingga seluruh proses pembacaan sensor, pengolahan data, dan evaluasi kelulusan emisi dilakukan secara lokal tanpa koneksi internet. 

Berdasarkan pembaruan sistem terbaru, proyek ini kini secara aktif menggunakan tiga sensor sekaligus, yaitu **MQ-2** untuk mendeteksi Hidrokarbon (HC), **MQ-7** untuk mendeteksi Karbon Monoksida (CO), dan **MQ-135** untuk mendeteksi Karbon Dioksida (CO2)[cite: 1].

---

## 🎯 Tujuan

- Merancang prototipe alat uji emisi kendaraan bermotor.
- Mendeteksi perubahan konsentrasi gas hasil pembakaran kendaraan dalam satuan *Part Per Million* (PPM)[cite: 1].
- Mengklasifikasikan kelulusan emisi secara otomatis berdasarkan standar pemerintah[cite: 1].
- Menampilkan hasil pengukuran secara real-time pada layar LCD dan Serial Monitor[cite: 1].

---

## ✨ Fitur Utama

- **Mikrokontroler ESP32:** Sebagai pusat pengolahan logika dan data[cite: 1].
- **Sistem Multisensor:** Pembacaan terintegrasi dari sensor MQ-2, MQ-7, dan MQ-135[cite: 1].
- **Antarmuka LCD Dinamis:** Layar LCD 16x2 I2C menampilkan dua halaman informasi secara bergantian setiap 3 detik[cite: 1].
- **Pemanasan Otomatis (Warm-up):** Sistem melakukan proses pemanasan sensor dengan indikator hitung mundur selama 60 detik sebelum pengujian dimulai[cite: 1].
- **Kalibrasi Baseline (Ro):** Pengambilan 50 sampel data di udara bersih secara otomatis untuk mengkalibrasi akurasi sensor[cite: 1].
- **Indikator Visual Berkedip:** Layar memberikan informasi status "LULUS UJI" atau "TIDAK LULUS" dengan efek teks berkedip setiap 500 milidetik[cite: 1].
- **Data Logging Lokal:** Sinkronisasi dan pengiriman data lengkap (HC, CO, CO2, dan status) ke Serial Monitor dengan *baud rate* 115200 untuk pencatatan di laptop[cite: 1].

---

## ⚖️ Standar Kelulusan Uji Emisi

Sistem klasifikasi kelulusan merujuk pada standar **Permen LHK No. 8 Tahun 2023**, dikhususkan untuk parameter motor produksi di bawah tahun 2010[cite: 1]:

- **Batas Maksimal Hidrokarbon (HC):** 2200 ppm[cite: 1].
- **Batas Maksimal Karbon Monoksida (CO):** 55000 ppm (setara dengan 5.5%)[cite: 1].

Jika pembacaan gas melampaui salah satu dari batas toleransi tersebut, sistem akan otomatis menetapkan status pengujian menjadi "TIDAK LULUS"[cite: 1].

---

## 🚧 Status Pengembangan

| Komponen | Status |
|----------|--------|
| ESP32 | ✅ Selesai |
| LCD 16x2 I2C | ✅ Selesai |
| Sensor MQ-2 | ✅ Digunakan |
| Sensor MQ-135 | ✅ Digunakan |
| Sensor MQ-7 | ✅ Digunakan |
| Pengujian Emisi Kendaraan | 🔄 Berlangsung |

---

## 🛠 Perangkat Keras & Konfigurasi Pin

- **Mikrokontroler:** ESP32 DevKit
- **Layar:** LCD 16x2 I2C (Terhubung ke Pin 21 untuk SDA, dan Pin 22 untuk SCL)[cite: 1]
- **Sensor MQ-2:** Terhubung ke Pin Analog 34[cite: 1]
- **Sensor MQ-7:** Terhubung ke Pin Analog 32[cite: 1]
- **Sensor MQ-135:** Terhubung ke Pin Analog 35[cite: 1]
- Modul I2C, Kabel Jumper, Kabel Micro USB, dan Box Proyek

---

## 💻 Perangkat Lunak

- Arduino IDE
- ESP32 Board Package
- Library `Wire.h`[cite: 1]
- Library `LiquidCrystal_I2C.h`[cite: 1]

---

## ⚙️ Cara Kerja Sistem

1. Saat ESP32 pertama kali dinyalakan, layar LCD akan menampilkan hitung mundur proses pemanasan (*Warm-up*) selama 60 detik[cite: 1].
2. Layar kemudian memberikan pesan konfirmasi "UJI COBA DIMULAI!" selama 2 detik[cite: 1].
3. Sistem secara otomatis mengkalibrasi nilai dasar (*baseline* / Ro) dengan mengeksekusi 50 iterasi pembacaan udara bersih pada ketiga sensor[cite: 1].
4. Sampel gas buang dimasukkan, dan mikrokontroler mengubah pembacaan nilai *raw* ADC menjadi format *Part Per Million* (PPM)[cite: 1].
5. Nilai PPM dievaluasi secara kontinu dan dibandingkan dengan batas maksimal standar emisi[cite: 1].
6. Tampilan layar LCD akan membagi informasi dan bertukar halaman setiap 3 detik (3000 ms)[cite: 1]:
   - **Halaman 1:** Menampilkan angka pembacaan secara real-time untuk gas HC, CO, dan CO2[cite: 1].
   - **Halaman 2:** Menampilkan kesimpulan status akhir ("LULUS UJI" / "TIDAK LULUS") secara berkedip untuk menarik perhatian[cite: 1].
7. Seluruh hasil pengukuran dan status secara paralel dikirimkan ke antarmuka Serial Monitor[cite: 1].

---

## 📁 Struktur Proyek

```text
.
├── Arduino/
│   └── rendi.ino
└── README.md
