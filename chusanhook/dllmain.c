#include <windows.h>

#include <stddef.h>
#include <stdlib.h>

#include "amex/amex.h"

#include "board/sg-reader.h"
#include "board/vfd.h"

#include "chusanhook/config.h"
#include "chusanhook/io4.h"
#include "chusanhook/slider.h"

#include "chuniio/chuniio.h"

#include "hook/process.h"

#include "hooklib/gfx.h"
#include "hooklib/serial.h"
#include "hooklib/spike.h"

#include "platform/platform.h"

#include "util/dprintf.h"

static HMODULE chusan_hook_mod;
static process_entry_t chusan_startup;
static struct chusan_hook_config chusan_hook_cfg;

static DWORD CALLBACK chusan_pre_startup(void)
{
    HMODULE d3dc;
    HMODULE dbghelp;
    HRESULT hr;

    dprintf("--- Begin chusan_pre_startup ---\n");

    /* Pin the D3D shader compiler. This makes startup much faster. */

    d3dc = LoadLibraryW(L"D3DCompiler_43.dll");

    if (d3dc != NULL) {
        dprintf("Pinned shader compiler, hMod=%p\n", d3dc);
    } else {
        dprintf("Failed to load shader compiler!\n");
    }

    /* Pin dbghelp so the path hooks apply to it. */

    dbghelp = LoadLibraryW(L"dbghelp.dll");

    if (dbghelp != NULL) {
        dprintf("Pinned debug helper library, hMod=%p\n", dbghelp);
    } else {
        dprintf("Failed to load debug helper library!\n");
    }

    /* Config load */

    chusan_hook_config_load(&chusan_hook_cfg, L".\\segatools.ini");

    /* Hook Win32 APIs */

    gfx_hook_init(&chusan_hook_cfg.gfx, chusan_hook_mod);
    serial_hook_init();

    /* Initialize emulation hooks */

    hr = platform_hook_init(
        &chusan_hook_cfg.platform,
        "SDHD",
        "ACA2",
        chusan_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = chuni_dll_init(&chusan_hook_cfg.dll, chusan_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = chusan_io4_hook_init(&chusan_hook_cfg.io4);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = slider_hook_init(&chusan_hook_cfg.slider);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = sg_reader_hook_init(&chusan_hook_cfg.aime, 4, chusan_hook_mod);

    if (FAILED(hr)) {
        goto fail;
    }

    hr = vfd_hook_init(2);

    if (FAILED(hr)) {
        goto fail;
    }

    /* Initialize debug helpers */

    spike_hook_init(L".\\segatools.ini");

    dprintf("---  End  chusan_pre_startup ---\n");

    /* Jump to EXE start address */

    return chusan_startup();

fail:
    ExitProcess(EXIT_FAILURE);
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
    HRESULT hr;

    if (cause != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    chusan_hook_mod = mod;

    hr = process_hijack_startup(chusan_pre_startup, &chusan_startup);

    if (!SUCCEEDED(hr)) {
        dprintf("Failed to hijack process startup: %x\n", (int) hr);
    }

    return SUCCEEDED(hr);
}
