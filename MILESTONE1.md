# Welcome, Player - Milestone 1 Sistem Operasi

## Kelompok OSkadon / K01
* Ronggur Mahendra Widya Putra / 13519008
* Muhammad Azhar Faturahman / 13519020
* Rayhan Alghifari Fauzta / 13519039

## Cara Kerja Interrupt
Interrupt berfungsi untuk menginterrupt proses pada BIOS guna melakukan operasi-operasi seperti menampilkan data di monitor dan membaca data dari keyboard. Interrupt digunakan dengan menentukan tipe interrupt yaitu value yang disimpan beserta informasi-informasi tambahan di register AH atau AL yang diinginkan. Interrupt yang kami gunakan saat ini adalah Interrupt 10H dan Interrupt 16H. Interrupt 10H berkaitan dengan video services untuk menampilkan tulisan ke layar seperti pada printString dan readString. Interrupt 16H berkaitan dengan keyboard services guna membaca input dari keyboard pada readString.

## Cara Kerja kernel.asm
kernel.asm berisi fungsi-fungsi assembly dasar yang digunakan untuk menjalankan proses-proses di kernel yang akan dibuat. 
