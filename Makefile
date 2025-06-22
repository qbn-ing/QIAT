IMAGE = os.img

$(IMAGE):  clean font bootblock boot16 boot32 head64 app 
	dd if=/dev/zero of=$(IMAGE) bs=512 count=1024
	fdisk $(IMAGE) < boot/fdisk.txt # 生成分区表和引导扇区标志
	dd if=bootblock.bin of=$(IMAGE) seek=0 bs=512 conv=notrunc
	dd if=boot16.bin of=$(IMAGE) seek=1 bs=512 conv=notrunc
	dd if=boot32.bin of=$(IMAGE) seek=3 bs=512 conv=notrunc
	dd if=head64.bin of=$(IMAGE) seek=5 bs=512 conv=notrunc
	dd if=fonts.bin of=$(IMAGE) seek=100 bs=512 conv=notrunc
	dd if=app3.bin of=$(IMAGE) seek=105 bs=512 conv=notrunc
	dd if=app4.bin of=$(IMAGE) seek=136 bs=512 conv=notrunc
	dd if=app1.bin of=$(IMAGE) seek=160 bs=512 conv=notrunc
	objdump -D -b binary -m i386:x86-64 os.img > os.asm
	cp os.img os.bin
	grep -r 'Disassembly of section .text.unlikely:'

CC = gcc
AS = as
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump
CFLAGSNO = -fno-pic -static -fno-builtin -fno-strict-aliasing  -Wall -MD -ggdb -Werror -fno-omit-frame-pointer -fno-stack-protector -std=c11 -I. -fcf-protection=none

CFLAGS = -std=c11 -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -Werror -fno-omit-frame-pointer -fno-stack-protector -std=c11 -I. -fcf-protection=none #-mno-sse
# -fno 不要生成位置无关代码 -static 静态 -fno-builtin 不使用内建函数 -fno-strict-aliasing 不使用严格别名规则 -O2 优化级别 -Wall 显示所有警告 -MD 生成依赖文件 -ggdb 生成调试信息 -Werror 警告当错误 -fno-omit-frame-pointer 不省略帧指针 -fno-stack-protector 不使用栈保护
# 
#-mno-sse 不使用sse指令集
ASFLAGS = -gdwarf-2 #-Wa,-divide
# -gdwarf-2 生成调试信息 -Wa,-divide 使用除法指令实现除法操作
LDFLAGS16 = -m elf_i8086
LDFLAGS32 = -m elf_i386
# -m elf_i386 生成i386架构的elf文件
LDFLAGS64 = -m elf_x86_64 #--gc-sections移除后会出现问题,访问不到某些变量
# -m elf_x86_64 生成x86_64架构的elf文件

#引导扇区
bootblock: boot/bootblock.S boot/bootloader.c
	chmod +x boot/check.pl
	$(CC) $(CFLAGS) -m16 -c boot/bootloader.c -o bootloader.o # 这里务必用-m16,否则无法正确执行
	$(AS) $(ASFLAGS) -32 boot/bootblock.S -o bootblock.o
	$(LD) $(LDFLAGS32) -N -e start -Ttext 0x7c00 -o bootblock bootblock.o bootloader.o
	$(OBJDUMP) -D bootblock > bootblock.asm
	$(OBJCOPY) -S  -O binary -j .text bootblock bootblock.bin
	$(OBJDUMP) -D -b binary -m i386 bootblock > bootblock.ASM
	boot/check.pl bootblock.bin # 避免引导扇区过大

#真 bootloader
boot16: boot/bootloader.S boot/boot32.c
	$(CC) $(CFLAGS) -m16 -c boot/boot32.c -o boot32.o
	$(CC) $(CFLAGS) -m32 -c boot/bootloader.S -o boot16.o
	$(LD) $(LDFLAGS32) -N -e start -Ttext 0x7e00 -o boot16 boot16.o boot32.o
	$(OBJDUMP) -D   boot16 > boot16.asm
	$(OBJCOPY) -S -O binary -j .text boot16 boot16.bin
	$(OBJDUMP) -D -b binary -m i8086 boot16.bin > boot16.ASM

