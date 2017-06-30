//
// Created by Administrator on 2017/6/28.
//

#ifndef TNETTEST_LOGGER_H
#define TNETTEST_LOGGER_H

#include <stdio.h>
#include <pthread.h>
#include "Singleton.h"

enum LOG_LEVEL{
    LOG_LEVEL_ERROR   = 1, //错误
    LOG_LEVEL_WARNING  = 2, //警告
    LOG_LEVEL_INFO     = 3, //普通
    LOG_LEVEL_DEBUG    = 4, //调试
    LOG_LEVEL_NULL
};

#define LOG_INIT(szLogPath,szBaseFileName,iMaxLogSize)  \
(Singleton<logger>::get_instance().loggerInit(szLogPath,szBaseFileName,iMaxLogSize))

#define LOG_DEBUG(format,args...)   (Singleton<logger>::get_instance().loggerInfo(LOG_LEVEL_DEBUG, format, ##args))
#define LOG_INFO(format,args...)    (Singleton<logger>::get_instance().loggerInfo(LOG_LEVEL_INFO, format, ##args))
#define LOG_WARNING(format,args...) (Singleton<logger>::get_instance().loggerInfo(LOG_LEVEL_WARNING, format, ##args))
#define LOG_ERROR(format,args...)    (Singleton<logger>::get_instance().loggerInfo(LOG_LEVEL_ERROR, format, ##args))

const int MAX_FILE_PATH_LEN = 256;
const int MAX_FILE_NAME_LEN = 64;
const int DEFAULT_LOG_SIZE  = (10*1024*1024); //10M

typedef unsigned int uint;


class mutexLock{
public:
    mutexLock();
    virtual ~mutexLock();
    void lock();
    void unlock();

private:
    pthread_mutex_t m_lock;
};

class logger {
private:
    friend class Singleton<logger>;

    logger();
    virtual ~logger();
    logger(const logger&);
    logger& operator=(const logger &);

public:
    /**
    * 初始化日志结构体对象
    * @param szLogPath 日志存放路径
    * @param szBaseFileName 日志文件名
    * @param iMaxLogSize 每个日志文件的最大文件大小，单位：byte
    * @return 成功返回0，失败返回错误码
    */
    int loggerInit(const char* szLogPath,const char* szBaseFileName,uint iMaxLogSize);

    /**
     * @param iLogLevel 日志级别
     * @param szFormat 格式化串
    * @return 成功返回0，失败返回错误码
    */
    int loggerInfo(uint iLogLevel, const char* szFormat, ...);

private:
    void closeLogFile(FILE* fp);
    int openLogFile(const char *szFileName);

    /* 返回指定日志级别的级别描述 */
    const char* getLogLevelName(int nLevel);

    /**
    * 循环嵌套创建路径和对应的日志文件
     * @param szPathName 路径名
     * @param iMode  权限
    * @return 成功返回0，失败返回错误码
    */
    int createLogDir(const char *szPathName,int iMode);
    int createLogFile(const char* szFileName,int iMode);

    /**
    * 将日志打印到文件
    * @param iLogLevel 日志打印级别
     * @param pstLogTv 日志打印时间
     * @param szFormat 格式化串
     * @param ap 打印参数列表
     * @return 成功返回0，失败返回错误码
    */
    int loggerWrite(int iLogLevel, struct timeval* pstLogTv, const char* szFormat, va_list ap);

    /**
    * 判断日志是否达到最大值，若达到，执行备份操作
    * @return 成功返回0，失败返回错误码
    */
    int checkLogFileSize();

    /**
    * 实现日志文件之间的切换和备份
    * @param szLogFileName 日志文件名
    * @return 成功返回0，失败返回错误码
    */
    int backupLogFiles( const char *szLogFileName);

private:
    mutexLock m_lock;                             //同步锁,用于多线程同步写
    char      m_szLogPath[MAX_FILE_PATH_LEN];    //日志文件路径名
    char      m_szLogBaseName[MAX_FILE_NAME_LEN];//日志文件文件名
    char      m_szLogFileName[MAX_FILE_PATH_LEN];//日志文件全路径，等价于m_szLogPath+m_szBaseName
    FILE*      m_fp;                             //日志文件句柄
    uint       m_iLogMaxSize;                    //设置的日志文件大小
    uint       m_isInitSuccess;                 //是否成功初始化
};

#endif //TNETTEST_LOGGER_H
