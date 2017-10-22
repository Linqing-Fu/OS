.data
.text
	.globl main
#os_size:
#	.dword 0
main:
	# check the offset of main
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
li $3,0xa0800200
addu $4,$0,$3
addiu $5,$0,0x200
addiu $6,$0,0x110

#li $10,0xa0800a0 #os_size
#lw $11,($10) #load os_size into 11
#addiu $9,$0,0x9
#sll $6,$11,$9# $11 * 512

jal $8
lui $8,0xa080
ori $8,$8,0x026c
jal $8

jr $31


#need add code
	#read kernel

