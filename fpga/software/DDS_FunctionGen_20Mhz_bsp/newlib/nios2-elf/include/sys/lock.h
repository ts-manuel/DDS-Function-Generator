#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

#if defined(__NEWLIB_GTHR_GENERIC)

/* Use gthr-generic lock hooks.  */

#include <gthr-generic.h>

typedef __gthread_mutex_t _LOCK_T;
typedef __gthread_recursive_mutex_t _LOCK_RECURSIVE_T;
 
#define __LOCK_INIT(class,lock)				\
  class __gthread_mutex_t lock;				\
  static __attribute__ ((__constructor__ (0))) void	\
  __init_##lock (void)					\
  {							\
    __generic_gxx_mutex_init_function (&lock);		\
  }
#define __LOCK_INIT_RECURSIVE(class,lock)			\
  class __gthread_recursive_mutex_t lock;			\
  static __attribute__ ((__constructor__ (0))) void		\
  __init_##lock (void)						\
  {								\
    __generic_gxx_recursive_mutex_init_function (&lock);	\
  }
#define __lock_init(lock) (__generic_gxx_mutex_init_function (&(lock)))
#define __lock_init_recursive(lock) \
  (__generic_gxx_recursive_mutex_init_function (&(lock)))
#define __lock_close(lock) (__generic_gxx_mutex_destroy (&(lock)))
#define __lock_close_recursive(lock) \
  (__generic_gxx_recursive_mutex_destroy (&(lock)))
#define __lock_acquire(lock) (__generic_gxx_mutex_lock (&(lock)))
#define __lock_acquire_recursive(lock) \
  (__generic_gxx_recursive_mutex_lock (&(lock)))
#define __lock_try_acquire(lock) (__generic_gxx_mutex_trylock (&(lock)))
#define __lock_try_acquire_recursive(lock) \
  (__generic_gxx_recursive_mutex_trylock (&(lock)))
#define __lock_release(lock) (__generic_gxx_mutex_unlock (&(lock)))
#define __lock_release_recursive(lock) \
  (__generic_gxx_recursive_mutex_unlock (&(lock)))

#else

/* dummy lock routines for single-threaded aps */

typedef int _LOCK_T;
typedef int _LOCK_RECURSIVE_T;
 
#include <_ansi.h>

#define __LOCK_INIT(class,lock) static int lock = 0;
#define __LOCK_INIT_RECURSIVE(class,lock) static int lock = 0;
#define __lock_init(lock) (_CAST_VOID 0)
#define __lock_init_recursive(lock) (_CAST_VOID 0)
#define __lock_close(lock) (_CAST_VOID 0)
#define __lock_close_recursive(lock) (_CAST_VOID 0)
#define __lock_acquire(lock) (_CAST_VOID 0)
#define __lock_acquire_recursive(lock) (_CAST_VOID 0)
#define __lock_try_acquire(lock) (_CAST_VOID 0)
#define __lock_try_acquire_recursive(lock) (_CAST_VOID 0)
#define __lock_release(lock) (_CAST_VOID 0)
#define __lock_release_recursive(lock) (_CAST_VOID 0)

#endif

#endif /* __SYS_LOCK_H__ */
