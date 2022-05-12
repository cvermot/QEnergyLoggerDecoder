# Minimal qmake file for building qenergyloggerdecoder.
# You will need to install the relevant QT development files from your
# distribution.
# Generate makefile with: qmake qenergyloggerdecoder.pro
# Build binaries with: make

SOURCES += main.cpp eldatahandler.cpp elfileparser.cpp qenergyloggerdecoder.cpp
HEADERS += eldatahandler.h elfileparser.h qenergyloggerdecoder.h
QT += widgets
