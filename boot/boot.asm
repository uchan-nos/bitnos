; 640x 480, 16bit
VESA2_MODE1	equ	0x114
VESA2_MODE2	equ	0x114
VESA2_MODE3	equ	0x111

;		4bit	8bit	15bit	16bit	24/32bit
;320x 200			0x10d	0x10e	0x10f
;640x 400		0x100			
;640x 480	(VGA)	0x101	0x110	0x111	0x112
;800x 600	0x102	0x103	0x113	0x114	0x115
;1024x 768	0x104	0x105	0x116	0x117	0x118
;1280x1024	0x106	0x107	0x119	0x11a	0x11b
;1600x1200		0x11c	0x11d	0x11e	0x11f


; bitnosのメモリマップメモ
; 0x00000000 - 0x0000ffff : free after boot
; 0x00010000 - 0x0001ffff : 64KB GDT
; 0x00020000 - 0x000207ff : 2KB IDT
; 0x00100000 - 0x00267fff : 1440KB fd
; 0x00268000 - 0x00ffffff : free after boot
; 0x01000000 - 0x010fffff : 1MB bootpack.nos
; 0x01100000 - 0x011fffff : 1MB stack + .data + .bss
; 0x01200000 -            : free

BOOTPACK	equ	0x01000000
DISKCACHE	equ	0x00100000
DISKCACHE0	equ	0x00008000


SECS	equ	0x0f00		; 読み込んだセクタ数（ブートセクタが設定する）
LEDS	equ	0x0f04		; LEDの状態
VCOL	equ	0x0f08		; 画面の色数（ビット）
VX	equ	0x0f0c		; Xの解像度
VY	equ	0x0f10		; Yの解像度
VRAM	equ	0x0f14		; VRAM開始アドレス


	org	0xc200
	
	; VESAのサポートチェック
	mov	ax, 0x7e0	; 0x7e00からの256バイトに受け取る
	mov	es, ax
	mov	di, 0
	mov	ax, 0x4f00
	int	0x10
	
	cmp	al, 0x4f
	jne	vesa_nonsupport
	mov	ax, [es:di + 0x04]	; VESAのバージョン（例：1.02->0x0102）
	cmp	ah, 2
	jae	vesa_ver2
	
vesa_ver1:			; VESAバージョン1
vesa_nonsupport:		; VESAのサポートはなし
	mov	si, msg_vesa_ver_fail
	call	puts
	jmp	fin

vesa_ver2:			; VESAバージョン2以上
	mov	ax, 0x4f02
	mov	bx, VESA2_MODE1 + 0x4000
	int	0x10
	cmp	ax, 0x004f
	je	vesa_mode_end
	
	mov	ax, 0x4f02
	mov	bx, VESA2_MODE2 + 0x4000
	int	0x10
	cmp	ax, 0x004f
	je	vesa_mode_end
	
	mov	ax, 0x4f02
	mov	bx, VESA2_MODE3 + 0x4000
	int	0x10
	cmp	ax, 0x004f
	je	vesa_mode_end
	
	mov	si, msg_vesa_mode_fail
	call	puts
	jmp	fin
	
vesa_mode_end:			; complete vesa setting
	; 各種数値をメモする
	mov	ax, 0x7e0	; 0x7e00からの256バイトに受け取る
	mov	es, ax
	mov	di, 0
	mov	ax, 0x4f01
	mov	cx, bx
	and	cx, 0x3fff	; 画面モード番号を取り出す
	int	0x10		; そのモード番号の情報を取得
	
	cmp	ax, 0x004f
	jne	vesa_get_fail
	
	cmp	byte [es:di + 0x1b], 0x06
	jne	vesa_fail
	
	mov	eax, 0
	mov	al, [es:di + 0x19]
	mov	[VCOL], eax
	mov	ax, [es:di + 0x12]
	mov	[VX], eax
	mov	ax, [es:di + 0x14]
	mov	[VY], eax
	mov	eax, [es:di + 0x28]
	mov	[VRAM], eax
	
	jmp	keystatus

vesa_get_fail:
	mov	si, msg_vesa_get_fail
	call	puts
	jmp	fin

vesa_fail:
	mov	si, msg_vesa_fail
	call	puts
	jmp	fin

