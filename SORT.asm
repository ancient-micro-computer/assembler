// マクロ定義
.DEF	REG_SORT_MODE	r0
.DEF	REG_START_ADR	r1
.DEF	REG_END_ADR		r2

.ORG	$0000	// IPLエリア
	mov		r0, 1
	mov		r1, DATA_START
	mov		r2, DATA_END
	call	SORT_START
	halt

// 割込みベクタ設定
.ORG	$0010
.DS		1,	INT_TMR_SUB
.DS		1,	INT_SWI_SUB

.ORG	$1000	// ユーザエリア
SORT_ROUTINE_1:	mov	r3	r1
LOOP_1_START:	cmp	r3	r2
	brgt	LOOP_1_END
	breq	LOOP_1_END
	mov	r4	r2
LOOP_2_START:	cmp	r4	r3
	brlt	LOOP_2_END
	breq	LOOP_2_END		
	mov	r5	0	
	mov	r6	*(r4)	0
	mov	r7	*(r4)	-1
	cmp	r6	r7	
	breq	CHECK_END		
	brlt	CHECK_END		
	mov	r4	r7	
	mov	r5	r4	
	dec	r5		
	mov	r5	r6	
CHECK_END:	sub	r4	1	
	br	LOOP_2_START		
LOOP_2_END:	add	r3	1	
	br	LOOP_1_START		
LOOP_1_END:	br	SORT_ROUTINE_END

SORT_ROUTINE_2:	mov	r3	r1	
LOOP_1_START:	cmp	r3	r2	
	brgt	LOOP_1_END		
	breq	LOOP_1_END		
	mov	r4	r2	
LOOP_2_START:	cmp	r4	r3	
	brlt	LOOP_2_END		
	breq	LOOP_2_END		
	mov	r5	0	
	mov	r6	*(r4)	0
	mov	r7	*(r4)	-1
	cmp	r6	r7	
	breq	CHECK_END		
	brgt	CHECK_END		
	mov	r4	r7	
	mov	r5	r4	
	dec	r5		
	mov	r5	r6	
CHECK_END:	sub	r4	1	
	br	LOOP_2_START		
LOOP_2_END:	add	r3	1	
	br	LOOP_1_START		
LOOP_1_END:	br	SORT_ROUTINE_END		
SORT_ROUTINE_END:	pop	r0		
	pop	r1		
	pop	r2		
	pop	r3		
	pop	r4		
	pop	r5		
	pop	r6		
	pop	r7		
	pop	r8		
SORT_END:	ret

SORT_START:	push	r8		
	push	r7		
	push	r6		
	push	r5		
	push	r4		
	push	r3		
	push	r2		
	push	r1		
	push	r0		
	mov r7, $0001
	mov r8,	$0018
	mov *(r8), r7		// 1=timer enable
	mov r7, $03e8
	mov r8,	$0019
	mov *(r8), r7		// interval=1000msec

	mov	r8	REG_END_ADR
	sub	r8	REG_START_ADR
	cmp	r8	0	
	brlt	SORT_ROUTINE_END		
	breq	SORT_ROUTINE_END		
	cmp	REG_SORT_MODE	1
	breq	SORT_ROUTINE_1		
	cmp	REG_SORT_MODE	2
	breq	SORT_ROUTINE_2		
	br	SORT_ROUTINE_END		

INT_TMR_SUB:
	DI					// 割込み禁止
	push	r1
	push	r2
	mov		r1, $ffff
	sub		r1, $0100	// タイマー割込み要因クリアマスク生成
	and		INT, r1		// 要因クリア
	// カウンタ更新
	mov		r2,	G_TMR_CNT
	mov		r1, *(r2), 0
	inc		r1
	mov		*(r2), r1
	pop		r2
	pop		r1
	EI					// 割込み許可
	RETI

INT_SWI_SUB:
	DI					// 割込み禁止
	push	r1
	push	r2
	mov		r1, $ffff
	sub		r1, $0200	// SW割込み要因クリアマスク生成
	and		INT, r1		// 要因クリア
	pop		r2
	pop		r1
	EI					// 割込み許可
	RETI

G_TMR_CNT:
.DS		1, $0000

.ORG	$A000
DATA_START:
.DS		1, $0002
.DS		1, $0003
.DS		1, $0006
.DS		1, $0015
.DS		1, $0011
.DS		1, $001a
.DS		1, $0005
.DS		1, $0007
.DS		1, $0000
.DS		1, $000a
.DS		1, $001c
.DS		1, $0004
.DS		1, $000d
.DS		1, $000f
.DS		1, $0010
.DS		1, $0012
.DS		1, $0013
.DS		1, $001f
.DS		1, $0001
.DS		1, $000c
.DS		1, $0008
.DS		1, $0009
.DS		1, $000e
.DS		1, $0016
.DS		1, $0017
.DS		1, $000b
.DS		1, $001d
.DS		1, $0018
.DS		1, $001b
.DS		1, $0014
.DS		1, $0019
.DS		1, $001e
DATA_END:
