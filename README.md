# pet2bmp
Convert Commodore PETSCII character ROM images into bitmap or text file(-s) and
vice versa(!).

Extract Commodore PETSCII characters from character ROM image files and make
them visible as bitmaps or easily modifiable as text files.

You can change the extracted data, turn it back into a ROM file and this way use
your own font/modifications with your Commodore PET (e.g. via custom EEPROM)!

**Commandline options:**

- Character ROM to bitmap file conversion:
  `pet2bmp b <input ROM file path> <output bitmap file path>`

- Bitmap to character ROM file conversion:
  `pet2bmp c <input bitmap file path> <output ROM file path>`

- Character ROM to text file conversion:
  `pet2bmp t <input ROM file path> <output text file path>`

- Text to character ROM file conversion:
  `pet2bmp u <input text file path> <output ROM file path>`

- (Any kind of) file to text file with hexadecimal byte values (like a C array):
  `pet2bmp h <input file path> <output hexadecimal text file path>`

**Example:**

Input ROM file:

http://www.zimmers.net/anonftp/pub/cbm/firmware/computers/pet/characters-japan.901447-12.bin

Output bitmap file:

![Japanese ROM](./characters-japan_901447-12_bin.bmp)