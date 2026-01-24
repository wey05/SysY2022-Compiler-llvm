	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.file	"sysy"
	.globl	getA
	.p2align	1
	.type	getA,@function
getA:
	.cfi_startproc
	lui	a1, %hi(count)
	lw	a0, %lo(count)(a1)
	addiw	a0, a0, 1
	sw	a0, %lo(count)(a1)
	ret
.Lfunc_end0:
	.size	getA, .Lfunc_end0-getA
	.cfi_endproc

	.globl	f1
	.p2align	1
	.type	f1,@function
f1:
	.cfi_startproc
	addi	sp, sp, -32
	.cfi_def_cfa_offset 32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	.cfi_offset ra, -8
	.cfi_offset s0, -16
	lui	s0, %hi(sum)
	lw	a1, %lo(sum)(s0)
	sw	a0, 12(sp)
	addw	a0, a0, a1
	sw	a0, %lo(sum)(s0)
	call	getA@plt
	lw	a1, %lo(sum)(s0)
	sw	a0, 12(sp)
	addw	a0, a0, a1
	sw	a0, %lo(sum)(s0)
	call	getA@plt
	lw	a1, %lo(sum)(s0)
	lw	a2, 12(sp)
	addw	a0, a0, a1
	slliw	a1, a2, 1
	addw	a0, a0, a1
	sw	a0, %lo(sum)(s0)
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end1:
	.size	f1, .Lfunc_end1-f1
	.cfi_endproc

	.globl	f2
	.p2align	1
	.type	f2,@function
f2:
	.cfi_startproc
	addi	sp, sp, -32
	.cfi_def_cfa_offset 32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	sd	s1, 8(sp)
	.cfi_offset ra, -8
	.cfi_offset s0, -16
	.cfi_offset s1, -24
	lui	s0, %hi(sum)
	lw	a0, %lo(sum)(s0)
	lui	s1, %hi(a)
	lw	a1, %lo(a)(s1)
	addw	a0, a0, a1
	sw	a0, %lo(sum)(s0)
	call	getA@plt
	lw	a0, %lo(sum)(s0)
	lw	a1, %lo(a)(s1)
	addw	a0, a0, a1
	sw	a0, %lo(sum)(s0)
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	ld	s1, 8(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end2:
	.size	f2, .Lfunc_end2-f2
	.cfi_endproc

	.globl	f3
	.p2align	1
	.type	f3,@function
f3:
	.cfi_startproc
	addi	sp, sp, -32
	.cfi_def_cfa_offset 32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	sd	s1, 8(sp)
	.cfi_offset ra, -8
	.cfi_offset s0, -16
	.cfi_offset s1, -24
	call	getA@plt
	lui	s1, %hi(sum)
	lw	a1, %lo(sum)(s1)
	addw	a0, a0, a1
	sw	a0, %lo(sum)(s1)
	call	getA@plt
	lw	a1, %lo(sum)(s1)
	mv	s0, a0
	addw	a0, a0, a1
	sw	a0, %lo(sum)(s1)
	call	getA@plt
	lw	a0, %lo(sum)(s1)
	addw	a0, a0, s0
	sw	a0, %lo(sum)(s1)
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	ld	s1, 8(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end3:
	.size	f3, .Lfunc_end3-f3
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
	sd	s1, 8(sp)
	.cfi_offset ra, -8
	.cfi_offset s0, -16
	.cfi_offset s1, -24
	lui	s0, %hi(sum)
	sw	zero, %lo(sum)(s0)
	call	getA@plt
	lw	a1, %lo(sum)(s0)
	lui	a2, %hi(a)
	sw	a0, %lo(a)(a2)
	addw	a0, a0, a1
	sw	a0, %lo(sum)(s0)
	call	getA@plt
	mv	s0, a0
	sw	a0, 0(sp)
	call	f1@plt
	call	f2@plt
	call	f3@plt
	mv	a0, s0
	call	f1@plt
	call	f2@plt
	call	f3@plt
	call	getA@plt
	lw	a0, 0(sp)
	call	f1@plt
	call	f2@plt
	call	f3@plt
	call	getA@plt
	call	getA@plt
	call	f1@plt
	call	f2@plt
	call	f3@plt
	call	getA@plt
	call	f1@plt
	call	f2@plt
	call	f3@plt
	call	getA@plt
	call	f1@plt
	call	f2@plt
	call	f3@plt
	lw	a0, 0(sp)
	call	f1@plt
	call	f2@plt
	call	f3@plt
	sw	zero, 4(sp)
	li	s0, 3
	li	s1, 1
	lw	a0, 4(sp)
	blt	a0, s0, .LBB4_2
	j	.LBB4_4
.LBB4_1:
	lw	a0, 0(sp)
	call	f1@plt
	call	f2@plt
	call	f3@plt
	call	getA@plt
	lw	a1, 4(sp)
	sw	a0, 0(sp)
	addiw	a0, a1, 1
	sw	a0, 4(sp)
	lw	a0, 4(sp)
	bge	a0, s0, .LBB4_4
.LBB4_2:
	lw	a0, 0(sp)
	call	f1@plt
	call	f2@plt
	call	f3@plt
	lw	a0, 4(sp)
	bne	a0, s1, .LBB4_1
	call	getA@plt
	call	f1@plt
	call	f2@plt
	call	f3@plt
	lw	a0, 4(sp)
	addiw	a0, a0, 1
	sw	a0, 4(sp)
	lw	a0, 4(sp)
	blt	a0, s0, .LBB4_2
.LBB4_4:
	j	.LBB4_4
	lui	a0, %hi(sum)
	lw	a0, %lo(sum)(a0)
	call	putint@plt
	li	a0, 10
	call	putch@plt
	li	a0, 0
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	ld	s1, 8(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end4:
	.size	__sysy_main, .Lfunc_end4-__sysy_main
	.cfi_endproc

	.type	a,@object
	.section	.sbss,"aw",@nobits
	.globl	a
	.p2align	2
a:
	.word	0
	.size	a, 4

	.type	sum,@object
	.globl	sum
	.p2align	2
sum:
	.word	0
	.size	sum, 4

	.type	count,@object
	.globl	count
	.p2align	2
count:
	.word	0
	.size	count, 4

	.section	".note.GNU-stack","",@progbits
