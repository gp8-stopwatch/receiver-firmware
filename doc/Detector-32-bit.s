	.cpu cortex-m0
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"Detector.cc"
	.section	.text._ZN3etl11function_fvIXadL_Z12__enable_irqEEED2Ev,"ax",%progbits
	.align	1
	.p2align 2,,3
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZN3etl11function_fvIXadL_Z12__enable_irqEEED2Ev, %function
_ZN3etl11function_fvIXadL_Z12__enable_irqEEED2Ev:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	@ sp needed
	bx	lr
	.size	_ZN3etl11function_fvIXadL_Z12__enable_irqEEED2Ev, .-_ZN3etl11function_fvIXadL_Z12__enable_irqEEED2Ev
	.thumb_set _ZN3etl11function_fvIXadL_Z12__enable_irqEEED1Ev,_ZN3etl11function_fvIXadL_Z12__enable_irqEEED2Ev
	.section	.text._ZN3etl11function_fvIXadL_Z13__disable_irqEEED2Ev,"ax",%progbits
	.align	1
	.p2align 2,,3
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZN3etl11function_fvIXadL_Z13__disable_irqEEED2Ev, %function
_ZN3etl11function_fvIXadL_Z13__disable_irqEEED2Ev:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	@ sp needed
	bx	lr
	.size	_ZN3etl11function_fvIXadL_Z13__disable_irqEEED2Ev, .-_ZN3etl11function_fvIXadL_Z13__disable_irqEEED2Ev
	.thumb_set _ZN3etl11function_fvIXadL_Z13__disable_irqEEED1Ev,_ZN3etl11function_fvIXadL_Z13__disable_irqEEED2Ev
	.section	.text._ZNK3etl11function_fvIXadL_Z12__enable_irqEEEclEv,"ax",%progbits
	.align	1
	.p2align 2,,3
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZNK3etl11function_fvIXadL_Z12__enable_irqEEEclEv, %function
_ZNK3etl11function_fvIXadL_Z12__enable_irqEEEclEv:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	@ sp needed
	bx	lr
	.size	_ZNK3etl11function_fvIXadL_Z12__enable_irqEEEclEv, .-_ZNK3etl11function_fvIXadL_Z12__enable_irqEEEclEv
	.section	.text._ZNK3etl11function_fvIXadL_Z13__disable_irqEEEclEv,"ax",%progbits
	.align	1
	.p2align 2,,3
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZNK3etl11function_fvIXadL_Z13__disable_irqEEEclEv, %function
_ZNK3etl11function_fvIXadL_Z13__disable_irqEEEclEv:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	@ sp needed
	bx	lr
	.size	_ZNK3etl11function_fvIXadL_Z13__disable_irqEEEclEv, .-_ZNK3etl11function_fvIXadL_Z13__disable_irqEEEclEv
	.section	.text._ZN3etl11function_fvIXadL_Z12__enable_irqEEED0Ev,"ax",%progbits
	.align	1
	.p2align 2,,3
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZN3etl11function_fvIXadL_Z12__enable_irqEEED0Ev, %function
_ZN3etl11function_fvIXadL_Z12__enable_irqEEED0Ev:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	movs	r4, r0
	movs	r1, #4
	bl	_ZdlPvj
	@ sp needed
	movs	r0, r4
	pop	{r4, pc}
	.size	_ZN3etl11function_fvIXadL_Z12__enable_irqEEED0Ev, .-_ZN3etl11function_fvIXadL_Z12__enable_irqEEED0Ev
	.section	.text._ZN3etl11function_fvIXadL_Z13__disable_irqEEED0Ev,"ax",%progbits
	.align	1
	.p2align 2,,3
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZN3etl11function_fvIXadL_Z13__disable_irqEEED0Ev, %function
_ZN3etl11function_fvIXadL_Z13__disable_irqEEED0Ev:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	movs	r4, r0
	movs	r1, #4
	bl	_ZdlPvj
	@ sp needed
	movs	r0, r4
	pop	{r4, pc}
	.size	_ZN3etl11function_fvIXadL_Z13__disable_irqEEED0Ev, .-_ZN3etl11function_fvIXadL_Z13__disable_irqEEED0Ev
	.section	.text._ZN10EdgeFilter6onEdgeERK4Edge,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	_ZN10EdgeFilter6onEdgeERK4Edge
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZN10EdgeFilter6onEdgeERK4Edge, %function
_ZN10EdgeFilter6onEdgeERK4Edge:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	mov	r5, r8
	mov	lr, fp
	mov	r7, r10
	mov	r6, r9
	push	{r5, r6, r7, lr}
	ldrb	r3, [r0, #16]
	sub	sp, sp, #28
	movs	r4, r0
	movs	r5, r1
	cmp	r3, #0
	bne	.L49
	movs	r3, #132
	ldr	r2, [r1, #12]
	ldr	r3, [r0, r3]
	str	r3, [sp, #4]
	cmp	r3, r2
	bne	.LCB99
	b	.L44	@long jump
.LCB99:
.L49:
	movs	r3, #24
	mov	ip, r3
	add	ip, ip, r4
	mov	r3, ip
	str	r3, [sp, #4]
	movs	r3, #40
	mov	r8, r3
	adds	r3, r3, #16
	mov	r10, r3
	adds	r3, r3, #16
	mov	r9, r3
	adds	r3, r3, #48
	movs	r7, r4
	movs	r6, r4
	mov	fp, r3
	add	r8, r8, r4
	add	r10, r10, r4
	add	r9, r9, r4
	adds	r7, r7, #88
	adds	r6, r6, #104
	add	fp, fp, r4
.L9:
	movs	r2, #16
	mov	r1, r8
	ldr	r0, [sp, #4]
	bl	memcpy
	movs	r2, #16
	mov	r1, r10
	mov	r0, r8
	bl	memcpy
	movs	r2, #16
	mov	r1, r9
	mov	r0, r10
	bl	memcpy
	movs	r2, #16
	movs	r1, r7
	mov	r0, r9
	bl	memcpy
	movs	r2, #16
	movs	r1, r6
	movs	r0, r7
	bl	memcpy
	movs	r2, #16
	mov	r1, fp
	movs	r0, r6
	bl	memcpy
	movs	r2, #16
	movs	r1, r5
	mov	r0, fp
	bl	memcpy
	movs	r3, #0
	strb	r3, [r4, #16]
	ldrb	r3, [r4, #4]
	cmp	r3, #0
	bne	.LCB158
	b	.L8	@long jump
.LCB158:
	movs	r3, #200
	ldr	r3, [r4, r3]
	cmp	r3, #1
	bne	.LCB162
	b	.L8	@long jump
.LCB162:
	movs	r6, #128
	ldr	r2, [r4, #32]
	ldr	r3, [r4, r6]
	subs	r7, r3, r2
	bl	_Z9getConfigv
	ldrb	r3, [r0, #2]
	ldr	r1, [r4, #80]
	muls	r7, r3
	ldr	r0, [r4, #48]
	ldr	r3, [r4, #64]
	ldr	r2, [r4, #112]
	str	r3, [sp, #8]
	mov	r9, r7
	ldr	r3, [r4, #32]
	adds	r7, r0, r1
	adds	r7, r7, r2
	subs	r7, r7, r3
	ldr	r3, [sp, #8]
	subs	r7, r7, r3
	ldr	r3, [r4, #96]
	subs	r3, r7, r3
	mov	ip, r3
	ldr	r3, [r4, #96]
	ldr	r7, [r4, r6]
	mov	r10, r3
	ldr	r3, [sp, #8]
	ldr	r6, [sp, #4]
	add	r3, r3, r10
	adds	r3, r3, r7
	subs	r0, r3, r0
	ldr	r3, [r4, #36]
	subs	r1, r0, r1
	subs	r2, r1, r2
	mov	r0, r8
	cmp	r3, #1
	bne	.LCB205
	b	.L13	@long jump
.LCB205:
	ldr	r3, [r4, #8]
	cmp	r3, r2
	bls	.L14
	movs	r1, #172
	ldr	r3, [r4, r1]
	adds	r3, r3, #1
	str	r3, [r4, r1]
	movs	r3, #100
	muls	r2, r3
	cmp	r2, r9
	bhi	.LCB218
	b	.L51	@long jump
.LCB218:
.L14:
	movs	r3, #140
	ldr	r2, [r4, r3]
	cmp	r2, #1
	bne	.LCB224
	b	.L50	@long jump
.LCB224:
	movs	r6, r4
	movs	r2, #1
	adds	r6, r6, #144
	str	r2, [r4, r3]
	movs	r1, #1
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	movs	r7, r4
	str	r2, [r6]
	str	r3, [r6, #4]
	ldr	r0, .L56
	bl	_ZN4Gpio3setEb
	adds	r7, r7, #152
.L18:
	ldr	r3, [r6, #4]
	ldr	r1, [r7, #4]
	ldr	r2, [r6]
	ldr	r0, [r7]
	cmp	r1, r3
	bhi	.LCB244
	b	.L52	@long jump
.LCB244:
.L29:
	movs	r1, #160
	mov	r8, r1
	add	r8, r8, r4
	mov	r1, r8
	ldr	r1, [r1, #4]
	cmp	r1, r3
	bcc	.LCB252
	b	.L53	@long jump
.LCB252:
.L30:
	ldr	r1, [r4, #8]
	mov	r9, r1
	subs	r1, r0, r2
	cmp	r1, r9
	bcc	.L8
	ldr	r1, [r5]
	subs	r0, r1, r0
	cmp	r9, r0
	bhi	.L8
	movs	r1, #188
	ldr	r1, [r4, r1]
	cmp	r1, #0
	bne	.L8
	adds	r1, r1, #168
	ldr	r1, [r4, r1]
	cmp	r1, #0
	bne	.L8
	adds	r1, r1, #136
	ldr	r0, [r4, r1]
	ldr	r1, [r0]
	ldr	r1, [r1]
	mov	r9, r1
	movs	r1, #0
	blx	r9
	movs	r1, #128
	movs	r0, #144
	lsls	r1, r1, #2
	lsls	r0, r0, #23
	bl	HAL_GPIO_TogglePin
	bl	_Z9getConfigv
	ldrh	r3, [r0, #4]
	cmp	r3, #0
	beq	.L24
	movs	r3, #200
	movs	r2, #1
	str	r2, [r4, r3]
	movs	r3, r4
	ldr	r1, [r5]
	ldr	r2, [r5, #4]
	adds	r3, r3, #208
	str	r1, [r3]
	str	r2, [r3, #4]
.L24:
	ldr	r2, [r7]
	ldr	r3, [r7, #4]
	mov	r1, r8
	str	r2, [r1]
	str	r3, [r1, #4]
	str	r2, [r6]
	str	r3, [r6, #4]
	movs	r3, #140
	movs	r2, #2
	str	r2, [r4, r3]
.L8:
	add	sp, sp, #28
	@ sp needed
	pop	{r2, r3, r4, r5}
	mov	r8, r2
	mov	r9, r3
	mov	r10, r4
	mov	fp, r5
	pop	{r4, r5, r6, r7, pc}
.L44:
	movs	r3, #24
	mov	ip, r3
	adds	r3, r3, #16
	mov	r8, r3
	add	ip, ip, r0
	mov	r0, ip
	add	r8, r8, r4
	movs	r2, #16
	mov	r1, r8
	str	r0, [sp, #4]
	bl	memcpy
	movs	r3, #56
	mov	r10, r3
	add	r10, r10, r4
	movs	r2, #16
	mov	r1, r10
	mov	r0, r8
	bl	memcpy
	movs	r3, #72
	mov	r9, r3
	movs	r7, r4
	movs	r6, r4
	add	r9, r9, r4
	adds	r7, r7, #88
	movs	r2, #16
	mov	r1, r9
	mov	r0, r10
	bl	memcpy
	adds	r6, r6, #104
	movs	r2, #16
	movs	r1, r7
	mov	r0, r9
	bl	memcpy
	movs	r2, #16
	movs	r1, r6
	movs	r0, r7
	bl	memcpy
	movs	r3, #120
	mov	fp, r3
	add	fp, fp, r4
	movs	r2, #16
	mov	r1, fp
	movs	r0, r6
	bl	memcpy
	movs	r2, #16
	movs	r1, r5
	mov	r0, fp
	bl	memcpy
	ldr	r2, [r5]
	ldr	r3, [r5, #4]
	ldr	r1, [r5, #12]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	ldr	r3, [r5, #8]
	movs	r2, #16
	str	r3, [sp, #16]
	subs	r3, r1, #1
	movs	r1, r3
	subs	r3, r1, #1
	sbcs	r1, r1, r3
	mov	r0, r8
	str	r1, [sp, #20]
	mov	r1, r10
	bl	memcpy
	movs	r2, #16
	mov	r1, r9
	mov	r0, r10
	bl	memcpy
	movs	r2, #16
	movs	r1, r7
	mov	r0, r9
	bl	memcpy
	movs	r2, #16
	movs	r1, r6
	movs	r0, r7
	bl	memcpy
	movs	r2, #16
	mov	r1, fp
	movs	r0, r6
	bl	memcpy
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #12]
	str	r2, [r4, #120]
	str	r3, [r4, #124]
	movs	r3, #128
	ldr	r2, [sp, #16]
	ldr	r1, [sp, #20]
	str	r2, [r4, r3]
	adds	r3, r3, #4
	str	r1, [r4, r3]
	b	.L9
.L13:
	ldr	r6, [r4, #8]
	cmp	r6, r2
	bls	.L16
	movs	r1, #172
	ldr	r3, [r4, r1]
	adds	r3, r3, #1
	str	r3, [r4, r1]
.L16:
	movs	r1, #100
	mov	r3, ip
	muls	r3, r1
	cmp	r9, r3
	bcs	.L54
	ldr	r0, [sp, #4]
	b	.L14
.L55:
	mov	r6, r8
.L20:
	movs	r3, #140
	ldr	r2, [r4, r3]
	cmp	r2, #2
	beq	.L50
	movs	r2, #2
	str	r2, [r4, r3]
	movs	r3, r4
	ldr	r1, [r6]
	ldr	r2, [r6, #4]
	adds	r3, r3, #160
	str	r1, [r3]
	str	r2, [r3, #4]
	ldr	r0, .L56
	movs	r1, #0
	bl	_ZN4Gpio3setEb
.L50:
	movs	r6, r4
	movs	r7, r4
	adds	r6, r6, #144
	adds	r7, r7, #152
	ldr	r3, [r6, #4]
	ldr	r1, [r7, #4]
	ldr	r2, [r6]
	ldr	r0, [r7]
	cmp	r1, r3
	bls	.LCB468
	b	.L29	@long jump
.LCB468:
.L52:
	cmp	r1, r3
	beq	.LCB471
	b	.L8	@long jump
.LCB471:
	cmp	r0, r2
	bls	.LCB473
	b	.L29	@long jump
.LCB473:
	b	.L8
.L53:
	cmp	r1, r3
	beq	.LCB479
	b	.L8	@long jump
.LCB479:
	mov	r1, r8
	ldr	r1, [r1]
	cmp	r1, r2
	bcs	.LCB484
	b	.L30	@long jump
.LCB484:
	b	.L8
.L54:
	muls	r1, r2
	cmp	r1, r9
	bcs	.L19
	cmp	r6, r2
	bhi	.L55
.L19:
	movs	r3, #140
	ldr	r2, [r4, r3]
	cmp	r2, #0
	beq	.L50
	movs	r7, r4
	movs	r2, #0
	movs	r6, r4
	adds	r7, r7, #152
	str	r2, [r4, r3]
	movs	r1, #0
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	ldr	r0, .L56
	str	r2, [r7]
	str	r3, [r7, #4]
	bl	_ZN4Gpio3setEb
	adds	r6, r6, #144
	b	.L18
.L51:
	mov	r2, ip
	muls	r3, r2
	cmp	r3, r9
	bcc	.L20
	movs	r0, r6
	b	.L19
.L57:
	.align	2
.L56:
	.word	.LANCHOR0
	.size	_ZN10EdgeFilter6onEdgeERK4Edge, .-_ZN10EdgeFilter6onEdgeERK4Edge
	.global	__aeabi_uldivmod
	.global	__aeabi_ldivmod
	.global	__aeabi_lmul
	.section	.text._ZN10EdgeFilter3runEv,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	_ZN10EdgeFilter3runEv
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZN10EdgeFilter3runEv, %function
_ZN10EdgeFilter3runEv:
	@ args = 0, pretend = 0, frame = 48
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	mov	r5, r8
	mov	lr, fp
	mov	r7, r10
	mov	r6, r9
	push	{r5, r6, r7, lr}
	ldrb	r3, [r0, #4]
	sub	sp, sp, #52
	movs	r5, r0
	cmp	r3, #0
	beq	.L58
	ldrb	r4, [r0, #16]
	cmp	r4, #0
	beq	.L116
.L58:
	add	sp, sp, #52
	@ sp needed
	pop	{r2, r3, r4, r5}
	mov	r8, r2
	mov	r9, r3
	mov	r10, r4
	mov	fp, r5
	pop	{r4, r5, r6, r7, pc}
.L116:
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r2, [r0, #120]
	ldr	r3, [r0, #124]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	movs	r3, #132
	movs	r2, #128
	ldr	r3, [r0, r3]
	lsls	r2, r2, #23
	mov	r8, r3
	ldr	r3, .L130
	ldr	r3, [r3, #36]
	ldr	r2, [r2, #36]
	str	r3, [sp, #4]
	str	r2, [sp]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r3, #200
	ldr	r3, [r0, r3]
	cmp	r3, #1
	bne	.LCB582
	b	.L117	@long jump
.LCB582:
.L62:
	ldr	r1, [sp, #8]
	ldr	r2, [sp, #12]
	ldr	r3, [sp]
	ldr	r4, [sp, #4]
	subs	r3, r3, r1
	sbcs	r4, r4, r2
	movs	r1, #0
	mov	fp, r1
	ldr	r2, [r5, #8]
	str	r3, [sp, #16]
	str	r4, [sp, #20]
	str	r2, [sp, #32]
	cmp	fp, r4
	bne	.LCB593
	b	.L118	@long jump
.LCB593:
.L89:
	movs	r3, #140
	mov	r1, r8
	ldr	r2, [r5, r3]
	cmp	r1, #1
	bne	.LCB599
	b	.L119	@long jump
.LCB599:
	cmp	r2, #0
	beq	.LCB601
	b	.L113	@long jump
.LCB601:
.L115:
	movs	r3, #144
.L114:
	mov	r10, r3
	adds	r3, r3, #8
	mov	r9, r3
	add	r10, r10, r5
	add	r9, r9, r5
.L64:
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r3, #140
	ldr	r3, [r5, r3]
	mov	r8, r3
	mov	r3, r10
	ldr	r4, [r3, #4]
	ldr	r3, [r3]
	str	r3, [sp, #8]
	str	r4, [sp, #12]
	mov	r3, r9
	ldr	r4, [r3, #4]
	ldr	r3, [r3]
	str	r3, [sp, #24]
	str	r4, [sp, #28]
	movs	r3, #160
	mov	ip, r3
	add	ip, ip, r5
	mov	r3, ip
	ldr	r2, [r3]
	str	r3, [sp, #36]
	ldr	r3, [r3, #4]
	str	r2, [sp, #44]
	str	r3, [sp, #40]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r3, r5
	ldr	r0, [sp]
	ldr	r1, [sp, #4]
	adds	r3, r3, #176
	ldr	r6, [r3]
	ldr	r7, [r3, #4]
	subs	r0, r0, r6
	sbcs	r1, r1, r7
	cmp	r1, #0
	bne	.LCB640
	b	.L120	@long jump
.LCB640:
.L90:
	movs	r2, #188
	ldr	r2, [r5, r2]
	cmp	r2, #1
	beq	.L67
	ldr	r1, [sp]
	ldr	r2, [sp, #4]
	str	r1, [r3]
	str	r2, [r3, #4]
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r3, #172
	movs	r2, #0
	ldr	r0, [r5, r3]
	str	r2, [r5, r3]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	cmp	r0, #0
	bgt	.LCB655
	b	.L69	@long jump
.LCB655:
	movs	r3, #175
	lsls	r3, r3, #3
	muls	r0, r3
	ldr	r2, .L130+4
	movs	r3, #0
	movs	r1, #0
	bl	__aeabi_uldivmod
	adds	r4, r0, #0
	uxtb	r0, r0
	cmp	r0, #14
	bls	.LCB666
	b	.L121	@long jump
.LCB666:
.L70:
	adds	r4, r4, #1
	uxtb	r2, r4
	movs	r3, #184
	movs	r6, r2
	strb	r2, [r5, r3]
.L71:
	movs	r4, #168
	ldr	r3, [r5, r4]
	cmp	r3, #0
	bne	.LCB678
	b	.L122	@long jump
.LCB678:
.L72:
	cmp	r3, #1
	bne	.LCB681
	b	.L123	@long jump
.LCB681:
.L67:
	bl	_Z9getConfigv
	ldr	r3, [sp]
	ldr	r4, [sp, #4]
	movs	r2, r5
	adds	r2, r2, #192
	ldr	r6, [r2]
	ldr	r7, [r2, #4]
	ldrh	r1, [r0, #6]
	subs	r3, r3, r6
	sbcs	r4, r4, r7
	movs	r0, #250
	movs	r6, r3
	movs	r7, r4
	adds	r3, r1, #0
	lsls	r0, r0, #2
	cmp	r1, r0
	bcs	.L76
	adds	r3, r0, #0
.L76:
	uxth	r1, r3
	lsls	r3, r1, #5
	subs	r3, r3, r1
	lsls	r3, r3, #2
	adds	r3, r3, r1
	lsls	r3, r3, #3
	cmp	r7, #0
	bne	.L91
	cmp	r3, r6
	bls	.L91
.L102:
	ldr	r3, [sp, #16]
	ldr	r4, [sp, #20]
	cmp	fp, r4
	bne	.LCB716
	b	.L124	@long jump
.LCB716:
.L92:
	mov	r3, r8
	cmp	r3, #0
	beq	.LCB720
	b	.L58	@long jump
.LCB720:
	ldr	r1, [sp, #24]
	ldr	r2, [sp, #28]
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	cmp	r2, r4
	bhi	.LCB724
	b	.L125	@long jump
.LCB724:
.L93:
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	ldr	r2, [sp, #40]
	cmp	r4, r2
	bhi	.LCB729
	b	.L126	@long jump
.LCB729:
.L94:
	ldr	r0, [sp, #24]
	ldr	r1, [sp, #28]
	ldr	r6, [sp, #8]
	ldr	r7, [sp, #12]
	movs	r3, #0
	subs	r0, r0, r6
	sbcs	r1, r1, r7
	ldr	r2, [r5, #8]
	cmp	r3, r1
	bne	.LCB737
	b	.L127	@long jump
.LCB737:
.L95:
	ldr	r0, [sp]
	ldr	r1, [sp, #4]
	ldr	r6, [sp, #24]
	ldr	r7, [sp, #28]
	subs	r0, r0, r6
	sbcs	r1, r1, r7
	cmp	r3, r1
	bne	.LCB743
	b	.L128	@long jump
.LCB743:
.L96:
	movs	r3, #188
	ldr	r3, [r5, r3]
	cmp	r3, #0
	beq	.LCB748
	b	.L58	@long jump
.LCB748:
	adds	r3, r3, #168
	ldr	r3, [r5, r3]
	cmp	r3, #0
	beq	.LCB752
	b	.L58	@long jump
.LCB752:
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	adds	r3, r3, #136
	ldr	r0, [r5, r3]
	movs	r1, #0
	ldr	r3, [r0]
	ldr	r4, [r3]
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #12]
	blx	r4
	movs	r1, #128
	movs	r0, #144
	lsls	r1, r1, #2
	lsls	r0, r0, #23
	bl	HAL_GPIO_TogglePin
	bl	_Z9getConfigv
	ldrh	r3, [r0, #4]
	cmp	r3, #0
	beq	.L87
	movs	r3, #200
	movs	r2, #1
	str	r2, [r5, r3]
	movs	r3, r5
	ldr	r1, [sp]
	ldr	r2, [sp, #4]
	adds	r3, r3, #208
	str	r1, [r3]
	str	r2, [r3, #4]
	b	.L87
.L118:
	cmp	r2, r3
	bhi	.LCB782
	b	.L89	@long jump
.LCB782:
	b	.L115
.L117:
	movs	r3, r0
	ldr	r6, [sp]
	ldr	r7, [sp, #4]
	adds	r3, r3, #208
	ldr	r2, [r3]
	ldr	r3, [r3, #4]
	subs	r6, r6, r2
	sbcs	r7, r7, r3
	bl	_Z9getConfigv
	ldrh	r2, [r0, #4]
	lsls	r3, r2, #5
	subs	r3, r3, r2
	lsls	r3, r3, #2
	adds	r3, r3, r2
	lsls	r3, r3, #3
	cmp	r4, r7
	beq	.L129
.L88:
	movs	r3, #200
	movs	r2, #0
	str	r2, [r5, r3]
	b	.L62
.L91:
	movs	r3, #168
	ldr	r3, [r5, r3]
	cmp	r3, #1
	beq	.L102
	ldr	r3, [sp]
	ldr	r4, [sp, #4]
	str	r3, [r2]
	str	r4, [r2, #4]
	movs	r2, #188
	ldr	r3, [r5, r2]
	cmp	r3, #0
	bne	.L78
	mov	r3, r8
	cmp	r3, #1
	beq	.LCB822
	b	.L102	@long jump
.LCB822:
	bl	_Z9getConfigv
	movs	r2, r6
	movs	r3, r7
	ldrb	r0, [r0, #2]
	movs	r1, #0
	bl	__aeabi_lmul
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #12]
	movs	r4, r1
	mov	r8, r0
	ldr	r0, [sp]
	ldr	r1, [sp, #4]
	subs	r0, r0, r2
	sbcs	r1, r1, r3
	movs	r2, #100
	movs	r3, #0
	bl	__aeabi_lmul
	cmp	r4, r1
	bls	.LCB839
	b	.L58	@long jump
.LCB839:
	bne	.L79
	cmp	r8, r0
	bls	.LCB843
	b	.L58	@long jump
.LCB843:
.L79:
	movs	r3, #188
	movs	r2, #1
	str	r2, [r5, r3]
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	subs	r3, r3, #52
	ldr	r0, [r5, r3]
	movs	r1, #3
	ldr	r3, [r0]
	ldr	r4, [r3]
	ldr	r2, [sp]
	ldr	r3, [sp, #4]
	blx	r4
	b	.L87
.L78:
	cmp	r3, #1
	beq	.LCB861
	b	.L102	@long jump
.LCB861:
	mov	r3, r8
	cmp	r3, #0
	beq	.LCB864
	b	.L58	@long jump
.LCB864:
	str	r3, [r5, r2]
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	adds	r3, r3, #136
	ldr	r0, [r5, r3]
	movs	r1, #4
	ldr	r3, [r0]
	ldr	r4, [r3]
	ldr	r2, [sp]
	ldr	r3, [sp, #4]
	blx	r4
.L87:
	mov	r3, r9
	ldr	r1, [sp, #36]
	ldr	r2, [r3]
	ldr	r3, [r3, #4]
	str	r2, [r1]
	str	r3, [r1, #4]
	mov	r1, r10
	str	r2, [r1]
	str	r3, [r1, #4]
	movs	r3, #140
	movs	r2, #2
	str	r2, [r5, r3]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L58
.L121:
	movs	r4, #14
	b	.L70
.L125:
	cmp	r2, r4
	beq	.LCB896
	b	.L58	@long jump
.LCB896:
	cmp	r1, r3
	bls	.LCB898
	b	.L93	@long jump
.LCB898:
	b	.L58
.L129:
	cmp	r3, r6
	bls	.LCB904
	b	.L58	@long jump
.LCB904:
	b	.L88
.L113:
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r2, #0
	str	r2, [r5, r3]
	movs	r3, #152
	mov	r9, r3
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	add	r9, r9, r5
	mov	r2, r9
	str	r3, [r2]
	str	r4, [r2, #4]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #0
	ldr	r0, .L130+8
	bl	_ZN4Gpio3setEb
	movs	r3, #144
	mov	r10, r3
	add	r10, r10, r5
	b	.L64
.L120:
	ldr	r2, .L130+12
	cmp	r0, r2
	bls	.LCB931
	b	.L90	@long jump
.LCB931:
	b	.L67
.L124:
	ldr	r2, [sp, #32]
	cmp	r2, r3
	bhi	.LCB938
	b	.L92	@long jump
.LCB938:
	b	.L58
.L119:
	cmp	r2, #1
	bne	.L112
	adds	r3, r3, #4
	b	.L114
.L69:
	movs	r3, #184
	strb	r2, [r5, r3]
	movs	r3, #0
	movs	r6, r3
	b	.L71
.L131:
	.align	2
.L130:
	.word	1073742848
	.word	1000000
	.word	.LANCHOR0
	.word	999999
.L122:
	bl	_Z9getConfigv
	ldrb	r3, [r0, #10]
	cmp	r3, r6
	bls	.L73
	ldr	r3, [r5, r4]
	b	.L72
.L112:
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r5, r3]
	movs	r3, #144
	mov	r10, r3
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	add	r10, r10, r5
	mov	r2, r10
	str	r3, [r2]
	str	r4, [r2, #4]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #1
	ldr	r0, .L132
	bl	_ZN4Gpio3setEb
	movs	r3, #152
	mov	r9, r3
	add	r9, r9, r5
	b	.L64
.L123:
	movs	r3, #184
	ldrb	r4, [r5, r3]
	bl	_Z9getConfigv
	ldrb	r3, [r0, #11]
	cmp	r3, r4
	bcs	.LCB1000
	b	.L67	@long jump
.LCB1000:
	movs	r3, #168
	movs	r2, #0
	str	r2, [r5, r3]
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	subs	r3, r3, #32
	ldr	r0, [r5, r3]
	movs	r1, #2
	ldr	r3, [r0]
	ldr	r4, [r3]
	ldr	r2, [sp]
	ldr	r3, [sp, #4]
	blx	r4
	b	.L87
.L126:
	cmp	r4, r2
	beq	.LCB1017
	b	.L58	@long jump
.LCB1017:
	ldr	r2, [sp, #44]
	cmp	r3, r2
	bls	.LCB1020
	b	.L94	@long jump
.LCB1020:
	b	.L58
.L73:
	movs	r3, #1
	str	r3, [r5, r4]
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	adds	r3, r3, #135
	ldr	r0, [r5, r3]
	movs	r1, #1
	ldr	r3, [r0]
	ldr	r4, [r3]
	ldr	r2, [sp]
	ldr	r3, [sp, #4]
	blx	r4
	b	.L87
.L127:
	cmp	r2, r0
	bhi	.LCB1040
	b	.L95	@long jump
.LCB1040:
	b	.L58
.L128:
	cmp	r2, r0
	bhi	.LCB1046
	b	.L96	@long jump
.LCB1046:
	b	.L58
.L133:
	.align	2
.L132:
	.word	.LANCHOR0
	.size	_ZN10EdgeFilter3runEv, .-_ZN10EdgeFilter3runEv
	.section	.text._ZN10EdgeFilter20recalculateConstantsEv,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	_ZN10EdgeFilter20recalculateConstantsEv
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_ZN10EdgeFilter20recalculateConstantsEv, %function
_ZN10EdgeFilter20recalculateConstantsEv:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	movs	r4, r0
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	bl	_Z9getConfigv
	ldrh	r1, [r0, #6]
	lsls	r3, r1, #5
	subs	r3, r3, r1
	lsls	r3, r3, #2
	adds	r3, r3, r1
	lsls	r3, r3, #3
	str	r3, [r4, #8]
	bl	_Z9getConfigv
	ldrb	r3, [r0, #1]
	lsls	r3, r3, #28
	lsrs	r3, r3, #31
	strb	r3, [r4, #4]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	@ sp needed
	pop	{r4, pc}
	.size	_ZN10EdgeFilter20recalculateConstantsEv, .-_ZN10EdgeFilter20recalculateConstantsEv
	.section	.text.startup._GLOBAL__sub_I_consoleRx,"ax",%progbits
	.align	1
	.p2align 2,,3
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	_GLOBAL__sub_I_consoleRx, %function
_GLOBAL__sub_I_consoleRx:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	mov	lr, r9
	mov	r7, r8
	push	{r7, lr}
	ldr	r4, .L136
	ldr	r3, .L136+4
	ldr	r0, .L136+8
	sub	sp, sp, #20
	adds	r3, r3, #8
	movs	r2, r4
	ldr	r1, .L136+12
	str	r3, [r0]
	bl	__aeabi_atexit
	ldr	r3, .L136+16
	ldr	r0, .L136+20
	adds	r3, r3, #8
	movs	r2, r4
	ldr	r1, .L136+24
	str	r3, [r0]
	bl	__aeabi_atexit
	movs	r3, #144
	lsls	r3, r3, #23
	mov	r8, r3
	movs	r3, #3
	movs	r5, #0
	movs	r2, #128
	mov	r9, r3
	ldr	r6, .L136+28
	str	r3, [sp, #4]
	mov	r1, r8
	subs	r3, r3, #2
	str	r5, [sp, #8]
	str	r5, [sp]
	lsls	r2, r2, #3
	movs	r0, r6
	bl	_ZN4GpioC1EP12GPIO_TypeDefmmmmm
	ldr	r7, .L136+32
	movs	r2, r4
	movs	r1, r7
	movs	r0, r6
	bl	__aeabi_atexit
	mov	r3, r9
	movs	r2, #128
	ldr	r6, .L136+36
	str	r3, [sp, #4]
	mov	r1, r8
	subs	r3, r3, #2
	str	r5, [sp, #8]
	str	r5, [sp]
	lsls	r2, r2, #2
	movs	r0, r6
	bl	_ZN4GpioC1EP12GPIO_TypeDefmmmmm
	movs	r2, r4
	movs	r1, r7
	movs	r0, r6
	bl	__aeabi_atexit
	add	sp, sp, #20
	@ sp needed
	pop	{r2, r3}
	mov	r8, r2
	mov	r9, r3
	pop	{r4, r5, r6, r7, pc}
.L137:
	.align	2
.L136:
	.word	__dso_handle
	.word	.LANCHOR2
	.word	.LANCHOR1
	.word	_ZN3etl11function_fvIXadL_Z13__disable_irqEEED1Ev
	.word	.LANCHOR4
	.word	.LANCHOR3
	.word	_ZN3etl11function_fvIXadL_Z12__enable_irqEEED1Ev
	.word	.LANCHOR0
	.word	_ZN4GpioD1Ev
	.word	.LANCHOR5
	.size	_GLOBAL__sub_I_consoleRx, .-_GLOBAL__sub_I_consoleRx
	.section	.init_array,"aw",%init_array
	.align	2
	.word	_GLOBAL__sub_I_consoleRx(target1)
	.global	consoleTx
	.global	consoleRx
	.section	.bss._ZL4lock,"aw",%nobits
	.align	2
	.set	.LANCHOR1,. + 0
	.type	_ZL4lock, %object
	.size	_ZL4lock, 4
_ZL4lock:
	.space	4
	.section	.bss._ZL6unlock,"aw",%nobits
	.align	2
	.set	.LANCHOR3,. + 0
	.type	_ZL6unlock, %object
	.size	_ZL6unlock, 4
_ZL6unlock:
	.space	4
	.section	.bss.consoleRx,"aw",%nobits
	.align	2
	.set	.LANCHOR0,. + 0
	.type	consoleRx, %object
	.size	consoleRx, 24
consoleRx:
	.space	24
	.section	.bss.consoleTx,"aw",%nobits
	.align	2
	.set	.LANCHOR5,. + 0
	.type	consoleTx, %object
	.size	consoleTx, 24
consoleTx:
	.space	24
	.section	.rodata._ZTVN3etl11function_fvIXadL_Z12__enable_irqEEEE,"a",%progbits
	.align	2
	.set	.LANCHOR4,. + 0
	.type	_ZTVN3etl11function_fvIXadL_Z12__enable_irqEEEE, %object
	.size	_ZTVN3etl11function_fvIXadL_Z12__enable_irqEEEE, 20
_ZTVN3etl11function_fvIXadL_Z12__enable_irqEEEE:
	.word	0
	.word	0
	.word	_ZN3etl11function_fvIXadL_Z12__enable_irqEEED1Ev
	.word	_ZN3etl11function_fvIXadL_Z12__enable_irqEEED0Ev
	.word	_ZNK3etl11function_fvIXadL_Z12__enable_irqEEEclEv
	.section	.rodata._ZTVN3etl11function_fvIXadL_Z13__disable_irqEEEE,"a",%progbits
	.align	2
	.set	.LANCHOR2,. + 0
	.type	_ZTVN3etl11function_fvIXadL_Z13__disable_irqEEEE, %object
	.size	_ZTVN3etl11function_fvIXadL_Z13__disable_irqEEEE, 20
_ZTVN3etl11function_fvIXadL_Z13__disable_irqEEEE:
	.word	0
	.word	0
	.word	_ZN3etl11function_fvIXadL_Z13__disable_irqEEED1Ev
	.word	_ZN3etl11function_fvIXadL_Z13__disable_irqEEED0Ev
	.word	_ZNK3etl11function_fvIXadL_Z13__disable_irqEEEclEv
	.hidden	__dso_handle
	.ident	"GCC: (crosstool-NG crosstool-ng-1.23.0-242-g521d6c17) 7.1.1 20170707"
