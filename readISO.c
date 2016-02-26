#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CHUNK 2048 
#define IS_DEBUG 0
#define PRINTF(...) if (IS_DEBUG) printf(__VA_ARGS__);

void readSector(char* buf, int sector);
#if IS_DEBUG == 1
void printInt8(char* ptr);
void printInt16(char* ptr);
void printInt32(char* ptr);
void printString(char* ptr, int length);
#endif
int strcmp(char* str1, char* str2);
int main() {
	char buf[CHUNK];
	char* bootloaderSecondStageName = "BOOT2.SYS;1";
	readSector(buf, 0x10); //0x10 sector Primary Volume Descriptor, 0x11 Boot Record
#if IS_DEBUG == 1
	printInt8((char*)buf);
	printString(((char*)buf) + 1, 5);
#endif
	int rootDirExtendLBA = *((int32_t*)(buf + 156 + 2));
	int rootDirExtendSize = *((int32_t*)(buf + 156 + 10));
	PRINTF("rootDirExtendLBA=%d rootDirExtendSize=%d\n", rootDirExtendLBA, rootDirExtendSize);
	
//-------------- EXTEND of ROOT dir ----------------------------
	readSector(buf, rootDirExtendLBA);
#if IS_DEBUG == 1
	printInt8(buf);
#endif
	int i = 0;
	int locationOfExtendLBA;
	int sizeOfExtendLBA;
	while (i <  rootDirExtendSize) {
		int dirRecordLength = buf[i];
		if (dirRecordLength ==0 ) break;
		PRINTF("-------------------------------------------------------------------------------\n");
		PRINTF("dirRecordLength=%d\n", dirRecordLength);
		int fileNameLength = buf[i + 32];
#if IS_DEBUG == 1
		printString(buf + i + 33, fileNameLength);
#endif
		locationOfExtendLBA = *((int32_t*)(buf + i + 2));
		sizeOfExtendLBA = *((int32_t*)(buf + i + 10));
		PRINTF("locationOfExtendLBA=%d sizeOfExtendLBA=%d\n", locationOfExtendLBA, sizeOfExtendLBA);
		if (strcmp(bootloaderSecondStageName, buf + i + 33)) {
			break;
		}
		i += dirRecordLength;
	}

	FILE *f = fopen("BOOT2.SYS", "wb");
	if (f == NULL)
	{
		PRINTF("Error opening file!\n");
		exit(1);
	}

	i = 0;
	while (i < sizeOfExtendLBA) {
		readSector(buf, i);
		if (sizeOfExtendLBA - i < CHUNK) {
			fwrite (buf , sizeof(char), sizeOfExtendLBA - i, f);
		} else {
			fwrite (buf , sizeof(char), sizeof(buf), f);
		}
		i += CHUNK;	
	}
	fclose(f);
	
}

int strcmp(char* str1, char* str2) {
	while (*str1 != ';') {
		if (*str1 != *str2) {
			return 0;		
		}		
		str1++;
		str2++;
	}
	return 1;
}
#if IS_DEBUG == 1
void printInt8(char* ptr) {
	printf("%d\n", *ptr);
}

void printInt16(char* ptr) {
	PRINTF("%d", *((uint16_t*)ptr));
}

void printInt32(char* ptr) {
	PRINTF("%d", *((uint32_t*)ptr));
}

void printString(char* ptr, int length) {
	int i;	
	for (i = 0; i < length; i++) {
		PRINTF("%c", ptr[i]);
	}
	PRINTF("\n");
}
#endif

void readSector(char* buf, int sector) {
	FILE *file;

	file = fopen("os.iso", "r");
	if( file == NULL ) {
	  PRINTF("Error while opening the file.\n");
	  exit(1);
	}
	if (fseek(file, sector * CHUNK, SEEK_SET)) {
		PRINTF("Error seeking\n");
	}
	if (fread(buf, 1, CHUNK, file) == 0) {
		PRINTF("EOF\n");
	}
	if (ferror(file) != 0) {
		PRINTF("Error reading file\n");	
	}
	fclose(file);
}
