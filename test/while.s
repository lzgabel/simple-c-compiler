.align 4
.globl main
main:
	push %ebp
	movl %esp, %ebp
	movl $a, %esi
	movl $a, %eax
	pushl %eax
	movl $1, %eax
	popl %ebx
	movl %eax, (%ebx)
	mov %esi, %eax
	add $0, %eax
	movl (%eax), %eax
	pushl %eax
	movl $10, %eax
	popl %ebx
	cmpl %eax, %ebx
	movl %eax, %edx
	jl __while0
__while0:
	mov %esi, %eax
	add $0, %eax
	incl (%eax)
	movl (%eax), %eax
	cmpl %edx, %eax
	jl __while0
__while_end0:
