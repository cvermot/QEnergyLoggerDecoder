# QEnergyLoggerDecoder
Voltcraft Energy Logger 4000 (and 3500 ?) binary files decoder

Allow to decode file produced by Conrad Energy Logger. Software provided by Voltcraft is slow and buggy, and run only on Windows. QEnergyLoggerDecoder is aimed to provide an alternative in order to manage efficiently this files on any platform supported by Qt.

![QEnergyLoggerDecoder v1.1 screenshot under Linux](http://afterflight.org/wp-content/uploads/sites/9/2018/12/QEnergyLoggerDecoder-1.1.png)

Minimum version of Qt : 5.10 (required for commit and rollback transactions on QDataStream).

Binaries versions are provided for Windows and MacOS (look at the release page : https://github.com/verdesroches/QEnergyLoggerDecoder/releases). Currently, I provide no binaries versions for Linux, but you should be able to build the software by your own (How To will come later).
