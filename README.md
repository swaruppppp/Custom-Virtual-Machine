# CVM++

A from-scratch compiler and virtual machine for a custom scripting language, written entirely in C++17.

CVM++ takes raw source code and runs it through four stages — lexing, parsing, compiling, and executing — producing results on a stack-based virtual machine. No external dependencies. No frameworks. Just C++.

---

## How It Works

```
  .cvm source file
        │
        ▼
   ┌─────────┐
   │  Lexer   │──▶ Breaks text into tokens
   └─────────┘
        │
        ▼
   ┌─────────┐
   │  Parser  │──▶ Builds an Abstract Syntax Tree
   └─────────┘
        │
        ▼
   ┌──────────┐
   │ Compiler │──▶ Emits flat bytecode (uint8_t array)
   └──────────┘
        │
        ▼
   ┌─────────┐
   │   VM     │──▶ Executes bytecode on a stack machine
   └─────────┘
        │
        ▼
     Output
```

---

## Quick Start

**Build:**

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o cvm
```

**Run a script:**

```bash
./cvm examples/demo.cvm
```

**Launch the REPL:**

```bash
./cvm
```

---

## The Language

CVM++ scripts use the `.cvm` extension. Every statement ends with `;`.

```
let name = 42;
let flag = true;
print name + 8;

let x = input;
print x * 2;

if (name < 100) {
    print 1;
} else {
    print 0;
}

let counter = 0;
while (counter < 5) {
    print counter;
    counter = counter + 1;
}
```

### What You Can Do

| Category    | Syntax                          |
|-------------|---------------------------------|
| Declare     | `let x = 10;`                   |
| Assign      | `x = 20;`                       |
| Math        | `+`  `-`  `*`  `/`             |
| Compare     | `==`  `<`                       |
| Print       | `print x;`                      |
| Input       | `let x = input;`                |
| Branch      | `if (cond) { } else { }`        |
| Loop        | `while (cond) { }`              |
| Comment     | `// ignored by the compiler`     |

Types are simple — integers and booleans only. Booleans are `1` and `0` under the hood.

---

## Interactive Mode

Running `./cvm` with no arguments drops you into a REPL. Variables persist between lines.

```
CVM++ | Type 'exit' to quit, 'reset' to clear.

>> let radius = 5;
>> print radius * radius;
25
>> reset
(cleared)
>> exit
```

Use `{` and `}` for multi-line blocks — the REPL waits for the closing brace:

```
>> while (x < 3) {
..     print x;
..     x = x + 1;
.. }
0
1
2
```

---

## Project Layout

```
CVM++/
│
├── include/             Headers
│   ├── token.h          Token enum + struct
│   ├── lexer.h          Lexer class
│   ├── ast.h            AST node types
│   ├── parser.h         Recursive descent parser
│   ├── compiler.h       Bytecode emitter + OpCodes
│   └── vm.h             Stack-based VM
│
├── src/                 Implementation
│   ├── lexer.cpp
│   ├── parser.cpp
│   ├── compiler.cpp
│   ├── vm.cpp
│   └── main.cpp         Entry point (REPL + file runner)
│
├── examples/
│   └── demo.cvm         Sample script
│
├── CMakeLists.txt
└── README.md
```

---

## Under the Hood

A few things that make this more than a toy:

- **Variables resolve to slot indices at compile time.** The VM never touches strings — it reads and writes `globals[slot_number]` directly, just like a real compiler.

- **Control flow uses backpatching.** `if/else` and `while` emit jump instructions with placeholder addresses, then go back and fill in the real targets once they're known.

- **Bytecode is a flat `uint8_t` array.** Same design used by CPython, Lua, and the JVM. Each opcode is one byte; operands follow inline.

- **16-bit jump offsets.** Enough for 64KB of bytecode — more than sufficient for educational use, and it keeps the instruction format simple.

---

## Building with CMake

If you prefer CMake over a raw `g++` command:

```bash
cmake -B build
cmake --build build
```

The binary ends up in `build/` (or `build/Debug/` on Windows).

---

## Contributing

1. Fork the repo
2. Create your branch — `git checkout -b feature/something`
3. Commit — `git commit -m "Add something"`
4. Push — `git push origin feature/something`
5. Open a PR

Keep it clean, keep it compiling without warnings.

---

## License

MIT — do whatever you want with it.

---

*Made by [PeakshitKaajla](https://github.com/PeakshitKaajla)*
