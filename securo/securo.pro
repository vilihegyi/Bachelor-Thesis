CONFIG += qt debug c++17 link_pkgconfig
QT += sql websockets

*-g++* {
    QMAKE_CXXFLAGS += "-Wall -Wextra -pedantic"
}

SOURCES += \
    src/Block.cpp \
    src/Blockchain.cpp \
    src/Client.cpp \
    src/Daemon.cpp \
    src/Database.cpp \
    src/main.cpp \
    src/SHA256.cpp

HEADERS += \
    src/Block.h \
    src/Blockchain.h \
    src/Client.h \
    src/Daemon.h \
    src/Database.h \
    src/SHA256.h
