shared_library(
    'chunewhook',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,
    c_pch : '../precompiled.h',
    dependencies : [
        capnhook.get_variable('hook_dep'),
        capnhook.get_variable('hooklib_dep'),
    ],
    link_with : [
        aimeio_dll,
        board_lib,
        chuniio_dll,
        hooklib_lib,
        platform_lib,
        util_lib,
    ],
    sources : [
        'config.c',
        'config.h',
        'dllmain.c',
        'io4.c',
        'io4.h',
        'slider.c',
        'slider.h',
    ],
)
