#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

struct Context {
    struct Context *prev;
    const char *functionName;
    size_t errorTypes;
    size_t serial;
    struct Context *next;
};

size_t Context_getSerial(struct Context *c);
void Context_setErrorTypes(struct Context *c, size_t types);

size_t
Context_getSerial(struct Context *c)
{
    return c->serial;
}

void
Context_setErrorTypes(struct Context *c, size_t types)
{
    c->errorTypes = types;
}

static struct Context *top;
static struct Context *bottom;
static struct Context *current;

struct Context * Drill_getContext(const char *functionName);
void Drill_initialize(void);
void Drill_first(void);
void Drill_next(void);
int Drill_hasNext(void);

struct Context *
Drill_getContext(const char *functionName)
{
    if (bottom == NULL || current == NULL) {
        struct Context *p = malloc(sizeof(*p));
        assert(p != NULL);
        p->prev = bottom;
        p->functionName = functionName;
        p->errorTypes = 0;
        p->serial = 0;
        p->next = NULL;
        bottom = p;
        return p;
    }
    struct Context *p = current;
    current = current->next;
    return p;
}

void
Drill_initialize(void)
{
    top = NULL;
    bottom = NULL;
    current = NULL;
}

int
Drill_hasNext(void)
{
    if (bottom == NULL) {
        return 0;
    }
    if (top == NULL) {
        struct Context *p = bottom;
        while (p->prev != NULL) {
            struct Context *prev = p->prev;
            assert(p->errorTypes > 0);
            prev->next = p;
            p = prev;
        }
        top = p;
        current = top;
        ++(bottom->serial);
        return 1;
    }
    if (current == NULL) {
        struct Context *p = bottom;
        struct Context *prev = p->prev;
        while (prev != NULL && prev->next == NULL) {
            assert(p->errorTypes > 0);
            prev->next = p;
            p = prev;
            prev = p->prev;
        }
    }
    current = top;
    for (;;) {
        if (bottom->serial < bottom->errorTypes) {
            current = top;
            ++(bottom->serial);
            return 1;
        }
        struct Context *p = bottom->prev;
        free(bottom);
        bottom = p;
        if (bottom == NULL) {
            return 0;
        }
        bottom->next = NULL;
    }
}

void * W_malloc(size_t size);

void *
W_malloc(size_t size)
{
    struct Context *c;
    int s;

    c = Drill_getContext(__func__);
    s = Context_getSerial(c);
    if (s == 1) {
        errno = ENOMEM;
        return NULL;
    }
    assert(s == 0);
    Context_setErrorTypes(c, (size_t)1);
    void *r = malloc(size);
    assert(r != NULL);
    return r;
}

static void *
Foo_bar(void)
{
    void *p;
    void *q;
    void **array;

    warnx("1");
    if ((p = W_malloc((size_t)100)) == NULL) {
        warnx("1-1");
        return NULL;
    }
    warnx("2");
    if ((q = W_malloc((size_t)100)) == NULL) {
        warnx("2-1");
        if ((q = W_malloc((size_t)10)) == NULL) {
            warnx("2-1-1");
            free(p);
            return NULL;
        }
    }
    warnx("3");
    if ((array = W_malloc(sizeof(void *) * 2)) == NULL) {
        warnx("3-1");
        free(p);
        free(q);
        return NULL;
    }
    warnx("4");
    array[0] = p;
    array[1] = q;
    return array;
}

static void **
Foo_baz(size_t size, size_t len)
{
    void **array;

    if  ((array = W_malloc(sizeof(void *) * size)) == NULL) {
        return NULL;
    }
    for (size_t k = 0; k < size; ++k) {
        if ((array[k] = W_malloc(len)) == NULL) {
            for (size_t j = 0; j < k; ++j) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }
    return array;
}

int
main(void)
{
    Drill_initialize();
    assert(Foo_bar() != NULL);
    while (Drill_hasNext()) {
        (void)Foo_bar();
    }

    Drill_initialize();
    assert(Foo_baz((size_t)5, (size_t)16) != NULL);
    while (Drill_hasNext()) {
        (void)Foo_baz((size_t)5, (size_t)16);
    }

    Drill_initialize();
    printf("a\n");
    assert(!Drill_hasNext());

    exit(0);
}
