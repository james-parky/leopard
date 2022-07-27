# Leopard - ## A MIPS32 Transpiler

&lt;neg_value> ::= "-"&lt;dec_num> | "-"&lt;dec_num>":"&lt;dec_num> | "-"&lt;dec_num>":"&lt;hex_num>

&lt;dec_value> ::= &lt;dec_num> | &lt;dec_num>":"&lt;dec_num> | &lt;dec_num>":"&lt;hex_num>

&lt;hex_value> ::= &lt;hex_num> | &lt;hex_num>":"&lt;dec_num> | &lt;hex_num>":"&lt;hex_num>

&lt;data_value> ::= &lt;neg_value> | &lt;dec_value> | &lt;hex_value>

&lt;data_list> ::= &lt;data_value> | &lt;data_value>","&lt;data_list>

&lt;data_type> ::= "ascii" | "asciiz" | "word" | "half" | "byte" | "space"

&lt;var_dec> ::= "."&lt;data_type>&lt;data_list>

&lt;var_decs> ::= &lt;var_dec> | &lt;var_dec>&lt;var_decs>

&lt;data_section> ::= ".data"&lt;var_decs>

&lt;num_reg> ::= "0" | "1" | ... | "32"

&lt;name_reg> ::= "zero" | "at" | "v0" | "v1" | "a0" | "a1" | "a2" | "a3"  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;| "t0" | "t1" | "t2" | "t3" | "t4" | "t5" | "t6" | "t7"  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;| "s0" | "s1" | "s2" | "s3" | "s4" | "s5" | "s6" | "s7"  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;| "t8" | "t9" | "k0" | "k1" | "gp" | "sp" | "fp" | "ra"

&lt;register> ::= "$"&lt;num_reg> | "$"&lt;name_reg>

&lt;offset_register> ::= "("&lt;register>")"  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;| &lt;dec_num>"("&lt;register>")"  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;| &lt;hex_num>"("&lt;register>")"  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;| &lt;string>"+""("&lt;register>")"  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;| &lt;string>"("&lt;register>")"

&lt;operand> ::= &lt;register> | &lt;offset_register> | &lt;data_value>

&lt;operand_list> ::= &lt;operand> | &lt;operand>","&lt;operand_list>

&lt;instruction> ::= &lt;opcode> | &lt;opcode>&lt;operand_list>

&lt;label> ::= &lt;string>":"

&lt;text_statement> ::= &lt;label> | &lt;instruction>

&lt;text_body> ::= &lt;text_statement> | &lt;text_statement>&lt;text_body>

&lt;text_section> ::= ".text"".globl"&lt;string>&lt;text_body> | ".text"&lt;text_body>

&lt;program> ::= &lt;text_section>
| &lt;data_section>&lt;text_section>
| &lt;text_section>&lt;data_section>
