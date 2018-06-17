GCC=g++
CPPFLAGS=-std=c++11 -Wall -I.
DEPS = defines.hpp tree_vert_defs.hpp tree_decomp_defs.hpp
OBJ = compute_td.o fot.o change_degree.o t_d_from_tree.o make_tree.o simple_func.o 1_out.o tree_decomp_impl.o tree_vert_impl.o

%.o: %.c $(DEPS)
	$(GCC) -c -o $@ $<  $(CPPFLAGS)

wz: $(OBJ)
	$(GCC) -o $@ $^ $(CPPFLAGS)

clean:
	rm -f *.o tdc
