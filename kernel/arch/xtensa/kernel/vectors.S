


#include <linux/linkage.h>
#include <asm/ptrace.h>
#include <asm/current.h>
#include <asm/asm-offsets.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/page.h>
#include <asm/thread_info.h>

#define WINDOW_VECTORS_SIZE   0x180



	.section .UserExceptionVector.text, "ax"

ENTRY(_UserExceptionVector)

	xsr	a3, EXCSAVE_1		# save a3 and get dispatch table
	wsr	a2, DEPC		# save a2
	l32i	a2, a3, EXC_TABLE_KSTK	# load kernel stack to a2
	s32i	a0, a2, PT_AREG0	# save a0 to ESF
	rsr	a0, EXCCAUSE		# retrieve exception cause
	s32i	a0, a2, PT_DEPC		# mark it as a regular exception
	addx4	a0, a0, a3		# find entry in table
	l32i	a0, a0, EXC_TABLE_FAST_USER	# load handler
	jx	a0


	.section .KernelExceptionVector.text, "ax"

ENTRY(_KernelExceptionVector)

	xsr	a3, EXCSAVE_1		# save a3, and get dispatch table
	wsr	a2, DEPC		# save a2
	addi	a2, a1, -16-PT_SIZE	# adjust stack pointer
	s32i	a0, a2, PT_AREG0	# save a0 to ESF
	rsr	a0, EXCCAUSE		# retrieve exception cause
	s32i	a0, a2, PT_DEPC		# mark it as a regular exception
	addx4	a0, a0, a3		# find entry in table
	l32i	a0, a0, EXC_TABLE_FAST_KERNEL	# load handler address
	jx	a0



	.section .DoubleExceptionVector.text, "ax"
	.begin literal_prefix .DoubleExceptionVector

ENTRY(_DoubleExceptionVector)

	/* Deliberately destroy excsave (don't assume it's value was valid). */

	wsr	a3, EXCSAVE_1		# save a3

	/* Check for kernel double exception (usually fatal). */

	rsr	a3, PS
	_bbci.l	a3, PS_UM_BIT, .Lksp

	/* Check if we are currently handling a window exception. */
	/* Note: We don't need to indicate that we enter a critical section. */

	xsr	a0, DEPC		# get DEPC, save a0

	movi	a3, XCHAL_WINDOW_VECTORS_VADDR
	_bltu	a0, a3, .Lfixup
	addi	a3, a3, WINDOW_VECTORS_SIZE
	_bgeu	a0, a3, .Lfixup

	/* Window overflow/underflow exception. Get stack pointer. */

	mov	a3, a2
	movi	a2, exc_table
	l32i	a2, a2, EXC_TABLE_KSTK

	/* Check for overflow/underflow exception, jump if overflow. */

	_bbci.l	a0, 6, .Lovfl

	/* a0: depc, a1: a1, a2: kstk, a3: a2, depc: a0, excsave: a3  */

	/* Restart window underflow exception.
	 * We return to the instruction in user space that caused the window
	 * underflow exception. Therefore, we change window base to the value
	 * before we entered the window underflow exception and prepare the
	 * registers to return as if we were coming from a regular exception
	 * by changing depc (in a0).
	 * Note: We can trash the current window frame (a0...a3) and depc!
	 */

	wsr	a2, DEPC		# save stack pointer temporarily
	rsr	a0, PS
	extui	a0, a0, PS_OWB_SHIFT, 4
	wsr	a0, WINDOWBASE
	rsync

	/* We are now in the previous window frame. Save registers again. */

	xsr	a2, DEPC		# save a2 and get stack pointer
	s32i	a0, a2, PT_AREG0

	wsr	a3, EXCSAVE_1		# save a3
	movi	a3, exc_table

	rsr	a0, EXCCAUSE
	s32i	a0, a2, PT_DEPC		# mark it as a regular exception
	addx4	a0, a0, a3
	l32i	a0, a0, EXC_TABLE_FAST_USER
	jx	a0

.Lfixup:/* Check for a fixup handler or if we were in a critical section. */

	/* a0: depc, a1: a1, a2: a2, a3: trashed, depc: a0, excsave1: a3 */

	movi	a3, exc_table
	s32i	a2, a3, EXC_TABLE_DOUBLE_SAVE	# temporary variable

	/* Enter critical section. */

	l32i	a2, a3, EXC_TABLE_FIXUP
	s32i	a3, a3, EXC_TABLE_FIXUP
	beq	a2, a3, .Lunrecoverable_fixup	# critical!
	beqz	a2, .Ldflt			# no handler was registered

	/* a0: depc, a1: a1, a2: trash, a3: exctable, depc: a0, excsave: a3 */

	jx	a2

.Ldflt:	/* Get stack pointer. */

	l32i	a3, a3, EXC_TABLE_DOUBLE_SAVE
	addi	a2, a3, -PT_USER_SIZE

