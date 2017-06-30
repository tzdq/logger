# logger

单例模式下的日志，只支持类*nix系统，支持线程安全

Singleton是一个单例模式的简单实现，在C++0x版本后，这个单例模式的实现是线程安全的

errorcode用于定义错误码

mutexLock定义了posix下的互斥锁的基本操作

使用时需要使用：gcc main.cpp logger.cpp -o main -lstdc++来编译这个源码


