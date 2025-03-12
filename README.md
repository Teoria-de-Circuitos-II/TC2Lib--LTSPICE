# TCLib -- LTSpice

## Utilización

1. Clonar (o descargar y luego unzippear completamente) el repositorio.
2. Cerrar todas las instancias abiertas de LTSpice.
3. Ejecutar el patcher `TCLib.exe` como administrador (`TCLib` en linux) y usar el menu para aplicar los parches deseados.

## Integridad de archivos


### Windows

Compilado mediante el comando `g++ -static TCLib.cpp setup-funcs.cpp -o ../TCLib.exe` desde el directorio `source/`, con `g++.exe (Rev3, Built by MSYS2 project) 13.2.0`. Hash MD5 de `TCLib.exe`: `aa0e0b046b41c316783a8c8db28f49c5`.

### Linux

Compilado mediante el comando `g++ -static TCLib.cpp setup-funcs.cpp -o ../TCLib` desde el directorio `source/`, con `g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`. Hash MD5 de `TCLib`: `a8ca475a17e13b2ba786719776e26aad`
