
READ_SEC	equ	0x7e00
TEMP		equ	0x7e04

	org	0x7c00

	JMP	entry
	DB	0x90
	DB	"BITNOS  "	; ブートセクタの名前を自由に書いてよい（8バイト）
	DW	512		; 1セクタの大きさ（512にしなければいけない）
	DB	1		; クラスタの大きさ（1セクタにしなければいけない）
	DW	1		; FATがどこから始まるか（普通は1セクタ目からにする）
	DB	2		; FATの個数（2にしなければいけない）
	DW	224		; ルートディレクトリ領域の大きさ（普通は224エントリにする）
	DW	2880		; このドライブの大きさ（2880セクタにしなければいけない）
	DB	0xf0		; メディアのタイプ（0xf0にしなければいけない）
	DW	9		; FAT領域の長さ（9セクタにしなければいけない）
	DW	18		; 1トラックにいくつのセクタがあるか（18にしなければいけない）
	DW	2		; ヘッドの数（2にしなければいけない）
	DD	0		; パーティションを使ってないのでここは必ず0
	DD	2880		; このドライブ大きさをもう一度書く
	DB	0,0,0x29	; よくわからないけどこの値にしておくといいらしい
	DD	0xffffffff	; たぶんボリュームシリアル番号
	DB	"BITNOS BOOT"	; ディスクの名前（11バイト）
	DB	"FAT12   "	; フォーマットの名前（8バイト）
	TIMES	18 DB 0		; とりあえず18バイトあけておく

; プログラム本体

entry:
	mov	ax, 0
	mov	ss, ax
	mov	sp, 0x7c00		; 0x7c00以前は空いている
	mov	ds, ax
	mov	es, ax
	mov	si, mesg
	call	puts

	; ルートディレクトリエントリを使いたいため
	; とりあえず20セクタ以上読み込む
	
	; シリンダ0をすべて読む
	mov	ax, 0x0820
	mov	es, ax			; save at 0x8200 of memory
	mov	ch, 0			; cylinder 0
	mov	dh, 0			; head 0
	mov	cl, 2			; sector 2
	mov	di, 35
	call	fd_read
	
	; 0x2600からルートディレクトリエントリ
	; calculate file size of BOOT.SYS
	mov	ax, [0xa600 + 28]	; low 16 bits of file size
	shr	ax, 9			; sectors of file (low 7 bits)
	mov	dx, [0xa600 + 30]	; high 16 bits of file size
	shl	dx, 7			; sectors of file (high 9 bits)
	or	ax, dx
	inc	ax			; sectors of file
	;add	ax, 21			; sectors to read
	mov	di, ax			; save the number
	
	;cmp	di, 36			; 36sectors have already been read
	mov	ax, di
	add	ax, 33
	mov	word [0x0f00], ax
	mov	word [0x0f02], 0

	cmp	di, 3
	jbe	boot_program
	;sub	di, 36
	sub	di, 3
	

	; read file from disk
	mov	ax, 0x0c80
	mov	es, ax			; save at 0xc800 of memory
	mov	ch, 1			; cylinder 1
	mov	dh, 0			; head 0
	mov	cl, 1			; sector 1
	call	fd_read

boot_program:
	mov	si, mesg_ok
	call	puts
	mov	si, mesg2
	call	puts
	jmp	0xc200			; jump to boot.sys

fd_read:
fd_read_loop:
	mov	si, 0			; reset fail counter
fd_read_retry:
	mov	ah, 0x02		; read mode
	mov	al, 1			; read 1 sector
	mov	dl, 0x00		; drive A
	mov	bx, 0
	int	0x13			; read disk C-ch H-dh S-cl
					; and save at ES:BX
	jnc	fd_read_next		; if no error then jump
	
	inc	si			; increment fail counter
	cmp	si, 5
	jae	fd_read_error		; if fail counter >= 5 then jump
	
	mov	ah, 0x00
	mov	dl, 0x00		; drive A
	int	0x13			; reset drive
	jc	fd_read_retry_error
	jmp	fd_read_retry
fd_read_next:
	dec	di
	cmp	di, 0
	je	fd_end
	
	mov	ax, es
	add	ax, 0x20
	mov	es, ax
	inc	cl
	cmp	cl, 18
	jbe	fd_read_loop
	mov	cl, 1
	inc	dh
	cmp	dh,1
	jbe	fd_read_loop
	mov	dh, 0
	inc	ch
	jmp	fd_read_loop
fd_read_error:
fd_read_retry_error:
	mov	si, mesg_fail
	call	puts
	jmp	fin

fd_end:
	ret
					; 0x8000 + 0x4200 = 0xc200
fin:
	hlt
	jmp	fin

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

sethex4:	; ax : integer to convert
		; di : destination
	
	add	di, 3
	mov	cx, 4
sethex4_l3:
	mov	bx, ax
	and	bx, 0x000f
	cmp	bx, 10
	jae	sethex4_l1
	add	bx, '0'
	jmp	sethex4_l2
sethex4_l1:
	add	bx, 'A' - 10
sethex4_l2:
	mov	[di], bl
	shr	ax, 4
	dec	di
	dec	cx
	cmp	cx, 0
	ja	sethex4_l3
	ret

mesg:
	db	"loading boot file ", 0x00
mesg2:
	db	"booting operating system", 0x0a, 0x00

mesg_fail:
	db	"[FAILED]", 0x0a, 0x00
mesg_ok:
	db	"[  OK  ]", 0x0a, 0x00

	times	0x1fe - ($ - $$) db 0
	db	0x55, 0xaa

