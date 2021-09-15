# Collision checker
By Vincent AUBRIOT

This is a (small ?) implementation of the attack specified in [this document - section 1](https://www-ljk.imag.fr/membres/Pierre.Karpman/cry_intro2020_tp.pdf), that follows the requirements of the last part of the subject.
The code was written in March/April 2021 for a graded practical work, and it took me dozens of hours of reflexion before being able to put things together in a working state.
I provide no guarantee on whether the code works on your platform, nor the resource consumption that can occur when launching the program.

The entire implementation of the attack is contained in the file hikari.c (except for the originally provided source code)

## Compilation
A makefile is provided, typing make will just compile the binary.
You can delete the object files and the binary by typing make mrproper.

## Execution
You can launch the program by running:

./hikari [d]

By default, d is set to 1, and thus generating a 2-collision.
