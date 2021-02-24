# TugasBesarSistemOperasi

## Kelompok OSkadon / K01
* Ronggur Mahendra Widya Putra / 13519008
* Muhammad Azhar Faturahman / 13519020
* Rayhan Alghifari Fauzta / 13519039

## Requirement
- nasm, bcc, bin86, bochs, bochs-x
- Pada sistem operasi Ubuntu dapat langsung didapatkan dengan menjalankan perintah pada terminal sebagai berikut
```
sudo apt update
sudo apt install nasm bcc bin86 bochs bochs-x
```

## How to Use
- Siapkan disk image, compile dan masukkan bootloader ke dalam disk image tersebut. Dapat langsung menggunakan perintah `bash ./initialization.sh` pada terminal linux
- Compile kernel lalu masukkan ke dalam disk image. Dapat langsung dijalankan dengan perintah `bash ./script.sh` pada terminal linux.
- Untuk menjalankan emulator bochs bisa dijalankan dengan perintah `bochs -f if2230.config` pada terminal linux dan pencet c.

*untuk cara kerja bisa dibaca pada MILESTONE1.md
