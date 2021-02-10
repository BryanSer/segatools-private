#pragma once

#include <stddef.h>

#include "board/config.h"

#include "hooklib/gfx.h"

#include "platform/config.h"

#include "chunewhook/slider.h"

struct chunew_hook_config {
    struct platform_config platform;
    struct aime_config aime;
    struct io4_config io4;
    struct gfx_config gfx;
    struct slider_config slider;
};

void slider_config_load(struct slider_config* cfg, const wchar_t* filename);
void chunew_hook_config_load(
        struct chunew_hook_config *cfg,
        const wchar_t *filename);
