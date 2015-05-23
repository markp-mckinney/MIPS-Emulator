         .data
numA:    .word    0x86D23358
numB:	 .word	  0x86D23368
opSpce:	 .asciiz  "  and  "
sumLbl:  .asciiz  " -> sum: "
difLbl:	 .asciiz  "   dif: "
prdLbl:  .asciiz  "   prd: "
buff:    .space   10

EXPMASK: .word	  0x7F800000	  
MAGMASK: .word	  0x007FFFFF
SINMASK: .word	  0x80000000

         .text
         .globl   main




#a0 is number to convert to hex
#a1 points to a buffer to write that hex string into
#t0 is cursor in output buffer
#t1 is current byte being converted
#t2 is current 4 bits being converted to hex char
#t3 is loop counter (4, 3, 2, 1)
#t4 is address of current byte being converted
bintohex:
         #Callee doesn't push $ra cuz this won't call other functions
         #callee pushes frame pointer
         addi  $sp, $sp, -4
         sw    $fp, 0($sp)

         #callee saves local variable spot for $a0
         addi  $sp, $sp, -4
         #callee doesn't need to save any $sx

         sw    $a0, 0($sp)          #copy $a1 to first local var
         addi  $t4, $sp, 3          #save address of number being converetd
         addi  $t0, $a1, 0          #copy $a1 into $t0
         addi  $t3, $zero, 4        #$t3 = 4

looptop: 
         lb    $t1, 0($t4)          #copy the ith byte into $t1
         addi  $t4, $t4, -1				#for every byte in $a0 (store the byte in $t1):

         srl   $t2, $t1, 4          #logical shift $t1 4 bits store in $2
         andi  $t2, $t2, 15         #Get rid of any sign extension that came from lb load byte

#converts the lower 4 bytes in $t1 to the ascii code representing
#it in hex.  Jumps back to the address stored in $t8
         addi  $t9, $zero, 9
         ble   $t2, $t9, skippedA	#if $t2 <= 9, it's 0-9

         addi  $t2, $t2, 7         #$t2 + 7 -> $t2
skippedA:  
         addi  $t2, $t2, 48         #$t2 + 48 -> $t2

         sb    $t2, 0($t0)          #$t2 -> store in *$t0
         addi  $t0, $t0, 1          #increment $t0 by one char
            

         andi  $t2, $t1, 15         #and $t1 with xF -> store in $t2
         addi  $t9, $zero, 9
         ble   $t2, $t9, skippedB   #if $t2 <= 9, skip the next step

         addi  $t2, $t2, 7         #$t2 + 7 -> $t2
skippedB:  
         addi  $t2, $t2, 48         #$t2 + 48 -> $t2

         sb    $t2, 0($t0)          #$t2 -> store in *$t0
         addi  $t0, $t0, 1          #increment $t0 by one char

         addi  $t3, $t3, -1
         bgt   $t3, $zero, looptop  #if $t3 is nonzero, go back to the start of the for
			
		 sb		$zero, 0($t0)

         #callee pops frame pointer into $fp
         lw    $fp, 0($sp)
         addi  $sp, $sp, 4

         #callee pops return address and jumps to it
         jr    $ra
# end of binToHex

#a0 exponent
#a1 magnitude
#v0 float result
#t8 is sign
packFloat:
		 beq	$a1, $zero, retZero
		
		 bgt	$a1, $zero, positive
		 addi	$t8, $zero, 1
		 sub	$a1, $zero, $a1

positive:
		 
		 #shift shift left until there's a bit in the exponent area (hopefully there's one and it's the hidden bit)
		 lw	    $t9, MAGMASK
		 xor	$t9, $t9, $zero
topNormUp:
		 and	$t7, $a1, $t9
		 beq	$t7, $zero, doNormUp
		 j		endNormUp
doNormUp:
		 sll	$a1, $a1, 1
		 addi	$a0, $a0, -1
		 j		topNormUp
endNormUp:

		 sll	$t9, $t9, 1
topNormDown:
		 and	$t7, $a1, $t9
		 bne	$t7, $zero, doNormDown
		 j		endNormDown
doNormDown:
		 srl	$a1, $a1, 1
		 addi	$a0, $a0, 1
		 j		topNormDown
endNormDown:
		 

		 beq	$a1, $zero, retZero
		 
		 #add in sign bit
		 sll	$v0, $t8, 31
		
		 #add in exp section
		 addi	$a0, $a0, 127
		 sll	$a0, $a0, 23
		 lw		$t9, EXPMASK
		 and	$a0, $a0, $t9
		 or		$v0, $v0, $a0

		 #add in mag section
		 lw		$t9, MAGMASK
		 and	$a1, $a1, $t9
		 or		$v0, $v0, $a1
		 
		 jr		$ra

retZero:
		 add	$v0, $zero, $zero
		 jr	    $ra
#end fo packFloat
               

#takes float to unpck in $a0
#puts exponent in $v0
#puts magnitude in $v1 (with sign from sign bit)
unpackFloat:
		 lw 	$v0, EXPMASK
		 and	$v0, $v0, $a0
		 srl	$v0, $v0, 23
		 addi	$v0, $v0, -127
		 lw		$v1, MAGMASK
		 and	$v1, $v1, $a0
		 lw		$t9, SINMASK
		 and	$t9, $t9, $a0
		 beq	$t9, $zero, unpackFloatEnd
		 sub	$v1, $zero, $v1
