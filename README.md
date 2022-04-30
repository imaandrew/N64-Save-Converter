# N64-Save-Converter
Converts N64 save files to N64 emulator save files

## Usage
To convert a single file:
```
./convertsave -i <save> -o <output directory>
```

To convert all files in a directory recursively:
```
./convertsave -d -i <save directory> -o <output directory>
```

## License
To create BizHawk save files, this repository includes code from [mupen64plus-core](https://github.com/mupen64plus/mupen64plus-core/blob/master/doc/gpl-license), which is licensed under the GNU General Public License v2.0
