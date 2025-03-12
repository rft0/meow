## Meow - The Purrfect Programming Language
Meow is an esoteric language created for fun purposes and this repo contains bytecode interpreter for meow.

## Building
```
mkdir bin
make
```

## Usage
```
meow input.meow
```

## Instructions
Every meow instruction is 8-bit, first 3 bit represents operation type, last 5 represents
operation itself.

Meow instructions are encoded into two parts firstly `#m + #e - 2` gives us info about
type of instruction there are 4 types at total:
* **V-Type** (Operations to manipulate variables)
* **A-Type** (Arithmetic operations)
* **C-Type** (Control flow operations)
* **S-Type** (Operations that makes syscalls like printf)

We use `#o + #w - 2` to get information about second part of instruction.\
this part specifies which instruction we want ie:
* `meow` is decoded to `00000000` which is equilevant of `LOAD_CONST` in bytecode.
* `meoow` is decoded to `00000001` which is equilevant of `LOAD_VAR` in bytecode.
* `meeow` is decoded to `00100000` which is equilevant of `ADD` in bytecode.
* `mmeeoooww` is decoded to `01000101` which is equilevant of `JGE` in bytecode.
it is worthy to remind `meoow` and `meoww` generates same instruction.

You can find more information about instruction encodings at `src/lut.c`.


## Code Example
Here is an example to find nth fibonacci number in meow. 
```
nyan main:
    meow 4
    meooww ~~n

    meow 0
    meow 1
    meow 0

    meooww ~~a
    meooww ~~b
    meooww ~~tmp

    meoww ~~n
    meow 0
    mmeeow
    mmeeooww base

    meow 2
    meooww ~~it_count

nyan fib_rec:
    meoow ~~a
    meoow ~~b
    meeow
    meooww ~~tmp

    meoww ~~b
    meooww ~~a
    meoow ~~tmp
    meooww ~~b

    meoow ~~it_count
    meow 1
    mmeow
    meooww ~~it_count
    meoow ~~it_count
    meoow ~~n
    mmeeow
    meeeooowwwwww fib_rec
    mmeeoow fin

nyan base:
    meow 0
    meeeeow
    meeeeoww

nyan fin:
    meoww ~~n
    meow ". Fibonacci number is "
    meeow
    meoww ~~b
    meeow
    meeeeow
```