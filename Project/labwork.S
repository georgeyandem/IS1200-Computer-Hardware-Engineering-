  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #	      #because the GNU Assembler (GAS), which is part of the GCC toolchain, uses ".globl" instead of ".global".
.globl hexasc   #Global användas för att göra exmeplvis en function tillgänglig i andra filer än bara den man jobbar i. Mer specifikt gör man symbolen synlig till en linker som göt det möjligt för funktionen att refereras i andra filer
.globl delay   #For example, if you have a function defined in one file and you want to use it in another file, you can use the ".global" directive to make the function symbol visible to the linker so that it can be linked and used in the other file
.globl time2string
.globl enable_interrupt

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm

  # Please add your own code below this line

.data

.text
 hexasc:

	andi $v0,$a0,0xf        # save only low 4 bits
	addi $v0,$v0,0x30   # turn into ASCII
	ble $v0,0x39,foo     # om 0x39 less than or equal to v0 --- jump to foo
	nop
	addi $v0,$v0,7      #   add additional 7 for 'A'-'F'
foo:
	jr $ra
	nop

delay:

PUSH $s0

addi $s0, $a0, 0 
addi $t1, $zero, 0 
addi $t4, $zero, 1 
addi $t5, $zero, 4511 

	while:
	beq $s0, $zero, end
	nop
	
	slti $t1, $s0, 0 
	beq $t1, $t4, end
	nop
	
	addi $s0, $s0, -1 

		for:
		beq $t1, $t5, while 
		nop
		addi $t1,$t1, 1
		j for
		nop
	
	j while
	nop

end:
 POP $s0
 jr $ra
 nop

 		
time2string:

# Sätter s0,s1,ra i stacken för att kunna spara innehållet eftersom vi behöver återställa dessa i slutet
    PUSH    $s0
    PUSH    $s1            
    PUSH    $ra       
    move    $s1, $a1        # s1 = a1, innehåller tidinfo (0x5957)
    move     $s0, $a0       # s0 = a0, innehåller adressen för string (timstr)

    # NR 1
    # a1 = 0101100101010111 = 59:57 = s1 = a0
    #av 16-bits skiftas de 4 MSB till LSB och sparas i a0.
    # Funktionen hexa kallas som tar de 4 LSB och konverterar till ASCII
    # Detta returvärdet sparas i första bytepositionen i $s0.
    srl     $a0, $s1, 12       
    jal hexasc
    nop
    sb     $v0, 0($s0)         


    # NR 2, fungerar på samma sätt som NR 1 men 8 ist för 12 o sparar i andra bytepos i s0
    srl     $a0, $s1, 8
    jal     hexasc
    nop
    sb     $v0, 1($s0)

    # Ett kolon behövs för att separera min o sek
    li     $t1, 0x3a         # laddar adressen av ASCII-koden för ett kolon tecken
    sb     $t1, 2($s0)        # spara t1 (:) i tredje byte-positionen s0

    # NR3, funkar på likannde säätt som de andra
    srl     $a0, $s1, 4
    jal     hexasc
    nop
    sb     $v0, 3($s0)

    # NR 4
    srl     $a0, $s1, 0       
    jal     hexasc
    nop
    sb     $v0, 4($s0)

    # null byte
    li     $t1, 0x00         
    sb     $t1, 5($s0)        
    j     exit
    nop
    # Slutet, restore register och hoppa tillbaka till caller.
exit:

#Återställer innehllet av ra, s1 , s0 genom POP. Last in first out.
        POP     $ra
        POP     $s1
        POP     $s0
        jr     $ra
        nop

enable_interrupt:
	ei
	jr $ra
	nop