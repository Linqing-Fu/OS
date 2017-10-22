.data
os_size:
	.word 0
	.word 0
.text
	.globl main
main:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

li $8,0x8007b1a8
li $4,0xa0800200
addiu $5,$0,0x200
li $10,0xa0800090 #os_size
lw $11,($10) #load os_size into 11
sll $6,$11,9# $11 * 512
jal $8
li $8,0xa08002bc #need to change
jal $8

jr $31


#need add code
	#read kernel

