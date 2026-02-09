#include "mcu_tsmr.h"
#include <stdlib.h>
#include <string.h>

struct tsmr_ringbuf_t {
    tsmr_meas_t *buf;
    size_t capacity;
    size_t head; /* index of next write */
    size_t len;  /* number of stored elements */
};

tsmr_ringbuf_t *tsmr_create(size_t capacity) {
    if (capacity == 0) return NULL;
    tsmr_ringbuf_t *rb = (tsmr_ringbuf_t *)malloc(sizeof(tsmr_ringbuf_t));
    if (!rb) return NULL;
    rb->buf = (tsmr_meas_t *)malloc(sizeof(tsmr_meas_t) * capacity);
    if (!rb->buf) { free(rb); return NULL; }
    rb->capacity = capacity;
    rb->head = 0;
    rb->len = 0;
    return rb;
}

void tsmr_destroy(tsmr_ringbuf_t *rb) {
    if (!rb) return;
    if (rb->buf) free(rb->buf);
    free(rb);
}

void tsmr_push(tsmr_ringbuf_t *rb, const tsmr_meas_t *m) {
    if (!rb || !m) return;
    rb->buf[rb->head] = *m;
    rb->head = (rb->head + 1) % rb->capacity;
    if (rb->len < rb->capacity) rb->len++;
}

int tsmr_find_by_ts(tsmr_ringbuf_t *rb, uint64_t ts_ms, tsmr_meas_t *out) {
    if (!rb || rb->len == 0) return -1;
    size_t start = (rb->head + rb->capacity - rb->len) % rb->capacity;
    for (size_t i = 0; i < rb->len; ++i) {
        size_t idx = (start + i) % rb->capacity;
        if (rb->buf[idx].ts_ms == ts_ms) {
            if (out) *out = rb->buf[idx];
            return 0;
        }
    }
    return -1;
}

int tsmr_apply_correction(tsmr_ringbuf_t *rb, const tsmr_meas_t *corr) {
    if (!rb || rb->len == 0 || !corr) return -1;
    size_t start = (rb->head + rb->capacity - rb->len) % rb->capacity;
    for (size_t i = 0; i < rb->len; ++i) {
        size_t idx = (start + i) % rb->capacity;
        if (rb->buf[idx].ts_ms == corr->ts_ms && rb->buf[idx].id == corr->id) {
            rb->buf[idx] = *corr;
            return 0;
        }
    }
    return -1;
}

size_t tsmr_pop_older_than(tsmr_ringbuf_t *rb, uint64_t ts_ms, tsmr_meas_t *out, size_t max_out) {
    if (!rb || rb->len == 0 || max_out == 0) return 0;
    size_t count = 0;
    size_t start = (rb->head + rb->capacity - rb->len) % rb->capacity;
    size_t i = 0;
    while (i < rb->len && count < max_out) {
        size_t idx = (start + i) % rb->capacity;
        if (rb->buf[idx].ts_ms < ts_ms) {
            if (out) out[count] = rb->buf[idx];
            count++;
            i++;
        } else {
            break;
        }
    }
    /* remove popped elements from buffer (advance start by count) */
    if (count > 0) {
        rb->len -= count;
        /* head remains same; effectively start advanced */
        /* No data copy needed because we keep circular indices */
    }
    return count;
}
