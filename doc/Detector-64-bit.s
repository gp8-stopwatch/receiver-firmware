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
	.global	__aeabi_lmul
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
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	mov	lr, fp
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7, lr}
	ldrb	r3, [r0, #16]
	sub	sp, sp, #36
	mov	r10, r0
	mov	fp, r1
	cmp	r3, #0
	bne	.L61
	movs	r2, #128
	ldr	r3, [r0, r2]
	str	r3, [sp]
	ldr	r2, [sp]
	ldr	r3, [r1, #8]
	cmp	r2, r3
	bne	.LCB100
	b	.L58	@long jump
.LCB100:
.L61:
	movs	r3, #24
	add	r3, r3, r10
	str	r3, [sp]
	movs	r3, #40
	add	r3, r3, r10
	mov	r9, r3
	movs	r3, #120
	mov	r7, r10
	mov	r6, r10
	mov	r5, r10
	mov	r4, r10
	add	r3, r3, r10
	mov	r8, r3
	adds	r7, r7, #56
	adds	r6, r6, #72
	adds	r5, r5, #88
	adds	r4, r4, #104
.L9:
	movs	r2, #16
	mov	r1, r9
	ldr	r0, [sp]
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
	movs	r1, r5
	movs	r0, r6
	bl	memcpy
	movs	r2, #16
	movs	r1, r4
	movs	r0, r5
	bl	memcpy
	movs	r2, #16
	mov	r1, r8
	movs	r0, r4
	bl	memcpy
	movs	r2, #16
	mov	r1, fp
	mov	r0, r8
	bl	memcpy
	movs	r3, #0
	mov	r2, r10
	strb	r3, [r2, #16]
	ldrb	r3, [r2, #4]
	cmp	r3, #0
	bne	.LCB154
	b	.L8	@long jump
.LCB154:
	movs	r3, #200
	ldr	r3, [r2, r3]
	cmp	r3, #1
	bne	.LCB158
	b	.L8	@long jump
.LCB158:
	ldr	r4, [r2, #120]
	ldr	r5, [r2, #124]
	ldr	r3, [r2, #28]
	ldr	r2, [r2, #24]
	subs	r4, r4, r2
	sbcs	r5, r5, r3
	bl	_Z9getConfigv
	movs	r2, r4
	movs	r3, r5
	ldrb	r0, [r0, #2]
	movs	r1, #0
	bl	__aeabi_lmul
	mov	r2, r10
	mov	r3, r10
	ldr	r6, [r3, #40]
	ldr	r7, [r3, #44]
	ldr	r4, [r3, #28]
	ldr	r3, [r3, #24]
	str	r3, [sp, #16]
	str	r4, [sp, #20]
	ldr	r3, [r2, #72]
	ldr	r4, [r2, #76]
	str	r3, [sp]
	str	r4, [sp, #4]
	ldr	r3, [r2, #56]
	ldr	r4, [r2, #60]
	str	r6, [sp, #8]
	str	r7, [sp, #12]
	str	r3, [sp, #24]
	str	r4, [sp, #28]
	ldr	r3, [sp]
	ldr	r4, [sp, #4]
	adds	r6, r6, r3
	adcs	r7, r7, r4
	mov	r4, r10
	ldr	r5, [r4, #108]
	ldr	r4, [r4, #104]
	adds	r6, r6, r4
	adcs	r7, r7, r5
	ldr	r3, [sp, #16]
	ldr	r4, [sp, #20]
	subs	r6, r6, r3
	sbcs	r7, r7, r4
	ldr	r3, [sp, #24]
	ldr	r4, [sp, #28]
	mov	r8, r1
	ldr	r1, [r2, #88]
	ldr	r2, [r2, #92]
	subs	r6, r6, r3
	sbcs	r7, r7, r4
	adds	r3, r3, r1
	adcs	r4, r4, r2
	movs	r5, r4
	movs	r4, r3
	mov	r3, r10
	subs	r6, r6, r1
	sbcs	r7, r7, r2
	ldr	r2, [r3, #120]
	ldr	r3, [r3, #124]
	adds	r4, r4, r2
	adcs	r5, r5, r3
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #12]
	subs	r4, r4, r2
	sbcs	r5, r5, r3
	ldr	r2, [sp]
	ldr	r3, [sp, #4]
	subs	r4, r4, r2
	sbcs	r5, r5, r3
	mov	r3, r10
	ldr	r2, [r3, #104]
	ldr	r3, [r3, #108]
	subs	r4, r4, r2
	sbcs	r5, r5, r3
	mov	r3, r10
	ldr	r3, [r3, #32]
	mov	r9, r0
	cmp	r3, #1
	bne	.LCB219
	b	.L13	@long jump
.LCB219:
	mov	r3, r10
	ldr	r3, [r3, #12]
	cmp	r3, r5
	bls	.LCB223
	b	.L38	@long jump
.LCB223:
	bne	.LCB225
	b	.L63	@long jump
.LCB225:
.L14:
	movs	r3, #140
	mov	r2, r10
	ldr	r2, [r2, r3]
	cmp	r2, #1
	bne	.LCB232
	b	.L62	@long jump
.LCB232:
	mov	r6, r10
	movs	r2, #1
	mov	r1, r10
	adds	r6, r6, #144
	str	r2, [r1, r3]
	ldr	r0, .L68
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	movs	r1, #1
	str	r3, [r6]
	str	r4, [r6, #4]
	mov	r7, r10
	bl	_ZN4Gpio3setEb
	adds	r7, r7, #152
.L21:
	ldr	r2, [r6]
	ldr	r3, [r6, #4]
	ldr	r0, [r7]
	ldr	r1, [r7, #4]
	cmp	r1, r3
	bhi	.LCB250
	b	.L64	@long jump
.LCB250:
.L41:
	movs	r4, #160
	add	r4, r4, r10
	mov	r8, r4
	ldr	r4, [r4, #4]
	cmp	r4, r3
	bcc	.LCB257
	b	.L65	@long jump
.LCB257:
.L42:
	mov	r4, r10
	ldr	r4, [r4, #8]
	movs	r5, r1
	mov	r9, r4
	mov	r4, r10
	ldr	r4, [r4, #12]
	mov	ip, r4
	movs	r4, r0
	subs	r4, r4, r2
	sbcs	r5, r5, r3
	cmp	ip, r5
	bhi	.L8
	bne	.LCB271
	b	.L66	@long jump
.LCB271:
.L43:
	mov	r4, fp
	ldr	r5, [r4, #4]
	ldr	r4, [r4]
	subs	r4, r4, r0
	sbcs	r5, r5, r1
	cmp	ip, r5
	bhi	.L8
	bne	.LCB280
	b	.L67	@long jump
.LCB280:
.L44:
	movs	r1, #188
	mov	r0, r10
	ldr	r1, [r0, r1]
	cmp	r1, #0
	bne	.L8
	adds	r1, r1, #168
	ldr	r1, [r0, r1]
	cmp	r1, #0
	bne	.L8
	adds	r1, r1, #136
	ldr	r0, [r0, r1]
	ldr	r1, [r0]
	ldr	r4, [r1]
	movs	r1, #0
	blx	r4
	movs	r1, #128
	movs	r0, #144
	lsls	r1, r1, #2
	lsls	r0, r0, #23
	bl	HAL_GPIO_TogglePin
	bl	_Z9getConfigv
	ldrh	r3, [r0, #4]
	cmp	r3, #0
	beq	.L32
	movs	r3, #200
	movs	r2, #1
	mov	r1, r10
	str	r2, [r1, r3]
	mov	r3, r10
	mov	r2, fp
	adds	r3, r3, #208
	ldr	r1, [r2]
	ldr	r2, [r2, #4]
	str	r1, [r3]
	str	r2, [r3, #4]
.L32:
	ldr	r2, [r7]
	ldr	r3, [r7, #4]
	mov	r1, r8
	str	r2, [r1]
	str	r3, [r1, #4]
	str	r2, [r6]
	str	r3, [r6, #4]
	mov	r1, r10
	movs	r3, #140
	movs	r2, #2
	str	r2, [r1, r3]
.L8:
	add	sp, sp, #36
	@ sp needed
	pop	{r2, r3, r4, r5}
	mov	r8, r2
	mov	r9, r3
	mov	r10, r4
	mov	fp, r5
	pop	{r4, r5, r6, r7, pc}
.L58:
	movs	r3, #40
	movs	r0, #24
	add	r3, r3, r10
	mov	r7, r10
	mov	r9, r3
	mov	r6, r10
	mov	r5, r10
	mov	r4, r10
	movs	r1, r3
	adds	r7, r7, #56
	add	r0, r0, r10
	movs	r2, #16
	adds	r6, r6, #72
	str	r0, [sp]
	bl	memcpy
	movs	r2, #16
	movs	r1, r7
	mov	r0, r9
	bl	memcpy
	adds	r5, r5, #88
	movs	r2, #16
	movs	r1, r6
	movs	r0, r7
	bl	memcpy
	adds	r4, r4, #104
	movs	r2, #16
	movs	r1, r5
	movs	r0, r6
	bl	memcpy
	movs	r2, #16
	movs	r1, r4
	movs	r0, r5
	bl	memcpy
	movs	r3, #120
	add	r3, r3, r10
	mov	r8, r3
	movs	r1, r3
	movs	r2, #16
	movs	r0, r4
	bl	memcpy
	movs	r2, #16
	mov	r1, fp
	mov	r0, r8
	bl	memcpy
	mov	r3, fp
	ldr	r2, [r3]
	ldr	r3, [r3, #4]
	str	r2, [sp, #8]
	str	r3, [sp, #12]
	mov	r3, fp
	ldr	r3, [r3, #8]
	movs	r2, #16
	subs	r3, r3, #1
	movs	r1, r3
	subs	r3, r1, #1
	sbcs	r1, r1, r3
	mov	r0, r9
	str	r1, [sp, #16]
	movs	r1, r7
	bl	memcpy
	movs	r2, #16
	movs	r1, r6
	movs	r0, r7
	bl	memcpy
	movs	r2, #16
	movs	r1, r5
	movs	r0, r6
	bl	memcpy
	movs	r2, #16
	movs	r1, r4
	movs	r0, r5
	bl	memcpy
	movs	r2, #16
	mov	r1, r8
	movs	r0, r4
	bl	memcpy
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #12]
	mov	r0, r10
	str	r2, [r0, #120]
	str	r3, [r0, #124]
	movs	r2, #128
	ldr	r1, [sp, #16]
	str	r1, [r0, r2]
	b	.L9
.L13:
	mov	r3, r10
	ldr	r2, [r3, #8]
	ldr	r3, [r3, #12]
	str	r2, [sp, #24]
	str	r3, [sp]
	cmp	r3, r5
	bhi	.L39
	bne	.L18
	cmp	r2, r4
	bls	.L18
.L39:
	movs	r2, #172
	mov	r3, r10
	mov	r1, r10
	ldr	r3, [r3, r2]
	adds	r3, r3, #1
	str	r3, [r1, r2]
.L18:
	movs	r2, #100
	movs	r3, #0
	movs	r0, r6
	movs	r1, r7
	bl	__aeabi_lmul
	cmp	r1, r8
	bhi	.L37
	bne	.L34
	cmp	r0, r9
	bls	.L34
.L37:
	ldr	r3, [sp, #16]
	ldr	r4, [sp, #20]
	str	r3, [sp, #8]
	str	r4, [sp, #12]
	b	.L14
.L34:
	movs	r2, #100
	movs	r3, #0
	movs	r0, r4
	movs	r1, r5
	bl	__aeabi_lmul
	cmp	r8, r1
	bhi	.L40
	bne	.L22
	cmp	r9, r0
	bls	.L22
.L40:
	ldr	r3, [sp]
	cmp	r3, r5
	bhi	.L36
	bne	.L22
	ldr	r3, [sp, #24]
	cmp	r3, r4
	bls	.L22
.L36:
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	str	r3, [sp, #16]
	str	r4, [sp, #20]
.L24:
	movs	r3, #140
	mov	r2, r10
	ldr	r2, [r2, r3]
	cmp	r2, #2
	beq	.L62
	movs	r2, #2
	mov	r1, r10
	str	r2, [r1, r3]
	mov	r3, r10
	ldr	r1, [sp, #16]
	ldr	r2, [sp, #20]
	adds	r3, r3, #160
	str	r1, [r3]
	str	r2, [r3, #4]
	ldr	r0, .L68
	movs	r1, #0
	bl	_ZN4Gpio3setEb
.L62:
	mov	r6, r10
	mov	r7, r10
	adds	r6, r6, #144
	adds	r7, r7, #152
	ldr	r2, [r6]
	ldr	r3, [r6, #4]
	ldr	r0, [r7]
	ldr	r1, [r7, #4]
	cmp	r1, r3
	bls	.LCB521
	b	.L41	@long jump
.LCB521:
.L64:
	cmp	r1, r3
	beq	.LCB524
	b	.L8	@long jump
.LCB524:
	cmp	r0, r2
	bls	.LCB526
	b	.L41	@long jump
.LCB526:
	b	.L8
.L63:
	mov	r3, r10
	ldr	r3, [r3, #8]
	cmp	r3, r4
	bhi	.LCB535
	b	.L14	@long jump
.LCB535:
.L38:
	movs	r2, #172
	mov	r3, r10
	mov	r1, r10
	ldr	r3, [r3, r2]
	movs	r0, r4
	adds	r3, r3, #1
	str	r3, [r1, r2]
	movs	r2, #100
	movs	r3, #0
	movs	r1, r5
	bl	__aeabi_lmul
	cmp	r1, r8
	bls	.LCB550
	b	.L14	@long jump
.LCB550:
	bne	.L17
	cmp	r0, r9
	bls	.LCB554
	b	.L14	@long jump
.LCB554:
.L17:
	movs	r2, #100
	movs	r3, #0
	movs	r0, r6
	movs	r1, r7
	bl	__aeabi_lmul
	cmp	r8, r1
	bhi	.L24
	bne	.L35
	cmp	r9, r0
	bhi	.L24
.L35:
	ldr	r3, [sp, #16]
	ldr	r4, [sp, #20]
	str	r3, [sp, #8]
	str	r4, [sp, #12]
.L22:
	movs	r3, #140
	mov	r2, r10
	ldr	r2, [r2, r3]
	cmp	r2, #0
	beq	.L62
	mov	r7, r10
	movs	r2, #0
	mov	r1, r10
	mov	r6, r10
	adds	r7, r7, #152
	str	r2, [r1, r3]
	ldr	r0, .L68
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	movs	r1, #0
	str	r3, [r7]
	str	r4, [r7, #4]
	bl	_ZN4Gpio3setEb
	adds	r6, r6, #144
	b	.L21
.L65:
	cmp	r4, r3
	beq	.LCB595
	b	.L8	@long jump
.LCB595:
	mov	r4, r8
	ldr	r4, [r4]
	cmp	r4, r2
	bcs	.LCB600
	b	.L42	@long jump
.LCB600:
	b	.L8
.L66:
	cmp	r9, r4
	bhi	.LCB606
	b	.L43	@long jump
.LCB606:
	b	.L8
.L67:
	cmp	r9, r4
	bhi	.LCB612
	b	.L44	@long jump
.LCB612:
	b	.L8
.L69:
	.align	2
.L68:
	.word	.LANCHOR0
	.size	_ZN10EdgeFilter6onEdgeERK4Edge, .-_ZN10EdgeFilter6onEdgeERK4Edge
	.global	__aeabi_uldivmod
	.global	__aeabi_ldivmod
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
	beq	.L70
	ldrb	r4, [r0, #16]
	cmp	r4, #0
	beq	.L129
.L70:
	add	sp, sp, #52
	@ sp needed
	pop	{r2, r3, r4, r5}
	mov	r8, r2
	mov	r9, r3
	mov	r10, r4
	mov	fp, r5
	pop	{r4, r5, r6, r7, pc}
.L129:
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
	movs	r3, #128
	ldr	r3, [r0, r3]
	mov	r8, r3
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r2, #128
	ldr	r3, .L142
	lsls	r2, r2, #23
	ldr	r3, [r3, #36]
	ldr	r2, [r2, #36]
	str	r3, [sp, #4]
	str	r2, [sp]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r3, #200
	ldr	r3, [r0, r3]
	cmp	r3, #1
	bne	.LCB676
	b	.L130	@long jump
.LCB676:
.L74:
	ldr	r1, [sp, #8]
	ldr	r2, [sp, #12]
	ldr	r3, [sp]
	ldr	r4, [sp, #4]
	subs	r3, r3, r1
	sbcs	r4, r4, r2
	ldr	r2, [r5, #8]
	str	r3, [sp, #16]
	str	r4, [sp, #20]
	str	r2, [sp, #32]
	ldr	r2, [r5, #12]
	mov	r9, r2
	cmp	r2, r4
	bhi	.LCB687
	b	.L124	@long jump
.LCB687:
.L128:
	movs	r3, #144
.L127:
	mov	fp, r3
	adds	r3, r3, #8
	mov	r10, r3
	add	fp, fp, r5
	add	r10, r10, r5
.L76:
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r3, #140
	ldr	r3, [r5, r3]
	mov	r8, r3
	mov	r3, fp
	ldr	r4, [r3, #4]
	ldr	r3, [r3]
	str	r3, [sp, #8]
	str	r4, [sp, #12]
	mov	r3, r10
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
	bne	.LCB726
	b	.L131	@long jump
.LCB726:
.L102:
	movs	r2, #188
	ldr	r2, [r5, r2]
	cmp	r2, #1
	beq	.L79
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
	bgt	.LCB741
	b	.L81	@long jump
.LCB741:
	movs	r3, #175
	lsls	r3, r3, #3
	muls	r0, r3
	ldr	r2, .L142+4
	movs	r3, #0
	movs	r1, #0
	bl	__aeabi_uldivmod
	adds	r4, r0, #0
	uxtb	r0, r0
	cmp	r0, #14
	bls	.LCB752
	b	.L132	@long jump
.LCB752:
.L82:
	adds	r4, r4, #1
	uxtb	r2, r4
	movs	r3, #184
	movs	r6, r2
	strb	r2, [r5, r3]
.L83:
	movs	r4, #168
	ldr	r3, [r5, r4]
	cmp	r3, #0
	bne	.LCB764
	b	.L133	@long jump
.LCB764:
.L84:
	cmp	r3, #1
	bne	.LCB767
	b	.L134	@long jump
.LCB767:
.L79:
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
	bcs	.L88
	adds	r3, r0, #0
.L88:
	uxth	r1, r3
	lsls	r3, r1, #5
	subs	r3, r3, r1
	lsls	r3, r3, #2
	adds	r3, r3, r1
	lsls	r3, r3, #3
	cmp	r7, #0
	beq	.LCB796
	b	.L103	@long jump
.LCB796:
	cmp	r3, r6
	bhi	.LCB798
	b	.L103	@long jump
.LCB798:
.L87:
	ldr	r3, [sp, #16]
	ldr	r4, [sp, #20]
	cmp	r9, r4
	bls	.LCB802
	b	.L70	@long jump
.LCB802:
	bne	.LCB804
	b	.L135	@long jump
.LCB804:
.L104:
	mov	r3, r8
	cmp	r3, #0
	beq	.LCB808
	b	.L70	@long jump
.LCB808:
	ldr	r1, [sp, #24]
	ldr	r2, [sp, #28]
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	cmp	r2, r4
	bhi	.LCB812
	b	.L136	@long jump
.LCB812:
.L105:
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	ldr	r2, [sp, #40]
	cmp	r4, r2
	bhi	.LCB817
	b	.L137	@long jump
.LCB817:
.L106:
	ldr	r0, [sp, #24]
	ldr	r1, [sp, #28]
	ldr	r6, [sp, #8]
	ldr	r7, [sp, #12]
	subs	r0, r0, r6
	sbcs	r1, r1, r7
	ldr	r3, [r5, #12]
	ldr	r2, [r5, #8]
	cmp	r3, r1
	bls	.LCB825
	b	.L70	@long jump
.LCB825:
	bne	.LCB827
	b	.L138	@long jump
.LCB827:
.L107:
	ldr	r0, [sp]
	ldr	r1, [sp, #4]
	ldr	r6, [sp, #24]
	ldr	r7, [sp, #28]
	subs	r0, r0, r6
	sbcs	r1, r1, r7
	cmp	r3, r1
	bls	.LCB833
	b	.L70	@long jump
.LCB833:
	bne	.LCB835
	b	.L139	@long jump
.LCB835:
.L108:
	movs	r3, #188
	ldr	r3, [r5, r3]
	cmp	r3, #0
	beq	.LCB840
	b	.L70	@long jump
.LCB840:
	adds	r3, r3, #168
	ldr	r3, [r5, r3]
	cmp	r3, #0
	beq	.LCB844
	b	.L70	@long jump
.LCB844:
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
	bne	.LCB861
	b	.L99	@long jump
.LCB861:
	movs	r3, #200
	movs	r2, #1
	str	r2, [r5, r3]
	movs	r3, r5
	ldr	r1, [sp]
	ldr	r2, [sp, #4]
	adds	r3, r3, #208
	str	r1, [r3]
	str	r2, [r3, #4]
	b	.L99
.L124:
	ldr	r3, [sp, #16]
	ldr	r4, [sp, #20]
	cmp	r9, r4
	bne	.L101
	ldr	r2, [sp, #32]
	cmp	r2, r3
	bls	.LCB878
	b	.L128	@long jump
.LCB878:
.L101:
	movs	r3, #140
	mov	r1, r8
	ldr	r2, [r5, r3]
	cmp	r1, #1
	bne	.LCB884
	b	.L140	@long jump
.LCB884:
	cmp	r2, #0
	bne	.LCB886
	b	.L128	@long jump
.LCB886:
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r2, #0
	str	r2, [r5, r3]
	movs	r3, #152
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
	movs	r1, #0
	ldr	r0, .L142+8
	bl	_ZN4Gpio3setEb
	movs	r3, #144
	mov	fp, r3
	add	fp, fp, r5
	b	.L76
.L130:
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
	beq	.L141
.L100:
	movs	r3, #200
	movs	r2, #0
	str	r2, [r5, r3]
	b	.L74
.L103:
	movs	r3, #168
	ldr	r3, [r5, r3]
	cmp	r3, #1
	bne	.LCB933
	b	.L87	@long jump
.LCB933:
	ldr	r3, [sp]
	ldr	r4, [sp, #4]
	str	r3, [r2]
	str	r4, [r2, #4]
	movs	r2, #188
	ldr	r3, [r5, r2]
	cmp	r3, #0
	bne	.L90
	mov	r3, r8
	cmp	r3, #1
	beq	.LCB942
	b	.L87	@long jump
.LCB942:
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
	bls	.LCB959
	b	.L70	@long jump
.LCB959:
	bne	.L91
	cmp	r8, r0
	bls	.LCB963
	b	.L70	@long jump
.LCB963:
.L91:
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
	b	.L99
.L90:
	cmp	r3, #1
	beq	.LCB981
	b	.L87	@long jump
.LCB981:
	mov	r3, r8
	cmp	r3, #0
	beq	.LCB984
	b	.L70	@long jump
.LCB984:
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
.L99:
	mov	r3, r10
	ldr	r1, [sp, #36]
	ldr	r2, [r3]
	ldr	r3, [r3, #4]
	str	r2, [r1]
	str	r3, [r1, #4]
	mov	r1, fp
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
	b	.L70
.L132:
	movs	r4, #14
	b	.L82
.L136:
	cmp	r2, r4
	beq	.LCB1016
	b	.L70	@long jump
.LCB1016:
	cmp	r1, r3
	bls	.LCB1018
	b	.L105	@long jump
.LCB1018:
	b	.L70
.L141:
	cmp	r3, r6
	bls	.LCB1024
	b	.L70	@long jump
.LCB1024:
	b	.L100
.L131:
	ldr	r2, .L142+12
	cmp	r0, r2
	bls	.LCB1031
	b	.L102	@long jump
.LCB1031:
	b	.L79
.L135:
	ldr	r2, [sp, #32]
	cmp	r2, r3
	bhi	.LCB1038
	b	.L104	@long jump
.LCB1038:
	b	.L70
.L143:
	.align	2
.L142:
	.word	1073742848
	.word	1000000
	.word	.LANCHOR0
	.word	999999
.L140:
	cmp	r2, #1
	bne	.L125
	adds	r3, r3, #4
	b	.L127
.L81:
	movs	r3, #184
	strb	r2, [r5, r3]
	movs	r3, #0
	movs	r6, r3
	b	.L83
.L133:
	bl	_Z9getConfigv
	ldrb	r3, [r0, #10]
	cmp	r3, r6
	bls	.L85
	ldr	r3, [r5, r4]
	b	.L84
.L125:
	.syntax divided
@ 142 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
	.thumb
	.syntax unified
	str	r1, [r5, r3]
	movs	r3, #144
	mov	fp, r3
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #12]
	add	fp, fp, r5
	mov	r2, fp
	str	r3, [r2]
	str	r4, [r2, #4]
	.syntax divided
@ 131 "../deps/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r1, #1
	ldr	r0, .L144
	bl	_ZN4Gpio3setEb
	movs	r3, #152
	mov	r10, r3
	add	r10, r10, r5
	b	.L76
.L134:
	movs	r3, #184
	ldrb	r4, [r5, r3]
	bl	_Z9getConfigv
	ldrb	r3, [r0, #11]
	cmp	r3, r4
	bcs	.LCB1100
	b	.L79	@long jump
.LCB1100:
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
	b	.L99
.L137:
	cmp	r4, r2
	beq	.LCB1117
	b	.L70	@long jump
.LCB1117:
	ldr	r2, [sp, #44]
	cmp	r3, r2
	bls	.LCB1120
	b	.L106	@long jump
.LCB1120:
	b	.L70
.L85:
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
	b	.L99
.L138:
	cmp	r2, r0
	bhi	.LCB1140
	b	.L107	@long jump
.LCB1140:
	b	.L70
.L139:
	cmp	r2, r0
	bhi	.LCB1146
	b	.L108	@long jump
.LCB1146:
	b	.L70
.L145:
	.align	2
.L144:
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
	movs	r3, #0
	str	r3, [r4, #12]
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
	ldr	r4, .L148
	ldr	r3, .L148+4
	ldr	r0, .L148+8
	sub	sp, sp, #20
	adds	r3, r3, #8
	movs	r2, r4
	ldr	r1, .L148+12
	str	r3, [r0]
	bl	__aeabi_atexit
	ldr	r3, .L148+16
	ldr	r0, .L148+20
	adds	r3, r3, #8
	movs	r2, r4
	ldr	r1, .L148+24
	str	r3, [r0]
	bl	__aeabi_atexit
	movs	r3, #144
	lsls	r3, r3, #23
	mov	r8, r3
	movs	r3, #3
	movs	r5, #0
	movs	r2, #128
	mov	r9, r3
	ldr	r6, .L148+28
	str	r3, [sp, #4]
	mov	r1, r8
	subs	r3, r3, #2
	str	r5, [sp, #8]
	str	r5, [sp]
	lsls	r2, r2, #3
	movs	r0, r6
	bl	_ZN4GpioC1EP12GPIO_TypeDefmmmmm
	ldr	r7, .L148+32
	movs	r2, r4
	movs	r1, r7
	movs	r0, r6
	bl	__aeabi_atexit
	mov	r3, r9
	movs	r2, #128
	ldr	r6, .L148+36
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
.L149:
	.align	2
.L148:
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
