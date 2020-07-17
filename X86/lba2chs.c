#define FLOPPY_144_SECTORS_PER_TRACK 18
#include <stdio.h>
#include <stdint.h>

void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
{
    *cyl    = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
    *head   = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK);
    *sector = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1);
}

int main()
{
	uint16_t		cyl, head, sector;
	for (int i = 0; i < 1000; i++)
	{
		lba_2_chs(i, &cyl, &head, &sector);
		printf("%d:%d:%d\n", cyl, head, sector);
	}
}