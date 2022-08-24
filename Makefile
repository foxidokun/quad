PROJ = quad
BINDIR = bin
ODIR = obj
HEADERS = equation_solver.h

_DEPS = equation_solver.h
DEPS = $(patsubst %,.,$(_DEPS))

_OBJ = equation_solver.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CFLAGS = -D _DEBUG -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

SAFETY_COMMAND = set -Eeuf -o pipefail && set -x

$(BINDIR)/$(PROJ): $(ODIR) $(BINDIR) $(OBJ)
	g++ -o $(BINDIR)/$(PROJ) $(OBJ) $(CFLAGS)

run: $(BINDIR)/$(PROJ)
	$(BINDIR)/$(PROJ) -i

clean:
	$(SAFETY_COMMAND) && rm -rf $(ODIR) $(BINDIR)

test: #TODO генерелизовать с обычными запусками
	g++ -o $(BINDIR)/$(PROJ)_test main.cpp equation_solver.cpp test_equation_solver.cpp $(CFLAGS) -D TEST && $(BINDIR)/$(PROJ)_test > /dev/null

.PHONY: clean

$(ODIR):
	mkdir $(ODIR)

$(BINDIR):
	mkdir $(BINDIR)

$(ODIR)/%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(CFLAGS)
