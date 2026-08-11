#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ======================================================
// KONFIGURASI PERANGKAT & PIN SENSOR
// ======================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte PIN_MQ2   = 34; // Pin analog MQ-2 (Deteksi HC)
const byte PIN_MQ7   = 32; // Pin analog MQ-7 (Deteksi CO)
const byte PIN_MQ135 = 35; // Pin analog MQ-135 (Deteksi CO2)

// ======================================================
// AMBANG BATAS UJI EMISI ASLI (Permen LHK No. 8 Tahun 2023)
// Kita gunakan standar motor < 2010 (Shogun) sebagai batas maksimal alat
// ======================================================
const int MAKS_PASARAN_HC = 2200;  // Batas HC Maksimum: 2200 ppm
const int MAKS_PASARAN_CO = 55000; // Batas CO Maksimum: 5.5% = 55000 ppm

// ======================================================
// KONSTANTA KALIBRASI SENSOR MQ
// ======================================================
float Ro_MQ2   = 1.0;
float Ro_MQ7   = 1.0;
float Ro_MQ135 = 1.0;

const float A_MQ2 = 574.25;  const float B_MQ2 = -2.222;  
const float A_MQ7 = 99.042;  const float B_MQ7 = -1.518;  
const float A_MQ135 = 110.47; const float B_MQ135 = -2.862; 

// ======================================================
// VARIABEL TIMING & KONTROL SISTEM (MILLIS)
// ======================================================
unsigned long waktuSiklusMQ7 = 0;
bool faseBacaMQ7 = false;

unsigned long waktuPindahHalaman = 0;
bool halamanDua = false; // false = Halaman Data PPM, true = Halaman Status

// ======================================================
// FUNGSI HITUNG RESISTANSI & PPM
// ======================================================
float hitungRs(int nilaiADC) {
  if (nilaiADC <= 0) return 999999.0; 
  if (nilaiADC >= 4095) return 0.0001;
  return (4095.0 - nilaiADC) / (float)nilaiADC;
}

int hitungPPM(int nilaiADC, float Ro, float A_base, float B_slope) {
  float Rs = hitungRs(nilaiADC);
  float rasio = Rs / Ro;
  float ppm = A_base * pow(rasio, B_slope);
  if (ppm < 0) ppm = 0;
  if (ppm > 100000) ppm = 100000; // Pembatasan display
  return (int)ppm;
}

int bacaADC(byte pin) {
  unsigned long total = 0;
  for (byte i = 0; i < 10; i++) {
    total += analogRead(pin);
    delay(2);
  }
  return total / 10; 
}

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();
  
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_MQ2, ADC_11db);
  analogSetPinAttenuation(PIN_MQ7, ADC_11db);
  analogSetPinAttenuation(PIN_MQ135, ADC_11db);

  // ======================================================
  // 1. PEMANASAN SENSOR (60 DETIK)
  // ======================================================
  for (int i = 60; i > 0; i--) {
    lcd.setCursor(0, 0);
    lcd.print("  UJI EMISI ");
    lcd.setCursor(0, 1);
    lcd.print("  PEMANASAN: ");
    if (i < 10) lcd.print("0"); // Tambah angka 0 agar tampil 2 digit
    lcd.print(i);
    lcd.print("s  ");
    delay(1000);
  }

  // ======================================================
  // 2. TAMPILKAN "UJI COBA DIMULAI"
  // ======================================================
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   UJI  COBA    ");
  lcd.setCursor(0, 1);
  lcd.print("    DIMULAI!    ");
  delay(2000);

  // ======================================================
  // 3. KALIBRASI SENSOR RO (UDARA BERSIH)
  // ======================================================
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("ALAT UJI EMISI");
  lcd.setCursor(0, 1); lcd.print("HASIL PEMBAKARAN");
  
  long totalMQ2 = 0, totalMQ7 = 0, totalMQ135 = 0;
  for (int i = 0; i < 50; i++) {
    totalMQ2 += analogRead(PIN_MQ2);
    totalMQ7 += analogRead(PIN_MQ7);
    totalMQ135 += analogRead(PIN_MQ135);
    delay(100);
  }
  Ro_MQ2   = hitungRs(totalMQ2 / 50);
  Ro_MQ7   = hitungRs(totalMQ7 / 50);
  Ro_MQ135 = hitungRs(totalMQ135 / 50);

  waktuSiklusMQ7 = millis();
  waktuPindahHalaman = millis();
  lcd.clear();
}

