// main {
//   var i;
//   i = 1 2 3 4 - * +; // 1+2*(3-4) = -1($ffff)
// }

.ORG	$0000	// IPLエリア
	call	_MAIN
	halt

.ORG	$1000	// ユーザエリア
_MAIN:
	mov r0, 1
	push r0
	mov r0, 2
	push r0
	mov r0, 3
	push r0
	mov r0, 4
	push r0

	pop r1 // 3
	pop r0 // 4
	sub r0, r1 // r0←3-4
	push r0

	pop r1 // (3-4)
	pop r0 // 2
	mul r0, r1 // r0←2*(3-4)
	push r0

	pop r1 // 2*(3-4)
	pop r0 // 1
	add r0, r1 // r0←1+2*(3-4)
	push r0	// 計算結果をstackに戻す

	pop r0		// r0←stack上の結果を取得
	mov r1, _I	// r1←_Iの番地(=$2000)
	mov *(r1),  r0	// _I←r0 (計算結果をI_の番地に格納する)
	
	ret


.ORG	$2000
DATA_START:
_I:
.DS		1, $0
