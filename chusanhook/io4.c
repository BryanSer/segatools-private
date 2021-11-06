#include <windows.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "board/io4.h"

#include "chusanhook/chuni-dll.h"
#include "util/dprintf.h"

struct chunithm_jvs_ir_mask {
    uint16_t p1;
    uint16_t p2;
};

// Incorrect IR beam mappings retained for backward compatibility
static const struct chunithm_jvs_ir_mask chunithm_jvs_ir_masks_v1[] = {
    { 0, 1 << 13 },
    { 1 << 13, 0 },
    { 0, 1 << 12 },
    { 1 << 12, 0 },
    { 0, 1 << 11 },
    { 1 << 11, 0 },
};

static const struct chunithm_jvs_ir_mask chunithm_jvs_ir_masks[] = {
    { 1 << 13, 0 },
    { 0, 1 << 13 },
    { 1 << 12, 0 },
    { 0, 1 << 12 },
    { 1 << 11, 0 },
    { 0, 1 << 11 },
};

static HRESULT chusan_io4_poll(void* ctx, struct io4_state* state);

static const struct io4_ops chusan_io4_ops = {
    .poll = chusan_io4_poll,
};

HRESULT chusan_io4_hook_init(const struct io4_config* cfg)
{
    HRESULT hr;

    assert(chuni_dll.jvs_init != NULL);

    dprintf("USB I/O: Starting IO backend\n");
    hr = chuni_dll.jvs_init();

    if (FAILED(hr)) {
        dprintf("USB I/O: Backend error, I/O disconnected: %x\n", (int)hr);

        return hr;
    }

    io4_hook_init(cfg, &chusan_io4_ops, NULL);

    return S_OK;
}

static HRESULT chusan_io4_poll(void* ctx, struct io4_state* state)
{
    uint8_t opbtn;
    uint8_t beams;
    size_t i;

    memset(state, 0, sizeof(*state));

    opbtn = 0;
    beams = 0;

    chuni_dll.jvs_poll(&opbtn, &beams);

    if (opbtn & 0x01) {
        state->buttons[0] |= IO4_BUTTON_TEST;
    }

    if (opbtn & 0x02) {
        state->buttons[0] |= IO4_BUTTON_SERVICE;
    }

    for (i = 0; i < 6; i++) {
        /* Beam "press" is active-low hence the ~ */
        if (~beams & (1 << i)) {
            state->buttons[0] |= chunithm_jvs_ir_masks[i].p1;
            state->buttons[1] |= chunithm_jvs_ir_masks[i].p2;
        }
    }

    return S_OK;
}
