	.file	"exercice3.c"
	.text
	.section .rdata,"dr"
.LC4:
	.ascii "x=%f,y=%f,time=%f\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$80, %rsp
	.seh_stackalloc	80
	.seh_endprologue
	call	__main
	movsd	.LC0(%rip), %xmm0
	movsd	%xmm0, -32(%rbp)
	movsd	.LC1(%rip), %xmm0
	movsd	%xmm0, -40(%rbp)
	pxor	%xmm0, %xmm0
	movsd	%xmm0, -8(%rbp)
	pxor	%xmm0, %xmm0
	movsd	%xmm0, -16(%rbp)
	call	clock
	movl	%eax, -44(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L2
.L3:
	movsd	-32(%rbp), %xmm0
	mulsd	-40(%rbp), %xmm0
	movsd	-8(%rbp), %xmm1
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -8(%rbp)
	movsd	-32(%rbp), %xmm0
	mulsd	-40(%rbp), %xmm0
	movsd	-16(%rbp), %xmm1
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	addl	$1, -20(%rbp)
.L2:
	cmpl	$99999999, -20(%rbp)
	jle	.L3
	call	clock
	movl	%eax, -48(%rbp)
	movl	-48(%rbp), %eax
	subl	-44(%rbp), %eax
	pxor	%xmm0, %xmm0
	cvtsi2sdl	%eax, %xmm0
	movsd	.LC3(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movapd	%xmm0, %xmm2
	movq	%xmm0, %rcx
	movsd	-16(%rbp), %xmm1
	movq	-16(%rbp), %rdx
	movsd	-8(%rbp), %xmm0
	movq	-8(%rbp), %rax
	movapd	%xmm2, %xmm3
	movq	%rcx, %r9
	movapd	%xmm1, %xmm2
	movq	%rdx, %r8
	movapd	%xmm0, %xmm1
	movq	%rax, %rdx
	leaq	.LC4(%rip), %rax
	movq	%rax, %rcx
	call	printf
	movl	$0, %eax
	addq	$80, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC0:
	.long	-1717986918
	.long	1072798105
	.align 8
.LC1:
	.long	858993459
	.long	1072902963
	.align 8
.LC3:
	.long	0
	.long	1083129856
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
	.def	clock;	.scl	2;	.type	32;	.endef
	.def	printf;	.scl	2;	.type	32;	.endef
