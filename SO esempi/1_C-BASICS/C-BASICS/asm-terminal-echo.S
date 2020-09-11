
//neverending echo from the standard input to the standard output
//please compile with "gcc -c" "and "ld" or with "-nostartfiles"

.file "asm-terminal-echo.S"
.text
.globl _start 
.type _start, @function

_start:
	sub $0x4, %rsp 	//reserve 4 bytes for hosting chars to read/write
	.loop:
	mov $0x0, %rax 	//from this line we pack registers for calling the read sycall (CODE=0)
	mov $0x0, %rdi
	mov %rsp, %rsi
	mov $0x1, %rdx
	syscall
	mov $0x1,%rax 	//from this line we update registers for calling the write syscall 			  // (CODE=1)
	mov $0x1, %rdi
	syscall	
	jmp .loop


	/* %rax = registro per codici di chiamata syscall;
	   %rdi = registro per primo parametro syscall (canale I/O);
	   %rsi = registro per 2° parametro syscall;
	   %rdx = registro per 3° parametro syscall (numero di byte);
	   %rsp = register stack pointer */		
