## Equation solver

Solves quadric equation in real numbers

### How to build
```bash
git clone --depth=1 https://git.forestfox.tk/mipt_tasks/quadratic_equation.git
cd quad
make
```

Then run
```bash
./bin/quad -i #for interactive mode
./bin/quad 1 0 -4 # solve 1*x^2 + 0*x - 4 =0
./bin/quad #for help
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