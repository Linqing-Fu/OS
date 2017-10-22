#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Elf32_Phdr * read_exec_file(FILE **execfile, char *filename, Elf32_Ehdr **ehdr)
{
	Elf32_Phdr *phdr;
	size_t ehdr_size,phdr_size;
	*execfile = fopen(filename,"rb");
	if (*execfile == NULL){
		printf("%s open error! \n",filename);
		exit(0);
	}
	*ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));

	ehdr_size = fread(*ehdr, 1, sizeof(Elf32_Ehdr), *execfile);//read ELF header
	/*judge if read correct: correct then fread return sizeof(Elf32_Ehdr) */
	assert(ehdr_size == sizeof(Elf32_Ehdr));

	fseek((*execfile), (*ehdr)->e_phoff, SEEK_SET);//point to program header
	phdr = calloc(sizeof(Elf32_Phdr), (*ehdr)->e_phnum);
	phdr_size = fread(phdr, sizeof(Elf32_Phdr), (*ehdr)->e_phnum, *execfile);//read all program headers into phdr
	assert(phdr_size == (*ehdr)->e_phnum);
	fseek((*execfile), 0, SEEK_SET);//relocate to the head

	return phdr;


}

void write_bootblock(FILE **image_file, FILE *boot_file, Elf32_Ehdr *boot_header,
		Elf32_Phdr *boot_phdr)
{
	void *buf = calloc(512, sizeof(int));

	fseek(*image_file, 0, SEEK_SET);
	int i;
	/*for each program header*/
	for(i=0; i<boot_header->e_phnum; i++){
		fseek(boot_file, boot_phdr[i].p_offset, SEEK_SET);
		fread(buf, boot_phdr[i].p_filesz, 1, boot_file);
		fwrite(buf, boot_phdr[i].p_filesz, 1, *image_file);
	}


	fseek(*image_file, 510L, SEEK_SET);
	fputc(0x55, *image_file);
	fputc(0xaa, *image_file);
	free(buf);
	return;
}

void write_kernel(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr)
{
	void *buf = calloc(kernel_phdr->p_filesz, sizeof(int));
	fseek(*image_file, 512L, 0);
	int i;
	/*for each program header*/
	for(i=0; i < kernel_ehdr->e_phnum; i++){
		fseek(kernel_file, kernel_phdr[i].p_offset, SEEK_SET);
		fread(buf, kernel_phdr[i].p_filesz, 1, kernel_file);
		fwrite(buf, kernel_phdr[i].p_filesz, 1, *image_file);
	}
	free(buf);
	return;
}

int count_kernel_sectors(Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){
	size_t size;
	int head_number;
	for (head_number = 0,size = 0; head_number < (kernel_header->e_phnum); head_number++){
		size += kernel_phdr[head_number].p_memsz;
	}
	if ((size % 512) != 0)
		return (int)(size / 512 + 1);// sector size is 512 bytes
	else
		return (int)(size / 512);
}

void record_kernel_sectors(FILE **image_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr, int num_sec){
	fseek(*image_file, 0x90, SEEK_SET);
	fwrite(&num_sec, sizeof(int), 1, *image_file);
	fseek(*image_file, 0, SEEK_SET);//relocate to the head

}

