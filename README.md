# mygrep

A C++ command-line tool for searching patterns in files, similar to `grep`. Supports plain text and regex search, case sensitivity, and handles large files efficiently.

## Features

- Search for plain text or regular expressions in files
- Case-sensitive and case-insensitive search
- Handles large files efficiently
- Graceful handling of binary files and empty patterns
- Unicode support

## Low level design

```sh
+-------------------+
|      Config       |  (namespace)
+-------------------+
| SEARCH_PATTERN    |
| CASE_SENSITIVE    |
| USE_REGEX         |
| SEARCH_RECURSIVELY|
+-------------------+

+----------------------+
|   PatternSearcher    |
+----------------------+
| +searchInFile(path)  |
+----------------------+

+----------------------+
|   ThreadManager      |
+----------------------+
| +run()               |
+----------------------+

+----------------------+
|      utility         |
+----------------------+
| +logError(msg)       |
| +isTextFile(path)    |
| +printMatchSnippet() |
| +usage()             |
+----------------------+

+----------------------+
|       main           |
+----------------------+
| - parses args        |
| - sets config        |
| - starts ThreadMgr   |
+----------------------+

main --> Config
main --> ThreadManager
ThreadManager --> PatternSearcher
PatternSearcher --> utility
PatternSearcher --> Config
```

## Building

Requires CMake and a C++20-compatible compiler.

```sh
mkdir build
cd build
cmake ..
```
To build with tests:

```sh
makecmake -DBUILD_TESTS=ON ..
make
```

Running

```sh
./mygrep [options] <pattern> <file>
```

Testing
To run the tests, ensure you have built with the `BUILD_TESTS` option enabled. After building, you can run the tests using:

```sh
./mygrep_tests
```