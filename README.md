# Kaleidoscope Language

This repository contains all the code necessary for implementing the Kaleidoscope language, which is an educational and simple language proposed by LLVM. 
Our language will strictly obey the following assumptions:
    - All variables will be intialized in a global scope
    - The only data type is "Double" (IEEE-754 floating point number with extended precision)
    - We use LLVM as the intermediate representation of this compiler
    - No semantic analysis will be performed

The description of Kaleidoscope grammar can be found at the parser.h file for a more detailed description. 
This interpreter which aim is to convert the language to LLVM-IR can be compilated with g++/clang++ using the following command:

``` g++ -std=c++14 main.cpp parser.cpp AST.cpp lexer.cpp `llvm-config --system-libs --cppflags --ldflags --libs core` -o main ```

[Kaleidoscope: Implementing a Language with LLVM](https://llvm.org/docs/tutorial/LangImpl01.html)