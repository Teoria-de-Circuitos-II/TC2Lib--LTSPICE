# TCLib -- LTSpice

## Utilización

1. Clonar (o descargar y luego unzippear completamente) el repositorio.
2. Cerrar todas las instancias abiertas de LTSpice.
3. Ejecutar el patcher `TCLib.exe` como administrador (`TCLib` en linux) y usar el menu para aplicar los parches deseados.

## Integridad de archivos


### Windows

Compilado mediante el comando `g++ -static TCLib.cpp setup-funcs.cpp -o ../TCLib.exe` desde el directorio `source/`, con `g++.exe (Rev3, Built by MSYS2 project) 13.2.0`. Hash MD5 de `TCLib.exe`: `467e95f025ef15bbf748f0ba97601ad7`.

### Linux

Compilado mediante el comando `g++ -static TCLib.cpp setup-funcs.cpp -o ../TCLib` desde el directorio `source/`, con `g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`. Hash MD5 de `TCLib`: `445c1e2fe1472d246c0c726b20cec0a9`
