	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.file	"sysy"
	.globl	getMaxNum
	.p2align	1
	.type	getMaxNum,@function
getMaxNum:
	.cfi_startproc
	addi	sp, sp, -16
	.cfi_def_cfa_offset 16
	sw	a0, 4(sp)
	sw	zero, 8(sp)
	sw	zero, 12(sp)
	j	.LBB0_2
.LBB0_1:
	lw	a0, 12(sp)
	addiw	a0, a0, 1
	sw	a0, 12(sp)
.LBB0_2:
	lw	a0, 12(sp)
	lw	a2, 4(sp)
	bge	a0, a2, .LBB0_5
	lw	a0, 12(sp)
	slli	a0, a0, 2
	add	a0, a0, a1
	lw	a0, 0(a0)
	lw	a2, 8(sp)
	bge	a2, a0, .LBB0_1
	lw	a0, 12(sp)
	slli	a0, a0, 2
	add	a0, a0, a1
	lw	a0, 0(a0)
	sw	a0, 8(sp)
	j	.LBB0_1
.LBB0_5:
	lw	a0, 8(sp)
	addi	sp, sp, 16
	ret
.Lfunc_end0:
	.size	getMaxNum, .Lfunc_end0-getMaxNum
	.cfi_endproc

	.globl	getNumPos
	.p2align	1
	.type	getNumPos,@function
getNumPos:
	.cfi_startproc
	addi	sp, sp, -16
	.cfi_def_cfa_offset 16
	sw	a0, 8(sp)
	sw	a1, 4(sp)
	sw	zero, 12(sp)
	lui	a0, %hi(base)
.LBB1_1:
	lw	a1, 12(sp)
	lw	a2, 4(sp)
	bge	a1, a2, .LBB1_3
	lw	a1, 8(sp)
	lw	a2, %lo(base)(a0)
	lw	a3, 12(sp)
	divw	a1, a1, a2
	sw	a1, 8(sp)
	addiw	a1, a3, 1
	sw	a1, 12(sp)
	j	.LBB1_1
.LBB1_3:
	lw	a0, 8(sp)
	lui	a1, %hi(base)
	lw	a1, %lo(base)(a1)
	remw	a0, a0, a1
	addi	sp, sp, 16
	ret
.Lfunc_end1:
	.size	getNumPos, .Lfunc_end1-getNumPos
	.cfi_endproc

	.globl	radixSort
	.p2align	1
	.type	radixSort,@function
radixSort:
	.cfi_startproc
	addi	sp, sp, -272
	.cfi_def_cfa_offset 272
	sd	ra, 264(sp)
	sd	s0, 256(sp)
	sd	s1, 248(sp)
	sd	s2, 240(sp)
	sd	s3, 232(sp)
	sd	s4, 224(sp)
	.cfi_offset ra, -8
	.cfi_offset s0, -16
	.cfi_offset s1, -24
	.cfi_offset s2, -32
	.cfi_offset s3, -40
	.cfi_offset s4, -48
	mv	s3, a1
	sw	a0, 12(sp)
	sw	a2, 8(sp)
	sw	a3, 4(sp)
	sd	zero, 16(sp)
	sd	zero, 24(sp)
	sd	zero, 32(sp)
	sd	zero, 40(sp)
	sd	zero, 48(sp)
	sd	zero, 56(sp)
	sd	zero, 64(sp)
	sd	zero, 72(sp)
	sd	zero, 80(sp)
	sd	zero, 88(sp)
	sd	zero, 96(sp)
	sd	zero, 104(sp)
	sd	zero, 112(sp)
	sd	zero, 120(sp)
	sd	zero, 128(sp)
	sd	zero, 136(sp)
	sd	zero, 200(sp)
	sd	zero, 192(sp)
	sd	zero, 184(sp)
	sd	zero, 176(sp)
	sd	zero, 168(sp)
	sd	zero, 160(sp)
	lw	a1, 12(sp)
	sd	zero, 152(sp)
	sd	zero, 144(sp)
	li	a2, -1
	li	a0, 1
	bne	a1, a2, .LBB2_2
	bnez	a0, .LBB2_3
	j	.LBB2_4
.LBB2_2:
	lw	a0, 8(sp)
	lw	a1, 4(sp)
	addiw	a0, a0, 1
	slt	a0, a0, a1
	xori	a0, a0, 1
	beqz	a0, .LBB2_4
.LBB2_3:
	ld	ra, 264(sp)
	ld	s0, 256(sp)
	ld	s1, 248(sp)
	ld	s2, 240(sp)
	ld	s3, 232(sp)
	ld	s4, 224(sp)
	addi	sp, sp, 272
	ret
