# Cril
re-implemented the bril interpreter for a class.

## Deps
Just clang

## Building
`make` will suffice. If you want to check for memory errors, use `make debug`. 

## Running
There are two modes. 

1. `./cril <filename>` will run the bril code in `<filename>`, assumed to be
   a json file
2. `./cril --benchmark` will run the programs under the `test` directory and
    collect stats on average times (with standard deviations). The number of
    runs is controlled by the `NUM_RUNS` definition in `src/main.c`
