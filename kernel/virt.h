#ifndef VIRT_H
#define VIRT_H

#include "error.h"

error_t
virt_alloc(void** out_page);

void
virt_free(void* page);

#endif
