#include "header.h"

#include <intrin.h>

#pragma pack(push)
#pragma pack(1)

struct vbe_info {
   char signature[4];   // must be "VESA" to indicate valid VBE support
   uint16_t version;         // VBE version; high byte is major version, low byte is minor version
   uint32_t oem;         // segment:offset pointer to OEM
   uint32_t capabilities;      // bitfield that describes card capabilities
   uint32_t video_modes;      // segment:offset pointer to list of supported video modes
   uint16_t video_memory;      // amount of video memory in 64KB blocks
   uint16_t software_rev;      // software revision
   uint32_t vendor;         // segment:offset to card vendor string
   uint32_t product_name;      // segment:offset to card model name
   uint32_t product_rev;      // segment:offset pointer to product revision
   char reserved[222];      // reserved for future expansion
   char oem_data[256];      // OEM BIOSes store their strings in this area
} ;

struct vbe_mode_info {
   uint16_t attributes;      // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
   uint8_t window_a;         // deprecated
   uint8_t window_b;         // deprecated
   uint16_t granularity;      // deprecated; used while calculating bank numbers
   uint16_t window_size;
   uint16_t segment_a;
   uint16_t segment_b;
   uint32_t win_func_ptr;      // deprecated; used to switch banks from protected mode without returning to real mode
   uint16_t pitch;         // number of bytes per horizontal line
   uint16_t width;         // width in pixels
   uint16_t height;         // height in pixels
   uint8_t w_char;         // unused...
   uint8_t y_char;         // ...
   uint8_t planes;
   uint8_t bpp;         // bits per pixel in this mode
   uint8_t banks;         // deprecated; total number of banks in this mode
   uint8_t memory_model;
   uint8_t bank_size;      // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
   uint8_t image_pages;
   uint8_t reserved0;

   uint8_t red_mask;
   uint8_t red_position;
   uint8_t green_mask;
   uint8_t green_position;
   uint8_t blue_mask;
   uint8_t blue_position;
   uint8_t reserved_mask;
   uint8_t reserved_position;
   uint8_t direct_color_attributes;

   uint32_t framebuffer;      // physical address of the linear frame buffer; write here to draw to the screen
   uint32_t off_screen_mem_off;
   uint16_t off_screen_mem_size;   // size of memory in the framebuffer but not being displayed on the screen
   uint8_t reserved1[206];
} ;
#pragma pack(pop)

// https://zhuanlan.zhihu.com/p/100962276

// https://forum.osdev.org/viewtopic.php?f=2&t=30186

// https://wiki.osdev.org/User:Omarrx024/VESA_Tutorial

// http://goahomepage.free.fr/auteur/prod/doc/vbe/index.html

//uint32 pixel_offset = y * pitch + (x * (bpp/8)) + framebuffer;

static uint32_t 	framebuffer;
static uint16_t 	pitch, width, height;
static uint8_t 	bpp; 

struct pm_meta{
	uint16_t		cx;
	uint16_t		di;
	uint16_t		es;
};

typedef struct VESA_PM_INFO
{
	unsigned short setWindow;
	unsigned short setDisplayStart;
	unsigned short setPalette;
	unsigned short IOPrivInfo;
} VESA_PM_INFO;

VESA_PM_INFO * vpi;
void vesa_information()
{
	struct pm_meta *pm = (struct pm_meta *)0x9000;
	char *src = (char *) ((((uint32_t)pm->es) << 4) + pm->di);
	char *dest = (char *)0x300000;
	vpi =  (VESA_PM_INFO *)dest;
	uint16_t size = pm->cx;
	for (int i = 0; i < size; i ++)
	{
		dest[i] = src[i];
	}
	
	struct vbe_mode_info *vmis = (struct vbe_mode_info *)0x90000;
	
	framebuffer = vmis->framebuffer;
	bpp = vmis->bpp;
	pitch = vmis->pitch;
	width = vmis->width;
	height = vmis->height;
	
	//map:	10-10-12
	//5000	=>	8
	uint32_t		pfn = framebuffer >> 12;
	uint32_t		pde_idx = framebuffer >> 22;
	uint32_t		pte_idx = (framebuffer >> 12) & 0x3ff;
	uint32_t		*cr3_addr = (uint32_t *)__readcr3();
	cr3_addr[pde_idx] = (8 << 12) | 0x3;
	uint32_t		*e = (uint32_t *)0x8000;
	for (int i = 0; i < 1024; i++)
	{
		e[i] = pfn++ << 12 | 0x3;
	}
	
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width/2; x++)
		{
			uint32_t		pixel_offset = y * pitch + (x * (bpp/8)) + framebuffer;
			*(uint32_t *) pixel_offset = 0xffffffff;
		
		}
}