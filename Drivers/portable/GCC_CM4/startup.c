// This module contains the startup code for a portable embedded
// C/C++ application, built with newlib.
//
// Control reaches here from the reset handler via jump or call.
//
// The actual steps performed by _start are:
// - copy the initialised data region(s)
// - clear the BSS region(s)
// - initialise the system
// - run the preinit/init array (for the C++ static constructors)
// - initialise the arc/argv
// - branch to main()
// - run the fini array (for the C++ static destructors)
// - call _exit(), directly or via exit()
//
// If OS_INCLUDE_STARTUP_INIT_MULTIPLE_RAM_SECTIONS is defined, the
// code is capable of initialising multiple regions.
//
// The normal configuration is standalone, with all support
// functions implemented locally.
//
// For this to be called, the project linker must be configured without
// the startup sequence (-nostartfiles).

#include <stdint.h>
#include <sys/types.h>

#if !defined(OS_INCLUDE_STARTUP_GUARD_CHECKS)
#define OS_INCLUDE_STARTUP_GUARD_CHECKS (1)
#endif

// defined in linker script

extern unsigned int _sidata;
extern unsigned int __data_start__;
extern unsigned int __data_end__;
extern unsigned int __bss_start__;
extern unsigned int __bss_end__;
extern unsigned int _siccmram;
extern unsigned int __ccmram_start__;
extern unsigned int __ccmram_end__;

extern void __initialize_args (int*, char***);

// main() is the entry point for newlib based applications.
// By default, there are no arguments, but this can be customised
// by redefining __initialize_args(), which is done when the
// semihosting configurations are used.
extern int main (int argc, char* argv[]);

// The implementation for the exit routine; for embedded
// applications, a system reset will be performed.
extern void _exit (int) __attribute__((noreturn));

void _start (void);
void __initialize_data (unsigned int* from, unsigned int* region_begin, unsigned int* region_end);
void __initialize_bss (unsigned int* region_begin, unsigned int* region_end);
void __run_init_array (void);
void __run_fini_array (void);
void __initialize_hardware_early (void);
void __initialize_hardware (void);

inline __attribute__((always_inline)) void __initialize_data (unsigned int* from, unsigned int* region_begin,
    unsigned int* region_end)
{
	// Iterate and copy word by word.
	// It is assumed that the pointers are word aligned.
	unsigned int *p = region_begin;
	while (p < region_end)
		*p++ = *from++;
}

inline __attribute__((always_inline)) void __initialize_bss (unsigned int* region_begin, unsigned int* region_end)
{
	// Iterate and clear word by word.
	// It is assumed that the pointers are word aligned.
	unsigned int *p = region_begin;
	while (p < region_end)
		*p++ = 0;
}

// These magic symbols are provided by the linker.
extern void (*__preinit_array_start[]) (void) __attribute__((weak));
extern void (*__preinit_array_end[]) (void) __attribute__((weak));
extern void (*__init_array_start[]) (void) __attribute__((weak));
extern void (*__init_array_end[]) (void) __attribute__((weak));
extern void (*__fini_array_start[]) (void) __attribute__((weak));
extern void (*__fini_array_end[]) (void) __attribute__((weak));

// Iterate over all the preinit/init routines (mainly static constructors).
inline __attribute__((always_inline)) void __run_init_array (void)
{
	int count;
	int i;

	count = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < count; i++)
		__preinit_array_start[i]();

	// If you need to run the code in the .init section, please use
	// the startup files, since this requires the code in crti.o and crtn.o
	// to add the function prologue/epilogue.
	//_init(); // DO NOT ENABE THIS!

	count = __init_array_end - __init_array_start;
	for (i = 0; i < count; i++)
		__init_array_start[i]();
}

// Run all the cleanup routines (mainly static destructors).
inline __attribute__((always_inline)) void __run_fini_array (void)
{
	int count;
	int i;

	count = __fini_array_end - __fini_array_start;
	for (i = count; i > 0; i--)
		__fini_array_start[i - 1]();

	// If you need to run the code in the .fini section, please use
	// the startup files, since this requires the code in crti.o and crtn.o
	// to add the function prologue/epilogue.
	//_fini(); // DO NOT ENABE THIS!
}

#if defined(DEBUG) && (OS_INCLUDE_STARTUP_GUARD_CHECKS)

// These definitions are used to check if the routines used to
// clear the BSS and to copy the initialised DATA perform correctly.

#define BSS_GUARD_BAD_VALUE (0xCADEBABA)

