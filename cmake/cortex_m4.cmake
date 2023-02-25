

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
set(ARM_OPTIONS -mcpu=cortex-m4 -mfloat-abi=soft --specs=nano.specs)

add_compile_options(
  ${ARM_OPTIONS}
  -fmessage-length=0
  -funsigned-char
  -ffunction-sections
  -fdata-sections
  -MMD
  -MP
)