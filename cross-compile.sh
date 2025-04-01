#!/bin/bash
set -e

# Prettified by ChatGPT

# Color variables
RED='\e[31m'
GREEN='\e[32m'
YELLOW='\e[33m'
BLUE='\e[34m'
MAGENTA='\e[35m'
CYAN='\e[36m'
NC='\e[0m'       # No Color
BOLD='\e[1m'

# Functions for pretty output
print_header() {
  echo -e "\n${BOLD}${CYAN}========== $1 ==========${NC}\n"
}

print_info() {
  echo -e "${YELLOW}[INFO]${NC} $1"
}

print_success() {
  echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
  echo -e "${RED}[ERROR]${NC} $1"
}

# Process command-line arguments
print_header "Processing Arguments"
while [[ $# -gt 0 ]]; do
    arg="$1"
    case $arg in
        -d | --debug)
            debug=true # debug build without optimization
            print_info "Debug mode enabled."
            ;;
        *)
            unrecognized_arg=$arg
            break
            ;;
    esac
    shift
done

if [ -v unrecognized_arg ]; then
    print_error "Unrecognized argument \"$unrecognized_arg\", aborting."
    exit 1
fi

# Update submodules
print_header "Updating Git Submodules"
#git submodule update --init --recursive

# Check for MSVC_DIR
print_header "Checking Environment Variables"
if [ -z "$MSVC_DIR" ]; then
  print_error "The MSVC_DIR environment variable is not set."
  cat <<EOF
Please set it to your msvc-wine installation directory.

More info at: https://github.com/mstorsjo/msvc-wine/blob/49ae4b63f350770584469317e3995d100d4dcd39/README.md?plain=1#L28

Example:
  export MSVC_DIR="path/to/msvc-wine/install"
EOF
  exit 1
fi
print_success "MSVC_DIR is set to: $MSVC_DIR"

export PATH="$MSVC_DIR/bin/x86:$PATH"

# Print DLL instructions
print_header "DLL Instructions"
echo -e "${BOLD}${YELLOW}NOTE:${NC}${MAGENTA} When compiling manually, if meson reports that executables created by cl.exe are not runnable, please copy the following DLLs:${NC}"
echo -e "  - ${BLUE}$MSVC_DIR/Windows Kits/10/bin/10.0.22621.0/x86/ucrt/ucrtbased.dll${NC}"
echo -e "  - ${BLUE}$MSVC_DIR/VC/Redist/MSVC/14.42.34433/debug_nonredist/x86/Microsoft.VC143.DebugCRT/vcruntime140d.dll${NC}"
echo -e "to ${BOLD}${BLUE}build/meson/meson-private${NC}."
echo -e "${MAGENTA}This step is for testing the compiler.${NC}"

mkdir -p build/meson/meson-private
cp -rf "$MSVC_DIR/Windows Kits/10/bin/10.0.22621.0/x86/ucrt/ucrtbased.dll" build/meson/meson-private
cp -rf "$MSVC_DIR/VC/Redist/MSVC/14.42.34433/debug_nonredist/x86/Microsoft.VC143.DebugCRT/vcruntime140d.dll" build/meson/meson-private

echo " "

# Meson setup
print_header "Configuring Build"
if [ -v debug ]; then
  meson setup build/meson --cross-file wine-cross.txt
  print_info "Debug build selected. Note: Debug builds can be compiled but may not run under Wine."
else
  meson setup build/meson --cross-file wine-cross.txt --buildtype=release
fi

# Ninja build
print_header "Building with Ninja"
ninja -C build/meson

echo -e "${BOLD}Compilation Complete.${NC}"

# Determine install directory
if [ -z "$CODUO_DIR" ]; then
  #export CODUO_DIR=$(realpath ./build)
  export CODUO_DIR="/mnt/sda8/coduo"
fi

# Ask to install
print_header "Installation"
echo -e "Do you wish to install t1x-client to ${BLUE}$CODUO_DIR${NC}? [Y/n]"
read install
if [[ $install == [Yy]* ]]; then
  ninja -C build/meson install
  print_success "Installation complete."
  echo -e "${BOLD}Installed to:${NC} ${BLUE}$CODUO_DIR${NC}"
  # Copy it to build folder
  cp -rf build/meson/src/client/t1x.exe build/
else
  print_info "Skipping installation."
fi

print_success "All done, bye."