// ======================================================
// LOOP UTAMA
// ======================================================
void loop() {
  // Logika Software Siklus Semu MQ-7
  if (millis() - waktuSiklusMQ7 > 60000) {
    faseBacaMQ7 = !faseBacaMQ7;
    waktuSiklusMQ7 = millis();
  }

  // 1. Baca nilai raw ADC ketiga sensor
  int adc_mq2   = bacaADC(PIN_MQ2);
  int adc_mq7   = bacaADC(PIN_MQ7);
  int adc_mq135 = bacaADC(PIN_MQ135);

  // 2. Hitung nilai PPM masing-masing gas
  int ppmHC  = hitungPPM(adc_mq2,   Ro_MQ2,   A_MQ2,   B_MQ2);   
  int ppmCO  = hitungPPM(adc_mq7,   Ro_MQ7,   A_MQ7,   B_MQ7);   
  int ppmCO2 = hitungPPM(adc_mq135, Ro_MQ135, A_MQ135, B_MQ135);  

  // 3. Logika Kelulusan Asli
  String statusLayar = "LULUS UJI";
  if (ppmHC > MAKS_PASARAN_HC || ppmCO > MAKS_PASARAN_CO) {
    statusLayar = "TIDAK LULUS";
  }

  // ======================================================
  // LOGIKA PERGANTIAN HALAMAN LCD (Tiap 3 Detik / 3000 ms)
  // ======================================================
  if (millis() - waktuPindahHalaman > 3000) {
    halamanDua = !halamanDua; // Tukar status halaman
    waktuPindahHalaman = millis();
    lcd.clear(); // Bersihkan layar agar teks halaman sebelumnya tidak menumpuk
  }

  // 4. Manajemen Tampilan Layar LCD
  if (halamanDua == false) {
    // --------------------------------------------------
    // HALAMAN 1: TAMPILAN DATA ANGKA PPM REAL-TIME
    // --------------------------------------------------
    // Baris 1: Tampilkan data polutan utama (HC dan CO)
    lcd.setCursor(0, 0);
    lcd.print("HC:"); lcd.print(ppmHC);
    lcd.print(" CO:"); lcd.print(ppmCO);
    lcd.print("      "); // Menghapus sisa karakter angka lama

    // Baris 2: Tampilkan CO2
    lcd.setCursor(0, 1);
    lcd.print("CO2:"); lcd.print(ppmCO2);
    lcd.print(" ppm      ");
    
  } else {
    // --------------------------------------------------
    // HALAMAN 2: TAMPILAN STATUS KELULUSAN (KEDAP-KEDIP)
    // --------------------------------------------------
    // Membuat efek kedap-kedip tanpa delay berbasis sisa waktu millis (Tiap 500ms)
    if ((millis() / 500) % 2 == 0) {
      lcd.setCursor(0, 0);
      lcd.print("  HASIL UTAMA:  ");
      
      lcd.setCursor(0, 1);
      if (statusLayar == "LULUS UJI") {
        lcd.print("   LULUS UJI   "); // Diposisikan di tengah layar 16x2
      } else {
        lcd.print("  TIDAK LULUS   "); // Diposisikan di tengah layar 16x2
      }
    } else {
      // Saat kedip mati, kosongkan baris tulisan status agar terlihat berkedip
      lcd.setCursor(0, 0);
      lcd.print("  HASIL UTAMA:  ");
      lcd.setCursor(0, 1);
      lcd.print("                "); // Kosong 16 spasi (efek kedip mati)
    }
  }

  // 5. Cetak Detail ke Serial Monitor (Untuk Backup Data di Laptop)
  Serial.print("HC: "); Serial.print(ppmHC);
  Serial.print(" | CO: "); Serial.print(ppmCO);
  Serial.print(" | CO2: "); Serial.print(ppmCO2);
  Serial.print(" | STATUS: "); Serial.println(statusLayar);
  
  delay(100); // Diperkecil menjadi 100ms agar efek kedap-kedip LCD sangat halus dan akurat
}
