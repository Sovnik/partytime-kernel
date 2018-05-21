#include <keymap.h>

#define LINES 25
#define COLS 80
#define BYTES_PER_CHAR
#define SIZE LINES * COLS * BYTES_PER_CHAR
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C

extern unsigned char keymap[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

unsigned int current_loc = 0;

char *vidptr = (char*)0xb8000;

struct IDT_entry{
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

void idt_init(void){
    unsigned long keyboard_address;
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    keyboard_address = (unsigned long)keyboard_handler;
    IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
    IDT[0x21].selector = 0x08;
    IDT[0x21].zero = 0;
    IDT[0x21].type_attr = 0x8e;
    IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

    /*
    *       PIC1    PIC2
    *CMD    0x20    0xA0
    *DATA   0x21    0xA1
    */

    write_port(0x20, 0x11);
    write_port(0xA0, 0x11);

    write_port(0x21, 0x20);
    write_port(0xA1, 0x28);

    write_port(0x21, 0x01);
    write_port(0xA1, 0x00);

    write_port(0x21, 0x01);
    write_port(0xA1, 0x01);

    write_port(0x21, 0xff);
    write_port(0xA1, 0xff);

    idt_address = (unsigned long)IDT;
    idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;
    load_idt(idt_ptr);
}

void kb_init(void) {
    write_port(0x21, 0xFD);

}

void kprint(const char *str) {
    unsigned int i = 0;
    while (str[i] != '\0') {
        vidptr[current_loc++] = str[i++];
        vidptr[current_loc++] = 0x07;
    }
}

void kprintln(void)
{
    unsigned int line_size = BYTES_PER CHAR * COLS;
    current_loc = current_loc + (line_size - current_loc % (line_size))
}

void keyboard_handler_main(void) {
    unsigned char status;
    char keycode;

    write_port(0x20, 0x20);

    status = read_port(KEYBOARD_STATUS_PORT);
    if (status & 0x01) {
        keybode = read_port(KEYBOARD_DATA_PORT);
        if (keybode < 0)
            return;
        vidptr[current_loc++] = keymap[keycode];
        vidptr[current_loc++] = 0x07;
    }

}

void kernelmain(void)
{
    const char *str = "Party time kernel v0.3 weeeeeeeeeeeeeeee :^)";
    const char *gnulinux = "I would just like to interject for a moment. What you are referring to as Linux, is in fact, GNU/Linux, or as I have recently taken to calling it, GNU plus Linux. Linux is not an operating system unto itself, but rather another free component of a fully functioning GNU system made useful by the GNU corelibs, shell utilities and vital system components comprising a full OS as defined by POSIX. Many computer users run a modified version of the GNU system every day, without realizing it. Through a peculiar turn of events, the version of GNU which is widely used today is often called `Linux`, and many of its users are not aware that it is basically the GNU system, developed by the GNU Project. There really is a Linux, and these people are using it, but it is just a part of the system they use. Linux is the kernel: the program in the system that allocates the machine`s resources to the other programs that you run. The kernel is an essential part of an operating system, but useless by itself; it can only function in the context of a complete operating system. Linux is normally used in combination with the GNU operating system: the whole system is basically GNU with Linux added, or GNU/Linux. All the so-called `Linux` distributions are really distributions of GNU/Linux.";

    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int l = 1;

    while (j < 80 * 25 * 2) {
        vidptr[j] = ' ';
        vidptr[j+1] = 0x07;
        j += 2;
    }

    j = 0;

    while (str[j] != '\0') {
        vidptr[i] = str[j];
        vidptr[i+1] = 0x01 * (l);
        j++;
        l++;
        if (l > 15) {
            l = 1;
        }
        i += 2;
    }
    i = 80 * 2;
    j = 0;
    while (gnulinux[j] != '\0') {
        vidptr[i] = gnulinux[j];
        vidptr[i + 1] = 0x07;
        j++;
        i += 2;
    }
    return;
}


