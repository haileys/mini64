#ifndef ERROR_H
#define ERROR_H

#define ERROR_MAP(XX) \
    XX(OK,        0) \
    XX(ENOMEM,   -1) \
    XX(EALREADY, -2) \


typedef enum {
    #define XX(a,b) a = b,
    ERROR_MAP(XX)
    #undef XX
} error_t;

#define CHECKED(expr) if ((rc = (expr)) != OK) goto out

#define RETURN(err) do { rc = (err); goto out; } while (0)

#endif
