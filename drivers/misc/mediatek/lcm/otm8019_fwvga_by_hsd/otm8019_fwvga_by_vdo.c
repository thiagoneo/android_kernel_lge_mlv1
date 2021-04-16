#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"
#include <linux/gpio.h>

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (854)

/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */

static LCM_UTIL_FUNCS lcm_util = { 0 };

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY								0XFD
#define REGFLAG_END_OF_TABLE							0xFE	/* END OF REGISTERS MARKER */


/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
/* #define read_reg	 lcm_util.dsi_read_reg() */
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)



/* #define LCM_DSI_CMD_MODE */

struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {

	/*
	   Note :

	   Data ID will depends on the following rule.

	   count of parameters > 1      => Data ID = 0x39
	   count of parameters = 1      => Data ID = 0x15
	   count of parameters = 0      => Data ID = 0x05

	   Structure Format :

	   {DCS command, count of parameters, {parameter list}}
	   {REGFLAG_DELAY, milliseconds of time, {}},

	   ...

	   Setting ending by predefined flag

	   {REGFLAG_END_OF_TABLE, 0x00, {}}
	 */
       { 0x00, 1, {0x00} },
       { 0xFF, 3, {0x80, 0x19, 0x01} },
       { 0x00, 1, {0x80} },
       { 0xFF, 2, {0x80, 0x19} },
       { 0x00, 1, {0xB4} },
       { 0xC0, 1, {0x00} },
       { 0x00, 1, {0x82} },
       { 0xC5, 1, {0xB0} },
       { 0x00, 1, {0x90} },
       { 0xC5, 2, {0x4E, 0x86} },
       { 0x00, 1, {0x00} },
       { 0xD8, 2, {0x6F, 0x6F} },
       { 0x00, 1, {0xA6} },
       { 0xC1, 1, {0x01} },
       { 0x00, 1, {0x81} },
       { 0xC1, 1, {0x22} },
       { 0x00, 1, {0xA1} },
       { 0xC1, 1, {0x0E} },
       { 0x00, 1, {0x89} },
       { 0xC4, 1, {0x08} },
       { 0x00, 1, {0x92} },
       { 0xB3, 1, {0x45} },
       { 0x00, 1, {0x90} },
       { 0xB3, 1, {0x02} },
       { 0x00, 1, {0x80} },
       { 0xC0, 5, {0x00, 0x58, 0x00, 0x14, 0x16} },
       { 0x00, 1, {0x80} },
       { 0xC4, 1, {0x30} },
       { 0x00, 1, {0x90} },
       { 0xC0, 5, {0x00, 0x15, 0x00, 0x00, 0x00, 0x13} },
       { 0x00, 1, {0x82} },      
       { 0xC1, 1, {0x82} },
       { 0x00, 1, {0xB0} },
       { 0xCA, 1, {0xCC} },
       { 0x00, 1, {0xB3} },
       { 0xCA, 1, {0x60} },
       { 0x00, 1, {0xD1} },
       { 0xB0, 1, {0x07} },
       { 0x00, 1, {0x91} },
       { 0xB6, 1, {0x5A} },
       { 0x00, 1, {0xB1} },
       { 0xC5, 1, {0xA9} },
       { 0x00, 1, {0xB2} },
       { 0xC1, 1, {0x00} },
       { 0x00, 1, {0xA0} },
       { 0xC1, 1, {0x02} },
       //{ 0xC1, 1, {0xEA} },
       { 0x00, 1, {0x8B} },
       { 0xB0, 1, {0x40} },
       { 0x00, 1, {0x94} },
       { 0xC5, 1, {0x55} },
       { 0x00, 1, {0x00} },
       { 0xE1, 20, {0x00, 0x01, 0x09, 0x18, 0x2D, 0x43, 0x4F, 0x81, 0x70, 0x87, 
                    0x7E, 0x6D, 0x85, 0x70, 0x75, 0x6E, 0x66, 0x5D, 0x52, 0x25} },
       { 0x00, 1, {0x00} },                 
       { 0xE2, 20, {0x00, 0x02, 0x09, 0x18, 0x2D, 0x43, 0x4F, 0x82, 0x71, 0x88, 
                    0x7F, 0x6D, 0x84, 0x6F, 0x74, 0x6D, 0x65, 0x5C, 0x51, 0x24} },
       { 0x00, 1, {0x80} },
       { 0xCE, 12, {0x8B, 0x03, 0x0A, 0x8A, 0x03, 0x0A, 0x89, 0x03, 0x0A, 0x88, 0x03, 0x0A} },
       { 0x00, 1, {0xA0} },
       { 0xCE, 14, {0x38, 0x07, 0x83, 0x54, 0x8C, 0x10, 0x00, 0x38, 0x06, 0x83, 0x55, 0x8C, 0x10, 0x00} },
       { 0x00, 1, {0xB0} },
       { 0xCE, 14, {0x38, 0x05, 0x83, 0x56, 0x8C, 0x10, 0x00, 0x38, 0x04, 0x83, 0x57, 0x8C, 0x10, 0x00} },
       { 0x00, 1, {0xC0} },
       { 0xCE, 14, {0x38, 0x03, 0x83, 0x58, 0x8C, 0x10, 0x00, 0x38, 0x02, 0x83, 0x59, 0x8C, 0x10, 0x00} },        
       { 0x00, 1, {0xD0} },                         
       { 0xCE, 14, {0x38, 0x01, 0x83, 0x5A, 0x8C, 0x10, 0x00, 0x38, 0x00, 0x83, 0x5B, 0x8C, 0x10, 0x00} },      
       { 0x00, 1, {0xC0} },   
       { 0xCF, 10, {0x3d, 0x3d, 0x0d, 0x20, 0x00, 0x00, 0x01, 0x80, 0x03, 0x0a} },
       { 0x00, 1, {0xC0} },
       { 0xCB, 15, {0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
                    0x00, 0x00, 0x00, 0x00, 0x00} }, 
       { 0x00, 1, {0xD0} },      
       { 0xCB, 15, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		            0x00, 0x15, 0x15, 0x15, 0x15} }, 			  
       { 0x00, 1, {0xE0} },
       { 0xCB, 10, {0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x00, 0x00, 0x00, 0x00} },         
       { 0x00, 1, {0x80} },            
       { 0xCC, 10, {0x26, 0x25, 0x21, 0x22, 0x0c, 0x0a, 0x10, 0x0e, 0x02, 0x04} },
       { 0x00, 1, {0x90} },
       { 0xCC, 15, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		            0x00, 0x00, 0x00, 0x00, 0x00} },		  
       { 0x00, 1, {0xA0} },
       { 0xCC, 15, {0x00, 0x03, 0x01, 0x0d, 0x0f, 0x09, 0x0b ,0x22, 0x21, 0x25,
		            0x26, 0x00, 0x00, 0x00, 0x00} },
       { 0x00, 1, {0xB0} },
       { 0xCC, 10, {0x25, 0x26, 0x21, 0x22, 0x0b, 0x0d, 0x0f, 0x09, 0x03, 0x01} },
       { 0x00, 1, {0xC0} },
       { 0xCC, 15, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		            0x00, 0x00, 0x00, 0x00, 0x00} },
       { 0x00, 1, {0xD0} },
       { 0xCC, 15, {0x00, 0x02, 0x04, 0x0a, 0x10, 0x0e, 0x0c, 0x22, 0x21, 0x26,
		            0x25, 0x00, 0x00, 0x00, 0x00} },
       { 0x00, 1, {0x80} },
       { 0xC4, 1, {0x30} },
       { 0x00, 1, {0x98} },
       { 0xC0, 1, {0x00} },
       { 0x00, 1, {0xA9} },
       { 0xC0, 1, {0x0A} },
       { 0x00, 1, {0xB0} },
       { 0xC1, 3, {0x20, 0x00, 0x00} },
       { 0x00, 1, {0xE1} },
       { 0xC0, 2, {0x40, 0X30} },
       { 0x00, 1, {0x80} },
       { 0xC1, 2, {0x03, 0X33} },
       { 0x00, 1, {0xA0} },
       { 0xC1, 1, {0x02} },
       { 0x00, 1, {0x90} },
       { 0xB6, 1, {0xB4} },
       { REGFLAG_DELAY, 10, {} },
       { 0x00, 1, {0x00} },
       { 0xFB, 1, {0x01} },
      // { 0x35, 1, {0x01} },
       { 0x11, 0, {} },
       { REGFLAG_DELAY, 120, {} },
       { 0x29, 0, {} },
       { REGFLAG_DELAY, 10, {} },
	/* Note */
	/* Strongly recommend not to set Sleep out, Display On here. */
	/* That will cause messed frame to be shown as later the backlight is on. */

	/* Setting ending by predefined flag */
	{ REGFLAG_END_OF_TABLE, 0x00, {} }
};

