#include "sapistuff.h"
#include <windows.h>
#include <stdio.h>
// this could be C++, but I won't bother for now
// No perl types visible here
void
do_sapi_stuff(const char *text, sapi_cb_t cb, void *ctx) {
  SEvent evt;
  int i;

  evt.event_name = "starting";
  cb(ctx, &evt);

  Sleep(500);

  for (i = 0; i < 5; ++i) {
    char buf[100];
    sprintf(buf, "event %d", i);
    evt.event_name = buf;
    cb(ctx, &evt);
    Sleep(500);
  }

  evt.event_name = "Shutting down";
  cb(ctx, &evt);
}