.Lovfl:	/* Jump to default handlers. */

	/* a0: depc, a1: a1, a2: kstk, a3: a2, depc: a0, excsave: a3 */

	xsr	a3, DEPC
	s32i	a0, a2, PT_DEPC
	s32i	a3, a2, PT_AREG0

	/* a0: avail, a1: a1, a2: kstk, a3: avail, depc: a2, excsave: a3 */

	movi	a3, exc_table
	rsr	a0, EXCCAUSE
	addx4	a0, a0, a3
	l32i	a0, a0, EXC_TABLE_FAST_USER
	jx	a0

	/*
	 * We only allow the ITLB miss exception if we are in kernel space.
	 * All other exceptions are unexpected and thus unrecoverable!
	 */

#ifdef CONFIG_MMU
	.extern fast_second_level_miss_double_kernel

.Lksp:	/* a0: a0, a1: a1, a2: a2, a3: trashed, depc: depc, excsave: a3 */

	rsr	a3, EXCCAUSE
	beqi	a3, EXCCAUSE_ITLB_MISS, 1f
	addi	a3, a3, -EXCCAUSE_DTLB_MISS
	bnez	a3, .Lunrecoverable
1:	movi	a3, fast_second_level_miss_double_kernel
	jx	a3
#else
.equ	.Lksp,	.Lunrecoverable
#endif

	/* Critical! We can't handle this situation. PANIC! */

	.extern unrecoverable_exception

.Lunrecoverable_fixup:
	l32i	a2, a3, EXC_TABLE_DOUBLE_SAVE
	xsr	a0, DEPC

.Lunrecoverable:
	rsr	a3, EXCSAVE_1
	wsr	a0, EXCSAVE_1
	movi	a0, unrecoverable_exception
	callx0	a0

	.end literal_prefix



	.section .DebugInterruptVector.text, "ax"

ENTRY(_DebugInterruptVector)
	xsr	a0, EXCSAVE + XCHAL_DEBUGLEVEL
	jx	a0




	.section		.WindowVectors.text, "ax"


/* 4-Register Window Overflow Vector (Handler) */

	.align 64
.global _WindowOverflow4
_WindowOverflow4:
	s32e	a0, a5, -16
	s32e	a1, a5, -12
	s32e	a2, a5,  -8
	s32e	a3, a5,  -4
	rfwo


/* 4-Register Window Underflow Vector (Handler) */

	.align 64
.global _WindowUnderflow4
_WindowUnderflow4:
	l32e	a0, a5, -16
	l32e	a1, a5, -12
	l32e	a2, a5,  -8
	l32e	a3, a5,  -4
	rfwu


/* 8-Register Window Overflow Vector (Handler) */

	.align 64
.global _WindowOverflow8
_WindowOverflow8:
	s32e	a0, a9, -16
	l32e	a0, a1, -12
	s32e	a2, a9,  -8
	s32e	a1, a9, -12
	s32e	a3, a9,  -4
	s32e	a4, a0, -32
	s32e	a5, a0, -28
	s32e	a6, a0, -24
	s32e	a7, a0, -20
	rfwo

/* 8-Register Window Underflow Vector (Handler) */

	.align 64
.global _WindowUnderflow8
_WindowUnderflow8:
	l32e	a1, a9, -12
	l32e	a0, a9, -16
	l32e	a7, a1, -12
	l32e	a2, a9,  -8
	l32e	a4, a7, -32
	l32e	a3, a9,  -4
	l32e	a5, a7, -28
	l32e	a6, a7, -24
	l32e	a7, a7, -20
	rfwu


/* 12-Register Window Overflow Vector (Handler) */

	.align 64
.global _WindowOverflow12
_WindowOverflow12:
	s32e	a0,  a13, -16
	l32e	a0,  a1,  -12
	s32e	a1,  a13, -12
	s32e	a2,  a13,  -8
	s32e	a3,  a13,  -4
	s32e	a4,  a0,  -48
	s32e	a5,  a0,  -44
	s32e	a6,  a0,  -40
	s32e	a7,  a0,  -36
	s32e	a8,  a0,  -32
	s32e	a9,  a0,  -28
	s32e	a10, a0,  -24
	s32e	a11, a0,  -20
	rfwo

/* 12-Register Window Underflow Vector (Handler) */

	.align 64
.global _WindowUnderflow12
_WindowUnderflow12:
	l32e	a1,  a13, -12
	l32e	a0,  a13, -16
	l32e	a11, a1,  -12
	l32e	a2,  a13,  -8
	l32e	a4,  a11, -48
	l32e	a8,  a11, -32
	l32e	a3,  a13,  -4
	l32e	a5,  a11, -44
	l32e	a6,  a11, -40
	l32e	a7,  a11, -36
	l32e	a9,  a11, -28
	l32e	a10, a11, -24
	l32e	a11, a11, -20
	rfwu

	.text


