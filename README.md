# Daedalux

A model-checker for checking product lines written in C++.

## Getting Started

The model-checker consumes a product line written in Promela and outputs a

## How to build

```console
foo@bar:~$ cd daedalux
foo@bar:~$ mkdir build
foo@bar:~$ cd build
foo@bar:~$ cmake ../
foo@bar:~$ cmake --build .
```

Alternatively, you can build it using docker from the root directory of the project using the following command:

```console
foo@bar:~$ docker build -t daedalux.
```

After building the image you can run the model-checker using the following command:

```console
foo@bar:~$ docker run -it deadalux <path-to-promela-file> 
```

## How to run it

After building the project, you can run the model-checker using the following command:

```console
foo@bar:~$ ./daedalux <parameters> <path-to-promela-file> 
```

The parameters can be divided into the following categories:
* Options for model-checking:
  * -check: Model-check the product line until a property violation is found.
  * -exhaustive: Exhaustively model-check the product line until all possible traces have been explored to determine which products are valid.
  * -sample <number>: Search for a valid product by randomly exploring *n* traces of the product line.
  * -ksteps <number>: Bounded model-check the product line by exploring traces of length *n*.
  * -ltl <string>: LTL property to verify.
  * -ltlFile <path> File containing LTL properties to verify.
  * -multiLtl <string>: multiLTL property to verify.
  * -multiLtlFile <path>: File containing multiLTL properties to verify.
* Options for features and feature model:
  * -fm <path>: Specify the path to the feature model TVL. This option can be omitted if the feature model has the same name as the promela file.
* Options for output:
  * -nt: Do not print the trace to the terminal.
  * -st: Only prints states when there are no changed variables.
* Options for debugging:
  * -exec: Execute the model (does not print states, only model output).
  * -l <number>:  Stop when the given number of states were explored.
  * -s: Print static information (symbol table, FSMs, MTypes, ..) to the terminal.
  * -t: Do not delete generated temporary files.

A selection of Promela files can be found in the "test" folder in the root directory of the project.

###  A simple example

The following example is a simple product line written in Promela. The product line has one feature, Alarm that stop the motor of something dangerous occurs. The product line is written in Promela and is saved in a file called "example.pml".

```promela
typedef features {
    bool Alarm;
}

bool safe = true;
bool danger, stopped = false;

active proctype Motor() {
    do
    :: safe ->
        if :: skip;
           :: safe = false; danger = true fi;
    :: danger ->
        gd :: Alarm -> danger = false; 
                       stopped = true;
           :: else -> skip; dg;
    :: stopped
        if :: skip;
           :: stopped = false; safe = true; 
        fi; od}
```

The model-checker can be run by executing the following command in the terminal:

For example, the following command runs the model-checker on the file "example.pml" and prints the output to the terminal:

```console
foo@bar:~$ ./daedalux check -ltl '[](danger-><>stopped)' example.pml
```

The output of the model-checker is a file called "example.pml.out". The file contains the following output:

```console
Checking LTL property [](danger-><>stopped)..
[startNestedDFS]
 - Stack trace:
   features                            = All
   never                               @ NL27 
   globals.safe                        = 1
   globals.danger                      = 0
   globals.stopped                     = 0
   pid 00, Motor                       @ NL11
    --
   pid 00, Motor                       @ NL13
    --
   globals.safe                        = 0
   pid 00, Motor                       @ NL14
    --
   globals.danger                      = 1
   pid 00, Motor                       @ NL11
    --
    -- Loop beings here --
    --
   never                               @ NL32  (accepting)
   pid 00, Motor                       @ NL16
    --
    -- Loop begin repeated in full:
   features                            = All
   never                               @ NL32  (accepting)
   globals.safe                        = 0
   globals.danger                      = 1
   globals.stopped                     = 0
   pid 00, Motor                       @ NL16
    --
   features                            = (!Alarm)
   pid 00, Motor                       @ NL18
    --
   pid 00, Motor                       @ NL11
    --
   pid 00, Motor                       @ NL16
    --
    -- Final state repeated in full:
   features                            = (!Alarm)
   never                               @ NL32  (accepting)
   globals.safe                        = 0
   globals.danger                      = 1
   globals.stopped                     = 0
   pid 00, Motor                       @ NL16
    --

Exhaustive search finished  [explored 13 states, re-explored 0].
 -  One problem found covering the following products (others are ok):
(!Alarm)
```
This output shows that Daedalux found one trace that violates the LTL property. This trace counterexample belongs to the product without the Alarm feature. The other product (the one with the Alarm) satisfies the property.
Each state of the trace is separated by -- characters. Only state variables that have changed are printed by default. *features* is a boolean function variable, where *pid 00, Motor* and *never* are thread location variables. Others are global booleans.

### Dependencies

What things you need to install the software and how to install them.

- C++20 compiler
- Flex & Bison
- [CUDD library](https://github.com/ivmai/cudd)
- CMake 3.16

## Future work

The following is a list of features that are planned to be implemented in the future.

- [ ] Support for Timed Automata
  - [ ] Extension of input language
  - [ ] Extend the parser to support Timed Automata
  - [ ] Model-checking of Timed Automata
- [ ] Set up a CI pipeline (Github Actions)
- [x] Build a docker image
- [ ] Clean up the repository (remove unused files, etc.)
  - [ ] Sami I think you should do this.
    - [ ] Are all the mdp copy files needed?
    - [ ] What about the traces folder?
    - [ ] What about the tmp files?
    - [ ] Alternatively, we could just add them to the .gitignore file.
