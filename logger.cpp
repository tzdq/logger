#include "logger.h"
#include "errorcode.h"
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

mutexLock::mutexLock() {
    pthread_mutex_init(&m_lock,NULL);
}

mutexLock::~mutexLock() {
    pthread_mutex_destroy(&m_lock);
}

void mutexLock::lock() {
    pthread_mutex_lock(&m_lock);
}

void mutexLock::unlock(){
    pthread_mutex_unlock(&m_lock);
}

logger::logger() {
    memset(m_szLogFileName,0, sizeof(m_szLogFileName));
    memset(m_szLogBaseName,0, sizeof(m_szLogBaseName));
    memset(m_szLogPath,0, sizeof(m_szLogPath));
    m_fp = NULL;
    m_iLogMaxSize = DEFAULT_LOG_SIZE;
    m_isInitSuccess = false;
}

logger::~logger() {
    closeLogFile(m_fp);
}

void logger::closeLogFile(FILE *fp){
    if(fp){
        fclose(fp);
    }
    fp = NULL;
}

int logger::openLogFile(const char *szFileName){
    if(szFileName == NULL)return ERROR_LOG_PARAM_INVALID;

    m_fp = fopen(szFileName,"a+");
    if(m_fp == NULL)return ERROR_LOG_OPEN_FILE_FAILURE;

    return 0;
}

int logger::createLogDir(const char *szPathName, int iMode) {
    if(NULL == m_szLogPath || 0 == *m_szLogPath){
        fprintf(stderr,"create_dir failed,szPath == NULL");
        return ERROR_INPUT_PARAM_NULL;
    }
    size_t len = strlen(m_szLogPath);
    if (len >= MAX_FILE_PATH_LEN) {
        fprintf(stderr,"create_dir failed,filepath is too long");
        return ERROR_PATH_NAME_TOO_LONG;
    }

    char szCurrentPath[MAX_FILE_PATH_LEN] = {0};
    memcpy(szCurrentPath,szPathName,len);

    //如果路径的最后一个字符不是/，强行加上一个
    if(szCurrentPath[len-1] != '/' && len != MAX_FILE_PATH_LEN - 2)
        szCurrentPath[len] = '/';

    //目录存在
    if (0 == access(szCurrentPath, 0))return 0;

    for (int i = 0; i <= len; i++)
    {
        char &refChar = szCurrentPath[i];
        if ((refChar == '/' && i != 0))
        {
            refChar = '\0';
            //如果目录不存在,且创建
            if (0 != access(szCurrentPath, 0))
            {
                if ( ( ENOTDIR == errno ) || ( ENOENT == errno ) ){
                    if(0 != mkdir(szCurrentPath,0755)){
                        fprintf(stderr,"mkdir %s failed,errmsg:%s\n",szCurrentPath,strerror(errno));
                        return ERROR_PATH_CREATE_FAILURE;
                    }
                }
            }
            refChar = '/';
        }
    }

    return 0;
}

int logger::createLogFile(const char *szFileName, int iMode) {
    //文件不存在，创建，直接返回
    if(szFileName == NULL){
        fprintf(stderr,"create file failed,invalid param\n");
        return ERROR_LOG_PARAM_INVALID;
    }
    if(0 == access(szFileName,F_OK))return 0;

    int fd = open(szFileName,O_WRONLY|O_CREAT|O_TRUNC,iMode);
    if(fd == -1){
        fprintf(stderr,"open file %s failed,",szFileName);
        return ERROR_FILE_OPEN_FAILURE;
    }
    close(fd);
    return 0;
}

int logger::checkLogFileSize()
{
    struct stat stStat;

    if(stat(m_szLogFileName, &stStat) < 0) {
        fprintf(stderr,"stat failed");
        return ERROR_FILE_STAT_FAILURE;
    }

    if (stStat.st_size < m_iLogMaxSize) return 0;

    return backupLogFiles(m_szLogFileName);
}

int logger::backupLogFiles(const char *szLogFileName)
{
    char szNewLogFileName[MAX_FILE_PATH_LEN] = {0};
    int i;

    //只是判断日志文件的名字是否为NULL，不判断是否存在，一般来说事存在的
    if (NULL == szLogFileName){
        fprintf(stderr,"%s param invalid",__func__);
        return ERROR_INPUT_PARAM_NULL;
    }

    //获取日期
    struct tm* pLogTime;
    struct timeval pstLogTv;

    gettimeofday(&pstLogTv,NULL);
    pLogTime = localtime(&(pstLogTv.tv_sec));

    snprintf(szNewLogFileName, sizeof(szNewLogFileName),"%s_%04d%02d%02d_bak",
            szLogFileName,pLogTime->tm_year + 1900,pLogTime->tm_mon + 1,pLogTime->tm_mday);

    if (rename(szLogFileName, szNewLogFileName) < 0) {
        fprintf(stderr,"rename(%s,%s) failed",szLogFileName,szNewLogFileName);
        return ERROR_FILE_RENAME_FAILURE;
    }

    return 0;
}

