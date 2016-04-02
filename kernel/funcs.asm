[BITS 32]

	global	io_hlt
	global	io_cli
	global	io_sti
	global	io_stihlt
	global	io_load_eflags
	global	io_store_eflags
	global	load_cr0
	global	store_cr0
	global	load_gdtr
	global	load_idtr
	global	io_in8
	global	io_in16
	global	io_in32
	global	io_out8
	global	io_out16
	global	io_out32
	global	load_tr
	global	farjmp
	global	load_dr0
	global	load_dr1
	global	load_dr2
	global	load_dr3
	global	load_dr6
	global	load_dr7
	global	store_dr0
	global	store_dr1
	global	store_dr2
	global	store_dr3
	global	store_dr6
	global	store_dr7
	global	load_cs
	global	load_eip
	global	asm_inthandler01
	global	asm_inthandler0d
	global	asm_inthandler20
	global	asm_inthandler21
	global	asm_inthandler2c
	extern	inthandler01
	extern	inthandler0d
	extern	inthandler20
	extern	inthandler21
	extern	inthandler2c


[SECTION .text]
io_hlt:
	hlt
	ret

io_cli:
	cli
	ret

io_sti:
	sti
	ret

io_stihlt:
	sti
	hlt
	ret

io_load_eflags:	; uint32_t io_load_eflags(void);
	pushfd		; eflags -> stack
	pop	eax
	ret

io_store_eflags:	; void io_store_eflags(uint32_t eflags)
	mov	eax, [esp + 4]
	push	eax
	popfd		; stack -> eflags
	ret

load_cr0:
	mov	eax, cr0
	ret

store_cr0:
	mov	eax, [esp + 4]
	mov	cr0, eax
	ret

load_gdtr:		; void load_gdtr(uint32_t limit, uint32_t addr)
	mov	ax, [esp + 4]
	mov	[esp + 6], ax
	lgdt	[esp + 6]
	ret

load_idtr:		; void load_idtr(uint32_t limit, uint32_t addr)
	mov	ax, [esp + 4]
	mov	[esp + 6], ax
	lidt	[esp + 6]
	ret

io_in8:		; uint32_t io_int8(uint32_t port)
	mov	edx, [esp + 4]
	mov	eax, 0
	in	al, dx
	ret

io_in16:		; uint32_t io_int16(uint32_t port)
	mov	edx, [esp + 4]
	mov	eax, 0
	in	ax, dx
	ret

io_in32:		; uint32_t io_int32(uint32_t port)
	mov	edx, [esp + 4]
	in	eax, dx
	ret

io_out8:		; void io_out8(uint32_t port, uint32_t data)
	mov	edx, [esp + 4]
	mov	al, [esp + 8]
	out	dx, al
	ret

io_out16:		; void io_out16(uint32_t port, uint32_t data)
	mov	edx, [esp + 4]
	mov	ax, [esp + 8]
	out	dx, ax
	ret

io_out32:		; void io_out32(uint32_t port, uint32_t data)
	mov	edx, [esp + 4]
	mov	eax, [esp + 8]
	out	dx, eax
	ret

load_tr:		; void load_tr(uint32_t tr)
	ltr	[esp + 4]
	ret

farjmp:		; void farjmp(uint32_t eip, uint32_t cs)
	jmp	far [esp + 4]
	ret

load_dr0:	; uint32_t load_dr0()
	mov	eax, dr0
	ret

load_dr1:	; uint32_t load_dr1()
	mov	eax, dr1
	ret

load_dr2:	; uint32_t load_dr2()
	mov	eax, dr2
	ret

load_dr3:	; uint32_t load_dr3()
	mov	eax, dr3
	ret

load_dr6:	; uint32_t load_dr6()
	mov	eax, dr6
	ret

load_dr7:	; uint32_t load_dr7()
	mov	eax, dr7
	ret

store_dr0:	; void store_dr0(uint32_t dr0)
	mov	eax, [esp + 4]
	mov	dr0, eax
	ret

store_dr1:	; void store_dr1(uint32_t dr1)
	mov	eax, [esp + 4]
	mov	dr1, eax
	ret

store_dr2:	; void store_dr2(uint32_t dr2)
	mov	eax, [esp + 4]
	mov	dr2, eax
	ret

store_dr3:	; void store_dr3(uint32_t dr3)
	mov	eax, [esp + 4]
	mov	dr3, eax
	ret

store_dr6:	; void store_dr6(uint32_t dr6)
	mov	eax, [esp + 4]
	mov	dr6, eax
	ret

store_dr7:	; void store_dr7(uint32_t dr7)
	mov	eax, [esp + 4]
	mov	dr7, eax
	ret

load_cs:	; uint32_t load_cs()
	mov	eax, 0
	mov	ax, cs
	ret

load_eip:	; uint32_t load_eip()
	mov	eax, [esp]
	ret

asm_inthandler01:	; asm_inthandler01()
	push	es
	push	ds
	pushad
	mov	eax, esp
	push	eax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	call	inthandler01
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthandler0d:	; asm_inthandler0d()
	push	es
	push	ds
	pushad
	mov	eax, esp
	push	eax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	call	inthandler0d
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthandler20:	; asm_inthandler20()
	push	es
	push	ds
	pushad
	mov	eax, esp
	push	eax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	call	inthandler20
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthandler21:	; asm_inthandler21()
	push	es
	push	ds
	pushad
	mov	eax, esp
	push	eax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	call	inthandler21
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthandler2c:	; asm_inthandler2c()
	push	es
	push	ds
	pushad
	mov	eax, esp
	push	eax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	call	inthandler2c
	pop	eax
	popad
	pop	ds
	pop	es
	iretd
