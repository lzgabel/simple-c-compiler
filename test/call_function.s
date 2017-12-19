.globl func
.type func, @function
func:
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
	pushl %eax
	mov %esi, %eax
	add $0, %eax
	movl (%eax), %eax
	pushl %eax
	movl $10, %eax
	popl %ebx
	imul %ebx, %eax
	popl %ebx
	movl %eax, (%ebx)
	mov %esi, %eax
	add $0, %eax
	movl (%eax), %eax
	pushl %eax
	movl $10, %eax
	popl %ebx
	cmpl %eax, %ebx
	je __if0
__if0:
	pushl a
	pushl $msg0
	call printf
	mov %esi, %eax
	add $0, %eax
	movl (%eax), %eax
	add $4, %esp
	leave
	ret
.align 4
.globl main
main:
	push %ebp
	movl %esp, %ebp
	movl $b, %eax
	pushl %eax
	movl $func, %eax
	pushl %eax
	popl %ebx
	call *%ebx
	popl %ebx
	movl %eax, (%ebx)
	pushl b
	pushl $msg1
	call printf
	movl $0, %eax
	add $8, %esp
	leave
	ret
	add $8, %esp
	leave
	ret
.section .bss
	.lcomm a,4
	.lcomm b,4
.section .data
msg0:
	.string	"a : %d\n"
msg1:
	.string	"%d\n"