const char* logger::getLogLevelName(int nLevel)
{
    switch(nLevel)
    {
        case LOG_LEVEL_DEBUG:
            return "DEBUG";
        case LOG_LEVEL_INFO:
            return "INFO";
        case LOG_LEVEL_WARNING:
            return "WARNING";
        case LOG_LEVEL_ERROR:
            return "ERROR";
    }
    return "N/A";
}

int logger::loggerInit(const char* szLogPath,const char* szBaseFileName,uint iMaxLogSize)
{
    if ( NULL == szLogPath || NULL == szBaseFileName){
        fprintf(stderr,"[%s] param invalid",__func__);
        return ERROR_LOG_PARAM_INVALID;
    }

    if (iMaxLogSize <= 0) iMaxLogSize = DEFAULT_LOG_SIZE;

    strncpy(m_szLogPath, szLogPath, sizeof(m_szLogPath));
    strncpy(m_szLogBaseName, szBaseFileName,sizeof(m_szLogBaseName));
    snprintf(m_szLogFileName, sizeof(m_szLogFileName), "%s/%s.log", szLogPath, szBaseFileName);
    
    m_iLogMaxSize = iMaxLogSize;

    /* 初始化日志路径 */
    int ret = createLogDir(m_szLogPath,0755);
    if(ret != 0) {
        fprintf(stderr,"create log dir failed,ret = %d,errmsg : %s\n",ret,strerror(errno));
        return ERROR_PATH_CREATE_FAILURE;
    }

    ret = createLogFile(m_szLogFileName,0666);
    if(ret != 0) {
        fprintf(stderr,"create log file failed,ret = %d,errmsg:%s\n",ret,strerror(errno));
        return ERROR_PATH_CREATE_FAILURE;
    }

    /* 判断是否需要备份一下旧的日志 */
    ret = checkLogFileSize();
    if(ret != 0){
        fprintf(stderr,"backup log files failed,ret = %d,errmsg :%s\n",ret,strerror(errno));
        return ERROR_LOG_FILE_BACKUP_FAILURE;
    }

    /* 打开日志文件 */
    ret = openLogFile(m_szLogFileName);
    if(ret != 0){
        fprintf(stderr,"open log file failed,ret = %d,errmsg :%s\n",ret,strerror(errno));
        return ERROR_LOG_OPEN_FILE_FAILURE;
    }

    m_isInitSuccess = true;
    return 0;
}

int logger::loggerInfo(uint iLogLevel, const char* szFormat, ...)
{
    struct timeval stLogTv;
    va_list ap;

    if (false == m_isInitSuccess) {
        fprintf(stderr,"[%s] Logger Need Init!!!",__func__);
        return ERROR_LOG_NO_INIT;
    }

    if (NULL == szFormat) {
        fprintf(stderr,"[%s] param invalid",__func__);
        return ERROR_INPUT_PARAM_NULL;
    }

    m_lock.lock();
    va_start(ap, szFormat);
    gettimeofday(&stLogTv, NULL);
    loggerWrite(iLogLevel, &stLogTv, szFormat, ap);
    va_end(ap);
    m_lock.unlock();

    return 0;
}

int logger::loggerWrite(int iLogLevel, struct timeval* pstLogTv, const char* szFormat, va_list ap)
{
    struct tm* pLogTime;
    time_t tLogTime;

    if (NULL == szFormat || NULL == pstLogTv){
        fprintf(stderr,"[%s] param invalid",__func__);
        return ERROR_INPUT_PARAM_NULL;
    }

    /* 日志打印时间 */
    tLogTime = pstLogTv->tv_sec;
    pLogTime = localtime(&tLogTime);

    /* 打印日志头 */
    fprintf(m_fp, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] [%s]:%d:(%s): ", pLogTime->tm_year + 1900, pLogTime->tm_mon + 1, pLogTime->tm_mday, pLogTime->tm_hour, pLogTime->tm_min, pLogTime->tm_sec, getLogLevelName(iLogLevel),
    __FILE__,__LINE__,__func__);

    /* 打印日志内容 */
    vfprintf(m_fp, szFormat, ap);
    fprintf(m_fp, "\n");

    /* 检查日志是否过大，如果过大，备份起来 */
    checkLogFileSize();

    return 0;
}

