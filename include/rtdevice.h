#ifndef __RT_DEVICE_H__
#define __RT_DEVICE_H__

#include <rtdef.h>

#ifdef RT_USING_DEVICE
/**
 * @addtogroup Device
 */
/*@{*/

/**
 * device (I/O) class type
 */
enum rt_device_class_type
{
	RT_Device_Class_Char = 0,						/**< character device							*/
	RT_Device_Class_Block,							/**< block device 								*/
	RT_Device_Class_NetIf,							/**< net interface 								*/
	RT_Device_Class_MTD,							/**< memory device 								*/
	RT_Device_Class_CAN,							/**< CAN device 								*/
	RT_Device_Class_RTC,							/**< RTC device 								*/
	RT_Device_Class_Sound,							/**< Sound device 								*/
	RT_Device_Class_Unknown							/**< unknown device 							*/
};

/**
 * device flags defitions
 */
#define RT_DEVICE_FLAG_DEACTIVATE		0x000		/**< device is not not initialized 				*/

#define RT_DEVICE_FLAG_RDONLY			0x001		/**< read only 									*/
#define RT_DEVICE_FLAG_WRONLY			0x002		/**< write only 								*/
#define RT_DEVICE_FLAG_RDWR				0x003		/**< read and write 							*/

#define RT_DEVICE_FLAG_REMOVABLE		0x004		/**< removable device 							*/
#define RT_DEVICE_FLAG_STANDALONE		0x008		/**< standalone device							*/
#define RT_DEVICE_FLAG_ACTIVATED		0x010		/**< device is activated 						*/
#define RT_DEVICE_FLAG_SUSPENDED		0x020		/**< device is suspended 						*/
#define RT_DEVICE_FLAG_STREAM			0x040		/**< stream mode 								*/

#define RT_DEVICE_FLAG_INT_RX			0x100		/**< INT mode on Rx 							*/
#define RT_DEVICE_FLAG_DMA_RX			0x200		/**< DMA mode on Rx 							*/
#define RT_DEVICE_FLAG_INT_TX			0x400		/**< INT mode on Tx 							*/
#define RT_DEVICE_FLAG_DMA_TX			0x800		/**< DMA mode on Tx								*/

#define RT_DEVICE_OFLAG_CLOSE			0x000		/**< device is closed 							*/
#define RT_DEVICE_OFLAG_RDONLY			0x001		/**< read only access							*/
#define RT_DEVICE_OFLAG_WRONLY			0x002		/**< write only access							*/
#define RT_DEVICE_OFLAG_RDWR			0x003		/**< read and write 							*/
#define RT_DEVICE_OFLAG_OPEN			0x008		/**< device is opened 							*/

/**
 * general device commands
 */
#define RT_DEVICE_CTRL_RESUME	   		0x01		/**< resume device 								*/
#define RT_DEVICE_CTRL_SUSPEND	    	0x02		/**< suspend device 							*/

/**
 * special device commands
 */
#define RT_DEVICE_CTRL_CHAR_STREAM		0x10		/**< stream mode on char device 				*/
#define RT_DEVICE_CTRL_BLK_GETGEOME		0x10		/**< get geometry information 					*/
#define RT_DEVICE_CTRL_NETIF_GETMAC		0x10		/**< get mac address 							*/
#define RT_DEVICE_CTRL_MTD_FORMAT		0x10		/**< format a MTD device 						*/
#define RT_DEVICE_CTRL_RTC_GET_TIME		0x10		/**< get time 									*/
#define RT_DEVICE_CTRL_RTC_SET_TIME		0x11		/**< set time 									*/

typedef struct rt_device* rt_device_t;
/**
 * Device structure
 */
struct rt_device
{
	struct rt_object parent;						/**< inherit from rt_object 					*/

	enum rt_device_class_type type;					/**< device type 								*/
	rt_uint16_t flag, open_flag;					/**< device flag and device open flag			*/

	/* device call back */
	rt_err_t (*rx_indicate)(rt_device_t dev, rt_size_t size);
	rt_err_t (*tx_complete)(rt_device_t dev, void* buffer);

