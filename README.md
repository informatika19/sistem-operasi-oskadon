# TugasBesarSistemOperasi
Sistem operasi 16-bit yang dijalankan dalam emulator bosch

## Kelompok OSkadon / K01
* Ronggur Mahendra Widya Putra / 13519008
* Muhammad Azhar Faturahman / 13519020
* Rayhan Alghifari Fauzta / 13519039

## Requirement
- Linux (Ubuntu)
- nasm, bcc, bin86, bochs, bochs-x, hexedit


## How to Run
#### 1. Install Dependencies
```
sudo apt update
sudo apt install nasm bcc bin86 bochs bochs-x hexedit
```
#### 2. Inisialisasi os dan filesystem
```
bash ./init.sh
```
#### 3. Edit filesystem `map.img`, sesuaikan dengan ukuran kernel yaitu 16 sektor
```
hexedit map.img
00000000   FF FF FF FF  FF FF FF FF  FF FF FF FF  FF FF FF FF  ................
00000010   00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  ................
00000020   00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  ................
00000030   00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  ................
........   .. .. .. ..  .. .. .. ..  .. .. .. ..  .. .. .. ..  ................
```
tekan `ctrl+x` lalu `y` untuk menyimpan perubahan
#### 4. Compile library
```
cd lib
bash ./compile_lib.sh
cd ..
```
#### 5. Jalankan sistem operasi
```
bash ./run.sh
```
#### 6. (Optional) Jika ingin mengkompilasi sistem operasi tanpa menjalankannya
```
bash ./script.sh
```

## Shell Command
#### 1. Current Directory Listing
```
ls
```
Ex : `/root $ ls`
#### 2. Change Current Directory
```
cd <directory>
cd <directory path>
```
Ex : `/root $ cd ./bin/..`
#### 3. Move File or Directory
```
mv <file/directory name> <target directory>
mv <file/directory path> <target directory path>
```
Ex : `/root $ mv tes.txt titan/doc`
#### 4. Copy File or Directory
```
cp <file/directory name> <target directory>
cp <file/directory path> <target directory path>
```
Ex : `/root $ cp tes.txt titan`
#### 5. Create New Directory
```
mkdir <directory>
mkdir <directory path>
```
Ex : `/root $ mkdir doc`
#### 6. Remove File or Directory
```
rm <file/directory name>
rm <file/directory path>
```
Ex : `/root $ rm doc`
#### 7. Read Text File
```
cat <file name>
cat <file path>
```
Ex : `/root $ cat tes.txt`
#### 8. Symbolic Link
```
ln <file name> <target file name>
ln <file path> <target file path>
```
Ex : `/root $ ln titan/titan.txt titan2.txt`
