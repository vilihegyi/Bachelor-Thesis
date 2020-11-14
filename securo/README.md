### Introduction

SecuRO is an E-Voting system using the Blockchain technology to provide a solution for a safer voting, as the current method can be easily leading to corruption.

### Environment details:
Application was made using Ubuntu 16.04 LTS, so all components may depend on the Linux libraries.

### Dependencies:
* Qt 5.12, important modules: sql, websockets
* QMYSQL
* C++ 17

### Steps for setting up SecuRO

### Compiling:

    :::ssh
    mkdir build
    cd build
    qmake ..
    make

### To show all debugging messages type:

    :::sh
    export QT_LOGGING_RULES="*=true"

### Execute the binary:

    :::sh
    ./securo