#if 0
static struct LCM_setting_table lcm_sleep_out_setting[] = {
	/* Sleep Out */
	{ 0x11, 1, {0x00} },
	{ REGFLAG_DELAY, 20, {} },

	/* Display ON */
	{ 0x29, 1, {0x00} },
	{ REGFLAG_DELAY, 120, {} },
	{ REGFLAG_END_OF_TABLE, 0x00, {} }
};
#endif



void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {

		unsigned cmd;

		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}

}



/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
	
	params->type   = LCM_TYPE_DSI;
	
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
		// enable tearing-free
	params->dbi.te_mode 			= LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if defined(LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = BURST_VDO_MODE;//SYNC_PULSE_VDO_MODE;
#endif
	params->dsi.noncont_clock							= 1; 
	params->dsi.noncont_clock_period					= 2; //Unit : frames
	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_TWO_LANE;
	
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format	  = LCM_DSI_FORMAT_RGB888;
	
	// Video mode setting		
	params->dsi.intermediat_buffer_num = 2;
	
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
	params->dsi.cont_clock = 1;
	
	params->dsi.word_count=480*3;	//DSI CMD mode need set these two bellow params, different to 6577
	params->dsi.vertical_active_line=854;//800

	params->dsi.vertical_sync_active				= 6;
	params->dsi.vertical_backporch					= 32;
	params->dsi.vertical_frontporch 				= 36;
	params->dsi.vertical_active_line				= FRAME_HEIGHT;
	
	params->dsi.horizontal_sync_active			        = 8;
	params->dsi.horizontal_backporch				= 55;
	params->dsi.horizontal_frontporch				= 55;
	params->dsi.horizontal_blanking_pixel				= 120;
	params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;
		//improve clk quality
	params->dsi.PLL_CLOCK = 200; //240//this value must be in MTK suggested table
	params->dsi.compatibility_for_nvk = 1;
	params->dsi.ssc_disable = 1;
	
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;	

}


