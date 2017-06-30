//
// Created by Administrator on 2017/6/29.
//

#ifndef TNETTEST_SINGLETON_H
#define TNETTEST_SINGLETON_H

#define NONCOPYABLE(TypeName) \
    TypeName(const TypeName&); \
    TypeName &operator=(const TypeName &);

template <typename T>
class Singleton {
private:
    Singleton();
    ~Singleton();
    NONCOPYABLE(Singleton)

public:
    static T& get_instance();
};

//这个函数在C++0x之后的版本是线程安全的
template <typename T>
T& Singleton<T>::get_instance(){
    static T instance;
    return instance;
}


#endif //TNETTEST_SINGLETON_H
