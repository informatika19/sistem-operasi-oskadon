# OS - WeTheClown
Sebuah sistem operasi yang berjalan pada sistem 16-bit yang nanti akan disimulasikan dengan emulator bochs.

## Author
1. Gde Anantha Priharsena   - 13519026
2. Reihan Andhika Putra     - 13519043
3. Kinantan Arya Bagaspati  - 13519044

## How To Run
1. Clone this repository
2. Install Dependencies
```
sudo apt update
sudo apt install nasm bcc bin86 bochs bochs-x hexedit
```
2. Create map image.
```
dd if=/dev/zero of=map.img bs=512 count=1
```
3. Edit map image, so that you can load your kernel. The kernel size is 18.
```
hexedit map.img
00000000   FF FF FF FF  FF FF FF FF  FF FF FF FF  FF FF FF FF  ................
00000010   FF FF 00 00  00 00 00 00  00 00 00 00  00 00 00 00  ................
00000020   00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  ................
00000030   00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  ................
........   .. .. .. ..  .. .. .. ..  .. .. .. ..  .. .. .. ..  ................
```
press "ctrl+x" then "y' to save your changes.

4. For linux (Ubuntu) users, you can automaticallly run the the os by running the "run.sh" script. For WSL users, you can run the os by running the "run2.sh" script.

```
// For linux users
./run.sh

// For WSL users
./run2.sh
```

5. If you want to shut down the operating system itself, click the **power button** on the top right corner of the bochs.

## List of Command
1. Make Directory
```
shell@wetheclown> mkdir <directory>
```
2. Change Directory
```
shell@wetheclown> cd <directory>
shell@wetheclown> cd <directory path>
shell@wetheclown> cd ..
```
3. Directory listing
```
shell@wetheclown> ls
shell@wetheclown> ls <filename>
shell@wetheclown> ls <directory>
shell@wetheclown> ls <directory path>
```
4. Show file content (Concatenate)
```
shell@wetheclown> cat <filename>
```
5. Symbolic link (hard link)
```
shell@wetheclown> ln <filename1> <filename2>
```
6. Autocomplete file and folder for cd, ls, and cat use ','
7. History Shell
    - '{' or "shift + [" for showing earlier command that was given to the shell
    - '}' or "shift + ]" for showing later command that was given to the shell
7. Cursor movement
    - '<'  or "shift + ," for moving cursor to the left of the current cursor position
    - '>'  or "shift + ." for moving cursor to the right of the current cursor position

