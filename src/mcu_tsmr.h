#ifndef MCU_TSMR_H
#define MCU_TSMR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Simple measurement structure for TSMR (time-stamped measurement replay)
 * This is intentionally small and portable for MCU use (e.g., STM32).
 */
typedef struct {
    uint64_t ts_ms; /* measurement timestamp in milliseconds */
    int id;         /* detection unique id or -1 */
    float x, y, w, h; /* bbox normalized or pixel coords as agreed */
    float score;    /* confidence */
} tsmr_meas_t;

/* Opaque ring buffer handle */
typedef struct tsmr_ringbuf_t tsmr_ringbuf_t;

/* Create and initialize a ring buffer with given capacity.
 * Returns NULL on allocation failure.
 * The implementation uses malloc; adapt for static allocation on small MCUs.
 */
tsmr_ringbuf_t *tsmr_create(size_t capacity);

/* Free resources (or no-op if statically allocated) */
void tsmr_destroy(tsmr_ringbuf_t *rb);

/* Push a measurement into the ring buffer.
 * If buffer is full, oldest measurement is overwritten.
 */
void tsmr_push(tsmr_ringbuf_t *rb, const tsmr_meas_t *m);

/* Find a measurement by timestamp (exact match) -> returns 0 on found and copies into out. */
int tsmr_find_by_ts(tsmr_ringbuf_t *rb, uint64_t ts_ms, tsmr_meas_t *out);

/* Apply retroactive correction: update measurement that matches timestamp and id.
 * Returns 0 on success, -1 if not found.
 */
int tsmr_apply_correction(tsmr_ringbuf_t *rb, const tsmr_meas_t *corr);

/* Pop and retrieve measurements older than given timestamp (ts_ms).
 * Retrieved items are written to `out` array up to `max_out` and function
 * returns the number written.
 */
size_t tsmr_pop_older_than(tsmr_ringbuf_t *rb, uint64_t ts_ms, tsmr_meas_t *out, size_t max_out);

#ifdef __cplusplus
}
#endif

#endif /* MCU_TSMR_H */
