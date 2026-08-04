#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ======================================================
// KONFIGURASI PERANGKAT
// ======================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte PIN_MQ2   = 34;
const byte PIN_MQ135 = 35;

//======================================================
// MODE FINAL PROYEK
//======================================================

const unsigned long PEMANASAN_MS   = 60000;   // 60 detik
const unsigned long BASELINE_MS    = 20000;   // 20 detik
const unsigned long PERSIAPAN_MS   = 10000;   // 10 detik
const unsigned long PENGUKURAN_MS  = 60000;   // 60 detik
const unsigned long JENDELA_STABIL = 20000;   // 20 detik terakhir

const unsigned long INTERVAL_BACA  = 250;
const unsigned long INTERVAL_LCD   = 500;

// ======================================================
// AMBANG KLASIFIKASI RESPONS RELATIF
// ======================================================
const float AMBANG_SEDANG = 20.0;
const float AMBANG_TINGGI = 50.0;
const int AMBANG_BAHAYA = 4000;

// ======================================================
// VARIABEL HASIL
// ======================================================
float baselineMQ2   = 0;
float baselineMQ135 = 0;

float rataStabilMQ2   = 0;
float rataStabilMQ135 = 0;

float responsMQ2      = 0;
float responsMQ135    = 0;
float responsDominan  = 0;

int minimumMQ2   = 4095;
int minimumMQ135 = 4095;
int maksimumMQ2  = 0;
int maksimumMQ135 = 0;

unsigned long jumlahDataStabil = 0;

const char* kategoriAkhir = "BELUM";

// ======================================================
// MEMBACA SENSOR: RATA-RATA 10 PEMBACAAN
// ======================================================
int bacaSensor(byte pin) {
  unsigned long total = 0;

  for (byte i = 0; i < 10; i++) {
    total += analogRead(pin);
    delay(2);
  }

  return total / 10;
}

// ======================================================
// MEMBERSIHKAN SATU BARIS LCD
// ======================================================
void bersihkanBaris(byte baris) {
  lcd.setCursor(0, baris);
  lcd.print("                ");
}

// ======================================================
// HITUNG MUNDUR
// ======================================================
void tampilHitungMundur(const char* judul,
                        unsigned long durasi) {
  unsigned long mulai = millis();
  unsigned long tampilTerakhir = 0;

  lcd.clear();

  while (millis() - mulai < durasi) {
    unsigned long sekarang = millis();
    unsigned long berjalan = sekarang - mulai;
    unsigned long sisa =
      (durasi - berjalan + 999UL) / 1000UL;

    if (sekarang - tampilTerakhir >= 200 ||
        tampilTerakhir == 0) {
      bersihkanBaris(0);
      lcd.setCursor(0, 0);
      lcd.print(judul);

      bersihkanBaris(1);
      lcd.setCursor(0, 1);
      lcd.print("Sisa:");
      lcd.print(sisa);
      lcd.print(" detik");

      tampilTerakhir = sekarang;
    }

    delay(10);
  }
}

// ======================================================
// PEMANASAN SENSOR
// ======================================================
void pemanasanSensor() {
  Serial.println();
  Serial.println("===== PEMANASAN SENSOR =====");
  Serial.println("Biarkan sensor di udara bersih.");

  tampilHitungMundur("PEMANASAN MQ", PEMANASAN_MS);

  Serial.println("Pemanasan selesai.");
}