keystatus:
	; get keyboard status
	mov	ah, 0x02
	int	0x16
	and	eax, 0x000f
	mov	[LEDS], eax
	
	; PICの全割り込みを禁止にする
	mov	al, 0xff
	out	0x21, al		; output to PIC0-OCW1(IMR)
	nop
	out	0xa1, al		; output to PIC1-OCW1(IMR)
	
	; CPUレベルで割り込みを禁止にする
	cli
	
	; A20の制限をはずす
	call	wait_keyboardbusy
	mov	al, 0xd1
	out	0x64, al
	call	wait_keyboardbusy
	mov	al, 0xdf
	out	0x60, al
	call	wait_keyboardbusy
	
	; 486の本を参考に追加
	mov	al, 0xff
	out	0x64, al
	call	wait_keyboardbusy
	
	; プロテクトモードに移行する
	LGDT	[GDTR0]
	
	; PG、PEビット
	mov	eax, cr0
	and	eax, 0x7fffffff		; PG = 0 ページング禁止
	or	eax, 0x00000001		; PE = 1 プロテクトモード
	mov	cr0, eax
	jmp	pipeline		; パイプラインをクリアする
pipeline:
	mov	ax, 1 * 8		; read write 32bit segment
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	; copy bootpack
	
	mov	esi, bootpack0		; src
	mov	edi, BOOTPACK		; dest
	mov	ecx, 1024 * 1024 / 4	; size / 4
	call	memcpy

	mov	eax, bootpack0
	mov	[0xf30], eax
	
	; copy disk cache
	; boot sector
	mov	esi, 0x7c00
	mov	edi, DISKCACHE
	mov	ecx, 512 / 4
	call	memcpy
	
	; rest of cache
	mov	esi, DISKCACHE0 + 512
	mov	edi, DISKCACHE + 512
	mov	ecx, [SECS]		; sector number
	imul	ecx, 512 / 4		; convert to byte size
	sub	ecx, 512 / 4		; subtract the size of IPL
	call	memcpy
	
	; run the bootpack
	; copy .data section
	mov	ebx, BOOTPACK
	;mov	ecx, [ebx + 16]		; .data size
	mov	ecx, [ebx + 28]		; .data size
	add	ecx, 3
	shr	ecx, 2			; 4バイト単位にする
	jz	clearbss		; if .data size = 0 then jump
	;mov	esi, [ebx + 20]
	mov	esi, [ebx + 32]
	add	esi, ebx
	;mov	edi, [ebx + 12]
	mov	edi, [ebx + 24]
	call	memcpy

clearbss:
	;mov	ecx, [ebx + 32]		; heap開始アドレス
	mov	ecx, [ebx + 36]		; heap開始アドレス
	;sub	ecx, [ebx + 12]		; .data転送先アドレスを引く
	sub	ecx, [ebx + 24]		; .data転送先アドレスを引く
	;sub	ecx, [ebx + 16]		; .dataサイズを引く
	sub	ecx, [ebx + 28]		; .dataサイズを引く
	jz	execbootpack		; if .bss size = 0 then jump
	;mov	edi, [ebx + 12]
	mov	edi, [ebx + 24]
	;add	edi, [ebx + 16]
	add	edi, [ebx + 28]
clearbss_loop:
	mov	byte [edi], 0		; .bssを0クリア
	inc	edi
	dec	ecx
	jnz	clearbss_loop

execbootpack:
	;mov	esp, [ebx + 12]
	mov	esp, [ebx + 24]
	jmp	dword 2 * 8 : 0x00000003

fin:
	hlt
	jmp	fin

; ************
; サブルーチン
; ************
puts:
	mov	al, [si]
	inc	si
	cmp	al, 0x00
	je	puts_end
	mov	ah, 0x0e
	mov	bh, 0
	mov	bl, 0
	int	0x10
	jmp	puts
puts_end:
	ret

wait_keyboardbusy:
	in	al, 0x64
	and	al, 0x02
	jnz	wait_keyboardbusy
	ret

memcpy:
	; ecx = size
	; esi = src addr
	; edi = dest addr
	
	mov	eax, [esi]
	add	esi, 4
	mov	[edi], eax
	add	edi, 4
	dec	ecx
	jnz	memcpy
	ret

; ******
; データ
; ******
msg_vesa_ver_fail:
	db	"VESA 2.0 or above isn't supported.", 0x0a, 0x00

msg_vesa_mode_fail:
	db	"can't change VESA mode", 0x0a, 0x00

msg_vesa_get_fail:
	db	"can't get information of VESA mode", 0x0a, 0x00

msg_vesa_fail:
	db	"can't use VESA", 0x0a, 0x00


; グローバルディスクリプタ
GDT0:
	times	4 dw 0				; null selector
	dw	0xffff, 0x0000, 0x9200, 0x00cf	; read write base=0x00000000 limit=1M*4K
	dw	0x00ff, 0x0000, 0x9a00, 0x01c0	; read exec  base=0x01000000 limit=256*4K

	dw	0


GDTR0:
	dw	8 * 3 - 1
	dd	GDT0


bootpack0:
