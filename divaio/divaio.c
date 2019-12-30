#include <windows.h>

#include <process.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "divaio/divaio.h"

static unsigned int __stdcall diva_io_slider_thread_proc(void *ctx);

static bool diva_io_coin;
static uint16_t diva_io_coins;
static HANDLE diva_io_slider_thread;
static bool diva_io_slider_stop_flag;

HRESULT diva_io_jvs_init(void)
{
    return S_OK;
}

void diva_io_jvs_poll(uint8_t *opbtn_out, uint8_t *gamebtn_out)
{
    static const int opbtn_vk[] = { '1', '2' };
    static const int gamebtn_vk[] = { 'L', 'J', 'F', 'S', ' ' };

    uint8_t opbtn;
    uint8_t gamebtn;
    size_t i;

    opbtn = 0;
    gamebtn = 0;

    for (i = 0 ; i < _countof(opbtn_vk) ; i++) {
        if (GetAsyncKeyState(opbtn_vk[i]) & 0x8000) {
            opbtn |= 1 << i;
        }
    }

    for (i = 0 ; i < _countof(gamebtn_vk) ; i++) {
        if (GetAsyncKeyState(gamebtn_vk[i]) & 0x8000) {
            gamebtn |= 1 << i;
        }
    }

    *opbtn_out = opbtn;
    *gamebtn_out = gamebtn;
}

void diva_io_jvs_read_coin_counter(uint16_t *out)
{
    if (out == NULL) {
        return;
    }

    if (GetAsyncKeyState('3')) {
        if (!diva_io_coin) {
            diva_io_coin = true;
            diva_io_coins++;
        }
    } else {
        diva_io_coin = false;
    }

    *out = diva_io_coins;
}

void diva_io_jvs_set_coin_blocker(bool open)
{}

HRESULT diva_io_slider_init(void)
{
    return S_OK;
}

void diva_io_slider_start(diva_io_slider_callback_t callback)
{
    if (diva_io_slider_thread != NULL) {
        return;
    }

    diva_io_slider_thread = (HANDLE) _beginthreadex(
            NULL,
            0,
            diva_io_slider_thread_proc,
            callback,
            0,
            NULL);
}

void diva_io_slider_stop(void)
{
    diva_io_slider_stop_flag = true;

    WaitForSingleObject(diva_io_slider_thread, INFINITE);
    CloseHandle(diva_io_slider_thread);
    diva_io_slider_thread = NULL;
    diva_io_slider_stop_flag = false;
}

void diva_io_slider_set_leds(const uint8_t *rgb)
{}

static unsigned int __stdcall diva_io_slider_thread_proc(void *ctx)
{
    static const int keys[] = {
        'Q', 'W', 'E', 'R', 'U', 'I', 'O', 'P',
    };

    diva_io_slider_callback_t callback;
    uint8_t pressure_val;
    uint8_t pressure[32];
    size_t i;

    callback = ctx;

    while (!diva_io_slider_stop_flag) {
        for (i = 0 ; i < 8 ; i++) {
            if(GetAsyncKeyState(keys[i]) & 0x8000) {
                pressure_val = 20;
            } else {
                pressure_val = 0;
            }

            memset(&pressure[4 * i], pressure_val, 4);
        }

        callback(pressure);
        Sleep(1);
    }

    return 0;
}
