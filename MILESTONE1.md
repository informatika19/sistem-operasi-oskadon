# Milestone 1
## WeTheClown
1. Gde Anantha Priharsena   - 13519026
2. Reihan Andhika Putra     - 13519043
3. Kinantan Arya Bagaspati  - 13519044

## How To Run
1. Clone this repository
2. Install Dependencies
```
sudo apt update
sudo apt install nasm bcc bin86 bochs bochs-x
```
2. Run run.sh
```
./run.sh
```

## Hal yang sudah dikerjakan
1. printString
2. readString
   - Mengimplementasikan pembacaan karakter
   - Mengimplementasikan pembacaan *backspace* untuk menghapus karakter yang sudah diinput sebelumnya
   - Mengimplementasikan pembacaan *arrow key* untuk menggeser kursor, sehingga memungkinkan adanya penghapusan atau penambahan karakter pada awal, tengah, dan akhir string. Untuk geser ke kiri gunakan "shift + ," dan untuk geser ke kanan gunakan "shift + .". Penggunaan karakter "<" dan ">" dilakukan karena arrow key adalah suatu *special function key* sehingga tidak memiliki ascii code. 
3. clear
4. bootlogo
   - Membuat bootlogo dengan ascii art menggunakan fungsi putInMemory. 	
5. Fungsi aritmetika
   - Mod
   - Div 

## Penjelasan Cara Kerja Interrupt
Interrupt secara garis besar memiliki beberapa parameter bertipe integer yaitu number, AX, BX, CX, dan DX. Setiap parameter disesuaikan dengan fungsi interrupt yang akan dipanggil. Pada arsitektur x86, pemanggilan interrupt dilakukan dengan fungsi berikut.
```
interrupt(int number, int AX, int BX, int CX, int DX);
```

