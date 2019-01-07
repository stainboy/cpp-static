Demonstrate on how to use pure interface programing in C++

```
clang++ -o a.out *.cpp && ./a.out
```

```
export PATH=/home/miles/repo/llvm/build/bin:$PATH
clang -cc1 -load ~/repo/llvm/build/lib/b1cxx2java.so -plugin c2j foo.cpp
```

```
clang -fsyntax-only -Xclang -ast-print foo.cpp
```