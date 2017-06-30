//
// Created by Administrator on 2017/6/29.
//

#include "Singleton.h"
#include "logger.h"
#include <iostream>
using namespace std;

int main(){
    LOG_INIT("/root/tnet/log/log","server",DEFAULT_LOG_SIZE,5);
    LOG_DEBUG("%d",5);
    return 0;
}