static unsigned int lcm_compare_id(void)
{
	int array[4];
	char buffer[5];
	char id_high=0;
	char id_low=0;
	int chid_id=0;
	//int  lcm_adc = 0,data[4] = {0,0,0,0};

	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(30);
	SET_RESET_PIN(1);
	MDELAY(50);

	array[0] = 0x00053700;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(10);
	read_reg_v2(0xa1, buffer, 5);

	id_high = buffer[2];
	id_low = buffer[3];
	chid_id = (id_high<<8) | id_low;

#ifdef BUILD_LK
		printf("OTM8018B lk id = 0x%0x \n", chid_id);
#else
		printk("OTM8018B kernel id = 0x%0x \n", chid_id);
#endif

	if(chid_id == 0x8019)
		return 1;	  
	else
		return 0; 

}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(20);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(100);

	push_table(lcm_initialization_setting,
		   sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}



static void lcm_suspend(void)
{
	unsigned int data_array[2];
	//int gpio;

	data_array[0] = 0x00280500;	/* Display Off */
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(30);
	data_array[0] = 0x00100500;	/* Sleep In */
	dsi_set_cmdq(data_array, 1, 1);
	SET_RESET_PIN(0);
	MDELAY(150);

#ifdef BUILD_LK
	printf("uboot %s\n", __func__);
#else
	pr_debug("kernel %s\n", __func__);
#endif
}


static void lcm_resume(void)
{
#ifdef BUILD_LK
	printf("uboot %s\n", __func__);
#else
	pr_debug("kernel %s\n", __func__);
#endif
/* push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1); */
	lcm_init();
}

#ifdef LCM_DSI_CMD_MODE
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
	unsigned char x0_LSB = (x0 & 0xFF);
	unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
	unsigned char x1_LSB = (x1 & 0xFF);
	unsigned char y0_MSB = ((y0 >> 8) & 0xFF);
	unsigned char y0_LSB = (y0 & 0xFF);
	unsigned char y1_MSB = ((y1 >> 8) & 0xFF);
	unsigned char y1_LSB = (y1 & 0xFF);

	unsigned int data_array[16];

#ifdef BUILD_LK
	printf("uboot %s\n", __func__);
#else
	pr_debug("kernel %s\n", __func__);
#endif

	data_array[0] = 0x00053902;
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	data_array[3] = 0x00053902;
	data_array[4] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
	data_array[5] = (y1_LSB);
	data_array[6] = 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}
#endif

LCM_DRIVER otm8019_fwvga_by_hsd_drv = {

	.name = "otm8019_fwvga_by_hsd",
	.set_util_funcs = lcm_set_util_funcs,
	.compare_id = lcm_compare_id,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
#if defined(LCM_DSI_CMD_MODE)
	.update = lcm_update,
#endif
};
