	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.file	"sysy"
	.globl	__sysy_main
	.p2align	1
	.type	__sysy_main,@function
__sysy_main:
	.cfi_startproc
	addi	sp, sp, -32
	.cfi_def_cfa_offset 32
	li	a5, 0
	sw	zero, 8(sp)
	li	a6, 19
	li	a1, 9
	li	a2, 4
	li	a3, 2
	li	a4, 1
	j	.LBB0_2
.LBB0_1:
	lw	a5, 12(sp)
	addiw	a5, a5, 1
.LBB0_2:
	sw	a5, 12(sp)
	blt	a6, a5, .LBB0_18
	sw	zero, 16(sp)
.LBB0_4:
	lw	a5, 16(sp)
	blt	a1, a5, .LBB0_1
	li	a5, 0
	j	.LBB0_7
.LBB0_6:
	lw	a0, 20(sp)
	addiw	a5, a0, 1
.LBB0_7:
	sw	a5, 20(sp)
	blt	a2, a5, .LBB0_17
	sw	zero, 24(sp)
	j	.LBB0_10
.LBB0_9:
	lw	a0, 24(sp)
	lw	a5, 8(sp)
	addiw	a0, a0, 1
	sw	a0, 24(sp)
	addiw	a0, a5, 1
	sw	a0, 8(sp)
.LBB0_10:
	lw	a5, 24(sp)
	blt	a3, a5, .LBB0_6
	lw	a5, 24(sp)
	addiw	a0, a5, 1
	slti	a0, a0, 3
	seqz	a5, a5
	or	a0, a0, a5
	bnez	a0, .LBB0_14
	bnez	zero, .LBB0_14
	lw	a0, 24(sp)
	addiw	a0, a0, 1
	blt	a3, a0, .LBB0_10
.LBB0_14:
	sw	zero, 28(sp)
.LBB0_15:
	lw	a0, 28(sp)
	blt	a4, a0, .LBB0_9
	lw	a0, 28(sp)
	addiw	a0, a0, 1
	sw	a0, 28(sp)
	j	.LBB0_15
	lw	a0, 8(sp)
	addiw	a0, a0, 1
	sw	a0, 8(sp)
	j	.LBB0_9
.LBB0_17:
	lw	a5, 16(sp)
	j	.LBB0_4
	addiw	a5, a5, 2
	sw	a5, 16(sp)
	j	.LBB0_4
.LBB0_18:
	lw	a0, 8(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end0:
	.size	__sysy_main, .Lfunc_end0-__sysy_main
	.cfi_endproc

	.section	".note.GNU-stack","",@progbits
