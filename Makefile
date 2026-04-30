CC = gcc
LD = ld
AS = nasm

CFLAGS = -m64 -ffreestanding -fno-stack-protector -nostdlib -mno-red-zone -Iinclude/kernel -Wall -Wextra
LDFLAGS = -n -T linker.ld -m elf_x86_64
ASFLAGS = -f elf64

ASM_SOURCES = $(wildcard src/boot/*.asm) $(wildcard src/kernel/*.asm)
C_SOURCES = $(wildcard src/kernel/*.c)
OBJ = $(ASM_SOURCES:.asm=.o) $(C_SOURCES:.c=.o)

# All assembly files use elf64 format
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJ)

iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/kernel.bin
	echo 'set timeout=0' > isodir/boot/grub/grub.cfg
	echo 'set default=0' >> isodir/boot/grub/grub.cfg
	echo '' >> isodir/boot/grub/grub.cfg
	echo 'menuentry "Zoho OS" {' >> isodir/boot/grub/grub.cfg
	echo '	multiboot2 /boot/kernel.bin' >> isodir/boot/grub/grub.cfg
	echo '	boot' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o zoho_os.iso isodir

run: iso
	qemu-system-x86_64 -cdrom zoho_os.iso -serial stdio

clean:
	rm -rf src/boot/*.o src/kernel/*.o kernel.bin zoho_os.iso isodir
