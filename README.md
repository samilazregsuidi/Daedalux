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

What things you need to install the software and how to install them.

Sami fill this in.

### Dependencies

