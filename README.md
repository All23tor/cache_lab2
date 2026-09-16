# Lab 2

## Compilar

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
````

## Ejecutar

```bash
./build/loops
./build/matrix_classic
./build/matrix_block
```

## Ejecutar con Cachegrind

```bash
valgrind --tool=cachegrind --cache-sim=yes ./build/loops
valgrind --tool=cachegrind --cache-sim=yes ./build/matrix_classic
valgrind --tool=cachegrind --cache-sim=yes ./build/matrix_block
```
