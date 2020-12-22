/*
dd if=mbr.bin of=boot-86.img bs=512 count=1
dd if=head.bin of=boot-86.img seek=1 bs=512 count=2
dd if=loader.exe of=boot-86.img seek=3 bs=512 count=17
dd if=/dev/zero of=boot-86.img seek=20 bs=512 count=2860
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEC_SIZE 512

static long total_sec = 2880;
static char outf[0x200];
static int 	nIf = 0;
static struct {
	char fname[0x200];
	int	 nsec;
}
ifs [0x10];

void parse_args(const char *s)
{
	if (s[0] == '-')
	{
		switch (s[1])
		{
		case 'n':
			total_sec = strtol(&s[2], NULL, 10);
			break;
		case 'o':
			strcpy(outf, &s[2]);
			break;
		}
		return;
	}

	char filename[0x200] = {0};
	char nsec[0x20] = {0};
	char *pt = filename;
	const char *ps = s;
	while (ps[0] !='\0' && ps[0] != '=')
	{
		*pt++ = *ps++;
	}
	*pt = '\0';

	if (ps[0] == '\0')
	{
		ifs[nIf].nsec = 0;
		strcpy(ifs[nIf++].fname, filename);
		return;
	}

	//skip '='
	ps++;
	pt=nsec;
	while (ps[0] != '\0')
	{
		*pt++ = *ps++;
	}

	ifs[nIf].nsec = strtol(nsec, NULL, 10);
	strcpy(ifs[nIf++].fname, filename);
	return;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: -oOutputFileName -nSectorNumbers\nExample:\nmkimg -oboot-64.img -n2880 mbr.bin=1 head.bin=2 loader.exe=17\n");
		return -3;
	}

	for (int i = 1; i < argc; i++)
		parse_args(argv[i]);

	printf("total %d sectors\n", total_sec);

	FILE *fpimg = fopen(outf, "wb");
	if (NULL == fpimg)
		return -1;

	char buf[SEC_SIZE];
	for (int i = 0; i < nIf; i++)
	{
		FILE *fp = fopen(ifs[i].fname, "rb");
		int nSec = ifs[i].nsec;
		size_t fsize = 0;

		if (NULL == fp)
		{
			fprintf(stderr, "filed to open %s\n", ifs[i].fname);
			goto __err_ret;
		}

		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int rSec = (fsize + SEC_SIZE -1) / SEC_SIZE;
		if (nSec == 0)
		{
			nSec = rSec;
		}
		fprintf(stdout, "%s:%d secs\n", ifs[i].fname, nSec);

		for (int j = 0; j < nSec; j++)
		{
			fread(buf, SEC_SIZE, 1, fp);
			fwrite(buf, SEC_SIZE, 1, fpimg);
		}

		fclose(fp);
		total_sec -= nSec;
	}

	//fill with zero
	memset(buf, 0, sizeof(buf));
	for (int i = 0; i < total_sec; i++)
		fwrite(buf, SEC_SIZE, 1, fpimg);

	fclose(fpimg);
	return 0;
__err_ret:

	fclose(fpimg);
	return -2;
}