// ======================================================
// MENGAMBIL BASELINE UDARA BERSIH
// ======================================================
void ambilBaseline() {
  unsigned long mulai = millis();
  unsigned long bacaTerakhir = 0;
  unsigned long tampilTerakhir = 0;

  unsigned long totalMQ2 = 0;
  unsigned long totalMQ135 = 0;
  unsigned long jumlahData = 0;

  Serial.println();
  Serial.println("===== BASELINE UDARA BERSIH =====");
  Serial.println("Waktu_s,MQ2,MQ135");

  lcd.clear();

  while (millis() - mulai < BASELINE_MS) {
    unsigned long sekarang = millis();

    if (sekarang - bacaTerakhir >= INTERVAL_BACA) {
      bacaTerakhir = sekarang;

      int mq2 = bacaSensor(PIN_MQ2);
      int mq135 = bacaSensor(PIN_MQ135);

      totalMQ2 += mq2;
      totalMQ135 += mq135;
      jumlahData++;

      float waktu = (sekarang - mulai) / 1000.0;

      Serial.print(waktu, 2);
      Serial.print(",");
      Serial.print(mq2);
      Serial.print(",");
      Serial.println(mq135);

      if (sekarang - tampilTerakhir >= INTERVAL_LCD ||
          tampilTerakhir == 0) {
        unsigned long sisa =
          (BASELINE_MS - (sekarang - mulai) + 999UL) / 1000UL;

        bersihkanBaris(0);
        lcd.setCursor(0, 0);
        lcd.print("B2:");
        lcd.print(mq2);
        lcd.print(" T:");
        lcd.print(sisa);

        bersihkanBaris(1);
        lcd.setCursor(0, 1);
        lcd.print("B135:");
        lcd.print(mq135);

        tampilTerakhir = sekarang;
      }
    }

    delay(1);
  }

  if (jumlahData > 0) {
    baselineMQ2 = (float)totalMQ2 / jumlahData;
    baselineMQ135 = (float)totalMQ135 / jumlahData;
  }

  Serial.println();
  Serial.print("Baseline MQ-2   : ");
  Serial.println(baselineMQ2, 2);
  Serial.print("Baseline MQ-135 : ");
  Serial.println(baselineMQ135, 2);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("B2:");
  lcd.print(baselineMQ2, 0);

  lcd.setCursor(0, 1);
  lcd.print("B135:");
  lcd.print(baselineMQ135, 0);

  delay(3000);
}

// ======================================================
// PERSIAPAN SAMPEL
// ======================================================
void persiapanSampel() {
  Serial.println();
  Serial.println("===== BERIKAN GAS =====");
  Serial.println("Mulai alirkan gas ke kedua sensor.");
  Serial.println("Pertahankan gas sampai pengukuran selesai.");

  tampilHitungMundur("BERIKAN GAS", PERSIAPAN_MS);
}

// ======================================================
// PENGUKURAN GAS
// ======================================================
void ukurSampel() {
  unsigned long mulai = millis();
  unsigned long bacaTerakhir = 0;
  unsigned long tampilTerakhir = 0;

  unsigned long totalStabilMQ2 = 0;
  unsigned long totalStabilMQ135 = 0;

  jumlahDataStabil = 0;

  minimumMQ2 = 4095;
  minimumMQ135 = 4095;
  maksimumMQ2 = 0;
  maksimumMQ135 = 0;

  Serial.println();
  Serial.println("===== PENGUKURAN GAS =====");
  Serial.println("Waktu_s,MQ2,MQ135,Fase");

  lcd.clear();

  while (millis() - mulai < PENGUKURAN_MS) {
    unsigned long sekarang = millis();

    if (sekarang - bacaTerakhir < INTERVAL_BACA) {
      delay(1);
      continue;
    }

    bacaTerakhir = sekarang;

    unsigned long waktu = sekarang - mulai;

    int mq2 = bacaSensor(PIN_MQ2);
    int mq135 = bacaSensor(PIN_MQ135);

    if (mq2 < minimumMQ2) minimumMQ2 = mq2;
    if (mq2 > maksimumMQ2) maksimumMQ2 = mq2;

    if (mq135 < minimumMQ135) minimumMQ135 = mq135;
    if (mq135 > maksimumMQ135) maksimumMQ135 = mq135;

    bool faseStabil =
      waktu >= (PENGUKURAN_MS - JENDELA_STABIL);

    if (faseStabil) {
      totalStabilMQ2 += mq2;
      totalStabilMQ135 += mq135;
      jumlahDataStabil++;
    }

    Serial.print(waktu / 1000.0, 2);
    Serial.print(",");
    Serial.print(mq2);
    Serial.print(",");
    Serial.print(mq135);
    Serial.print(",");
    Serial.println(faseStabil ? "STABIL" : "TRANSISI");

    if (sekarang - tampilTerakhir >= INTERVAL_LCD ||
        tampilTerakhir == 0) {
      unsigned long sisa =
        (PENGUKURAN_MS - waktu + 999UL) / 1000UL;

      bersihkanBaris(0);
      lcd.setCursor(0, 0);
      lcd.print("M2:");
      lcd.print(mq2);
      lcd.print(" T:");
      lcd.print(sisa);

      bersihkanBaris(1);
      lcd.setCursor(0, 1);
      lcd.print("M135:");
      lcd.print(mq135);

      tampilTerakhir = sekarang;
    }
  }

  if (jumlahDataStabil > 0) {
    rataStabilMQ2 =
      (float)totalStabilMQ2 / jumlahDataStabil;

    rataStabilMQ135 =
      (float)totalStabilMQ135 / jumlahDataStabil;
  }
}

