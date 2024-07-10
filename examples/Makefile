lib_src = ../src/neat_str.c ../src/neat_core.c
build_cmnd = cc $(lib_src) -Wall -Wextra

all: $(wildcard *.c)
	$(build_cmnd) sstring.c -o sstring
	$(build_cmnd) string_buffer.c -o string_buffer
	$(build_cmnd) mutstr_ref.c -o anystr_ref
	$(build_cmnd) split_join.c -o split_join