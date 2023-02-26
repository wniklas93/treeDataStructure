

set(TOOLCHAIN_GCCARM_PATH "/home/niklas/Desktop/Projects/gccArm/gcc-arm-none-eabi-10.3-2021.10/bin")

# Target operating system
set(CMAKE_SYSTEM_NAME Generic CACHE STRING "Target system") # Sets "CMAKE_CROSSCOMPILING" to true

# Specify toolchain
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_GCCARM_PATH}/arm-none-eabi-g++ CACHE PATH armgcc_c++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_GCCARM_PATH}/arm-none-eabi-gcc CACHE PATH armgcc_c)
set(CMAKE_STRIP ${TOOLCHAIN_GCCARM_PATH}/arm-none-eabi-strip CACHE PATH armgcc_strip)
set(CMAKE_AR ${TOOLCHAIN_GCCARM_PATH}/arm-none-eabi-ar CACHE PATH armgcc_ar)

# Try building a static library (Note: We can't run a corss-compiled executable,
# an alternative would be to skip compiler tests)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Specify cross compiler options

set(RELEASE_OPTIONS "-O3")
set(DEBUG_OPTIONS "-g") # -Og                                   

add_compile_options(
  ### ARM Options ###
  -mcpu=cortex-m4                         # Target ARM processor
  -mfloat-abi=hard                        # Enable full hardware floating-point support
  -mfpu=fpv4-sp-d16                       # Single-precision only variant of the VFPv4-D16 extension
  --specs=nano.specs                      # Enable newlib-nano --> Code and data size reduction  
  -mthumb                                 # Instructions set
  ### GCC Options ###
  -ffunction-sections                     # Functions and variables are mapped to sections
  -fdata-sections                         # Linker discards unused funtions (--gc-sections)
  -std=c++20                              # C++ version
  -fno-rtti                               # Disable generation of runtime information
  -fno-exceptions                         # Disable exceptions
  -flto                                   # Enable linker code optimization
#  --gc-sections                           # Linker discards unreferenced sections
  $<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>
  $<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>
)