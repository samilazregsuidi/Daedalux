# Daedalux

A model-checker for checking product lines written in C++.

## Getting Started

The model-checker consumes a product line written in Promela and outputs a


## Documentation

Maybe add the articles here?

Sami fill this in.


## How to build

```console
foo@bar:~$ cd deadalux
foo@bar:~$ mkdir build
foo@bar:~$ cd build
foo@bar:~$ cmake ../
foo@bar:~$ cmake --build .
```

Alternatively, you can build it using docker from the root directory of the project using the following command:

```console
foo@bar:~$ docker build -t deadalux .
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
  * -ksample <number>: Bounded model-check the product line by exploring traces of length *n*.
  * -bfs: Use breadth-first search to explore the product line instead of depth-first search.
  * 
* Options for features and feature model:
  * -fm <path-to-feature-model>: Specify the path to the feature model tvl. This option can be omitted if the feature model has the same name as the promela file.
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

The following example is a simple product line written in Promela. The product line has two features, B1 and B2. The product line is written in Promela and is saved in a file called "test.pml".

```promela
typedef features {
    bool B1;
    bool B2	
}

features f;

active proctype test (){
    bool B1, B2;
    if :: f.B1 -> B1 = true; :: else; fi;
    if :: f.B2 -> B2 = true; :: else; fi;
}
```

The model-checker can be run by executing the following command in the terminal:

For example, the following command runs the model-checker on the file "test.pml" and prints the output to the terminal:

```console
foo@bar:~$ ./daedalux -check test.pml
```

The output of the model-checker is a file called "test.pml.out". The file contains the following output:

Sami fill this in.

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