// ======================================================
// MENGHITUNG HASIL
// ======================================================
void hitungHasil() {
  if (baselineMQ2 > 0) {
    responsMQ2 =
      ((rataStabilMQ2 - baselineMQ2) /
       baselineMQ2) * 100.0;
  }

  if (baselineMQ135 > 0) {
    responsMQ135 =
      ((rataStabilMQ135 - baselineMQ135) /
       baselineMQ135) * 100.0;
  }

  // Penurunan nilai tidak dihitung sebagai peningkatan gas
  if (responsMQ2 < 0) responsMQ2 = 0;
  if (responsMQ135 < 0) responsMQ135 = 0;

  responsDominan = max(responsMQ2, responsMQ135);

  if (maksimumMQ2 >= AMBANG_BAHAYA ||
      maksimumMQ135 >= AMBANG_BAHAYA) {
    kategoriAkhir = "BAHAYA";
  } else if (responsDominan < AMBANG_SEDANG) {
    kategoriAkhir = "SEHAT";
  } else if (responsDominan <= AMBANG_TINGGI) {
    kategoriAkhir = "SEDANG";
  } else {
    kategoriAkhir = "TINGGI";
  }
}

// ======================================================
// HASIL SERIAL MONITOR
// ======================================================
void cetakHasilSerial() {
  Serial.println();
  Serial.println("========== HASIL AKHIR ==========");

  Serial.print("Baseline MQ-2          : ");
  Serial.println(baselineMQ2, 2);
  Serial.print("Rata-rata stabil MQ-2  : ");
  Serial.println(rataStabilMQ2, 2);
  Serial.print("Minimum MQ-2           : ");
  Serial.println(minimumMQ2);
  Serial.print("Maksimum MQ-2          : ");
  Serial.println(maksimumMQ2);
  Serial.print("Respons MQ-2           : ");
  Serial.print(responsMQ2, 2);
  Serial.println("%");

  Serial.println();

  Serial.print("Baseline MQ-135        : ");
  Serial.println(baselineMQ135, 2);
  Serial.print("Rata-rata stabil MQ135 : ");
  Serial.println(rataStabilMQ135, 2);
  Serial.print("Minimum MQ-135         : ");
  Serial.println(minimumMQ135);
  Serial.print("Maksimum MQ-135        : ");
  Serial.println(maksimumMQ135);
  Serial.print("Respons MQ-135         : ");
  Serial.print(responsMQ135, 2);
  Serial.println("%");

  Serial.println();

  Serial.print("Respons dominan        : ");
  Serial.print(responsDominan, 2);
  Serial.println("%");
  Serial.print("Kategori akhir         : ");
  Serial.println(kategoriAkhir);

  if (maksimumMQ2 >= AMBANG_BAHAYA ||
      maksimumMQ135 >= AMBANG_BAHAYA) {
    Serial.println("PERINGATAN: ADC mendekati saturasi.");
  }

  Serial.println("=================================");
  Serial.println("Tekan EN/RESET untuk mengulang.");
}

