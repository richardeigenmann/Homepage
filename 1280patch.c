/*  1280patch - Fixes the i810-series video BIOS to support
 *              1280x768 resolution.  Currently only tested
 *              against the 855GM VBIOS, revision #2878.
 *              *VERY* likely that this won't work yet on any
 *              other revisions or chipsets!!!
 *
 * This code is based on the technique used in 855patch.  Many thanks
 * to Christian Zietz <czietz@gmx.net> for demonstrating how to
 * shadow the VBIOS into system RAM and then modify it.
 *
 * 1280patch is public domain source code.  Use at your own risk!
 * Andrew Tipton <andrewtipton@null.li>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <unistd.h>
#define __USE_GNU
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>

#define CHIPSET_ID          0x35808086
#define VBIOS               0xc0000
#define VBIOS_SIZE          0x10000
#define CFG_SIGNATURE "BIOS_DATA_BLOCK "
#define CFG_VERSION 29
#define EMODE_ADDR         0x03c3
#define TIMING_60          (6 + (28*0))
#define TIMING_75          (6 + (28*1))
#define TIMING_85          (6 + (28*2))
#define TIMING_CLOCK       0
#define TIMING_HTOTAL      4
#define TIMING_HBLANK      8
#define TIMING_HSYNC       12
#define TIMING_VTOTAL      16
#define TIMING_VBLANK      20
#define TIMING_VSYNC       24

/* I shouldn't use globals, but that's too bad! */
unsigned char *bios = 0;
int biosfd = 0;
unsigned char *bioscfg = 0;


unsigned int get_chipset() {
    outl(0x80000000, 0xcf8);
    return inl(0xcfc);
}

int unlock_bios() {
    outl(0x8000005a, 0xcf8);
    outb(0x33, 0xcfe);
    
    return 1;
}

int relock_bios() {
    outl(0x8000005a, 0xcf8);
    outb(0x11, 0xcfe);

    return 1;
}

void open_bios() {
    biosfd = open("/dev/mem", O_RDWR);
    if (biosfd < 0) {
        perror("Unable to open /dev/mem");
        exit(2);
    }

    bios = mmap((void *)VBIOS, VBIOS_SIZE,
        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED,
        biosfd, VBIOS);
    if (bios == NULL) {
        fprintf(stderr, "Cannot mmap() the video BIOS\n");
        close(biosfd);
        exit(2);
    }
}

void close_bios() {
    if (bios == NULL) {
        fprintf(stderr, "BIOS should be open already!\n");
        exit(2);
    }

    munmap(bios, VBIOS_SIZE);
    close(biosfd);
}

/* Finds the EMode table in the BIOS */
unsigned char *find_emode_table() {
    /* hardcoded for now... */
    return (bios + EMODE_ADDR);
}

/* Lists the modes available */
void list_modes(unsigned char *table) {
    return;
}

/* Returns a pointer to the parameter block for a mode */
unsigned char *get_emode_params(unsigned char *table, int mode) {
    unsigned short offset;

    while (*table != 0xff) {
        if (*table == mode) {
            offset = *(unsigned short *)(table + 2);
            printf("Timing parameter block: %04x\n", offset);
            return (bios + offset);
        }
        table += 5;         /* next record */
    }

    return 0;
}


/* Given an XFree86-compatible modeline, replaces the
 * timing block at *ptr.  No error checking yet!
 */
void set_timing(unsigned char *ptr, int clock,
    int hdisp, int hsyncstart, int hsyncend, int htotal,
    int vdisp, int vsyncstart, int vsyncend, int vtotal) {

    *(unsigned int *)(ptr + TIMING_CLOCK) = clock;
    *(unsigned int *)(ptr + TIMING_HTOTAL) = (htotal << 16) | (hdisp - 1);
    *(unsigned int *)(ptr + TIMING_HBLANK) = (htotal << 16) | (hdisp - 1);
    *(unsigned int *)(ptr + TIMING_HSYNC)  = (hsyncend << 16) | hsyncstart;
    *(unsigned int *)(ptr + TIMING_VTOTAL) = (vtotal << 16) | (vdisp - 1);
    *(unsigned int *)(ptr + TIMING_VBLANK) = (vtotal << 16) | (vdisp - 1);
    *(unsigned int *)(ptr + TIMING_VSYNC)  = (vsyncend << 16) | vsyncstart;

}



int main (int argc, char *argv[]) {
    unsigned int chipset;
    unsigned char *emode_table;
    unsigned char *mode_params;

    if (iopl(3) < 0) {
        perror("Unable to obtain the proper IO permissions");
        exit(2);
    }

    chipset = get_chipset();
    printf("Chipset: ");
    switch (chipset) {
        case 0x35808086:
            printf("855GM\n");
            break;
        default:
            printf("Unknown (0x%08x)\n", chipset);
            break;
    }

    open_bios();


    /* Find the configuration area of the BIOS */
    bioscfg = memmem(bios, VBIOS_SIZE, CFG_SIGNATURE, strlen(CFG_SIGNATURE));
    if (bioscfg == NULL) {
        fprintf(stderr, "Couldn't find the configuration area in the VBIOS!\n");
        close_bios();
        exit(2);
    }
    printf("BIOS Configuration area offset: 0x%04x bytes\n", bioscfg - bios);
    printf("BIOS Version: %.4s\n", bioscfg + CFG_VERSION);
    

    /* Get the timing block */
    emode_table = find_emode_table();
    mode_params = get_emode_params(emode_table, 0x54);

    /* Now it's time to patch the BIOS */
    unlock_bios();
    set_timing(mode_params + TIMING_60,
        80140, 1280, 1343, 1479, 1679, 768, 768, 771, 794);
    set_timing(mode_params + TIMING_75,
        102980, 1280, 1359, 1495, 1711, 768, 768, 771, 801);
    set_timing(mode_params + TIMING_85,
        118530, 1280, 1367, 1503, 1727, 768, 768, 771, 806);

    /* Finished */
    relock_bios();
    close_bios();

    printf("Patch complete.\n");

    return 0;
}
