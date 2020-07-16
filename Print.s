; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
	AREA DATA, ALIGN=2
Array SPACE 20
    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
COUNT 	  EQU 0
LCD_OutDec
    ; copy/paste Lab 7 solution here


		PUSH{R4,R5,R6,R7,R8,LR}
		SUB SP,#8 ;allocation
		;MOV SP, SP
		MOV R1,#0
		MOV R2,#0
		MOV R3,#0
		MOV R4,#0
		MOV R5,#0
		MOV R6,#0
		MOV R7,#0
		STR R7,[SP,#COUNT] ; access
		
		CMP R0, #9
		BLS ZERO
		
		LDR R1, =Array
		MOV R2,R0 ;R2=497
		MOV R6,#10
AGAIN	UDIV R3, R2,R6 ; R3=49 4
		MUL R4,R3,R6 ; R4=49*10=490 40
		SUB R5, R2,R4 ; R5 = 497-490=7; 9
		
		STRB R5,[R1] ; PUT IT INTO THE ARRAY
		CMP R4,#0
		BEQ PRINT ; THIS MEANS WE ARE DONE AND READY TO PRINT
		
		
		LDR R7, [SP,#COUNT]
		ADD R7,R7,#1 ;INCREMENT COUNTER
		STR R7, [SP,#COUNT]
		ADD R1,R1,#1 ;NEXT SPOT IN ARRAY
		MOV R2,R3
		B AGAIN

PRINT	LDR R7, [SP,#COUNT]
		ADD R7,R7,#1 ;INCREMENT COUNTER
		STR R7, [SP,#COUNT]
LOOP	LDRB R0,[R1] ; top of array = 4
		
		;STRB R2,[R1]
		ADD R0, R0,#0X30
		
		PUSH{R1,R2}
		BL ST7735_OutChar
		POP{R1,R2}
		
		LDR R7, [SP,#COUNT]
		SUB R7,R7,#1 ;INCREMENT COUNTER
		STR R7, [SP,#COUNT]
		CMP R7,#0
		BLE DONE
		SUB R1,R1,#1
		
		B LOOP
		
ZERO	ADD R0,R0,#0X30
		
		BL ST7735_OutChar
		
		
DONE	ADD SP, #8 ; deallocation
		;MOV SP,SP
		POP{R4,R5,R6,R7,R8,LR}

      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
DIVIDE EQU 0 
LCD_OutFix
    ; copy/paste Lab 7 solution here
	

		PUSH{R4,R5,R6, LR}
		SUB SP,#8 ; aloccation
		MOV R5,#0
		STR R5,[SP,#DIVIDE]
		CMP R0,#1000
		BHS FALSE
		
		
		MOV R1, R0 ;R1 IS THE VALUE =222
		MOV R2, #100
		UDIV R3, R1,R2 ;R3 = 2
		MUL R4, R3, R2 ; R4=200
		
		LDR R5, [SP,#DIVIDE]
		SUB R5, R1, R4 ; R5 = 222-200=22
		STR R5, [SP,#DIVIDE]
		
		MOV R0,R3
		ADD R0,R0,#0X30
		PUSH{R4,R5,R6,R7}
		BL ST7735_OutChar
		POP{R4,R5,R6,R7}
		MOV R0,R5
	
		MOV R0,#0X2E
		PUSH{R4,R5,R6,R7}
		BL ST7735_OutChar
		POP{R4,R5,R6,R7}
		MOV R0,R5
		
		
		MOV R1, R0 ;R1 IS THE VALUE =222
		MOV R2, #10
		UDIV R3, R1,R2 ;R3 = 2
		MUL R4, R3, R2 ; R4=200
		
		LDR R5, [SP,#DIVIDE]
		SUB R5, R1, R4 ; R5 = 222-200=22
		STR R5, [SP,#DIVIDE]
		
		MOV R0,R3
		ADD R0,R0,#0X30
		PUSH{R4,R5,R6,R7}
		BL ST7735_OutChar
		POP{R4,R5,R6,R7}
		MOV R0,R5
		
		MOV R1, R0 ;R1 IS THE VALUE =222
		MOV R2, #1
		UDIV R3, R1,R2 ;R3 = 2
		MUL R4, R3, R2 ; R4=200
		
		LDR R5, [SP,#DIVIDE]
		SUB R5, R1, R4 ; R5 = 222-200=22
		STR R5, [SP,#DIVIDE]
		
		MOV R0,R3
		ADD R0,R0,#0X30
		PUSH{R4,R5,R6,R7}
		BL ST7735_OutChar
		POP{R4,R5,R6,R7}
		MOV R0,R5
		
		
				
		B FIN
		
FALSE	MOV R0, #0X2A
		PUSH{R4,R5,R6,R7}
		BL ST7735_OutChar
		POP{R4,R5,R6,R7}
		MOV R0, #0X2E
		PUSH{R4,R5,R6,R7}
		BL ST7735_OutChar
		POP{R4,R5,R6,R7}
		MOV R0, #0X2A
		PUSH{R4,R5,R6,R7}
		BL ST7735_OutChar
		POP{R4,R5,R6,R7}
		MOV R0, #0X2A
		PUSH{R4,R5,R6,R7}
		BL ST7735_OutChar
		POP{R4,R5,R6,R7}
		
FIN		ADD SP, #8 ; deallocation
		POP{R4,R5,R6, LR}
     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
