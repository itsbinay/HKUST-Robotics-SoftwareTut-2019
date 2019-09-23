# CMake Project File
#
# If you're changing CMake stuff and it doesnt seem to change, delete the CMakeCache.txt file and retry
#
# For Windows, CMake tries to use Visual Studio if available, this fucks up the custom toolchain setting in here
# Always manually override to Ninja or Make
#
# For all platforms Ninja is much faster than Make so use it
#
# Usage: Command Line
#  # Create and goto the build directory
#  mkdir build && cd build
#  # Tell CMake to generate a Ninja build file based on the project
#  # Replace Ninja with any cmake generator, or leave it out if the default is okay
#  # cmake --help lists the support generators
#  cmake -G Ninja -DSTM32FXXX=?? ..
#  # Build the program, only this is needed after the initial cmake
#  ninja -j 0 -l 12 # OR
#  cmake --build . -- -j 0 -l 12
#  # This rebuilds all
#  cmake --build . --clean-first -- -j 0 -l 12
#
# Usage: Important options (For any build generator)
#  -G ??            -> Tell cmake which generator you want
#       - Ninja, Unix Makefiles, MinGW Makefiles, cmake --help for all available
#  -DSTM32FXXX=??   -> Which MCU are you using, try be as exact as possible ie. STM32F429II
#                   -> actually you should listen to GCC, except that STM32 Libraries don't so...
#  -DDEFAULT_PROGRAMMER=STM32CUBEPROG/OPENOCD/TEXANE/STLINKUTILITY
#      -> choose youre preferred tool for flashing
#  -DUSE_GCC=1      -> use gcc instead of armcc
#  -DSTFU_GCC=1     -> tell gcc to shut the fuck up because it has so many more warnings
#  -DCUBE_PROJECT=1 -> This is a cube generated project (DCUBE_PROJECT_LL means use LL)
#  -DCMAKE_BUILD_TYPE=Debug/Release -> Debug doesn't optimize so compiles faster and runs slower
#  -DUSING_FREERTOS=0 -> disable FreeRTOS
#  -DARMCC_GNU_COMPAT=1 -> add --gnu option to the armcc
#  -DARMCC_MICROLIB=1 -> tells arm to use the MicroLIB instead of std libc, broken for some reason
#  These may also be set in the project.cmake user_proj_config file
#
# Usage: Sublime - install CMakeBuilder
# 1. Create a ${anything}.sublime-project file at the root of the project
# 2. add this to the sublime-project
#       "settings":
#       {
#       	"cmake":
#       	{
#       		"build_folder": "${project_path}/cmbuild", #put your desired build directory here
#       		"command_line_overrides":
#       		{
#       			"CMAKE_BUILD_TYPE": "Release",
#       			"STM32FXXX": "STM32F429"
#       		},
#       		"generator": "Ninja"
#       	}
#       }
# 3. when saving the sublime-project it should auto gen the CMake project, if not then rune CMakeBuilder: Configure
# 4. run CMakeBuilder: Write Build Targets to Sublime Project File and it will add sublime build commands to build
# 5. You may use Build With and Ctrl+B to build the CMake project from now
#
# Usage: CLion
# 1. Open the CMakeLists.txt as a project in CLion
# 2. Go to Settings > Build, ... > CMake, Configure the CMake profile with the necesary options (-DCMAKE_BUILD_TYPE=Release -DSTM32FXXX=??)
# 3. Let CLion reload the CMake Project, ignore the little error about compiler id not working
# 4. TODO: Configure OpenOCD support within CLion
# 5. Use Build and flash from CLion (Using run will make it try run the MCU program on your computer... weird)
#
#

cmake_minimum_required(VERSION 3.0)

include(project.cmake OPTIONAL RESULT_VARIABLE PROJ_FILE_EXISTS)
message("${PROJ_FILE_EXISTS}")
if(PROJ_FILE_EXISTS)
	message("Loading Custom Project Config")
	user_proj_config()
endif()

if(NOT DEFINED STM32FXXX OR STM32FXXX STREQUAL "")
	message("Define which STM32 MCU you are using, options:")
	message("Terminal: pass in through -DSTM32FXXX=??")
	message("CLion: cmake profile add '-DCMAKE_BUILD_TYPE=Release -DSTM32FXXX=??'' (https://www.jetbrains.com/help/clion/cmake-profile.html)")
	message("Sublime: to X.sublime-project add \"command_line_overrides\" : \"CMAKE_BUILD_TYPE\": \"Release\",\"STM32FXXX\": \"??\"")
	message("Unrecommend: Hardcode in file, add set(STM32FXXX ??) just above here (within NOT DEFINED)")
	message(FATAL_ERROR "Define which STM32 MCU you are using, see above ^^^")
endif()

