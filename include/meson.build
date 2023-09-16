configure_file(
    input: 'config.h.in',
    output: 'config.h',
    configuration: {
        'demo_datadir': get_option('prefix') / get_option('datadir') / 'aperture_demo',
        'project_version': meson.project_version(),
        'project_name': meson.project_name(),
    },
)