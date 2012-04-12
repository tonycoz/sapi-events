#ifndef SAPISTUFF_H_
#define SAPISTUFF_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /* what ever you want to forward, presumably it will be more than this */
  const char *event_name;
} SEvent;

typedef void (*sapi_cb_t)(void *ctx, const SEvent *event);

extern void do_sapi_stuff(const char *text, sapi_cb_t cb, void *ctx);

#ifdef __cplusplus
}
#endif

#endif
