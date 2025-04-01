# Client source files

grab_cl_src = '''#!/bin/sh
for i in *.cpp; do
  echo $i
done
for i in components/*.cpp; do
  echo $i
done
for i in loader/*.cpp; do
  echo $i
done
for i in stock/*.cpp; do
  echo $i
done
'''
client_sources = files(run_command('sh', '-c', grab_cl_src, check : false).stdout().strip().split('\n'))

########### Include directories ###########

msvc_dir = run_command('sh', '-c', 'echo $MSVC_DIR', check : false).stdout().strip()
if msvc_dir == ''
  error('The MSVC_DIR environment variable is not set')
endif
client_includes = include_directories(
  '.',
  '../utils',
  '../../submodules/GSL/include',
  #'../../submodules/imgui',
  '../../submodules/minhook/include',
  msvc_dir + '/VC/Tools/MSVC/14.43.34808/include'
)

########### Dependencies ###########

### gsl ###
gsl_dep = declare_dependency(
  include_directories : include_directories('../../submodules/GSL/include')
)

### minhook ###
minhook_sources = files(
  '../../submodules/minhook/src/buffer.c',
  '../../submodules/minhook/src/hook.c',
  '../../submodules/minhook/src/trampoline.c',
  '../../submodules/minhook/src/hde/hde32.c'
)

#minhook_headers = files(
#  '../../submodules/minhook/include/MinHook.h',
#  '../../submodules/minhook/src/hde/hde32.h'
#)

minhook_lib = static_library('minhook', minhook_sources,
  include_directories : include_directories('../../submodules/minhook/include', '../../submodules/minhook/src/hde', '../../submodules/minhook/src')
)

minhook_dep = declare_dependency(
  include_directories : include_directories('../../submodules/minhook/include'),
  link_with : minhook_lib
)

### utils ###
grab_utils_src = '''#!/bin/sh
for i in ../utils/*.cpp; do
  echo $i
done
'''
utils_sources = files(run_command('sh', '-c', grab_utils_src, check : false).stdout().strip().split('\n'))

grab_utils_headers = '''#!/bin/sh
for i in ../utils/*.h; do
  echo $i
done
'''
utils_headers = files(run_command('sh', '-c', grab_utils_headers, check : false).stdout().strip().split('\n'))

utils_lib = static_library('utils', utils_sources, utils_headers,
  include_directories : include_directories('../utils'),
  dependencies : [gsl_dep, minhook_dep]
)

utils_dep = declare_dependency(
  include_directories : include_directories('../utils'),
  link_with : utils_lib
)


### imgui ###

#grab_imgui_src = '''#!/bin/sh
#for i in ../../submodules/imgui/*.cpp; do
#  echo $i
#done
#'''
#imgui_sources = files(
#  '../../submodules/imgui/backends/imgui_impl_opengl2.cpp',
#  '../../submodules/imgui/backends/imgui_impl_win32.cpp'
#) + files(run_command('sh', '-c', grab_imgui_src, check : false).stdout().strip().split('\n'))
#
#grab_imgui_headers = '''#!/bin/sh
#for i in ../../submodules/imgui/*.h; do
#  echo $i
#done
#'''
#imgui_headers = files(
#  '../../submodules/imgui/backends/imgui_impl_opengl2.h',
#  '../../submodules/imgui/backends/imgui_impl_win32.h'
#) + files(run_command('sh', '-c', grab_imgui_headers, check : false).stdout().strip().split('\n'))

#imgui_lib = static_library('imgui', imgui_sources, imgui_headers,
#  include_directories : include_directories('../../submodules/imgui')
#)

#imgui_dep = declare_dependency(
#  include_directories : include_directories('../../submodules/imgui'),
#  link_with : imgui_lib
#)

### discord-rpc ###
#discord_rpc_sources = files(
#  '../../submodules/discord-rpc/src/connection_win.cpp',
#  '../../submodules/discord-rpc/src/discord_rpc.cpp',
#  '../../submodules/discord-rpc/src/discord_register_win.cpp',
#  '../../submodules/discord-rpc/src/dllmain.cpp',
#  '../../submodules/discord-rpc/src/rpc_connection.cpp',
#  '../../submodules/discord-rpc/src/serialization.cpp'
#)

#discord_rpc_headers = files(
#  '../../submodules/discord-rpc/include/discord_rpc.h'
#)

#discord_rpc_lib = static_library('discord-rpc', discord_rpc_sources, discord_rpc_headers,
#  include_directories : include_directories('../../submodules/discord-rpc/include', '../../submodules/rapidjson/include')
#)

#discord_rpc_dep = declare_dependency(
#  include_directories : include_directories('../../submodules/discord-rpc/include', '../../submodules/rapidjson/include'),
#  link_with : discord_rpc_lib
#)

### rapidjson ###
#rapidjson_include = include_directories('../../submodules/rapidjson/include')

#rapidjson_dep = declare_dependency(
#  include_directories : rapidjson_include
#)

# resource file
windres = meson.get_external_property('windres', '')

resource = custom_target('resource',
  input : 'resource.rc',
  output : 'resource.res',
  command : [windres, '@INPUT@', '-O', 'coff', '-o', '@OUTPUT@']
)

coduo_dir = run_command('sh', '-c', 'echo $CODUO_DIR', check : false).stdout().strip()
if coduo_dir == ''
  #coduo_dir = meson.current_source_dir() + '/../../build'
  coduo_dir = '/mnt/sda8/coduo'
  warning('The `CODUO_DIR` environment variable is not set. Defaulting to ' + run_command('sh', '-c', 'echo $(realpath "' + coduo_dir + '")', check : true).stdout().strip())
endif

########### Executable ###########

executable('t1x', client_sources + resource,# client_headers, # Apparently not needed
  include_directories : client_includes,
  dependencies : [utils_dep, minhook_dep, gsl_dep],
  install_dir : coduo_dir,
  install : true,
  win_subsystem : 'windows',
  link_args : ['/DYNAMICBASE:NO', '/LAST:._text'],
  cpp_args : ['/D_MSVC_WINE']
)
