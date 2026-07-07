# Podoae 🍅

Podoae adalah aplikasi Pomodoro Timer minimalis yang dibangun menggunakan bahasa **C**, pustaka **MicroUI** untuk antarmuka pengguna grafis, serta **SDL2** dan **OpenGL** untuk perenderan.

Proyek ini dirancang sebagai sarana belajar pemrograman C, penataan antarmuka pengguna berbasis *Immediate Mode GUI*, dan pemisahan logika aplikasi dari presentasi UI.

---

## 🚀 Fitur Utama

- **Unified Dashboard**: Semua elemen UI (Timer, Log, Slider Pengaturan, dan Editor Style) disatukan ke dalam satu dasbor grid dua kolom yang intuitif.
- **Siklus Pomodoro Otomatis**: Mendukung mode **Kerja (Work)**, **Istirahat Pendek (Short Break)**, dan **Istirahat Panjang (Long Break)** (setiap kelipatan 4 sesi kerja selesai).
- **Custom Durations**: Durasi masing-masing sesi dapat disesuaikan langsung lewat slider.
- **Progress Bar Dinamis**: Visualisasi durasi waktu tersisa dengan bar berwarna adaptif.
- **Alarm Popup**: Kotak dialog alarm interaktif yang muncul ketika durasi waktu habis.
- **Riwayat Aktivitas (Log)**: Pencatatan log aktivitas yang berjalan langsung di antarmuka.
- **Editor Gaya UI**: Kustomisasi warna tema jendela, tombol, teks, dan latar belakang secara *real-time*.
- **Hemat Sumber Daya**: Frame rate dibatasi menggunakan delay SDL agar tidak membebani penggunaan CPU.

---

## 🛠️ Arsitektur Kode

Kode program telah dipisahkan secara modular untuk mempermudah pemeliharaan:

- [main.c](file:///home/yuma/Project/c_project/podoae/main.c): Mengatur inisialisasi SDL2/OpenGL, penanganan input event, serta penggambaran antarmuka grafis (UI rendering).
- [podoae.h](file:///home/yuma/Project/c_project/podoae/podoae.h) / [podoae.c](file:///home/yuma/Project/c_project/podoae/podoae.c): Berisi seluruh logika status aplikasi, penghitungan waktu (tick), transisi mode otomatis, dan validasi pengaturan timer.
- **Include/**: Folder pustaka eksternal seperti `microui` dan berkas pendukung `renderer` (SDL2 + OpenGL renderer).

---

## ⚙️ Persyaratan Sistem & Dependensi

Untuk mengompilasi proyek ini, pastikan sistem Anda telah terpasang:
- Kompilator C (misalnya `gcc`)
- Pustaka **SDL2**
- Pustaka **OpenGL (Mesa)**

Pada distro Linux berbasis Debian/Ubuntu, dependensi dapat dipasang dengan perintah:
```bash
sudo apt install build-essential libsdl2-dev libgl1-mesa-dev
```

---

## 🏗️ Kompilasi dan Menjalankan

Gunakan berkas bash script yang disediakan untuk melakukan kompilasi otomatis:

1. **Beri izin eksekusi berkas build (jika belum):**
   ```bash
   chmod +x build.sh
   ```

2. **Kompilasi proyek:**
   ```bash
   ./build.sh
   ```

3. **Jalankan aplikasi:**
   ```bash
   ./podoae
   ```

---

## 🔁 CI/CD dengan GitHub Actions

Proyek ini dilengkapi dengan pipeline CI/CD menggunakan **GitHub Actions** yang dikonfigurasi pada berkas `.github/workflows/build.yml`. Setiap kali Anda melakukan push atau pull request ke branch utama (`master` atau `main`), alur kerja otomatis akan berjalan untuk membangun aplikasi:
- **podoae-linux-x64**: Build Linux native executable 64-bit (`podoae-linux`).
- **podoae-windows-x64**: Build Windows 64-bit executable (`podoae.exe`). Build ini dirilis langsung bersama berkas `SDL2.dll` yang dibutuhkan tanpa dikemas dalam format ZIP.

