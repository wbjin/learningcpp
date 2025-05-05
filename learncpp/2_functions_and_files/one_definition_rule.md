1. Within a file, a variable, type, or template can only have on definition in a given scope. 
2. Within a program, each funciton or variable can onle have one definition in a given scope. 
3. Types, templates, inline functions, and inline variables are allowed to have duplicate definitions as long as each definition is identical.

Violating 1 and 2 will cause redefinition erors in the compiler and linker respectively. Violating 3 causes undefined behavior.