if (NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
	message("CMAKE_BUILD_TYPE defaulting to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()

if (NOT DEFINED CMAKE_VERBOSE_MAKEFILE)
	set(CMAKE_VERBOSE_MAKEFILE OFF)
endif()

if (NOT DEFINED USE_COMPILER OR USE_COMPILER STREQUAL "")
	if(DEFINED USE_GCC AND USE_GCC EQUAL 1)
		set(USE_COMPILER "GCC")
	elseif(DEFINED USE_ARMCC AND USE_ARMCC EQUAL 1)
		set(USE_COMPILER "ARMCC")
		if (UNIX AND NOT APPLE)
			message("Are you sure about ARMCC on Linux?")
		endif()
	elseif(UNIX)
		message("USE_COMPILER defaulting to GCC")
		set(USE_COMPILER "GCC")
	else()
		message("USE_COMPILER defaulting to ARMCC")
		set(USE_COMPILER "ARMCC")
	endif()
endif()

if(APPLE)
	find_program(STM32CubeProg "STM32_Programmer_CLI"
		"/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/MacOs/bin")
	# set(prog "/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/MacOs/bin/STM32_Programmer_CLI")
else()
	find_program(STM32CubeProg "STM32_Programmer_CLI"
		"C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"
		"C:/Program Files/STMicroelectronics/STM32CubeProgrammer/bin")
	# set(prog "STM32_Programmer_CLI")
endif()
find_program(STLINKUTIL "st-link_cli")
find_program(OPENOCD_EXE "openocd")
find_program(STFLASH_EXE "st-flash")
if(WIN32)
	find_program(STMFlashLoader "STMFlashLoader.exe"
		"C:/Program Files/STMicroelectronics/Software/Flash Loader Demo/"
		"C:/Program Files (x86)/STMicroelectronics/Software/Flash Loader Demo/")
endif()

if (NOT DEFINED XTAL)
	set(XTAL 8000000)
endif()

function(join VALUES GLUE OUTPUT)
  string (REGEX REPLACE "([^\\]|^);" "\\1${GLUE}" _TMP_STR "${VALUES}")
  string (REGEX REPLACE "[\\](.)" "\\1" _TMP_STR "${_TMP_STR}") #fixes escaping
  set (${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endfunction()

##############################################################################################################################################################################

string(TOLOWER ${STM32FXXX} stm32fxxx)
string(TOUPPER ${STM32FXXX} STM32FXXX)
message("STM32Fxyzab: ${STM32FXXX}")

# Determine STM32FXXX family
string(REGEX MATCH "^(STM32[FL][0-9])" STM32_FAMILY "${STM32FXXX}")
string(TOLOWER ${STM32_FAMILY} stm32_family)
message("STM32Fx Family: ${STM32_FAMILY}")

# Determine STM32FXXX series
string(REGEX MATCH "^(STM32[FL][0-9][0-9][0-9])" STM32_SERIES "${STM32FXXX}")
message("STM32Fxxx Series: ${STM32_SERIES}")
string(TOLOWER ${STM32_SERIES} stm32_series)

string(REGEX MATCH "^(STM32[FL][0-9][0-9][0-9])([A-Z])([A-Z])" CPU_FAMILY_MATCH "${STM32FXXX}")
set(STM32_SERIES_A "${CMAKE_MATCH_1}x${CMAKE_MATCH_3}")
set(STM32_SERIES_B "${CMAKE_MATCH_1}${CMAKE_MATCH_2}x")
string(TOLOWER ${STM32_SERIES_A} stm32_series_a)
string(TOLOWER ${STM32_SERIES_B} stm32_series_b)
message("${STM32_SERIES_A}/${STM32_SERIES_B}")

# Set CPU type for compiler
# TODO: F0, F2, F7, H7
if(${STM32_FAMILY} STREQUAL "STM32F1")
	set(GCC_CPU "cortex-m3")
	set(ARMCC_CPU "Cortex-M3")
	set(DEF_ARM_MATH "ARM_MATH_CM3")
	set(HAS_FPU 0)
elseif(${STM32_FAMILY} STREQUAL "STM32F4" OR ${STM32_FAMILY} STREQUAL "STM32F3")
	set(GCC_CPU "cortex-m4")
	set(ARMCC_CPU "Cortex-M4.fp")
	set(DEF_ARM_MATH "-DARM_MATH_CM4 -D__FPU_PRESENT=1U")
	set(HAS_FPU 1)
else()
	message(FATAL_ERROR "Unrecognised device family: ${STM32_FAMILY}")
endif()

set(STM32_DEFINES ${DEF_ARM_MATH} -D${STM32FXXX} -D${STM32_SERIES}xx -D${STM32_FAMILY} -D${STM32_SERIES_A} -D${STM32_SERIES_B})
if (STM32_SERIES STREQUAL STM32F405)
	set(STM32_DEFINES ${EFINES} -DSTM32F40_41xxx)
elseif (STM32_SERIES STREQUAL STM32F415)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F40_41xxx)
elseif (STM32_SERIES STREQUAL STM32F407)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F40_41xxx)
elseif (STM32_SERIES STREQUAL STM32F417)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F40_41xxx)
elseif (STM32_SERIES STREQUAL STM32F427)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F427_437xx)
elseif (STM32_SERIES STREQUAL STM32F437)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F427_437xx)
elseif (STM32_SERIES STREQUAL STM32F429)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F429_439xx)
elseif (STM32_SERIES STREQUAL STM32F439)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F429_439xx)
elseif (STM32_SERIES STREQUAL STM32F401) #xC
elseif (STM32_SERIES STREQUAL STM32F401) #xE
elseif (STM32_SERIES STREQUAL STM32F410) #Tx
elseif (STM32_SERIES STREQUAL STM32F410) #Cx
elseif (STM32_SERIES STREQUAL STM32F410) #Rx
elseif (STM32_SERIES STREQUAL STM32F411) #xE
elseif (STM32_SERIES STREQUAL STM32F446)
elseif (STM32_SERIES STREQUAL STM32F469)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F469_479xx)
elseif (STM32_SERIES STREQUAL STM32F479)
	set(STM32_DEFINES ${STM32_DEFINES} -DSTM32F469_479xx)
