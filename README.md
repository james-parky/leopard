# Leopard

## A MIPS32 Transpiler

> <neg_value> ::= "-"<dec_num> | "-"<dec_num>":"<dec_num> | "-"<dec_num>":"<hex_num>
> <dec_value> ::= <dec_num> | <dec_num>":"<dec_num> | <dec_num>":"<hex_num>
> <hex_value> ::= <hex_num> | <hex_num>":"<dec_num> | <hex_num>":"<hex_num>
> <data_value> ::= <neg_value> | <dec_value> | <hex_value>
> <data_list> ::= <data_value> | <data_value>","<data_list>
> <data_type> ::= "ascii" | "asciiz" | "word" | "half" | "byte" | "space"
> <var_dec> ::= "."<data_type><data_list>
> <var_decs> ::= <var_dec> | <var_dec><var_decs>
> <data_section> ::= ".data"<var_decs>
> <num_reg> ::= "0" | "1" | ... | "32"
> <name_reg> ::= "zero" | "at" | "v0" | "v1" | "a0" | "a1" | "a2" | "a3"
> | "t0" | "t1" | "t2" | "t3" | "t4" | "t5" | "t6" | "t7"
> | "s0" | "s1" | "s2" | "s3" | "s4" | "s5" | "s6" | "s7"
> | "t8" | "t9" | "k0" | "k1" | "gp" | "sp" | "fp" | "ra"
> <register> ::= "$"<num_reg> | "$"<name_reg>
> <offset_register> ::= "("<register>")"
> | <dec_num>"("<register>")"
> | <hex_num>"("<register>")"
> | <string>"+""("<register>")"
> | <string>"("<register>")"
> <operand> ::= <register> | <offset_register> | <data_value>
> <operand_list> ::= <operand> | <operand>","<operand_list>
> <instruction> ::= <opcode> | <opcode><operand_list>
> <label> ::= <string>":"
> <text_statement> ::= <label> | <instruction>
> <text_body> ::= <text_statement> | <text_statement><text_body>
> <text_section> ::= ".text"".globl"<string><text_body>
> | ".text"<text_body>
> <program> ::= <text_section>
> | <data_section><text_section>
> | <text_section><data_section>