boot32: boot/boot32.S  boot/head64.c
	$(CC) $(CFLAGS) -m32 -c boot/head64.c -o head64.o
	$(CC) $(CFLAGS) -m32 -c boot/boot32.S -o boot32.o
	$(LD) $(LDFLAGS32) -N -e start32 -Ttext 0xa000 -o boot32 boot32.o  head64.o
	$(OBJDUMP) -D boot32 > boot32.asm
	$(OBJCOPY) -S -O binary  boot32 boot32.bin
	$(OBJDUMP) -D -b binary -m i386 boot32.bin > boot32.ASM

font: fonts/char2.c
	$(CC)  -c  fonts/char2.c -o fonts.o 
	$(OBJCOPY) -j .rodata -O binary  fonts.o fonts.bin


SRCS = $(wildcard mm/*.c)  $(wildcard lib/*.c) $(wildcard drivers/*.c)  $(wildcard kernel/*.c) #$(wildcard app/*.c)  #apps/toy0.c
# 最后编译main.c，避免有的数组在main.c里看不到
OBJS = $(SRCS:.c=.o)

.depend: $(SRCS)
	@rm -f .depend
	@$(foreach src,$(SRCS), \
		echo -n $(dir $(src)) >> .depend; \
		gcc -I. -MM $(src) >> .depend; \
	)
include .depend

# CFLAGS+= -mcmodel=kernel 

%.o: %.c
	$(CC) $(CFLAGS) -m64 -mcmodel=kernel  -c -o $@ $<
	$(OBJDUMP) -D -m i386:x86-64 $@ > $*.asm




head64: kernel/head64.S kernel/handler.S $(OBJS)  syscall
	$(CC) $(CFLAGS) -m64 -mcmodel=kernel  -c kernel/head64.S -o head64.o
	$(CC) $(CFLAGS) -m64 -mcmodel=kernel  -c  kernel/handler.S -o handler.o
	$(LD) $(LDFLAGS64)  -N -e _start -Ttext 0xffffffff80100000 head64.o handler.o std.o $(OBJS) -o head64 
	# $(LD) $(LDFLAGS64)  -N -T kernel/ld.ld head64.o handler.o std.o $(OBJS) -o head64 
	$(OBJDUMP) -D -m i386:x86-64 head64 > head64.asm
	$(OBJDUMP) -D -m i386:x86-64 head64.o > head64o.asm
	$(OBJCOPY) -S -O binary  head64 head64.bin
	$(OBJDUMP) -D -b binary -m i386:x86-64 head64.bin > head64.ASM

drivers: drivers/atkbd.c drivers/vesa.c
	$(CC) $(CFLAGS) -m64 -mcmodel=kernel  -c -o atkbd.o drivers/atkbd.c 

syscall: lib/syscall.o
	$(LD) -r -o std.o $^ 



# APPS = $(wildcard apps/*.S)
# SBJS = $(APPS:.S=.o)
# %.o: %.c
# 	$(CC) $(CFLAGS) -m64 -mcmodel=kernel  -c -o $@ $<
# 	$(OBJDUMP) -D $@ > $*.asm


CFAPP = -m64 -O2  -fno-builtin -ggdb -fno-strict-aliasing -fcf-protection=none -static
CFAPP2 = -m64 -O2 -fno-pic -static  -fno-builtin -fno-strict-aliasing  -Wall -MD -ggdb -Werror -fno-omit-frame-pointer -fno-stack-protector -std=c11 -I. -fcf-protection=none
CFAPP3 = $(CFAPP2)  -mcmodel=kernel 
LDAPP = -m elf_x86_64 -Ttext 0x100000 

app: apps/app4.c  apps/app3.c  syscall $(OBJS)
	$(CC)  $(CFAPP3) -c apps/app4.c -o app4.o
	$(CC)  $(CFAPP3) -c apps/app3.c -o app3.o
	$(CC)  $(CFAPP3) -c apps/app1.c -o app1.o
	$(LD) $(LDAPP) -o app4.elf  app4.o std.o lib/string.o drivers/speaker.o
	$(LD) $(LDAPP) -o app3.elf  app3.o std.o lib/string.o lib/display.o lib/math.o
	$(LD) $(LDAPP) -o app1.elf  app1.o std.o lib/string.o lib/display.o lib/math.o
	$(OBJCOPY) -S -O binary  app4.elf app4.bin
	$(OBJCOPY) -S -O binary  app3.elf app3.bin
	$(OBJCOPY) -S -O binary  app1.elf app1.bin
	$(OBJDUMP) -D -m i386:x86-64 app1.elf > app1.asm
	$(OBJDUMP) -D -m i386:x86-64 app3.elf > app3.asm
	$(OBJDUMP) -D -m i386:x86-64 -m i386:x86-64 app4.elf > app4.asm
	$(OBJDUMP) -D -m i386:x86-64 app4.o > app4o.asm
	$(OBJDUMP) -S app1.o > app1o.asm
	$(OBJDUMP) -D -b binary -m i386:x86-64 app4.bin > app4.ASM
	$(OBJDUMP) -D -b binary -m i386:x86-64 app1.bin > app1.ASM

all: $(IMAGE)
.PHONY: clean run
clean:
	rm -f  .depend *.elf *.bin  *.img  *.log *.ASM bootblock boot16 boot32 head32 head64  serial.txt  *.vmdx
	find . -type f -name "*.o" -delete
	find . -type f -name "*.d" -delete
	find . -type f -name "*.asm" -delete

run:$(IMAGE)
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw -audiodev pa,id=hda -machine pcspk-audiodev=hda -display gtk,zoom-to-fit=true -vga vmware -mem-prealloc -m 4G
	

run0: $(IMAGE)
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw -display gtk,zoom-to-fit=true -vga vmware -mem-prealloc -m 4G

run2: $(IMAGE)
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw -audiodev pa,id=hda -machine pcspk-audiodev=hda -display gtk,zoom-to-fit=true -vga vmware -mem-prealloc -m 4G -d in_asm,cpu,int,guest_errors -D qemu.log
runser:$(IMAGE)
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw -audiodev pa,id=hda -machine pcspk-audiodev=hda -display gtk,zoom-to-fit=true -vga vmware -mem-prealloc -m 4G  -serial file:serial.txt

run386:$(IMAGE)
	qemu-system-i386 -drive file=$(IMAGE),format=raw

run3: $(IMAGE)
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw -audiodev pa,id=hda -machine pcspk-audiodev=hda -display gtk,zoom-to-fit=true -vga vmware -mem-prealloc -m 4G -d in_asm,cpu,int,guest_errors -D qemu.log -serial file:serial.txt
debug2: $(IMAGE)
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw -s -S

debug: $(IMAGE)
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw -s -S &
	gdb -tui -x init.gdb

debug3: $(IMAGE)
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw -audiodev pa,id=hda -machine pcspk-audiodev=hda -display gtk,zoom-to-fit=true -vga vmware -mem-prealloc -m 4G -s -S &
	gdb -tui -x init.gdb

usb: $(IMAGE) /dev/sda
	# sudo dd if=/dev/sda of=tmp.bin bs=512 count=1 conv=notrunc
	# cp tmp.bin usb.bin
	# sudo rm tmp.bin
	# dd if=bootblock.bin of=usb.bin seek=0 bs=446 count=1 conv=notrunc
	# dd if=boot16.bin of=usb.bin seek=1 bs=512 conv=notrunc
	# dd if=boot32.bin of=usb.bin seek=3 bs=512 conv=notrunc
	# dd if=head64.bin of=usb.bin seek=5 bs=512 conv=notrunc
	# sudo dd if=usb.bin of=/dev/sda seek=0 bs=512 conv=notrunc
	sudo dd if=os.img of=/dev/sda seek=0 bs=512 conv=notrunc
vm:$(IMAGE)
	qemu-img convert -f raw -O vmdk $(IMAGE) vm.vmdk
# qemu-system-x86_64 -hda os.img -s -S -curses
# qemu-system-x86_64 -hda os.img -serial file:serial.log
# memsave 0x0 0x100000 memory.dump
# objdump -D -b binary -m i386:x86-64 mem.bin > mem.asm
# qemu-system-x86_64 -drive file=os.img,format=raw -audiodev pa,id=hda -machine pcspk-audiodev=hda -display gtk,zoom-to-fit=true -vga vmware -mem-prealloc -m 4G