.LBB2_4:
	lw	a0, 8(sp)
	sw	a0, 208(sp)
	addi	s0, sp, 144
.LBB2_5:
	lw	a0, 208(sp)
	lw	a1, 4(sp)
	bge	a0, a1, .LBB2_7
	lw	a0, 208(sp)
	slli	a0, a0, 2
	add	a0, a0, s3
	lw	a0, 0(a0)
	lw	a1, 12(sp)
	call	getNumPos@plt
	sext.w	a0, a0
	lw	a1, 208(sp)
	slli	a0, a0, 2
	add	a0, a0, s0
	lw	a2, 0(a0)
	slli	a0, a1, 2
	add	a0, a0, s3
	lw	a0, 0(a0)
	lw	a1, 12(sp)
	addiw	s1, a2, 1
	call	getNumPos@plt
	sext.w	a0, a0
	lw	a1, 208(sp)
	slli	a0, a0, 2
	add	a0, a0, s0
	sw	s1, 0(a0)
	addiw	a0, a1, 1
	sw	a0, 208(sp)
	j	.LBB2_5
.LBB2_7:
	lw	a0, 8(sp)
	lw	a1, 144(sp)
	sw	a0, 16(sp)
	addw	a0, a0, a1
	sw	a0, 80(sp)
	li	a0, 1
	sw	a0, 208(sp)
	lui	a0, %hi(base)
	lw	a0, %lo(base)(a0)
	addi	a1, sp, 80
	addi	a6, sp, 16
	addi	a3, sp, 144
	lw	a4, 208(sp)
	bge	a4, a0, .LBB2_9
.LBB2_8:
	lw	a4, 208(sp)
	addiw	a5, a4, -1
	slli	a5, a5, 2
	add	a5, a5, a1
	lwu	a5, 0(a5)
	slli	s1, a4, 2
	add	s0, a3, s1
	lw	s0, 0(s0)
	add	a2, a6, s1
	sw	a5, 0(a2)
	addw	a2, a5, s0
	add	a5, a1, s1
	sw	a2, 0(a5)
	addiw	a2, a4, 1
	sw	a2, 208(sp)
	lw	a4, 208(sp)
	blt	a4, a0, .LBB2_8
.LBB2_9:
	lui	a0, %hi(base)
	lw	s2, %lo(base)(a0)
	sw	zero, 208(sp)
	addi	s0, sp, 16
	addi	s4, sp, 80
	j	.LBB2_11
.LBB2_10:
	lw	a0, 208(sp)
	addiw	a0, a0, 1
	sw	a0, 208(sp)
.LBB2_11:
	lw	a0, 208(sp)
	blt	a0, s2, .LBB2_13
	j	.LBB2_17
.LBB2_12:
	lw	a0, 208(sp)
	slli	a0, a0, 2
	add	a0, a0, s0
	lw	a0, 0(a0)
	lw	a1, 212(sp)
	slli	a0, a0, 2
	add	a0, a0, s3
	sw	a1, 0(a0)
	lw	a0, 208(sp)
	slli	a0, a0, 2
	add	a0, a0, s0
	lw	a1, 0(a0)
	addiw	a1, a1, 1
	sw	a1, 0(a0)
.LBB2_13:
	lw	a0, 208(sp)
	slli	a0, a0, 2
	add	a1, s0, a0
	lw	a1, 0(a1)
	add	a0, a0, s4
	lw	a0, 0(a0)
	bge	a1, a0, .LBB2_10
	lw	a0, 208(sp)
	slli	a0, a0, 2
	add	a0, a0, s0
	lw	a0, 0(a0)
	slli	a0, a0, 2
	add	a0, a0, s3
	lw	a0, 0(a0)
	sw	a0, 212(sp)
.LBB2_15:
	lw	a1, 12(sp)
	lw	a0, 212(sp)
	call	getNumPos@plt
	lw	a1, 208(sp)
	sext.w	a0, a0
	beq	a0, a1, .LBB2_12
	lw	a0, 212(sp)
	lw	a1, 12(sp)
	sw	a0, 216(sp)
	call	getNumPos@plt
	sext.w	a0, a0
	slli	a0, a0, 2
	add	a0, a0, s0
	lw	a0, 0(a0)
	slli	a0, a0, 2
	add	a0, a0, s3
	lw	a0, 0(a0)
	lw	s1, 216(sp)
	lw	a1, 12(sp)
	sw	a0, 212(sp)
	mv	a0, s1
	call	getNumPos@plt
	sext.w	a0, a0
	slli	a0, a0, 2
	add	a0, a0, s0
	lw	a0, 0(a0)
	slli	a0, a0, 2
	add	a0, a0, s3
	sw	s1, 0(a0)
	lw	a0, 216(sp)
	lw	a1, 12(sp)
	call	getNumPos@plt
	sext.w	a0, a0
	slli	a0, a0, 2
	add	a0, a0, s0
	lw	a2, 0(a0)
	lw	a1, 12(sp)
	lw	a0, 216(sp)
	addiw	s1, a2, 1
	call	getNumPos@plt
	sext.w	a0, a0
	slli	a0, a0, 2
	add	a0, a0, s0
	sw	s1, 0(a0)
	j	.LBB2_15