void extended_opt(FILE *boot_file, FILE *kernel_file, Elf32_Phdr *boot_phdr, int k_phnum, Elf32_Phdr *kernel_phdr, int num_sec){
	int i;
	long int len;
	size_t address;
	fseek(boot_file, 0, SEEK_END);
	len = ftell(boot_file);
	fseek(boot_file, 0, SEEK_SET);

	printf("length_of_bootblock=%ld\n", len);
	printf("p_offset=%d, p_filesz=%d\n",boot_phdr->p_offset,boot_phdr->p_filesz);
	fseek(kernel_file, 0, SEEK_END);
	len = ftell(kernel_file);
	fseek(kernel_file, 0, SEEK_SET);

	printf("length_of_kernel=%ld\n", len);
	printf("kernel_sectores:%d\n",num_sec-1);
	printf("p_offset[0]=%d, p_filesz[0]=%d\n",kernel_phdr->p_offset,kernel_phdr->p_filesz);
	
	printf("bootblock image info\n");
	printf("sectors: 1\n");
	printf("offset of segment in the file: 0x%X\n",boot_phdr->p_offset);
	printf("the image\'s virtural address of segment in memory: 0x%X\n", boot_phdr->p_vaddr);
	printf("the file image size of segment: 0x%X\n",boot_phdr->p_filesz);
	printf("the memory image size of segment: 0x%X\n",boot_phdr->p_memsz);
	printf("the size of write to the OS image: 0x%X\n",boot_phdr->p_filesz);
	address = ((boot_phdr[0].p_memsz - 1)/512 + 1)*512;
	printf("padding up to 0x%X\n", address);
	

	printf("kernel image info\n");
	printf("sectors: %d\n",num_sec-1);
	
	for(i=0; i < k_phnum; i++){
		printf("kernel_phdr[%d] offset of segment in the file: 0x%X\n",i,kernel_phdr[i].p_offset);
		printf("the image\'s virtural address of kernel_phdr[%d] segment in memory: 0x%X\n", i, kernel_phdr[i].p_vaddr);
		printf("the file image size of kernel_phdr[%d] segment: 0x%X\n",i,kernel_phdr[i].p_filesz);
		printf("the memory image size of kernel_phdr[%d] segment: 0x%X\n",i,kernel_phdr[i].p_memsz);
		printf("the kernel_phdr[%d] size of write to the OS image: 0x%X\n",i,kernel_phdr[i].p_filesz);
		address += kernel_phdr[i].p_memsz;
		if(i == k_phnum -1){ 
			address = ((address - 1)/512 + 1)*512;
		}
		printf("padding up to 0x%X\n", address);
		
	}
	/*
	printf("offset of segment in the file: 0x%X\n",kernel_phdr->p_offset);
	printf("the image\'s virtural address of segment in memory: 0x%X\n", kernel_phdr->p_vaddr);
	printf("the file image size of segment: 0x%X\n",kernel_phdr->p_filesz);
	printf("the memory image size of segment: 0x%X\n",kernel_phdr->p_memsz);
	printf("the size of write to the OS image: 0x%X\n",kernel_phdr->p_filesz);
	*/
	return;
	

}





int main(int argc, char *argv[]){
	Elf32_Phdr *kernel_phdr, *boot_phdr;
	Elf32_Ehdr *kernel_ehdr, *boot_ehdr;
	FILE *kernel_file, *boot_file,*image_file;

	int ker_num_sec;

	if (argc != 3 && argc != 4){
		printf("usage: createimage --extended bootblock kernel\n");
		exit(0);
	}
	image_file = malloc(sizeof(FILE *));
	if (!(image_file = fopen("image","w+"))){
		printf("image open error!\n");
		return -1;
	}


	/* read bootblock and write into image */
	boot_phdr = read_exec_file(&boot_file, argv[argc - 2], &boot_ehdr);
	write_bootblock(&image_file, boot_file, boot_ehdr, boot_phdr);

	/* read kernel and write into image */
	kernel_phdr = read_exec_file(&kernel_file, argv[argc - 1], &kernel_ehdr);
	write_kernel(&image_file, kernel_file, kernel_ehdr, kernel_phdr);
	
	/*count sector number of kernel*/
	ker_num_sec=count_kernel_sectors(kernel_ehdr, kernel_phdr);

	record_kernel_sectors(&image_file, kernel_ehdr, kernel_phdr, ker_num_sec);
	
	/*print infomation if have '--extended' */
	if (!strcmp(argv[1],"--extended")){
		extended_opt(boot_file, kernel_file, boot_phdr, kernel_ehdr->e_phnum, kernel_phdr, ker_num_sec+1);
	}

	//close file
	fclose(image_file);
	fclose(boot_file);
	fclose(kernel_file);

	//free pointer
	free(kernel_phdr);	free(kernel_ehdr); free(boot_phdr); free(boot_ehdr);
	
	return 0;
}
