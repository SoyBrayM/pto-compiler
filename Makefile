exec=bin
code_dirs=. incl
incl_dirs=incl

cc=clang
opt_flag=-O3
dep_flags=-MP -MD
cc_args=-Wall -Wextra -Werror -pedantic -ansi -g $(foreach D,$(incl_dirs),-I$(D)) $(opt_flag) $(dep_flags)

c_files=$(foreach D,$(code_dirs),$(wildcard $(D)/*.c))
objects=$(patsubst %.c,%.o,$(c_files))
d_files=$(patsubst %.c,%.d,$(c_files))

all: $(exec)
	@echo "Finished linking objects! Output: " $^

run:
	@clear
	@make clean
	@make all
	@echo "Executing output" $(exec)
	@./$(exec) example.pto

$(exec): $(objects)
	@echo "Compiled code into object files!"
	@$(cc) -o $@ $^

%.o:%.c
	@$(cc) $(cc_args) -c -o $@ $<

clean:
	@rm -rf $(exec) $(objects) $(d_files) generated_assembly.asm

-include $(d_files)

.PHONY: all clean 
