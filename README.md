Программа запускается с флагами:
```
./huffman [flags]
```
где флаги:
* `-c`: сжатие,
* `-u`: разжатие,
* `-f <path>`, `--file <path>`: имя входного файла,
* `-o <path>`, `--output <путь>`: имя результирующего файла.
Флаги могут указываться в любом порядке.

Программа выводит на экран статистику сжатия/распаковки: размер исходных данных, размер
полученных данных и размер, который был использован для хранения вспомогательных данных в выходном
файле.
Все размеры в байтах.

Например:
```
$ ./huffman -c -f myfile.txt -o result.bin
15678
6172
482
```

Размер исходного файла (исходные данные): 15678 байт, размер сжатых данных (без дополнительной
информации): 6172 байта, размер дополнительных данных: 482 байта. Размер всего сжатого файла: 6172 +
482 = 6654 байта.

```
$ ./huffman -u -f result.bin -o myfile_new.txt
6172
15678
482
```

Размер распакованного файла (полученные данные): 15678 байт, размер сжатых данных (без 
дополнительной информации): 6172 байта, размер дополнительных данных: 482 байта. Размер всего
исходного сжатого файла: 6172 + 482 = 6654 байта.