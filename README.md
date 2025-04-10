# Mini C Compiler in C++

This project implements a simple compiler for a subset of the C programming language using C++. It supports:

## Features
- **Lexical analysis**: Tokenizes input C-like code.
- **Syntax parsing**: Detects and validates basic statements.
- **Semantic analysis**: Checks for undeclared variables.
- **Intermediate Code Generation (ICG)**: Produces 3-address code.
- **ICG Optimization**: Performs constant folding, copy propagation, and removes redundant assignments.
- **Assembly Code Generation**: Emits pseudo-assembly instructions.

## Supported Syntax
```c
int x = 2 + 3;
int y = 10;

if (y > 5) {
    x = x + 1;
    x = x + 1;
}

while (x < 15) {
    x = x + 1;
}
```

## Control Flow Support
- `if` and `while` statements with conditionals like `>`, `<`.
- Uses labels (`L0`, `L1`, etc.) and jumps for flow control.

## Optimization Examples
```c
int x = 2 + 3;     // Constant folding
int y = x;         // Copy propagation
x = x + 0;         // Removed if redundant
```

## How to Use
1. Create a file named `test.c` with sample input code.
2. Compile the C++ compiler:
   ```bash
   g++ -o compiler compiler.cpp
   ./compiler
   ```
3. View the output:
   - ICG before and after optimization
   - Final assembly code

## Future Extensions
- Add `else` and nested blocks
- Support `==`, `!=`, `<=`, `>=`
- Include function declarations
- Output real `.asm` files for NASM

## File Structure
```
├── compiler.cpp   # Main compiler code
├── test.c         # Input source file
├── README.md      # This file
```
