
/*
 * Initialization.
 */
static void
PyThread__init_thread(void)
{
}

/*
 * Thread support.
 */
unsigned long
PyThread_start_new_thread(void (*func)(void *), void *arg)
{
    int success = 0;            /* init not needed when SOLARIS_THREADS and */
                /* C_THREADS implemented properly */

    dprintf(("PyThread_start_new_thread called\n"));
    if (!initialized)
        PyThread_init_thread();
    return success < 0 ? -1 : 0;
}

unsigned long
PyThread_get_thread_ident(void)
{
    if (!initialized)
        PyThread_init_thread();

    // there can be only one: https://open.spotify.com/track/78UcxP3Xm0EOXneQUDsStA?si=geoeQYBJTrCRINNvrCd-6w
    return 1;
}

void
PyThread_exit_thread(void)
{
    dprintf(("PyThread_exit_thread called\n"));
    if (!initialized)
        exit(0);
}

/*
 * Lock support.
 */
PyThread_type_lock
PyThread_allocate_lock(void)
{
    // very lock, much thread-safe
    static int current_lock = 0;

    dprintf(("PyThread_allocate_lock called\n"));
    if (!initialized)
        PyThread_init_thread();

    PyThread_type_lock lock = ++current_lock;
    dprintf(("PyThread_allocate_lock() -> %p\n", lock));
    return lock;
}

void
PyThread_free_lock(PyThread_type_lock lock)
{
    dprintf(("PyThread_free_lock(%p) called\n", lock));
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
    dprintf(("PyThread_acquire_lock_timed(%p, %lld, %d) -> %d\n",
	     lock, microseconds, intr_flag, PY_LOCK_ACQUIRED));
    return PY_LOCK_ACQUIRED;
}

void
PyThread_release_lock(PyThread_type_lock lock)
{
    dprintf(("PyThread_release_lock(%p) called\n", lock));
}

/* Thread Specific Storage (TSS) API

   Platform-specific components of TSS API implementation.
*/

int
PyThread_tss_create(Py_tss_t *key)
{
    return 0;
}

void
PyThread_tss_delete(Py_tss_t *key)
{
    return;
}

int
PyThread_tss_set(Py_tss_t *key, void *value)
{
    return 0;
}

void *
PyThread_tss_get(Py_tss_t *key)
{
    assert(key != NULL);
    return NULL;
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
