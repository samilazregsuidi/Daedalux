# Daedalux

A model-checker for checking product lines written in C++.

## Getting Started

The model-checker consumes a product line written in Promela and outputs a

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



Sami fill this in.

The output of the model-checker is a file called "test.pml.out". The file contains the following output:

Sami fill this in.

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

Sami fill this in.

After building the project, you can run the model-checker using the following command:

```console
foo@bar:~$ ./daedalux <path-to-promela-file> <parameters>
```

Note that the model-checker requires a Promela file as input. The model-checker also accepts the following parameters:

Sami fill this in.

- -h, --help: Prints the help message.
- -v, --verbose: Prints the output of the model-checker.
- -o, --output: Specifies the output file.

For example, the following command runs the model-checker on the file "test.pml" and prints the output to the terminal:

```console
foo@bar:~$ ./daedalux test.pml -v
```

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
