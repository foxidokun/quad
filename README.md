## Equation solver

Solves quadric equation in real numbers

### How to build
```bash
git clone --depth=1 https://git.forestfox.tk/mipt_tasks/quadratic_equation.git
cd quad
make
```
This program has 3 modes:
1. *Interactive mode*

```bash
$ ./bin/quad -i # aka "make run"
Enter equation (ax^n + ... + bx^2 + cx + d = 0) coefficients
Coefficient at x^2: 1
Coefficient at x^1: 0
Coefficient at x^0: -4
2 solutions: -2.000e+00 и 2.000e+00
```

2. *Normal mode*
```bash
$ ./bin/quad 1 0 -4
2 solutions: -2.000e+00 и 2.000e+00
```

3. *Help*
```
$ ./bin/quad -h
Quadratic equation solver
Usage:
    * `quad -i` for interactive mode
    * `quad a b c` for normal mode (solve ax^2 + bx + c = 0)
```

### How to generate documentration
```bash
cd quad
doxygen
```
Then open docs/html/index.html in your browser

### How to run test
```
cd quad
make test
```

`make test` will run tests and generate ./bin/quad_test binary, which can also be used for testing
If you want to write report to file instead of stdout, use `-r <filename>` option