unpackFloatEnd:	jr	$ra
# end of unpackFloat

#takes a float in $a0
#takes a float in $a1
#returns a float in $v0
#t0 is aExp
#t1 is aMag
#t2 is bExp
#t3 is bMag
floatAdd:
		 #save return address
         addi  $sp, $sp, -4
         sw    $ra, 0($sp)
		 
		 jal unpackFloat
		 
		 add   $t0, $v0, $zero
		 add   $t1, $v1, $zero
		
		 #unpack second float 
		 add   $a0, $a1, $zero
		 jal unpackFloat
		 
		 add   $t2, $v0, $zero
		 add   $t3, $v1, $zero

scaleATop:
		 blt   $t0, $t2, doScaleA
		 j	   endScaleA
doScaleA:
		 addi  $t0, $t0, 1
		 srl   $t1, $t1, 1
		 j     scaleATop
endScaleA:

scaleBTop:
		 blt   $t2, $t0, doScaleB
		 j	   endScaleB
doScaleB:
		 addi  $t2, $t2, 1
		 srl   $t3, $t3, 1
		 j	   scaleBTop
endScaleB:
		
		 addi  $a0, $t0, 0
		 add   $a1, $t1, $t3
		 jal   packFloat

		 lw	   $ra, 0($sp)
		 addi  $sp, $sp, 4
		 
		 jr	   $ra
# end of floatAdd

floatSub:
		 #save return address
         addi  $sp, $sp, -4
         sw    $ra, 0($sp)
		 
		 lw		$t1, SINMASK
		 and	$t2, $a1, $t1
		 beq	$t2, $zero, itPoz
		 j		itNeg
itPoz:
		 or		$a1, $t1, $a1
		 j		doneFlipping
itNeg:
		 nor	$t1, $t1, $t1
		 and	$a1, $t1, $a1
		 j		doneFlipping
doneFlipping:
		 jal   floatAdd

		 lw	   $ra, 0($sp)
		 addi  $sp, $sp, 4
		 
		 jr	   $ra
# end of floatSub


#takes a float in $a0
#takes a float in $a1
#returns a float in $v0
#t0 is aExp
#t1 is aMag
#t2 is bExp
#t3 is bMag
floatProd:
		 #save return address
         addi  $sp, $sp, -4
         sw    $ra, 0($sp)
		 
		 jal unpackFloat
		 
		 add   $t0, $v0, $zero
		 add   $t1, $v1, $zero
		
		 #unpack second float 
		 add   $a0, $a1, $zero
		 jal unpackFloat
		 
		 add   $t2, $v0, $zero
		 add   $t3, $v1, $zero

		 mult  $t1, $t3
		 mfhi  $t4
		 mflo  $t5
		 srl   $t4, $t4, 23
		 sll   $t5, $t5, 9
		 or	   $a1, $t4, $t5
		 add   $a0, $t0, $t2
		 jal   packFloat

		 lw	   $ra, 0($sp)
		 addi  $sp, $sp, 4
		 
		 jr	   $ra
# end of floatProd 


#main calls bintohex with an arbitrary number and prints result
main:
		 #print first operand
		 lw		$a0, numA
		 la		$a1, buff
		 jal	bintohex

		 la		$a0, buff
		 li		$v0, 4
		 syscall
		
		 la		$a0, opSpce
		 li		$v0, 4
		 syscall
	
		 lw		$a0, numB
		 la		$a1, buff
		 jal	bintohex

		 la		$a0, buff
		 li		$v0, 4
		 syscall
		
		 #SUMNATION SECTION ----------------------------------
		 #print label for sum
         la 	$a0, sumLbl 
         li 	$v0, 4
         syscall              #print the string starting at $a0

		 #calculate sum
         lw 	$a0, numA
         lw 	$a1, numB
         jal 	floatAdd

		 #convert sum to hex
		 add	$a0, $v0, $zero
		 la 	$a1, buff
		 jal 	bintohex

		 #print hex of sum
         la 	$a0, buff
         li 	$v0, 4
         syscall              #print the string starting at $a0

		 #DIFFERENCE SECTION -------------------------------

		 #print label for diff
         la 	$a0, difLbl 
         li 	$v0, 4
         syscall              #print the string starting at $a0
		 #calculate difference
         lw 	$a0, numA
         lw 	$a1, numB
         jal 	floatSub

		 #convert difference to hex
		 add	$a0, $v0, $zero
		 la 	$a1, buff
		 jal 	bintohex

		 #print hex
         la 	$a0, buff
         li 	$v0, 4
         syscall              #print the string starting at $a0

		 #PRODUCT SECTION-----------------------

		 #print label for prod 
         la 	$a0, prdLbl
         li 	$v0, 4
         syscall              #print the string starting at $a0

         #multiply
         lw 	$a0, numA
         lw 	$a1, numB
         jal 	floatProd
		
		 #convert product to hex
		 add	$a0, $v0, $zero
		 add	$t9, $v0 $zero
		 la 	$a1, buff
		 jal 	bintohex

		 #print the hex product
         la 	$a0, buff
         li 	$v0, 4
         syscall              #print the string starting at $a0


		 #HALT EXECUTION----------------------------------------
         li 	$v0, 10
         syscall              #quit

