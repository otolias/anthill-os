#ifndef _LIMITS_H
#define _LIMITS_H

#ifndef INT_MAX
#define INT_MAX __INT_MAX__
#endif /* INT_MAX */

#ifndef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#endif /* INT_MIN */

#ifndef UINT_MAX
#define UINT_MAX (INT_MAX * 2U + 1U)
#endif /* UINT_MAX */

#ifndef LONG_MAX
#define LONG_MAX __LONG_MAX__
#endif /* LONG_MAX */

#ifndef LONG_MIN
#define LONG_MIN (-LONG_MAX - 1L)
#endif /* LONG_MIN */

#ifndef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)
#endif /* ULONG_MAX */

#ifndef LLONG_MAX
#define LLONG_MAX __LONG_LONG_MAX__
#endif /* LLONG_MAX */

#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX - 1LL)
#endif /* LLONG_MIN */

#ifndef ULLONG_MAX
#define ULLONG_MAX (LLONG_MAX * 2ULL + 1ULL)
#endif /* ULLONG_MAX */

#endif /* _LIMITS_H */
