#include <wasi_python_shims.h>

/*
 * Initialization.
 */
static void
PyThread__init_thread(void)
{
  wt_init_thread();
}

/*
 * Thread support.
 */
unsigned long
PyThread_start_new_thread(void (*func)(void *), void *arg)
{
    dprintf(("PyThread_start_new_thread called\n"));
    if (!initialized)
        PyThread_init_thread();

    return wt_start_new_thread(func, arg);
}

unsigned long
PyThread_get_thread_ident(void)
{
    if (!initialized)
        PyThread_init_thread();

    return wt_get_thread_ident();
}

void
PyThread_exit_thread(void)
{
    dprintf(("PyThread_exit_thread called\n"));
    if (!initialized)
        exit(0);

    wt_exit_thread();
}

/*
 * Lock support.
 */
PyThread_type_lock
PyThread_allocate_lock(void)
{
    dprintf(("PyThread_allocate_lock called\n"));
    if (!initialized)
        PyThread_init_thread();

    WasiThreadLock *lock = wt_allocate_lock();
    dprintf(("PyThread_allocate_lock() -> %p\n", lock));
    return (PyThread_type_lock)lock;
}

void
PyThread_free_lock(PyThread_type_lock lock)
{
    dprintf(("PyThread_free_lock(%p) called\n", lock));
    wt_free_lock((WasiThreadLock *) lock);
}

int
PyThread_acquire_lock(PyThread_type_lock lock, int waitflag)
{
    return PyThread_acquire_lock_timed(lock, waitflag ? -1 : 0, 0);
}

PyLockStatus
PyThread_acquire_lock_timed(PyThread_type_lock lock, PY_TIMEOUT_T microseconds,
                            int intr_flag)
{
    dprintf(("PyThread_acquire_lock_timed(%p, %lld, %d) called\n", lock, microseconds, intr_flag));

    PyLockStatus status = PY_LOCK_FAILURE;
    if (wt_acquire_lock((WasiThreadLock *) lock)) {
      status = PY_LOCK_ACQUIRED;
    }

    dprintf(("PyThread_acquire_lock_timed(%p, %lld, %d) -> %d\n",
	     lock, microseconds, intr_flag, PY_LOCK_ACQUIRED));


    return PY_LOCK_ACQUIRED;
}

void
PyThread_release_lock(PyThread_type_lock lock)
{
    dprintf(("PyThread_release_lock(%p) called\n", lock));
    wt_release_lock((WasiThreadLock *) lock);
}

/* thread Specific Storage (TSS) API

   Platform-specific components of TSS API implementation.
*/

int
PyThread_tss_create(Py_tss_t *key)
{
  return wt_tss_create(key->_key) ? 0 : 1;
}

void
PyThread_tss_delete(Py_tss_t *key)
{
    if(!wt_tss_delete(key->_key))
      dprintf(("PyThread_tss_delete failed to delete key: %p", key->_key));
}

int
PyThread_tss_set(Py_tss_t *key, void *value)
{
  return wt_tss_set(key->_key, value) ? 0 : 1;
}

void *
PyThread_tss_get(Py_tss_t *key)
{
  assert(key != NULL);
  return wt_tss_get(key->_key);
}

/* The following are only needed if native TLS support exists */
/*
#define Py_HAVE_NATIVE_TLS

#ifdef Py_HAVE_NATIVE_TLS
int
PyThread_create_key(void)
{
    int result = 0;
    return result;
}

void
PyThread_delete_key(int key)
{

}

int
PyThread_set_key_value(int key, void *value)
{
    int ok;

    // A failure in this case returns -1
    if (!ok)
        return -1;
    return 0;
}

void *
PyThread_get_key_value(int key)
{
    void *result;

    return result;
}

void
PyThread_delete_key_value(int key)
{

}

void
PyThread_ReInitTLS(void)
{

}

#endif
*/