.LBB2_17:
	lw	a0, 8(sp)
	lw	a1, 144(sp)
	sw	a0, 16(sp)
	addw	a0, a0, a1
	sw	a0, 80(sp)
	sw	zero, 220(sp)
	lui	a0, %hi(base)
	lw	s0, %lo(base)(a0)
	addi	s1, sp, 80
	addi	s2, sp, 16
	addi	s4, sp, 144
	j	.LBB2_19
.LBB2_18:
	lw	a0, 220(sp)
	lw	a1, 12(sp)
	slli	a0, a0, 2
	add	a2, s2, a0
	add	a0, a0, s1
	lw	a3, 0(a0)
	lw	a2, 0(a2)
	addiw	a0, a1, -1
	mv	a1, s3
	call	radixSort@plt
	lw	a0, 220(sp)
	addiw	a0, a0, 1
	sw	a0, 220(sp)
.LBB2_19:
	lw	a0, 220(sp)
	bge	a0, s0, .LBB2_3
	lw	a0, 220(sp)
	blez	a0, .LBB2_18
	lw	a0, 220(sp)
	addiw	a1, a0, -1
	slli	a1, a1, 2
	add	a1, a1, s1
	lwu	a1, 0(a1)
	slli	a0, a0, 2
	add	a2, s4, a0
	lw	a2, 0(a2)
	add	a3, s2, a0
	sw	a1, 0(a3)
	addw	a1, a1, a2
	add	a0, a0, s1
	sw	a1, 0(a0)
	j	.LBB2_18
.Lfunc_end2:
	.size	radixSort, .Lfunc_end2-radixSort
	.cfi_endproc

	.globl	__sysy_main
	.p2align	1
	.type	__sysy_main,@function
__sysy_main:
	.cfi_startproc
	addi	sp, sp, -32
	.cfi_def_cfa_offset 32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	.cfi_offset ra, -8
	.cfi_offset s0, -16
	lui	a0, %hi(a)
	addi	s0, a0, %lo(a)
	mv	a0, s0
	call	getarray@plt
	sw	a0, 8(sp)
	call	starttime@plt
	lw	a3, 8(sp)
	li	a0, 8
	mv	a1, s0
	li	a2, 0
	call	radixSort@plt
	sw	zero, 12(sp)
	lui	a0, %hi(ans)
.LBB3_1:
	lw	a1, 12(sp)
	lw	a2, 8(sp)
	bge	a1, a2, .LBB3_3
	lw	a1, 12(sp)
	slli	a2, a1, 2
	add	a2, a2, s0
	lw	a2, 0(a2)
	lw	a3, %lo(ans)(a0)
	addiw	a4, a1, 2
	remw	a2, a2, a4
	mulw	a2, a1, a2
	addw	a2, a2, a3
	sw	a2, %lo(ans)(a0)
	addiw	a1, a1, 1
	sw	a1, 12(sp)
	j	.LBB3_1
.LBB3_3:
	lui	s0, %hi(ans)
	lw	a0, %lo(ans)(s0)
	bgez	a0, .LBB3_5
	lw	a0, %lo(ans)(s0)
	addi	a1, s0, %lo(ans)
	negw	a0, a0
	sw	a0, 0(a1)
.LBB3_5:
	call	stoptime@plt
	lw	a0, %lo(ans)(s0)
	call	putint@plt
	li	a0, 10
	call	putch@plt
	li	a0, 0
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end3:
	.size	__sysy_main, .Lfunc_end3-__sysy_main
	.cfi_endproc

	.type	base,@object
	.section	.rodata,"a",@progbits
	.globl	base
	.p2align	2
base:
	.word	16
	.size	base, 4

	.type	a,@object
	.bss
	.globl	a
	.p2align	2
a:
	.zero	120000040
	.size	a, 120000040

	.type	ans,@object
	.section	.sbss,"aw",@nobits
	.globl	ans
	.p2align	2
ans:
	.word	0
	.size	ans, 4

	.section	".note.GNU-stack","",@progbits
