Perfect — this is exactly the right moment to **freeze a reference spec**.
# Project Spec — Minimal Hybrid Systems + AI Language (Reference)

> Status: Early / Experimental  
> This document is a reference, not a commitment. Details may change.

---

## 1. Goal

Build a **small, sane, standalone programming language** that:
- Feels simple like Python
- Is memory-safe without GC (ARC-based)
- Compiles to efficient native code
- Supports normal systems programming
- Can integrate AI computation later without redesign

The priority is **clarity, correctness, and finishability**.

---

## 2. Design Principles

- **Keep the core small**
- **No kernel-level programming**
- **No hardware-specific code**
- **Delegate heavy optimization to proven backends**
- **Language semantics > performance tricks**
- **Readable implementation over cleverness**

---

## 3. Implementation Language

- Compiler & runtime written in **C++20**
- Boring, RAII-based style
- No complex inheritance
- Explicit ownership rules

---

## 4. High-Level Architecture

```

Source Code
↓
Lexer
↓
Parser
↓
AST
↓
Semantic Analysis
↓
Typed IR
↓───────────────┐
↓               ↓
Control Code     Tensor Code (future)
↓               ↓
Cranelift        TVM / other backend
↓
Native Binary

````

---

## 5. Language Core (v0)

### 5.1 Built-in Types

- `Int`
- `Float`
- `Bool`
- `String`
- `Unit` / `Void`

No generics in v0.

---

### 5.2 Structs

- Plain data containers
- No inheritance
- No methods initially
- Value semantics by default

Example:
```text
struct Point {
    x: Float
    y: Float
}
````

---

### 5.3 Functions

* Top-level functions
* Named parameters
* Explicit return types
* No closures in v0

Example:

```text
fn add(a: Int, b: Int) -> Int {
    return a + b
}
```

---

### 5.4 Control Flow

Supported:

* `if / else`
* `while` or `for`
* `return`

Not supported (v0):

* pattern matching
* exceptions
* async / await

---

### 5.5 Variables

* Immutable by default
* `mut` keyword for mutation
* Lexical scoping

Example:

```text
let x = 10
mut y = 20
y = y + x
```

---

## 6. Memory Model

* ARC / reference counting
* Deterministic destruction
* No garbage collection
* No raw pointers in user language
* Heap allocation handled by runtime

Key rule:

> Memory behavior must be predictable and visible.

---

## 7. Compiler Internals

### 7.1 Lexer

* Handwritten
* Produces tokens with source locations
* No regex or generators

---

### 7.2 Parser

* Handwritten recursive-descent
* One function per grammar rule
* Minimal error handling in v0

---

### 7.3 AST

* Dumb data structures
* No logic
* Represents syntax only

---

### 7.4 Typed IR (Semantic IR)

Purpose:

* Resolve names
* Attach types
* Make control flow explicit
* Insert ARC semantics

Properties:

* Simple
* Small instruction set
* No SSA requirement

---

### 7.5 Backend (v0)

**Control / CPU Code**

* Lowered to Cranelift IR
* Cranelift handles:

  * SSA
  * Dead code elimination
  * Constant folding
  * Register allocation
  * Machine code emission

Compiler does NOT implement optimizations.

---

## 8. Standard Library (v0)

Very small and minimal.

Included:

* `print`
* basic math
* string operations

Excluded (for now):

* networking
* async
* threading
* filesystem
* AI / tensors

---

## 9. Bootstrapping Plan

1. Compiler written in C++
2. Language can compile and run simple programs
3. Features added incrementally
4. Self-hosting only considered much later

---

## 10. Non-Goals (Important)

This project does NOT aim to:

* Compete with C++ or Rust
* Replace Python ecosystems
* Provide custom GPU kernels
* Support every accelerator
* Be production-ready quickly

---

## 11. Future Directions (Non-Binding)

Possible later additions:

* Tensor type (opaque handle)
* AI backend integration (TVM, ONNX, etc.)
* Concurrency model
* Parallel loops
* Rich standard library
* Tooling (formatter, LSP)

These are **explicitly out of scope for v0**.

---

## 12. Success Criteria (Early)

The project is successful if:

* The compiler is understandable end-to-end
* Simple programs compile and run
* Memory behavior is predictable
* The architecture does not collapse under new features

---

## 13. Guiding Rule

> If a feature makes the compiler harder to understand, it does not belong in v0.

```

---

### Final advice (important)

Keep this file **versioned**, not perfect.  
Every time you feel lost, come back to it and ask:

> “Am I still inside this box?”

If yes → keep going.  
If no → stop and simplify.

If you want next, I can:
- review this spec after *you* modify it,
- help you write a **grammar section**, or
- help you add a **Typed IR section** in more detail.

Just tell me.
```
