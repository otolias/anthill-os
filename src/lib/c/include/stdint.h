#ifndef _STDINT_H
#define _STDINT_H

/* Fixed width integer types */

#ifdef __INT8_TYPE__
typedef __INT8_TYPE__ int8_t;
#endif /* __INT8_TYPE__ */
#ifdef __INT16_TYPE__
typedef __INT16_TYPE__ int16_t;
#endif /* __INT16_TYPE__ */
#ifdef __INT32_TYPE__
typedef __INT32_TYPE__ int32_t;
#endif /* __INT32_TYPE__ */
#ifdef __INT32_TYPE__
typedef __INT64_TYPE__ int64_t;
#endif /* __INT64_TYPE__ */

#ifdef __UINT8_TYPE__
typedef __UINT8_TYPE__ uint8_t;
#endif /* __UINT8_TYPE__ */
#ifdef __UINT16_TYPE__
typedef __UINT16_TYPE__ uint16_t;
#endif /* __UINT16_TYPE__ */
#ifdef __UINT32_TYPE__
typedef __UINT32_TYPE__ uint32_t;
#endif /* __UINT32_TYPE__ */
#ifdef __UINT64_TYPE__
typedef __UINT64_TYPE__ uint64_t;
#endif /* __UINT64_TYPE__ */

/* Fixed width integer limits */

#ifdef __INT8_MAX__
#define INT8_MAX __INT8_MAX__
#define INT8_MIN (-__INT8_MAX__ -1)
#endif /* __INT8_MAX__ */
#ifdef __INT16_MAX__
#define INT16_MAX __INT16_MAX__
#define INT16_MIN (-__INT16_MAX__ -1)
#endif /* __INT16_MAX__ */
#ifdef __INT32_MAX__
#define INT32_MAX __INT32_MAX__
#define INT32_MIN (-__INT32_MAX__ -1)
#endif /* __INT32_MAX__ */
#ifdef __INT64_MAX__
#define INT64_MAX __INT64_MAX__
#define INT64_MIN (-__INT64_MAX__ -1)
#endif /* __INT64_MAX__ */

#ifdef __UINT8_MAX__
#define UINT8_MAX __UINT8_MAX__
#endif /* __UINT8_MAX__ */
#ifdef __UINT16_MAX__
#define UINT16_MAX __UINT16_MAX__
#endif /* __UINT16_MAX__ */
#ifdef __UINT32_MAX__
#define UINT32_MAX __UINT32_MAX__
#endif /* __UINT32_MAX__ */
#ifdef __UINT64_MAX__
#define UINT64_MAX __UINT64_MAX__
#endif /* __UINT64_MAX__ */

#endif /* _STDINT_H */
