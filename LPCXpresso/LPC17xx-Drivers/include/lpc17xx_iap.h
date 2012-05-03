#ifndef IAP_H_
#define IAP_H_

enum command_code {
	IAPCommand_Prepare_sector_for_write_operation = 50,
	IAPCommand_Copy_RAM_to_Flash,
	IAPCommand_Erase_sector,
	IAPCommand_Blank_check_sector,
	IAPCommand_Read_part_ID,
	IAPCommand_Read_Boot_Code_version,
	IAPCommand_Compare,
	IAPCommand_Reinvoke_ISP,
	IAPCommand_Read_device_serial_number
};

enum error_code
{
	CMD_SUCCESS,
	INVALID_COMMAND,
	SRC_ADDR_ERROR,
	DST_ADDR_ERROR,
	SRC_ADDR_NOT_MAPPED,
	DST_ADDR_NOT_MAPPED,
	COUNT_ERROR,
	INVALID_SECTOR,
	SECTOR_NOT_BLANK,
	SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,
	COMPARE_ERROR,
	BUSY,
	PARAM_ERROR,
	ADDR_ERROR,
	ADDR_NOT_MAPPED,
	CMD_LOCKED,
	INVALID_CODE,
	INVALID_BAUD_RATE,
	INVALID_STOP_BIT,
	CODE_READ_PROTECTION_ENABLED
};

#define     FLASH_SECTOR_0       0x00000000
#define     FLASH_SECTOR_1       0x00001000
#define     FLASH_SECTOR_2       0x00002000
#define     FLASH_SECTOR_3       0x00003000
#define     FLASH_SECTOR_4       0x00004000
#define     FLASH_SECTOR_5       0x00005000
#define     FLASH_SECTOR_6       0x00006000
#define     FLASH_SECTOR_7       0x00007000
#define     FLASH_SECTOR_8       0x00008000
#define     FLASH_SECTOR_9       0x00009000
#define     FLASH_SECTOR_10      0x0000A000
#define     FLASH_SECTOR_11      0x0000B000
#define     FLASH_SECTOR_12      0x0000C000
#define     FLASH_SECTOR_13      0x0000D000
#define     FLASH_SECTOR_14      0x0000E000
#define     FLASH_SECTOR_15      0x0000F000
#define     FLASH_SECTOR_16      0x00010000
#define     FLASH_SECTOR_17      0x00018000
#define     FLASH_SECTOR_18      0x00020000
#define     FLASH_SECTOR_19      0x00028000
#define     FLASH_SECTOR_20      0x00030000
#define     FLASH_SECTOR_21      0x00038000
#define     FLASH_SECTOR_22      0x00040000
#define     FLASH_SECTOR_23      0x00048000
#define     FLASH_SECTOR_24      0x00050000
#define     FLASH_SECTOR_25      0x00058000
#define     FLASH_SECTOR_26      0x00060000
#define     FLASH_SECTOR_27      0x00068000
#define     FLASH_SECTOR_28      0x00070000
#define     FLASH_SECTOR_29      0x00078000

#define     FLASH_SECTOR_SIZE_0_TO_15    ( 4 * 1024)
#define     FLASH_SECTOR_SIZE_16_TO_29   (32 * 1024)

static const char *sector_start_adress[] = {
    (char *)FLASH_SECTOR_0,
    (char *)FLASH_SECTOR_1,
    (char *)FLASH_SECTOR_2,
    (char *)FLASH_SECTOR_3,
    (char *)FLASH_SECTOR_4,
    (char *)FLASH_SECTOR_5,
    (char *)FLASH_SECTOR_6,
    (char *)FLASH_SECTOR_7,
    (char *)FLASH_SECTOR_8,
    (char *)FLASH_SECTOR_9,
    (char *)FLASH_SECTOR_10,
    (char *)FLASH_SECTOR_11,
    (char *)FLASH_SECTOR_12,
    (char *)FLASH_SECTOR_13,
    (char *)FLASH_SECTOR_14,
    (char *)FLASH_SECTOR_15,
    (char *)FLASH_SECTOR_16,
    (char *)FLASH_SECTOR_17,
    (char *)FLASH_SECTOR_18,
    (char *)FLASH_SECTOR_19,
    (char *)FLASH_SECTOR_20,
    (char *)FLASH_SECTOR_21,
    (char *)FLASH_SECTOR_22,
    (char *)FLASH_SECTOR_23,
    (char *)FLASH_SECTOR_24,
    (char *)FLASH_SECTOR_25,
    (char *)FLASH_SECTOR_26,
    (char *)FLASH_SECTOR_27,
    (char *)FLASH_SECTOR_28,
    (char *)FLASH_SECTOR_29
};

#define IAP_LOCATION 0x1FFF1FF1

typedef void (*IAP)(unsigned int [],unsigned int []);

typedef struct {
	unsigned int ser1; // high dword of serial number
	unsigned int ser2;
	unsigned int ser3;
	unsigned int ser4; // low /-/
} DeviceSerial;

unsigned int iap_read_serial(DeviceSerial *d);
unsigned int iap_read_id(unsigned int *d);
unsigned int iap_write_prepare(unsigned int start, unsigned int end);
unsigned int iap_write(const char *src, const char *dst, int size);
unsigned int iap_blank_check(unsigned int start, unsigned int end);
unsigned int iap_erase(unsigned int start, unsigned int end);
unsigned int iap_compare(const char *src, const char *dst, int size);
void iap_reinvoke_isp(void);

#endif /* IAP_H_ */