static uint32_t volatile __bss_begin_guard __attribute__ ((section(".bss_begin")));
static uint32_t volatile __bss_end_guard __attribute__ ((section(".bss_end")));

#define DATA_GUARD_BAD_VALUE (0xCADEBABA)
#define DATA_BEGIN_GUARD_VALUE (0x12345678)
#define DATA_END_GUARD_VALUE (0x98765432)

static uint32_t volatile __attribute__ ((section(".data_begin")))
__data_begin_guard = DATA_BEGIN_GUARD_VALUE;

static uint32_t volatile __attribute__ ((section(".data_end")))
__data_end_guard = DATA_END_GUARD_VALUE;

#endif // defined(DEBUG) && (OS_INCLUDE_STARTUP_GUARD_CHECKS)

// This is the place where Cortex-M core will go immediately after reset,
// via a call or jump from the Reset_Handler.
//
// For the call to work, and for the call to __initialize_hardware_early()
// to work, the reset stack must point to a valid internal RAM area.

__attribute__ ((section(".after_vectors"),noreturn,weak))
void _start (void)
{

	// Initialise hardware right after reset, to switch clock to higher
	// frequency and have the rest of the initialisations run faster.
	//
	// Mandatory on platforms like Kinetis, which start with the watch dog
	// enabled and require an early sequence to disable it.
	//
	// Also useful on platform with external RAM, that need to be
	// initialised before filling the BSS section.

	__initialize_hardware_early();

	// Use Old Style DATA and BSS section initialisation,
	// that will manage a single BSS sections.

#if defined(DEBUG) && (OS_INCLUDE_STARTUP_GUARD_CHECKS)
	__data_begin_guard = DATA_GUARD_BAD_VALUE;
	__data_end_guard = DATA_GUARD_BAD_VALUE;
#endif

#if !defined(OS_INCLUDE_STARTUP_INIT_MULTIPLE_RAM_SECTIONS)
	// Copy the DATA segment from Flash to RAM (inlined).
	__initialize_data(&_sidata, &__data_start__, &__data_end__);
	__initialize_data(&_siccmram, &__ccmram_start__, &__ccmram_end__);
#else

	// Copy the data sections from flash to SRAM.
	for (unsigned int* p = &__data_regions_array_start;
		p < &__data_regions_array_end;)
	{
		unsigned int* from = (unsigned int *) (*p++);
		unsigned int* region_begin = (unsigned int *) (*p++);
		unsigned int* region_end = (unsigned int *) (*p++);

		__initialize_data (from, region_begin, region_end);
	}

#endif

#if defined(DEBUG) && (OS_INCLUDE_STARTUP_GUARD_CHECKS)
	if ((__data_begin_guard != DATA_BEGIN_GUARD_VALUE) || (__data_end_guard != DATA_END_GUARD_VALUE)) {
		for (;;)
			;
	}
#endif

#if defined(DEBUG) && (OS_INCLUDE_STARTUP_GUARD_CHECKS)
	__bss_begin_guard = BSS_GUARD_BAD_VALUE;
	__bss_end_guard = BSS_GUARD_BAD_VALUE;
#endif

#if !defined(OS_INCLUDE_STARTUP_INIT_MULTIPLE_RAM_SECTIONS)
	// Zero fill the BSS section (inlined).
	__initialize_bss(&__bss_start__, &__bss_end__);
#else

	// Zero fill all bss segments
	for (unsigned int *p = &__bss_regions_array_start;
		p < &__bss_regions_array_end;)
	{
		unsigned int* region_begin = (unsigned int*) (*p++);
		unsigned int* region_end = (unsigned int*) (*p++);
		__initialize_bss (region_begin, region_end);
	}
#endif

#if defined(DEBUG) && (OS_INCLUDE_STARTUP_GUARD_CHECKS)
	if ((__bss_begin_guard != 0) || (__bss_end_guard != 0)) {
		for (;;)
			;
	}
#endif

	// Hook to continue the initialisations. Usually compute and store the
	// clock frequency in the global CMSIS variable, cleared above.
	__initialize_hardware();

	// Get the argc/argv (useful in semihosting configurations).
	int argc;
	char** argv;
	__initialize_args(&argc, &argv);

	// Call the standard library initialisation (mandatory for C++ to
	// execute the constructors for the static objects).
	__run_init_array();

	// Call the main entry point, and save the exit code.
	int code = main(argc, argv);

	// Run the C++ static destructors.
	__run_fini_array();

	_exit(code);

	// Should never reach this, _exit() should have already
	// performed a reset.
	for (;;)
		;
}
