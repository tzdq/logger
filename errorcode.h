#ifndef TNETTEST_ERRORCODE_H
#define TNETTEST_ERRORCODE_H

/* 普通错误码-1~-1000 */
#define ERROR_INPUT_PARAM_NULL                      -1      /* 输入参数不合法，为NULL */
#define ERROR_FILE_OPEN_FAILURE                     -2      /* 打开文件时，发生错误 */
#define ERROR_FILE_RENAME_FAILURE                   -3      /* 文件更名失败 */
#define ERROR_FILE_STAT_FAILURE                     -4      /* 获取文件信息失败 */
#define ERROR_PATH_NAME_TOO_LONG                    -5      /* 路径名太长 */
#define ERROR_PATH_CREATE_FAILURE                   -6      /* 创建路径失败 */

/* 日志处理错误码段-1601~-1700 */
#define ERROR_LOG_OPEN_FILE_FAILURE                 -1601   /* 打开日志文件出错 */
#define ERROR_LOG_PARAM_INVALID                      -1602   /* 日志参数非法 */
#define ERROR_LOG_NO_INIT                             -1603   /* 日志实例未成功初始化 */
#define ERROR_LOG_FILE_BACKUP_FAILURE               -1604  /*  日志备份失败 */

#endif //TNETTEST_ERRORCODE_H
