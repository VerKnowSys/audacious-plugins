#ifndef XS_SIDPLAYFP_H
#define XS_SIDPLAYFP_H

#include "xmms-sid.h"

#include <stdint.h>

bool xs_sidplayfp_probe(const void *buf, int64_t bufSize);
void xs_sidplayfp_close();
bool xs_sidplayfp_init();
bool xs_sidplayfp_initsong(int subtune);
unsigned xs_sidplayfp_fillbuffer(char *, unsigned);
bool xs_sidplayfp_load(const void *buf, int64_t bufSize);
xs_tuneinfo_t *xs_sidplayfp_getinfo(const char *, const void *buf, int64_t bufSize);
bool xs_sidplayfp_updateinfo(xs_tuneinfo_t *i, int subtune);

#endif /* XS_SIDPLAYFP_H */
