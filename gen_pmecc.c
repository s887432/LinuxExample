#include <stdio.h>
#include <stdlib.h>

int pmecc_get_ecc_bytes(int cap, int sector_size)
{
	int m = 12 + sector_size / 512;
	return (m * cap + 7) / 8;
}
	
unsigned int gen_pmecc_header(int page_size, int oob_size, int ecc_bits, int sector_size)
{
	unsigned int ret;
	
	int nbSectorPerPage = page_size / sector_size;
	int vSectorPerPage;
	switch(nbSectorPerPage)
	{
		case 1:vSectorPerPage=0;break;
		case 2:vSectorPerPage=1;break;
		case 4:vSectorPerPage=2;break;
		case 8:vSectorPerPage=3;break;
		case 16:vSectorPerPage=4;break;
	}
	int eccOffset = oob_size - nbSectorPerPage * pmecc_get_ecc_bytes(ecc_bits, sector_size);
	
	int vEcc;
	switch(ecc_bits)
	{
		case 2:vEcc=0;break;
		case 4:vEcc=1;break;
		case 8:vEcc=2;break;
		case 12:vEcc=3;break;
		case 24:vEcc=4;break;
		case 32:vEcc=5;break;
	}
	
	printf("nbSectorPerPage = %d\r\n", nbSectorPerPage);
	printf("eccOffset = %d\r\n", eccOffset);
	printf("oob_size = %d\r\n", oob_size);
	
	ret = 0xC<<28 | eccOffset<<18 | vEcc<<13 | oob_size << 4 | vSectorPerPage <<1 | 1;
	
	return ret;
}

int main(int argc, char **argv)
{
	unsigned int pmecc;
	int page_size, oob_size, ecc_bits, sector_size;
	
	if( argc != 5 )
	{
		printf("Usage: gen_pmecc PAGE_SIZE OOB_SIZE ECC_BITS SECTOR_SIZE\r\n");
		return -1; 
	}
	
	page_size = atoi(argv[1]);
	oob_size = atoi(argv[2]);
	ecc_bits = atoi(argv[3]);
	sector_size = atoi(argv[4]);
	
	pmecc = gen_pmecc_header(page_size, oob_size, ecc_bits, sector_size);
	
	printf("The PMECC is %08X\r\n", pmecc);
	return 0;
}

// end of file

