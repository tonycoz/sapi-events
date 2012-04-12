#ifdef __cplusplus
extern "C" {
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "sapistuff.h"

static void
my_cb(void *ctx, const SEvent *event) {
  dTHX;
  dSP;

  ENTER;
  SAVETMPS;
  EXTEND(SP, 1);
  PUSHMARK(SP);
  PUSHs(sv_2mortal(newSVpv(event->event_name, strlen(event->event_name))));
  PUTBACK;

  perl_call_sv(ctx, G_VOID);

  FREETMPS;
  LEAVE;
}

static void
SE_DoStuff(const char *text, CV *foo) {
  /* as an example we isolate the sapi from perl, providing a callback 
     function and a context pointer */
  do_sapi_stuff(text, my_cb, foo);
}

MODULE = SAPI::Events	PACKAGE = SAPI::Events	PREFIX = SE_

void
SE_DoStuff(const char *text, CV *foo)