	/* common device interface */
	rt_err_t  (*init)	(rt_device_t dev);
	rt_err_t  (*open)	(rt_device_t dev, rt_uint16_t oflag);
	rt_err_t  (*close)	(rt_device_t dev);
	rt_size_t (*read)	(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size);
	rt_size_t (*write)	(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size);
	rt_err_t  (*control)(rt_device_t dev, rt_uint8_t cmd, void *args);

#ifdef RT_USING_DEVICE_SUSPEND
	rt_err_t (*suspend) (rt_device_t dev);
	rt_err_t (*resumed) (rt_device_t dev);
#endif

	void* user_data;								/**< device private data 						*/
};

struct rt_device_ops
{
	rt_err_t (*init)   (rt_device_t dev);
	rt_err_t (*open)   (rt_device_t dev);
	rt_err_t (*close)  (rt_device_t dev);
	rt_err_t (*control)(rt_device_t dev, rt_uint8_t cmd, void* args);
};

struct rt_device_block_ops
{
	struct rt_device_ops parent;

	rt_err_t (*read)  (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size);
	rt_err_t (*write) (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size);
};

struct rt_device_char_ops
{
	struct rt_device_ops parent;
};

struct rt_device_spibus_ops
{
};

/**
 * block device geometry structure
 */
struct rt_device_blk_geometry
{
	rt_uint32_t sector_count;						/**< count of sectors							*/
	rt_uint32_t bytes_per_sector;					/**< number of bytes per sector 				*/
	rt_uint32_t block_size;							/**< size to erase one block 					*/
};


/**
 * graphic device control command
 */
#define RTGRAPHIC_CTRL_RECT_UPDATE  0
#define RTGRAPHIC_CTRL_POWERON      1
#define RTGRAPHIC_CTRL_POWEROFF     2
#define RTGRAPHIC_CTRL_GET_INFO     3
#define RTGRAPHIC_CTRL_SET_MODE     4

/* graphic deice */
enum
{
	RTGRAPHIC_PIXEL_FORMAT_MONO = 0,
	RTGRAPHIC_PIXEL_FORMAT_GRAY4,
	RTGRAPHIC_PIXEL_FORMAT_GRAY16,
	RTGRAPHIC_PIXEL_FORMAT_RGB332,
	RTGRAPHIC_PIXEL_FORMAT_RGB444,
	RTGRAPHIC_PIXEL_FORMAT_RGB565,
	RTGRAPHIC_PIXEL_FORMAT_RGB565P,
	RTGRAPHIC_PIXEL_FORMAT_RGB666,
	RTGRAPHIC_PIXEL_FORMAT_RGB888,
	RTGRAPHIC_PIXEL_FORMAT_ARGB888
};
/**
 * build a pixel position according to (x, y) coordinates.
 */
#define RTGRAPHIC_PIXEL_POSITION(x, y)	((x << 16) | y)

/**
 * graphic device information structure
 */
struct rt_device_graphic_info
{
	rt_uint8_t  pixel_format;       /**< graphic format           */
	rt_uint8_t  bits_per_pixel;     /**< bits per pixel           */
	rt_uint16_t reserved;           /**< reserved field           */

	rt_uint16_t width;              /**< width of graphic device  */
	rt_uint16_t height;             /**< height of graphic device */

	rt_uint8_t *framebuffer;        /**< frame buffer             */
};

/**
 * rectangle information structure
 */
struct rt_device_rect_info
{
	rt_uint16_t x, y;               /**< x, y coordinate          */
	rt_uint16_t width, height;      /**< width and height         */
};

/**
 * graphic operations
 */
struct rt_device_graphic_ops
{
	void (*set_pixel) (const char *pixel, int x, int y);
	void (*get_pixel) (char *pixel, int x, int y);

	void (*draw_hline)(const char *pixel, int x1, int x2, int y);
	void (*draw_vline)(const char *pixel, int x, int y1, int y2);

	void (*blit_line) (const char *pixel, int x, int y, rt_size_t size);
};
#define rt_graphix_ops(device)    ((struct rt_device_graphic_ops *)(device->user_data))

/*@}*/

#endif