elseif (STM32_SERIES STREQUAL STM32F412) #Cx
elseif (STM32_SERIES STREQUAL STM32F412) #Zx
elseif (STM32_SERIES STREQUAL STM32F412) #Rx
elseif (STM32_SERIES STREQUAL STM32F412) #Vx
elseif (STM32_SERIES STREQUAL STM32F413)
elseif (STM32_SERIES STREQUAL STM32F423)
endif()

if (NOT DEFINED OPENOCD_CFG)
	find_file(OPENOCD_CFG_FILE "openocd.cfg" "." "./src" "${CMAKE_BINARY_DIR}"
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	if (OPENOCD_CFG_FILE)
		set(OPENOCD_CFG -f ${OPENOCD_CFG_FILE})
		message("Found openocd.cfg: ${OPENOCD_CFG_FILE}")
	else()
		set(OPENOCD_CFG -f interface/stlink-v2.cfg -f target/${stm32_family}x.cfg)
	endif()
endif()

if (NOT DEFINED MCU_STARTUP_FILE)
	set(MCU_STARTUP_SEARCH_DIRS
		"." "./MDK-ARM" "./Startup"
		"./startup/${USE_COMPILER}" "./startup" 
		"./src/startup/${USE_COMPILER}" "./src/startup")
	message("Looking for Startup Assembly")
	find_file(MCU_STARTUP_FILE "startup_${stm32fxxx}.s" ${MCU_STARTUP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(MCU_STARTUP_FILE "startup_${stm32_series_b}.s" ${MCU_STARTUP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(MCU_STARTUP_FILE "startup_${stm32_series_a}.s" ${MCU_STARTUP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(MCU_STARTUP_FILE "startup_${stm32_series}xx.s" ${MCU_STARTUP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(MCU_STARTUP_FILE "startup_${stm32_family}xx.s" ${MCU_STARTUP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(MCU_STARTUP_FILE "startup_${stm32fxxx}tx.s" ${MCU_STARTUP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
endif()
if (MCU_STARTUP_FILE)
	message("MCU_STARTUP_FILE: ${MCU_STARTUP_FILE}")
else()
	message(FATAL_ERROR "Could not find a startup file")
endif()


set(MEMMAP_SEARCH_DIRS
	"." "./MDK-ARM" 
	"./src/mem/${USE_COMPILER}" "./src/mem")

if (NOT SCATTER_FILE AND USE_COMPILER STREQUAL "ARMCC")
	message("Looking for Scatter File (ARMCC)")
	find_file(SCATTER_FILE "${STM32FXXX}.sct" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(SCATTER_FILE "${STM32_SERIES_B}.sct" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(SCATTER_FILE "${STM32_SERIES_A}.sct" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(SCATTER_FILE "${STM32_SERIES}.sct" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(SCATTER_FILE "${STM32_FAMILY}.sct" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(SCATTER_FILE "${STM32FXXX}Tx_FLASH.sct" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	if(NOT SCATTER_FILE AND DEFINED CUBE_PROJECT OR DEFINED CUBE_PROJECT_LL)
		file(GLOB_RECURSE SCATTER_FILE "./*.sct")
	endif()
endif()
if (NOT SCATTER_FILE AND USE_COMPILER STREQUAL "ARMCC")
	message(FATAL_ERROR "Could not find a scatter file")
else()
	message("SCATTER_FILE: ${SCATTER_FILE}")
endif()

if (NOT LINKER_SCRIPT AND USE_COMPILER STREQUAL "GCC")
	message("Looking for Linker Script (GCC)")
	find_file(LINKER_SCRIPT "${STM32FXXX}.ld" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(LINKER_SCRIPT "${STM32_SERIES_B}.ld" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(LINKER_SCRIPT "${STM32_SERIES_A}.ld" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(LINKER_SCRIPT "${STM32_SERIES}.ld" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(LINKER_SCRIPT "${STM32_FAMILY}.ld" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
	find_file(LINKER_SCRIPT "${STM32FXXX}Tx_FLASH.ld" ${MEMMAP_SEARCH_DIRS}
		NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)
endif()
if (NOT LINKER_SCRIPT AND USE_COMPILER STREQUAL "GCC")
	message(FATAL_ERROR "Could not find a linker script")
else()
	message("LINKER_SCRIPT: ${LINKER_SCRIPT}")
endif()

if (NOT DEFINED XTAL)
	set(XTAL 8000000)
endif()

set(USER_DEFINES ${USER_DEFINES} -DHSE_VALUE=${XTAL})
if (NOT DEFINED USING_FREERTOS OR USING_FREETOS EQUAL 1)
	set(USER_DEFINES ${USER_DEFINES} -DUSING_FREERTOS)
endif()
if (DEFINED CUBE_PROJECT)
	set(USER_DEFINES ${USER_DEFINES} -DUSE_HAL_DRIVER)
elseif (DEFINED CUBE_PROJECT_LL)
	set(USER_DEFINES ${USER_DEFINES} -DUSE_LL_DRIVER)
else()
	set(USER_DEFINES ${USER_DEFINES} -DUSE_STDPERIPH_DRIVER)
endif()
message("USER_DEFINES = ${USER_DEFINES}")

if (NOT DEFINED SOURCE_FILES)
	if (DEFINED CUBE_PROJECT OR DEFINED CUBE_PROJECT_LL)
		file(GLOB_RECURSE MIDDLEWARE_SOURCES "./Middlewares/*.c")
		file(GLOB SOURCE_FILES "./Src/*.c" "./Drivers/${STM32_FAMILY}xx_HAL_Driver/Src/*.c")
		set(SOURCE_FILES ${SOURCE_FILES} ${MIDDLEWARE_SOURCES})
	elseif (DEFINED SOURCE_DIR)
		file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.c")
	else()
		file(GLOB_RECURSE SOURCE_FILES "./src/*.c")
	endif()
endif()
join("${SOURCE_FILES}" "\n -" PRINT_SRC_FILES)
message("SOURCE_FILES = \n -${PRINT_SRC_FILES}")

if (NOT DEFINED INCLUDE_DIRS)
	if (DEFINED CUBE_PROJECT OR DEFINED CUBE_PROJECT_LL)
		file(GLOB_RECURSE MIDDLEWARE_INCLUDES LIST_DIRECTORIES true "./Middlewares/*/Inc")
		# file(GLOB_RECURSE DRIVER_INCLUDES LIST_DIRECTORIES true "./Drivers/*/Inc")
		set(INCLUDE_DIRS
			./Inc/
			./Drivers/${STM32_FAMILY}xx_HAL_Driver/Inc
			./Drivers/${STM32_FAMILY}xx_HAL_Driver/Inc/Legacy
			./Drivers/CMSIS/Device/ST/${STM32_FAMILY}xx/Include
			./Drivers/CMSIS/Include
			${MIDDLEWARE_INCLUDES})
	elseif (DEFINED SOURCE_DIR)
		file(GLOB INCLUDE_DIRS "${SOURCE_DIR}/*/")
	else()
		file(GLOB INCLUDE_DIRS "./src/*/")
	endif()
endif()
join("${INCLUDE_DIRS}" "\n -" PRINT_INC_DIRS)
message("INCLUDE_DIRS = \n -${PRINT_INC_DIRS}")

if (NOT DEFINED LINK_LIBRARIES)
	if (USE_COMPILER STREQUAL "ARMCC")
		set(STATIC_LIB_EXT "lib")
	elseif (USE_COMPILER STREQUAL "GCC")
		set(STATIC_LIB_EXT "a")
	endif()

	if (DEFINED CUBE_PROJECT OR DEFINED CUBE_PROJECT_LL)
		# TODO:
	elseif(DEFINED STATICLIB_DIR)
		file(GLOB LINK_LIBRARIES "${STATICLIB_DIR}/${USE_COMPILER}/*.${STATIC_LIB_EXT}")
	else()
		file(GLOB LINK_LIBRARIES "./lib/${USE_COMPILER}/*.${STATIC_LIB_EXT}")
	endif()
endif()
message("LINK_LIBRARIES (${STATIC_LIB_EXT}) = ${LINK_LIBRARIES}")

##############################################################################################################################################################################

## Compiler Toolchain Definitions

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if (USE_COMPILER STREQUAL "ARMCC")
	set(ARMCC_DIR C:/Keil_v5/ARM/ARMCC/Bin)
	set(CMAKE_C_COMPILER ${ARMCC_DIR}/ArmCC.exe)
	set(CMAKE_ASM_COMPILER ${ARMCC_DIR}/ArmAsm.exe)
	set(CMAKE_CXX_COMPILER ${CMAKE_C_COMPILER})
	set(CMAKE_LINKER ${ARMCC_DIR}/armlink.exe)
	set(FROMELF ${ARMCC_DIR}/fromelf.exe)

	set(CMAKE_SYSROOT ${ARMCC_DIR}/..)
	set(CMAKE_FIND_ROOT_PATH ${ARMCC_DIR}/..)
elseif(USE_COMPILER STREQUAL "GCC")
	# SET(CMAKE_C_COMPILER_WORKS 1)
	SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
	SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)
	set(AS arm-none-eabi-as)
	set(AR arm-none-eabi-ar)
	set(OBJCOPY arm-none-eabi-objcopy)
	set(OBJDUMP arm-none-eabi-objdump)
	set(SIZE arm-none-eabi-size)

	# set(CMAKE_SYSROOT ${FCC_DIR}/..)
	# set(CMAKE_FIND_ROOT_PATH ${ARMCC_DIR}/..)
endif()

# TODO: WTF?
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

##############################################################################################################################################################################

if (NOT DEFINED PROJ_NAME)
	set(PROJ_NAME "Project")
endif()

## Start CMake Project
project (${PROJ_NAME})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
enable_language(C CXX ASM)

message("Generating project: ${PROJECT_NAME} for ${STM32FXXX}-${USE_COMPILER}-${CMAKE_BUILD_TYPE}")

add_executable(${PROJECT_NAME} ${MCU_STARTUP_FILE} ${SOURCE_FILES})

# COMPILER FLAGS
if (USE_COMPILER STREQUAL "ARMCC")
	set(COMP_DEFINES -D__UVISION_VERSION="526" -D_RTE_)

	set(COMPILER_INFO_FLAGS --asm --interleave -g)
	set(TIME_OPT_FLAGS -O3 -Otime --fpmode=fast)
	set(SPACE_OPT_FLAGS -O3 -Ospace --fpmode=fast)
	set(DEBUG_OPT_FLAGS -O0 -Otime)

	set(COMMON_FLAGS -g --apcs=interwork --cpu ${ARMCC_CPU})
	set(COMPILER_FLAGS --split_sections ${COMMON_FLAGS})
	if (DEFINED ARMCC_GNU_COMPAT AND ARMCC_GNU_COMPAT)
		set(COMPILER_FLAGS ${COMPILER_FLAGS} --gnu)
	endif()

	# LINKER FLAGS
	set(LINKER_INFO_FLAGS --info=summarysizes --info=sizes --info=totals --info=unused --info=veneers --callgraph --map --load_addr_map_info --list ${PROJECT_NAME}.map)
	set(LINKER_OPT_FLAGS --inline --tailreorder)

	set(LIST_CMAKE_C_FLAGS --C99 ${COMPILER_FLAGS})
	set(LIST_CMAKE_C_FLAGS_DEBUG ${DEBUG_OPT_FLAGS} ${COMPILER_INFO_FLAGS})
	set(LIST_CMAKE_C_FLAGS_MINSIZEREL ${SPACE_OPT_FLAGS})
	set(LIST_CMAKE_C_FLAGS_RELEASE ${TIME_OPT_FLAGS})
	set(LIST_CMAKE_C_FLAGS_RELWITHDEBINFO ${TIME_OPT_FLAGS} ${COMPILER_INFO_FLAGS})
	set(LIST_CMAKE_ASM_FLAGS ${COMMON_FLAGS})
	set(LIST_CMAKE_ASM_FLAGS_DEBUG)
	set(LIST_CMAKE_ASM_FLAGS_MINSIZEREL)
	set(LIST_CMAKE_ASM_FLAGS_RELEASE)
	set(LIST_CMAKE_ASM_FLAGS_RELWITHDEBINFO)
	set(LIST_CMAKE_CXX_FLAGS --cpp11 ${COMPILER_FLAGS})
	set(LIST_CMAKE_CXX_FLAGS_DEBUG ${DEBUG_OPT_FLAGS} ${COMPILER_INFO_FLAGS})
	set(LIST_CMAKE_CXX_FLAGS_MINSIZEREL ${SPACE_OPT_FLAGS})
	set(LIST_CMAKE_CXX_FLAGS_RELEASE ${TIME_OPT_FLAGS})
	set(LIST_CMAKE_CXX_FLAGS_RELWITHDEBINFO ${TIME_OPT_FLAGS} ${COMPILER_INFO_FLAGS})
	set(LIST_CMAKE_EXE_LINKER_FLAGS --cpu ${ARMCC_CPU} --strict --summary_stderr --xref --symbols --scatter \"${SCATTER_FILE}\")
	set(LIST_CMAKE_EXE_LINKER_FLAGS_DEBUG ${LINKER_INFO_FLAGS})
	set(LIST_CMAKE_EXE_LINKER_FLAGS_MINSIZEREL )
	set(LIST_CMAKE_EXE_LINKER_FLAGS_RELEASE ${LINKER_OPT_FLAGS})
	set(LIST_CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO ${LINKER_INFO_FLAGS})

	if(CMAKE_BUILD_TYPE STREQUAL Release)
	elseif(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
		if (NOT DEFINED ARMCC_MICROLIB)
			set(ARMCC_MICROLIB 1)
		endif()
	elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
	elseif (CMAKE_BUILD_TYPE STREQUAL Debug)
	endif()

	if (DEFINED ARMCC_MICROLIB AND ARMCC_MICROLIB)
		set(COMPILER_FLAGS ${COMPILER_FLAGS} -D__MICROLIB)
		set(LINKER_FLAGS "--library_type=microlib ${LINKER_FLAGS}")
		set(WARNING "MicroLIB does NOT work, idk why, you try it, remove this if you fix it")
	endif()

    # set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${SCATTER_FILE}")

	set(ALL_DEFINES ${USER_DEFINES} ${COMP_DEFINES} ${STM32_DEFINES})
	# set(ASM_FLAGS ${ASM_FLAGS} ${ALL_DEFINES})

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${FROMELF} --i32combined --output=${PROJECT_NAME}.hex ${PROJECT_NAME}.elf
		COMMENT "Building HEX File")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${FROMELF} --m32combined --output=${PROJECT_NAME}.bin ${PROJECT_NAME}.elf
		COMMENT "Building BIN File")

elseif(USE_COMPILER STREQUAL "GCC")

	set(COMP_DEFINES -D__pure=__attribute__\(\(const\)\) -D__forceinline=__attribute__\(\(always_inline\)\) -D__value_in_regs= -D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\))

	set(COMPILER_INFO_FLAGS -g)
	set(TIME_OPT_FLAGS -O3) #Ofast is danger boi #-flto breaks freertos
	if(USE_LTO)
		set(TIME_OPT_FLAGS ${TIME_OPT_FLAGS} -flto)
	endif()
	set(SPACE_OPT_FLAGS -Os)
	set(DEBUG_OPT_FLAGS -Og)

	SET(COMMON_FLAGS -mcpu=${GCC_CPU} -mthumb -mthumb-interwork -fno-math-errno)
	if (HAS_FPU)
		set(COMMON_FLAGS ${COMMON_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16)
	else()
		set(COMMON_FLAGS ${COMMON_FLAGS} -mfpu=vfp -msoft-float)
	endif()
	if(NOT DEFINED GCC_SPECS)
		if (DEFINED CUBE_PROJECT OR DEFINED CUBE_PROJECT_LL)
			set(COMMON_FLAGS ${COMMON_FLAGS} -specs=nano.specs)
		else()
			set(COMMON_FLAGS ${COMMON_FLAGS} -specs=rdimon.specs)
		endif()
	else()
		set(COMMON_FLAGS ${COMMON_FLAGS} -specs=${GCC_SPECS})
	endif()

	if (DEFINED STFU_GCC AND STFU_GCC EQUAL 1)
		SET(GCC_WARN_FLAGS -w)
	else()
		SET(GCC_WARN_FLAGS -Wextra -Wall -Wno-unused-parameter -Wno-attributes -Wno-unused-function)
	endif()
	SET(GCC_FEAT_FLAGS -g -fsingle-precision-constant -fms-extensions -fno-builtin -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-common -fmessage-length=0)
	SET(COMPILER_FLAGS ${COMMON_FLAGS} ${GCC_WARN_FLAGS} ${GCC_FEAT_FLAGS})
	
	set(LIST_CMAKE_C_FLAGS ${COMPILER_FLAGS})
	set(LIST_CMAKE_C_FLAGS_DEBUG ${DEBUG_OPT_FLAGS} ${COMPILER_INFO_FLAGS})
	set(LIST_CMAKE_C_FLAGS_MINSIZEREL ${SPACE_OPT_FLAGS})
	set(LIST_CMAKE_C_FLAGS_RELEASE ${TIME_OPT_FLAGS})
	set(LIST_CMAKE_C_FLAGS_RELWITHDEBINFO ${TIME_OPT_FLAGS} ${COMPILER_INFO_FLAGS})
	set(LIST_CMAKE_ASM_FLAGS ${COMMON_FLAGS})
	set(LIST_CMAKE_ASM_FLAGS_DEBUG)
	set(LIST_CMAKE_ASM_FLAGS_MINSIZEREL)
	set(LIST_CMAKE_ASM_FLAGS_RELEASE)
	set(LIST_CMAKE_ASM_FLAGS_RELWITHDEBINFO)
	set(LIST_CMAKE_CXX_FLAGS ${COMPILER_FLAGS})
	set(LIST_CMAKE_CXX_FLAGS_DEBUG ${DEBUG_OPT_FLAGS} ${COMPILER_INFO_FLAGS})
	set(LIST_CMAKE_CXX_FLAGS_MINSIZEREL ${SPACE_OPT_FLAGS})
	set(LIST_CMAKE_CXX_FLAGS_RELEASE ${TIME_OPT_FLAGS})
	set(LIST_CMAKE_CXX_FLAGS_RELWITHDEBINFO ${TIME_OPT_FLAGS} ${COMPILER_INFO_FLAGS})
	SET(LIST_CMAKE_EXE_LINKER_FLAGS -Wl,-gc-sections -T ${LINKER_SCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,--cref)
	set(LIST_CMAKE_EXE_LINKER_FLAGS_DEBUG ${LINKER_INFO_FLAGS})
	set(LIST_CMAKE_EXE_LINKER_FLAGS_MINSIZEREL )
	set(LIST_CMAKE_EXE_LINKER_FLAGS_RELEASE ${LINKER_OPT_FLAGS})
	set(LIST_CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO ${LINKER_INFO_FLAGS})

	if(CMAKE_BUILD_TYPE STREQUAL Release)
	elseif(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
		if (NOT DEFINED ARMCC_MICROLIB)
			set(ARMCC_MICROLIB 1)
		endif()
	elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		    COMMAND ${CMAKE_OBJDUMP} -d -S ${PROJECT_NAME}.elf > ${PROJECT_NAME}.lst
		    COMMENT "Dumping Assembly Listing")
	elseif (CMAKE_BUILD_TYPE STREQUAL Debug)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		    COMMAND ${CMAKE_OBJDUMP} -d -S ${PROJECT_NAME}.elf > ${PROJECT_NAME}.lst
		    COMMENT "Dumping Assembly Listing")
	endif()

	# target_sources(${PROJECT_NAME} PUBLIC ${LINKER_SCRIPT})

	# get_target_property(_cur_link_deps ${PROJECT_NAME} LINK_DEPENDS)
    # string(APPEND _cur_link_deps ${LINKER_SCRIPT})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")

	set(ALL_DEFINES ${USER_DEFINES} ${COMP_DEFINES} ${STM32_DEFINES})

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	    COMMAND ${CMAKE_OBJCOPY} -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
	    COMMENT "Building HEX File")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	    COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
	    COMMENT "Building BIN File")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	    COMMAND ${CMAKE_OBJDUMP} -d -S ${PROJECT_NAME}.elf > ${PROJECT_NAME}.lst
	    COMMENT "Dumping Assembly Listing")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
		COMMAND ${CMAKE_OBJSIZE} ${TARGET}
		COMMENT "Object Size:")

endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    SUFFIX ".elf"
)

target_compile_definitions(${PROJECT_NAME} PUBLIC ${ALL_DEFINES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})

join("${LIST_CMAKE_C_FLAGS}" " " CMAKE_C_FLAGS)
join("${LIST_CMAKE_C_FLAGS_DEBUG}" " " CMAKE_C_FLAGS_DEBUG)
join("${LIST_CMAKE_C_FLAGS_MINSIZEREL}" " " CMAKE_C_FLAGS_MINSIZEREL)
join("${LIST_CMAKE_C_FLAGS_RELEASE}" " " CMAKE_C_FLAGS_RELEASE)
join("${LIST_CMAKE_C_FLAGS_RELWITHDEBINFO}" " " CMAKE_C_FLAGS_RELWITHDEBINFO)
join("${LIST_CMAKE_ASM_FLAGS}" " " CMAKE_ASM_FLAGS)
join("${LIST_CMAKE_ASM_FLAGS_DEBUG}" " " CMAKE_ASM_FLAGS_DEBUG)
join("${LIST_CMAKE_ASM_FLAGS_MINSIZEREL}" " " CMAKE_ASM_FLAGS_MINSIZEREL)
join("${LIST_CMAKE_ASM_FLAGS_RELEASE}" " " CMAKE_ASM_FLAGS_RELEASE)
join("${LIST_CMAKE_ASM_FLAGS_RELWITHDEBINFO}" " " CMAKE_ASM_FLAGS_RELWITHDEBINFO)
join("${LIST_CMAKE_CXX_FLAGS}" " " CMAKE_CXX_FLAGS)
join("${LIST_CMAKE_CXX_FLAGS_DEBUG}" " " CMAKE_CXX_FLAGS_DEBUG)
join("${LIST_CMAKE_CXX_FLAGS_MINSIZEREL}" " " CMAKE_CXX_FLAGS_MINSIZEREL)
join("${LIST_CMAKE_CXX_FLAGS_RELEASE}" " " CMAKE_CXX_FLAGS_RELEASE)
join("${LIST_CMAKE_CXX_FLAGS_RELWITHDEBINFO}" " " CMAKE_CXX_FLAGS_RELWITHDEBINFO)
join("${LIST_CMAKE_EXE_LINKER_FLAGS}" " " CMAKE_EXE_LINKER_FLAGS)
join("${LIST_CMAKE_EXE_LINKER_FLAGS_DEBUG}" " " CMAKE_EXE_LINKER_FLAGS_DEBUG)
join("${LIST_CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}" " " CMAKE_EXE_LINKER_FLAGS_MINSIZEREL)
join("${LIST_CMAKE_EXE_LINKER_FLAGS_RELEASE}" " " CMAKE_EXE_LINKER_FLAGS_RELEASE)
join("${LIST_CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}" " " CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO)

##############################################################################################################################################################################

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
		COMMAND echo * > .gitignore)

find_file(GDB_DEBUG_FILE "debug.gdb" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}"
	NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_SYSTEM_ENVIRONMENT_PATH)

function(stlinkutil_targets)
	if(STLINKUTIL)
		add_custom_target(flash_stlinkutil
			${STLINKUTIL} -c -P ${PROJECT_NAME}.hex -Rst -Run
			DEPENDS ${PROJECT_NAME}
			COMMENT "Flashing target hardware (by STLink Utility CLI)")

		add_custom_target(erase_stlinkutil
			${STLINKUTIL} -c -ME
			COMMENT "Mass Erase all flash sectors")

		set(FLASH_TARGET flash_stlinkutil PARENT_SCOPE)
		set(ERASE_TARGET erase_stlinkutil PARENT_SCOPE)
	endif()
endfunction()

function(stflash_targets)
	if(STFLASH_EXE)
		add_custom_target(flash_stflash
			${STFLASH_EXE} --reset --format ihex write ${PROJECT_NAME}.hex
			DEPENDS ${PROJECT_NAME}
			COMMENT "Flashing target hardware (by Texane/STlink) (Remember to reset to run the program)")

		add_custom_target(erase_stflash
			${STFLASH_EXE} erase
			COMMENT "Mass Erase all flash sectors")

		find_program(STUTIL "st-util")
		if (STUTIL)
			add_custom_target(debug_stutil
			${STUTIL} -m -p 3333
			DEPENDS ${PROJECT_NAME}
			COMMENT "Running ST-UTIL as a GDB host (port 3333)")
			set(DEBUG_TARGET debug_stutil PARENT_SCOPE)
		endif()

		set(FLASH_TARGET flash_stflash PARENT_SCOPE)
		set(ERASE_TARGET erase_stflash PARENT_SCOPE)
	endif()
endfunction()

function(cubeprog_targets)
	if (STM32CubeProg)
		add_custom_target(flash_cubeprog
			${STM32CubeProg} -c port=SWD -w ${PROJECT_NAME}.elf -g
			DEPENDS ${PROJECT_NAME}
			COMMENT "Flashing target hardware (by STM32CubeProgrammer)")
		add_custom_target(flash_uart
			${STM32CubeProg} -c port=COM5 -w ${PROJECT_NAME}.elf -g
			DEPENDS ${PROJECT_NAME}
			COMMENT "Flashing target hardware (by UART)")
		add_custom_target(flash_usb
			${STM32CubeProg} -c port=usb0 -w ${PROJECT_NAME}.elf -g
			DEPENDS ${PROJECT_NAME}
			COMMENT "Flashing target hardware (by USB DFU)")
		add_custom_target(erase_cubeprog
			${STM32CubeProg} -c port-SWD -e all
			COMMENT "Mass Erase all flash sectors")
		
		add_custom_target(debug_cubeprog
			echo "Debug server unimplemented for Cube Programmer")
		add_custom_target(gdb_cubeprog
			echo "GDB unimplemented for Cube Programmer")

		set(FLASH_TARGET flash_cubeprog PARENT_SCOPE)
		set(ERASE_TARGET erase_cubeprog PARENT_SCOPE)
		# TODO: GDB Debug Server
	endif()
endfunction()

function(openocd_targets)
	if (OPENOCD_EXE)
		add_custom_target(flash_openocd
		  ${OPENOCD_EXE} ${OPENOCD_CFG} -c "transport select hla_swd"
		  -c "init" -c "reset halt"
		  -c "flash write_image erase ${PROJECT_NAME}.elf" -c "reset run" -c "shutdown"
		  DEPENDS ${PROJECT_NAME}
		  COMMENT "Flashing target hardware (by OpenOCD)")

		add_custom_target(debug_openocd
		  ${OPENOCD_EXE} ${OPENOCD_CFG}
		  DEPENDS ${PROJECT_NAME}
		  COMMENT "Running OpenOCD as a GDB host (port 3333)")

		add_custom_target(gdb_openocd
			gdbgui -g arm-none-eabi-gdb --args="${PROJECT_NAME}.elf")

		add_custom_target(erase_openocd
		  ${OPENOCD_EXE} ${OPENOCD_CFG} -c "transport select hla_swd"
		  -c "init" -c "reset halt"
		  -c "flash erase_sector 0 0 last" -c "shutdown"
		  DEPENDS ${PROJECT_NAME}
		  COMMENT "Mass Erase all flash sectors")

		set(FLASH_TARGET flash_openocd PARENT_SCOPE)
		set(ERASE_TARGET erase_openocd PARENT_SCOPE)
		set(DEBUG_TARGET debug_openocd PARENT_SCOPE)
		set(GDB_TARGET gdb_openocd PARENT_SCOPE)
	endif()
endfunction()

if(DEFAULT_PROGRAMMER STREQUAL "STM32CUBEPROG")
	stlinkutil_targets()
	stflash_targets()
	openocd_targets()
	cubeprog_targets()	
elseif(DEFAULT_PROGRAMMER STREQUAL "OPENOCD")
	stlinkutil_targets()
	stflash_targets()
	cubeprog_targets()	
	openocd_targets()
elseif(DEFAULT_PROGRAMMER STREQUAL "STFLASH" OR DEFAULT_PROGRAMMER STREQUAL "TEXANE")
	stlinkutil_targets()
	openocd_targets()
	cubeprog_targets()	
	stflash_targets()
elseif(DEFAULT_PROGRAMMER STREQUAL "STLINKUTILITY")
	stflash_targets()
	openocd_targets()
	cubeprog_targets()	
	stlinkutil_targets()
else()
	stlinkutil_targets()
	stflash_targets()
	openocd_targets()
	cubeprog_targets()	
endif()

if (DEFINED FLASH_TARGET)
	add_custom_target(flash DEPENDS ${FLASH_TARGET})
	message("flash using ${FLASH_TARGET}")
else()
	message("No program found for flash")
endif()
if (DEFINED ERASE_TARGET)
	add_custom_target(erase DEPENDS ${ERASE_TARGET})
	message("erase using ${ERASE_TARGET}")
else()
	message("No program found for erase")
endif()
if (DEFINED DEBUG_TARGET)
	add_custom_target(debug DEPENDS ${DEBUG_TARGET})
	message("debug using ${DEBUG_TARGET}")
else()
	message("No program found for debug")
endif()
if (DEFINED GDB_TARGET)
	add_custom_target(gdb DEPENDS ${GDB_TARGET})
	message("gdb using ${GDB_TARGET}")
else()
	message("No program found for gdb")
endif()
