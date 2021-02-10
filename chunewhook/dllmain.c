#include <windows.h>

#include <stddef.h>
#include <stdlib.h>

#include "amex/amex.h"

#include "board/sg-reader.h"
#include "board/vfd.h"

#include "chunewhook/config.h"
#include "chunewhook/io4.h"
#include "chunewhook/slider.h"

#include "chuniio/chuniio.h"

#include "hook/process.h"

#include "hooklib/gfx.h"
#include "hooklib/serial.h"
#include "hooklib/spike.h"

#include "platform/platform.h"

#include "util/dprintf.h"

static HMODULE chunew_hook_mod;
static process_entry_t chunew_startup;
static struct chunew_hook_config chunew_hook_cfg;

static DWORD CALLBACK chunew_pre_startup(void)
{
    HMODULE d3dc;
    HRESULT hr;

    dprintf("--- Begin chunew_pre_startup ---\n");

    /* Pin the D3D shader compiler. This makes startup much faster. */

    d3dc = LoadLibraryW(L"D3DCompiler_43.dll");

    if (d3dc != NULL) {
        dprintf("Pinned shader compiler, hMod=%p\n", d3dc);
    }
    else {
        dprintf("Failed to load shader compiler!\n");
    }

    /* Config load */

    chunew_hook_config_load(&chunew_hook_cfg, L".\\segatools.ini");

    /* Hook Win32 APIs */

    gfx_hook_init(&chunew_hook_cfg.gfx);
    serial_hook_init();

    /* Initialize emulation hooks */

    hr = platform_hook_init(
        &chunew_hook_cfg.platform,
        "SDGS",
        "ACA2",
        chunew_hook_mod);

    if (FAILED(hr)) {
        return hr;
    }

    hr = sg_reader_hook_init(&chunew_hook_cfg.aime, 4);

    if (FAILED(hr)) {
        return hr;
    }

    hr = vfd_hook_init(2);

    if (FAILED(hr)) {
        return hr;
    }

    hr = chunew_io4_hook_init(&chunew_hook_cfg.io4);

    if (FAILED(hr)) {
        return hr;
    }

    hr = slider_hook_init(&chunew_hook_cfg.slider);

    if (FAILED(hr)) {
        return hr;
    }

    /* Initialize debug helpers */

    spike_hook_init(L".\\segatools.ini");

    dprintf("---  End  chunew_pre_startup ---\n");

    /* Jump to EXE start address */

    return chunew_startup();
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void* ctx)
{
    HRESULT hr;

    if (cause != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    chunew_hook_mod = mod;

    hr = process_hijack_startup(chunew_pre_startup, &chunew_startup);

    if (!SUCCEEDED(hr)) {
        dprintf("Failed to hijack process startup: %x\n", (int)hr);
    }

    return SUCCEEDED(hr);
}
