# Welcome, Player - Milestone 1 Sistem Operasi

## Kelompok OSkadon / K01
* Ronggur Mahendra Widya Putra / 13519008
* Muhammad Azhar Faturahman / 13519020
* Rayhan Alghifari Fauzta / 13519039

## Cara Kerja Interrupt
Interrupt berfungsi untuk menginterrupt proses pada BIOS guna melakukan operasi-operasi seperti menampilkan data di monitor dan membaca data dari keyboard. Interrupt digunakan dengan menentukan tipe interrupt yaitu value yang disimpan beserta informasi-informasi tambahan di register AH, AL, DL, atau address lain yang diinginkan. Interrupt yang kami gunakan saat ini adalah Interrupt 10H dan Interrupt 16H. Interrupt 10H berkaitan dengan video services untuk menampilkan tulisan ke layar seperti pada printString dan readString. Interrupt 16H berkaitan dengan keyboard services guna membaca input dari keyboard pada readString.

## Cara Kerja kernel.asm
kernel.asm berisi fungsi-fungsi assembly dasar yang digunakan untuk menjalankan proses-proses di kernel yang akan dibuat. Terdapat empat fungsi utama dalam kernel.asm yaitu:
* putInMemory
* interrupt
* makeInterrupt21
* handleInterrupt21

Prosedur putInMemory memiliki tigs parameter. Pertama adalah segment (int) yaitu digit hexadecimal dikalikan 0x100 sebagai address bagian pertama. Parameter kedua adalah empat digit hexadecimal dari address yang ingin ditulis. Parameter ketiga yaitu character (char) adalah karakter yang ingin ditulis ke layar.

Fungsi interrupt berguna untuk melakukan interrupt dengan memanggil nomor interrupt yang diinginkan. Interrupt juga mengambil parameter lainnya yaitu AX, BX, CX, dan DX untuk menspesifikkan value interrupt selain dari register AL. 

Selanjutnya ada prosedur makeInterrupt21. Prosedur ini digunakan untuk membuat vector interrupt 0x21 (33 dalam desimal) yang akan mengambil vector ke-34 dalam interrupt table sebagai MS-DOS API call. makeInterrupt21 akan mengambil address dari interrupt21ServiceRoutine guna memanggil fungsi eksternal handleInterrupt21 dari kernel.c. handleInterrup21 inilah yang akan menjalankan instruksi printString (mencetak string ke layar) dan readString (membaca masukan string dari user). Instruksi ini bergantung pada input parameter AX di handleInterrupt21, jika AX = 0x0 maka printString dan jika AX = 0x1 maka readString.

