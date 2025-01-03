#ifndef _RPC_MACROS_H
#define _RPC_MACROS_H

#include <llbc.h>
#include <coroutine>

#define COND_EXP(condition, expr, ...) \
    {                                  \
        if (condition) {               \
            expr;                      \
        }                              \
    }

#define COND_RET(condition, ...) \
    {                            \
        if (condition) {         \
            return __VA_ARGS__;  \
        }                        \
    }

#define COND_RET_TLOG(condition, retCode, ...) \
    {                                          \
        if (condition) {                       \
            LLOG_TRACE(__VA_ARGS__);           \
            return retCode;                    \
        }                                      \
    }

#define COND_RET_ILOG(condition, retCode, ...) \
    {                                          \
        if (condition) {                       \
            LLOG_INFO(__VA_ARGS__);            \
            return retCode;                    \
        }                                      \
    }

#define COND_RET_WLOG(condition, retCode, ...) \
    {                                          \
        if (condition) {                       \
            LLOG_WARN(__VA_ARGS__);            \
            return retCode;                    \
        }                                      \
    }

#define COND_RET_ELOG(condition, retCode, ...) \
    {                                          \
        if (condition) {                       \
            LLOG_ERROR(__VA_ARGS__);           \
            return retCode;                    \
        }                                      \
    }

#define COND_EXP_ELOG(condition, expr, ...) \
    {                                       \
        if (condition) [[unlikely]] {       \
            LLOG_ERROR(__VA_ARGS__);        \
            expr;                           \
        }                                   \
    }

#define CO_COND_RET_TLOG(condition, retCode, ...) \
    {                                             \
        if (condition) {                          \
            LLOG_TRACE(__VA_ARGS__);              \
            co_return retCode;                    \
        }                                         \
    }

#define CO_COND_RET_ILOG(condition, retCode, ...) \
    {                                             \
        if (condition) {                          \
            LLOG_INFO(__VA_ARGS__);               \
            co_return retCode;                    \
        }                                         \
    }

#define CO_COND_RET_WLOG(condition, retCode, ...) \
    {                                             \
        if (condition) {                          \
            LLOG_WARN(__VA_ARGS__);               \
            co_return retCode;                    \
        }                                         \
    }

#define CO_COND_RET_ELOG(condition, retCode, ...) \
    {                                             \
        if (condition) {                          \
            LLOG_ERROR(__VA_ARGS__);              \
            co_return retCode;                    \
        }                                         \
    }

#endif  // _RPC_MACROS_H