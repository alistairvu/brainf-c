# BF Interpreter

This interpreter will run a valid brainf\*ck file. A valid brainf\*ck file here is defined as one with the extension `.b` or `.bf`, and with all `[` starting loop brackets having a matching `]` closing loop bracket.

## Compilation

Just type

```
make all
```

into the terminal. The Makefile assumes that gcc is pre-installed on the system. After the compilation process, a `brainf` binary file will be created.

## Example

```
./brainf hello_world.bf
```

should output `Hello World!` onto the terminal.
