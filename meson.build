project(
    'Aperture-Demo',
    'c',
    default_options: [
        'c_std=c11',
    ],
    license: 'Apache-2.0',
    version: '0.3.0',
)

cc = meson.get_compiler('c')

subdir('include')

executable(
    'aperture_demo',
    sources: files(
        'src' / 'database.c',
        'src' / 'light.c',
        'src' / 'main.c',
        'src' / 'utils.c',
    ),
    dependencies: [
        dependency('glfw3'),
        cc.find_library('aperture'),
        cc.find_library('m'),
    ],
    include_directories: include_directories(
        'include',
        '../include',
        '../',
    ),
    install: true
)

meson.add_install_script('scripts/install_demo_data.sh')