Pada Milestone 1 ini kami menggunakan 2 jenis interrupt, yaitu interrupt 10h dan interrupt 16h. Berikut penjelasan mengenai kedua interrupt tersebut:
1. Interrupt 10h merupakan  fungsi interrupt dengan parameter pertama 0x10. Fungsi ini dipanggil pada fungsi readString dan fungsi printString. Interrupt 10h hanya menggunakan parameter kedua (AX) yang dapat dibagi menjadi dua **AH** dan **AL**. Pada kasus ini, interrupt 10h digunakan untuk menulis karakter pada *text mode* untuk ditampilkan ke layar. Sehingga, **AH** akan diisi dengan **0xE** dan **AL** akan diisi dengan **karakter (dalam ascii)** yang akan ditampilkan ke layar. Pada fungsi readString dan printString kami mengimplemntasikan pemecahan AX menjadi AH dan AL dengan melakukan operasi bitwise, yaitu 0xE*256+c (selain ini dapat juga dituliskan dengan 0xE<<8+c) dengan c adalah kode ascii karakter yang akan ditampilkan ke layar. Untuk penjelasan detail dapat dilihat [disini](https://stanislavs.org/helppc/int_10-e.html).
2. Interrupt 16h merupakan fungsi interrupt dengan parameter pertama 0x16. Fungsi ini dipanggil pada fungsi readString. Interrupt 16h hanya menggunakan parameter kedua (AX) yang dapat dibagi menjadi dua **AH** dan **AL**. Pada kasus ini, interrupt 16h digunakan untuk membaca masukkan dari keyboard dan mengembalikkan nilai ascii dari karakter yang dibaca atau 0 jika yang dibaca adalah *special function key* pada **AL**. Sehingga, AH akan diisi dengan 0x0 terlebih dahulu untuk menjalankan fungsi membaca masukkan dari keyboard atau secara khusus disebut dalam dokumentasi interrupt adalah "*Wait for Keypress and Read Character*". Jika pengguna memberikan masukkan dari keyboard, fungsi akan mengembalikan nilai AX yang berisi **keyboard scan code pada AH** dan **kode ascii karakter pada AL**. Untuk penjelasan detail dapat dilihat [disini](http://staff.ustc.edu.cn/~xyfeng/research/cos/resources/BIOS/Resources/assembly/int16-0.html). Selanjutnya, kode ascii ini akan ditampilkan ke layar dengan fungsi interrupt 10h yang dijelaskan pada poin 1.

Selain dari kedua fungsi ini, masih banyak fungsi-fungsi interrupt lainnya yang bisa digunakan untuk pengembangan OS ini dikemudian hari. Akan tetapi, untuk milestone ini kami hanya menggunakan dua jenis interrupt.

## Penjelasan Kernel.asm
Kernel.asm berisikan fungsi-fungsi dasar interrupt yang dapat digunakan pada kernel.c yang ditulis dengan bahasa assembly. Berikut adalah penjelasan dari kernel.asm :

```
global _putInMemory
....
extern _handleInterrupt21
```
Pada baris tujuh hingga sepuluh terdapat** deklarasi fungsi-fungsi** yang akan ditulis. Fungsi yang dideklarasikan dengan *keyword* **global** akan bisa dikenali oleh kode lainnya pada **proses *linking object code*** sehingga kita bisa **menggunakan fungsi tersebut pada program lain** seperti kernel.c ataupun kode assembly tambahan lainnya.
Fungsi yang dideklarasikan dengan *keyword* **extern** bermakna bahwa code yang diketik sekarang akan menggunakan fungsi tersebut namun isi(*body*) dari **fungsi tersebut didefinisikan pada file lain**. Contohnya, pada kasus handleInterrupt21 didefinikan di kernel.c

```
;void putInMemory (int segment, int address, char character)
_putInMemory:
  ....
	mov ax,[bp+4]
	mov si,[bp+6]
	mov cl,[bp+8]
	mov ds,ax
	mov [si],cl
  ....
```
Pada baris 14 hingga 24 terdapat isi dari **fungsi putInMemory**. Fungsi ini bertujuan untuk menulis paksa isi dari memory di suatu segmen dengan offset alamat tertentu. Saat fungsi ini dijalankan, ax akan berisi parameter segmen, si berisi offset alamat, dan cl berisi karacter yang akan menimpa. Selain itu, ds(*data segment*) akan diisi parameter segmen.
Isi dari offset alamat ([si]) akan ditimpa dengan parameter char(cl). Kerja dari fungsi ini tergantung segmen apa yang menjadi argumen. Apabila parameter segmen diisi dengan segmen video memori(0xB000) maka fungsi ini akan menulis paksa tampilan pada layar kernel yang kita buat. Penjelasan lebih lanjutnya ada di dokumen Milestone 1.

```
;int interrupt (int number, int AX, int BX, int CX, int DX)
_interrupt:
  ....
	push bp
	mov bp,sp
	mov ax,[bp+4]	;get the interrupt number in AL
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,intr
	mov [si+1],al	;change the 00 below to the contents of AL
	pop ds
	mov ax,[bp+6]	;get the other parameters AX, BX, CX, and DX
	mov bx,[bp+8]
	mov cx,[bp+10]
	mov dx,[bp+12]
  ....
```
Pada baris 28 hingga 40 terdapat isi dari **fungsi interrupt**, diikuti fungsi intr pada baris 42 hingga 46. Fungsi intr bertujuan meng-nol-kan nilai ah, sehingga hanya tersisa 2 byte terakhir dari ax yakni al. Pertama fungsi interrupt menyimpan parameter number pada register ax. Kemudian si diisi dengan hasil fungsi intr, yakni al. Kemudian diikuti dengan penyimpanan semua parameter fungsi yakni AX, BX, CX, dan DX pada register yang sesuai.

```
_makeInterrupt21:
	;get the address of the service routine
	mov dx,_interrupt21ServiceRoutine
	push ds
	mov ax, 0	;interrupts are in lowest memory
	mov ds,ax
	mov si,0x84	;interrupt 0x21 vector (21 * 4 = 84)
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	ret
 ```
Pada baris 54 hingga 64 terdapat isi dari **fungsi makeInterrupt21**. Fungsi ini menyimpan alamat Interrupt21ServiceRoutine dalam register dx. Register ax pun diisi 0 karena fungsi menginginkan interrupt berlangsung di memori terendah. Register si diisi dengan 0x84 dalam rangka menyiapkan 4 byte memori untuk interrupt 0x21. Kemudian alamat dari Interrupt21ServiceRoutine  dan nilai dari segmen saat ini berturut-turut disimpan dalam si dengan offset 2 byte.

```
_interrupt21ServiceRoutine:
	....
	call _handleInterrupt21
	....
```
Selanjutnya interrupt21ServiceRoutine pada baris 70 sampai 80 bertujuan **memanggil handleInterrupt21** dengan parameter yang diambil dari register ax hingga dx.
