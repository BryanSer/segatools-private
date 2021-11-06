#pragma once

#include <stddef.h>

#include "board/config.h"

#include "hooklib/gfx.h"

#include "platform/config.h"

#include "chusanhook/chuni-dll.h"
#include "chusanhook/slider.h"

struct chusan_hook_config {
    struct platform_config platform;
    struct aime_config aime;
    struct io4_config io4;
    struct gfx_config gfx;
    struct chuni_dll_config dll;
    struct slider_config slider;
};

void chuni_dll_config_load(
        struct chuni_dll_config *cfg,
        const wchar_t *filename);
void slider_config_load(struct slider_config *cfg, const wchar_t *filename);
void chusan_hook_config_load(
        struct chusan_hook_config *cfg,
        const wchar_t *filename);
