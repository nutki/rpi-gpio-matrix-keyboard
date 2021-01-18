#ifndef _RPI_GPIO_H
#define _RPI_GPIO_H

#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#define INPUT  0
#define OUTPUT 1

#define PUD_OFF  0
#define PUD_DOWN 1
#define PUD_UP   2

#define FSEL_OFFSET                 0   // 0x00 / 4
#define SET_OFFSET                  7   // 0x1c / 4
#define CLR_OFFSET                  10  // 0x28 / 4
#define PINLEVEL_OFFSET             13  // 0x34 / 4
#define EVENT_DETECT_OFFSET         16  // 0x40 / 4
#define RISING_ED_OFFSET            19  // 0x4c / 4
#define FALLING_ED_OFFSET           22  // 0x58 / 4
#define HIGH_DETECT_OFFSET          25  // 0x64 / 4
#define LOW_DETECT_OFFSET           28  // 0x70 / 4
#define PULLUPDN_OFFSET             37  // 0x94 / 4
#define PULLUPDNCLK_OFFSET          38  // 0x98 / 4
#define PULLUPDN_OFFSET_2711        57  // 0xe4 / 4
#define IS_BCM2711 (gpio_map[PULLUPDN_OFFSET_2711 + 3] != 0x6770696f)

#define GPIO_PAGE_SIZE 4096

static volatile uint32_t *gpio_map;

void spin_delay(int n) {
    for (int i=0; i<n; i++) asm volatile("nop");
}

static inline int read1(int reg, int gpio) {
    return (gpio_map[reg] >> gpio) & 1;    
}
static inline int read2(int reg, int gpio) {
    return (gpio_map[reg + gpio/16] >> (gpio % 16 * 2)) & 3;
}
static inline int read3(int reg, int gpio) {
    return (gpio_map[reg + gpio/10] >> (gpio % 10 * 3)) & 7;    
}
static inline void write1(int reg, int gpio, int val) {
    uint32_t bit = 1 << gpio;
    if (val & 1) gpio_map[reg] |= bit; else gpio_map[reg] &= ~bit;
}
static inline void write2(int reg, int gpio, int val) {
    uint32_t bit = gpio % 16 * 2;
    reg += gpio / 16;
    gpio_map[reg] = gpio_map[reg] & ~(3 << bit) | ((val & 3) << bit);
}
static inline void write3(int reg, int gpio, int val) {
    uint32_t bit = gpio % 10 * 3;
    reg += gpio / 10;
    gpio_map[reg] = gpio_map[reg] & ~(7 << bit) | ((val & 7) << bit);
}

static inline int rpi_gpio_init(void) {
    int mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
    if (mem_fd < 0)
        return -1;
    gpio_map = (uint32_t *)mmap(NULL, GPIO_PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    close(mem_fd);
    if (gpio_map == MAP_FAILED)
        return -1;
    return 0;
}

static inline void rpi_gpio_set_pull(int gpio, int pud) {
    if (IS_BCM2711) {
        // BCM2711 of Pi 4 has direct pull registers
        write2(PULLUPDN_OFFSET_2711, gpio, pud == PUD_UP ? 1 : pud == PUD_DOWN ? 2 : 0);
    } else {
        // Older Pi models (BCM2835/6/7)
        write3(PULLUPDN_OFFSET, 0, pud);
        spin_delay(150);
        write1(PULLUPDNCLK_OFFSET, gpio, 1);
        spin_delay(150);
        write3(PULLUPDN_OFFSET, 0, 0);
        write1(PULLUPDNCLK_OFFSET, gpio, 0);
    }
}

static inline void rpi_gpio_set_fn(int gpio, int direction) {
    write3(FSEL_OFFSET, gpio, direction);
}

static inline void rpi_gpio_setup(int gpio, int direction, int pud) {
    rpi_gpio_set_pull(gpio, pud);
    rpi_gpio_set_fn(gpio, direction);
}

static inline int rpi_gpio_get_fn(int gpio) {
    return read3(FSEL_OFFSET, gpio);
}

static inline void rpi_gpio_output(int gpio, int value) {
    gpio_map[value ? SET_OFFSET : CLR_OFFSET] = 1 << gpio;
}

void rpi_gpio_output_all(uint32_t mask, int value) {
    gpio_map[value ? SET_OFFSET : CLR_OFFSET] = mask;
}

int rpi_gpio_input(int gpio) {
    return read1(PINLEVEL_OFFSET, gpio);
}

uint32_t rpi_gpio_input_all(void) {
    return gpio_map[PINLEVEL_OFFSET];
}

void rpi_gpio_cleanup(void) {
    if (gpio_map)
        munmap((void *)gpio_map, GPIO_PAGE_SIZE);
}

#endif