// ======================================================
// HASIL LCD BERGANTIAN
// ======================================================
void tampilkanHasilLCD() {
  while (true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HASIL:");
    lcd.print(kategoriAkhir);

    lcd.setCursor(0, 1);
    lcd.print("Dominan:");
    lcd.print(responsDominan, 1);
    lcd.print("%");

    delay(3000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MQ2:");
    lcd.print(responsMQ2, 1);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("MQ135:");
    lcd.print(responsMQ135, 1);
    lcd.print("%");

    delay(3000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Max2:");
    lcd.print(maksimumMQ2);

    lcd.setCursor(0, 1);
    lcd.print("Max135:");
    lcd.print(maksimumMQ135);

    delay(3000);
  }
}

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_MQ2, ADC_11db);
  analogSetPinAttenuation(PIN_MQ135, ADC_11db);

  lcd.clear();
lcd.setCursor(0,0);
lcd.print("KLASIFIKASI");

lcd.setCursor(0,1);
lcd.print("RESPONS GAS");

  Serial.println();
Serial.println("=================================");
  Serial.println("Sensor : MQ-2 & MQ-135");
  Serial.println("Mikrokontroler : ESP32");
  Serial.println("Supply Sensor : 5 Volt");
  Serial.println("ADC ESP32 : 12-bit");
Serial.println("MQ-2 DAN MQ-135");
Serial.println("Sensor Supply : 5V");
Serial.println("ESP32 ADC     : 12-bit");
Serial.println("=================================");

  delay(3000);

  pemanasanSensor();
  ambilBaseline();
  persiapanSampel();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MENGUKUR GAS");
  lcd.setCursor(0, 1);
  lcd.print("Gas tetap masuk");

  delay(1000);

  ukurSampel();
  hitungHasil();
  cetakHasilSerial();
  tampilkanHasilLCD();
}

// ======================================================
// LOOP
// ======================================================
void loop() {
  // Pengujian berlangsung satu kali.
  // Tekan tombol EN/RESET untuk mengulang.
}

//================== RINGKASAN HASIL ==================
void tampilRingkasan(float respMQ2,float respMQ135,String kategori){
  Serial.println();
  Serial.println("==========================");
  Serial.println("HASIL PENGUJIAN");
  Serial.println("==========================");
  Serial.print("Baseline MQ-2    : "); Serial.println(baselineMQ2);
  Serial.print("Baseline MQ-135  : "); Serial.println(baselineMQ135);
  Serial.print("Respons MQ-2 (%) : "); Serial.println(respMQ2,2);
  Serial.print("Respons MQ-135(%): "); Serial.println(respMQ135,2);
  Serial.print("MQ-2 Maksimum    : "); Serial.println(maksimumMQ2);
  Serial.print("MQ-135 Maksimum  : "); Serial.println(maksimumMQ135);
  Serial.print("Kategori         : "); Serial.println(kategori);
  Serial.println("==========================");
}

//==================== KATEGORI LCD ====================
String kategoriGas(float respons){
  if(respons < 10.0) return "SEHAT";
  else if(respons < 30.0) return "SEDANG";
  else if(respons < 60.0) return "TINGGI";
  else return "BAHAYA";
}

/*
Contoh penggunaan setelah menghitung respons:
float responsTerbesar = max(responsMQ2, responsMQ135);
String status = kategoriGas(responsTerbesar);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("MQ2:");
lcd.print(responsMQ2,1);
lcd.print("%");

lcd.setCursor(0,1);
lcd.print("MQ135:");
lcd.print(responsMQ135,1);
lcd.print("%");

lcd.setCursor(0,2);
lcd.print("STATUS:");

lcd.setCursor(0,3);
lcd.print(status);
*/