cmake_minimum_required(VERSION 3.11)
# Project name
project("IBB MCU Firmware")

# Product filename
set(PRODUCT_NAME IBBE)

# Version number (derived from changeset)
execute_process(
    COMMAND bash -c "hg sum | grep parent | cut -d : -f 2 | xargs"
    OUTPUT_VARIABLE VERSION_N3
)
execute_process(
    COMMAND bash -c "echo $((${VERSION_N3}/256))"
    OUTPUT_VARIABLE VERSION_N2
)
string(STRIP ${VERSION_N2} VERSION_N2)
message(STATUS "VERSION_N2 = '${VERSION_N2}'")
execute_process(
    COMMAND bash -c "echo $((${VERSION_N3}-(${VERSION_N2}*256)))"
    OUTPUT_VARIABLE VERSION_N3
)
string(STRIP ${VERSION_N3} VERSION_N3)
message(STATUS "VERSION_N3 = '${VERSION_N3}'")

## AVR Chip Configuration
# 8MHz crystal
set(F_CPU 16000000UL)
# CPU
set(MCU atmega1280)
# Default Baudrate for UART
set(BAUD 38400)
# Programmer
set(PROG_TYPE jtag1)

# AVR fuses according to hardware and F_CPU
# # https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega128a
# set(E_FUSE 0xfd)
set(E_FUSE 0xxx)
# set(H_FUSE 0x99)
set(H_FUSE 0xxx)
# set(L_FUSE 0xe4)
set(L_FUSE 0xxx)
set(LOCK_BIT 0xff)

# Use AVR GCC toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)

# Pass defines to compiler
add_definitions(
   #  -DF_CPU=${F_CPU}
   #  -DBAUD=${BAUD}
   -DVERSION_N2=${VERSION_N2}
   -DVERSION_N3=${VERSION_N3}
)

set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU})

add_compile_options(
    -mmcu=${MCU} # MCU
    -std=gnu99 # C99 standard
    -Os # optimize
    -Wall # enable warnings
    -Wno-main
    -Wundef
    -pedantic
   #  -Wstrict-prototypes
   #  -Werror
    -Wfatal-errors
    -Wl,--relax,--gc-sections
    # -g
    # -gdwarf-2
    -funsigned-char # a few optimizations
    -funsigned-bitfields
    -fpack-struct
    -fshort-enums
    -ffunction-sections
    -fdata-sections
    -fno-split-wide-types
    -fno-tree-scev-cprop
)

# Include directories
include_directories(src)

file(GLOB SRC_H_FILES "src/*.h") # Load all h files in src folder
file(GLOB SRC_C_FILES "src/*.c") # Load all c files in src folder

message(STATUS "Source files: ${SRC_H_FILES} ${SRC_C_FILES}")

# Create one target
add_executable(${PRODUCT_NAME} ${SRC_H_FILES} ${SRC_C_FILES})

# Rename the output to .elf as we will create multiple files
set_target_properties(${PRODUCT_NAME} PROPERTIES OUTPUT_NAME ${PRODUCT_NAME}.elf)

# Strip binary for upload
add_custom_target(strip ALL avr-strip ${PRODUCT_NAME}.elf DEPENDS ${PRODUCT_NAME})

# Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.hex DEPENDS strip)
# Transform binary into hex file, this is the eeprom part
add_custom_target(eeprom avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.eep DEPENDS strip)

# Transform binary into srec file, we ignore the eeprom segments in the step
add_custom_target(srec ALL avr-objcopy -R .eeprom -O srec ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.srec DEPENDS strip)
# Transform binary into srec file, this is the eeprom part
# add_custom_target(eeprom avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 -O srec ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.eep DEPENDS strip)

# Upload the firmware with avrdude
add_custom_target(upload avrdude -c ${PROG_TYPE} -p ${MCU} -U flash:w:${PRODUCT_NAME}.hex DEPENDS hex)

# Upload the eeprom with avrdude
add_custom_target(upload_eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PRODUCT_NAME}.eep DEPENDS eeprom)

# Burn fuses
add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU}  -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m )

# Clean extra files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.srec;${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")
