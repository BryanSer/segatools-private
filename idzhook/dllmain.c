#include <windows.h>

#include <stddef.h>
#include <stdlib.h>

#include "amex/amex.h"

#include "board/sg-reader.h"

#include "hook/process.h"

#include "hooklib/gfx.h"
#include "hooklib/serial.h"
#include "hooklib/spike.h"

#include "idzhook/config.h"
#include "idzhook/jvs.h"
#include "idzhook/zinput.h"

#include "platform/platform.h"

#include "util/dprintf.h"

static HMODULE idz_hook_mod;
static process_entry_t idz_startup;
static struct idz_hook_config idz_hook_cfg;

static DWORD CALLBACK idz_pre_startup(void)
{
    HRESULT hr;

    dprintf("--- Begin idz_pre_startup ---\n");

    /* Config load */

    idz_hook_config_load(&idz_hook_cfg, L".\\segatools.ini");

    /* Hook Win32 APIs */

    serial_hook_init();
    zinput_hook_init(&idz_hook_cfg.zinput);

    /* Initialize emulation hooks */

    hr = platform_hook_init(
            &idz_hook_cfg.platform,
            "SDDF",
            "AAV2",
            idz_hook_mod);

    if (FAILED(hr)) {
        return hr;
    }

    hr = amex_hook_init(&idz_hook_cfg.amex, idz_jvs_init);

    if (FAILED(hr)) {
        return hr;
    }

    hr = sg_reader_hook_init(&idz_hook_cfg.aime, 10);

    if (FAILED(hr)) {
        return hr;
    }

    /* Initialize debug helpers */

    spike_hook_init(L".\\segatools.ini");

    dprintf("---  End  idz_pre_startup ---\n");

    /* Jump to EXE start address */

    return idz_startup();
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
    HRESULT hr;

    if (cause != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    idz_hook_mod = mod;

    hr = process_hijack_startup(idz_pre_startup, &idz_startup);

    if (!SUCCEEDED(hr)) {
        dprintf("Failed to hijack process startup: %x\n", (int) hr);
    }

    return SUCCEEDED(hr);
}
