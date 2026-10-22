# mops-os kernel

## Documentation

### General
The header file **common.h** works as a centralized configuration file and contains type aliases, error codes and macro definitions.

**Table 1 - Comonly used type aliases**
| Type     | Alias |
|:--------:|:-----:|
| int8_t   | i8    |
| int16_t  | i16   |
| int32_t  | i32   |
| int64_t  | i64   |
| uint8_t  | u8    |
| uint16_t | u16   |
| uint32_t | u32   |
| uint64_t | u64   |
| size_t   | szt   |

### Code Style Converntions

#### Naming
- global/local variables: *snake_case*
```c
int foo_bar;
```

- constants and macros: *SCREAMING_SNAKE_CASE*
```c
#define FOO_BAR 32
const int BAZ = 67;
```

- struct/union/enum declarations: *snake_case*; prefixed by an underscore if part of a **typedef**
```c
struct foo_bar;

typedef enum _foo_bar_baz
{
    ...
} foo_bar_baz_t;
```

- functions: *snake_case*; prefixed by an underscore if static
```c
void foo_bar(void);

static int _bar_baz(...) {
    ...
}
```

- custom types: *snake_case* followed by "_t", with some exceptions (see Table 1)
```c
typedef unsigned int foo_bar_t;
```

- opaque types: *SCREAMING_SNAKE_CASE*
```c
typedef struct foo_bar FOO_BAR; 
```


#### Bracket Style
- functions: opening curly bracket placed on the same line as the signature
```c
void foo(void) {
    ...
}
```

- structs/unions/enums: opening curly bracket placed on the very next line
```c
union foo
{
    ...
};
```

- conditional statements and loops: opening curly bracket placed on the same line as the condition
```c
if (foo) {
    ...
}

while (bar) {
    ...
}
```

#### Application Binary Interface
- The **ASMCALL** macro is an alias for the **((cdecl))** gcc compiler attribute and is used in the declaration of functions which are either defined in assembly files or are to be called from assembly. It should be placed after the return type. 
```c
void ASMCALL foo(void) {
    ...
}
```
```c
// NASM
extern foo
call foo
```

- All functions defined in assembly files must follow the C declaration convertion and be declared explicitly as **extern** in C.
```c
extern void ASMCALL foo(...);
```
```c
// NASM
global foo
foo:
    push ebp
    mov ebp, esp
    ...
    pop ebp
    ret
```
