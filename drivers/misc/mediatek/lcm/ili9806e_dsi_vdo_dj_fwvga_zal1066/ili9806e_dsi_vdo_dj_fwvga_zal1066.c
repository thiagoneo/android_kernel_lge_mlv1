#ifdef BUILD_LK
#include <cust_gpio_usage.h>
#include <platform/mt_gpio.h>
#else
#include <linux/string.h>
#endif
#include "lcm_drv.h"
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(854)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER

#define LCM_ID_ILI9806E		0x0604

#define LCM_DSI_CMD_MODE									0


#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif

 

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};
//static unsigned int lcm_esd_test = FALSE; 
#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};
//static  int lcm_is_init=0xff; 
#define PAD_AUX_XP  12
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

static struct LCM_setting_table lcm_initialization_setting_dj[] = {

//****************************************************************************//
//****************************** Page 1 Command ******************************//
//****************************************************************************//
		{0xFF,5,{0xFF,0x98,0x06,0x04,0x01}},      //Change to Page 1

		{0x08,1,{0x10}},                 //output SDA

		{0x21,1,{0x01}},                  //DE = 1 Active

		{0x30,1,{0x01}},                  //480 X 854

		{0x31,1,{0x00}},                  //column

		{0x40,1,{0x10}},                // BT  15

		{0x41,1,{0x44}},               //DVDDH DVDDL clamp 

		{0x42,1,{0x03}},                 // VGHVGL 

		{0x43,1,{0x09}},                  //VGH_CLAMP 0FF ;

		{0x44,1,{0x09}},                  //VGL_CLAMP OFF ; 

		{0x50,1,{0x70}},                  //VGMP

		{0x51,1,{0x70}},                 //VGMN

		{0x52,1,{0x00}},                   //Flicker

		{0x53,1,{0x43}} ,                //Flicker 35 32

		{0x57,1,{0x50}},                   //Low voltage detect

		{0x60,1,{0x0a}},                  //SDTI

		{0x61,1,{0x00}},                 //CRTI

		{0x62,1,{0x08}},                 //EQTI

		{0x63,1,{0x00}},                 //PCTI

		//++++++++++++++++++ Gamma Setting ++++++++++++++++++//

		{0xA0,1,{0x00}},             // Gamma 0

		{0xA1,1,{0x06}},            // Gamma 4 

		{0xA2,1,{0x06}},            // Gamma 8

		{0xA3,1,{0x0D}},             // Gamma 16

		{0xA4,1,{0x04}},            // Gamma 24

		{0xA5,1,{0x07}},           // Gamma 52

		{0xA6,1,{0x07}},            // Gamma 80

		{0xA7,1,{0x04}},             // Gamma 108

		{0xA8,1,{0x08}},             // Gamma 147

		{0xA9,1,{0x0D}},             // Gamma 175

		{0xAA,1,{0x0D}},             // Gamma 203

		{0xAB,1,{0x08}},             // Gamma 231

		{0xAC,1,{0x10}},             // Gamma 239

		{0xAD,1,{0x19}},             // Gamma 247

		{0xAE,1,{0x11}},            // Gamma 251

		{0xAF,1,{0x00}},             // Gamma 255

		///=============           =Nagitive
		{0xC0,1,{0x00}},             // Gamma 255 

		{0xC1,1,{0x06}},             // Gamma 251

		{0xC2,1,{0x06}},             // Gamma 247

		{0xC3,1,{0x0D}},             // Gamma 239

		{0xC4,1,{0x04}},             // Gamma 231

		{0xC5,1,{0x07}},             // Gamma 203

		{0xC6,1,{0x07}},             // Gamma 175

		{0xC7,1,{0x04}},             // Gamma 147

		{0xC8,1,{0x08}},             // Gamma 108

		{0xC9,1,{0x0C}},             // Gamma 80

		{0xCA,1,{0x0E}},             // Gamma 52

		{0xCB,1,{0x08}},             // Gamma 24

		{0xCC,1,{0x10}},             // Gamma 16

		{0xCD,1,{0x19}},            // Gamma 8

		{0xCE,1,{0x11}},             // Gamma 4

		{0xCF,1,{0x00}},             // Gamma 0


		//****************************************************************************//
		//****************************** Page 6 Command ******************************//
		//****************************************************************************//
		{0xFF,5,{0xFF,0x98,0x06,0x04,0x06}},

		{0x00,1,{0x21}},

		{0x01,1,{0x09}},

		{0x02,1,{0x00}},

		{0x03,1,{0x00}},

		{0x04,1,{0x01}},

		{0x05,1,{0x01}},

		{0x06,1,{0x80}},

		{0x07,1,{0x05}},

		{0x08,1,{0x02}},

		{0x09,1,{0x80}},

		{0x0A,1,{0x00}},

		{0x0B,1,{0x00}},

		{0x0C,1,{0x0a}},

		{0x0D,1,{0x0a}},

		{0x0E,1,{0x00}},

		{0x0F,1,{0x00}},

		{0x10,1,{0xe0}},

		{0x11,1,{0xe4}},

		{0x12,1,{0x04}},

		{0x13,1,{0x00}},

		{0x14,1,{0x00}},

		{0x15,1,{0xC0}},

		{0x16,1,{0x08}},

		{0x17,1,{0x00}},

		{0x18,1,{0x00}},

		{0x19,1,{0x00}},

		{0x1A,1,{0x00}},

		{0x1B,1,{0x00}},

		{0x1C,1,{0x00}},

		{0x1D,1,{0x00}},

		{0x20,1,{0x01}},

		{0x21,1,{0x23}},

		{0x22,1,{0x45}},

		{0x23,1,{0x67}},

		{0x24,1,{0x01}},

		{0x25,1,{0x23}},

		{0x26,1,{0x45}},

		{0x27,1,{0x67}},

		{0x30,1,{0x01}},

		{0x31,1,{0x11}},

		{0x32,1,{0x00}},

		{0x33,1,{0xEE}},

		{0x34,1,{0xFF}},

		{0x35,1,{0xBB}},

		{0x36,1,{0xCA}},

		{0x37,1,{0xDD}},

		{0x38,1,{0xAC}},

		{0x39,1,{0x76}},

		{0x3A,1,{0x67}},

		{0x3B,1,{0x22}},

		{0x3C,1,{0x22}},

		{0x3D,1,{0x22}},

		{0x3E,1,{0x22}},

		{0x3F,1,{0x22}},

		{0x40,1,{0x22}},

		{0x52,1,{0x10}},

		{0x53,1,{0x10}},
		
		{0xFF,5,{0xFF,0x98,0x06,0x04,0x07}},      //Change to Page 7
		
		{0x17,1,{0x22}},      //VGL_REG ON  
		{0x02,1,{0x77}},   
	
		{0xE1,1,{0x79}},  
		{0x26,1,{0xb2}},
		{0x18,1,{0x1d}},
		
		{0xB3,1,{0x10}}, 
		{0xFF,5,{0xFF,0x98,0x06,0x04,0x05}},
		{0x1A,1,{0x16}},

		{0x25,1,{0x03}},
		{0x26,1,{0x03}},
		{0x27,1,{0x01}},
		{0x28,1,{0x01}},
		{0x29,1,{0x02}},
		{0x2A,1,{0x02}},
		{0x2B,1,{0x02}},
		{0x2C,1,{0x02}},
		{0x2D,1,{0x02}},
		{0x2E,1,{0x02}},
		{0x2F,1,{0x03}},
		{0x30,1,{0x03}},

		{0x31,1,{0x03}},
		{0x32,1,{0x03}},
		{0x33,1,{0x03}},
		{0x34,1,{0x02}},
		{0x35,1,{0x02}},
		{0x36,1,{0x02}},
		{0x37,1,{0x02}},
		{0x38,1,{0x02}},
		{0x39,1,{0x02}},
		{0x3A,1,{0x02}},
		{0x3B,1,{0x02}},
		{0x3C,1,{0x02}},
		{0xFF,5,{0xFF,0x98,0x06,0x04,0x00}},      //Change to Page 0
		{0x11,1,{0x00}},                  //Sleep-Out
                {REGFLAG_DELAY, 120, {}},
                {0x29,1,{0x00}},  // Display On
                {REGFLAG_DELAY, 10, {}},
                {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {

	// Display off sequence
	{0xFF,5,{0xFF,0x98,0x06,0x04,0x00}},
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},
    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
	// Sleep Out
	{0xFF,5,{0xFF,0x98,0x06,0x04,0x00}},
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
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
    params->dbi.te_mode				= LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
    	params->dsi.mode   = CMD_MODE;
#else
    	params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

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
    
    params->dsi.word_count=480*3;	//DSI CMD mode need set these two bellow params, different to 6577
    

    params->dsi.packet_size             = 256;
    params->dsi.vertical_sync_active	= 16; 
    params->dsi.vertical_backporch = 20;//10 9 
    params->dsi.vertical_frontporch = 20;//10 12 
    params->dsi.vertical_active_line	= FRAME_HEIGHT; 
    
    params->dsi.horizontal_sync_active	= 20;//20 
    params->dsi.horizontal_backporch	= 80;//100 
    params->dsi.horizontal_frontporch	= 80;//100
    params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

    params->dsi.PLL_CLOCK = 230; //240//this value must be in MTK suggested table
    params->dsi.compatibility_for_nvk = 1;
    params->dsi.ssc_disable = 0;
    params->dsi.ssc_range = 2;
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
    
    params->dsi.lcm_esd_check_table[1].cmd = 0x0d;
    params->dsi.lcm_esd_check_table[1].count = 1;
    params->dsi.lcm_esd_check_table[1].para_list[0] = 0x00;
    
    params->dsi.lcm_esd_check_table[2].cmd = 0x0B;
    params->dsi.lcm_esd_check_table[2].count = 1;
    params->dsi.lcm_esd_check_table[2].para_list[0] = 0x00;
    
    	

}



static void lcm_init(void)
{
#ifdef BUILD_LK
    printf("ili9806_dj init start \n");
#else
    printk("ili9806_dj inint start \n");
#endif

    SET_RESET_PIN(1);
    MDELAY(10); //20
    SET_RESET_PIN(0);
    MDELAY(10); //20
    SET_RESET_PIN(1);
    MDELAY(120);//150

    push_table(lcm_initialization_setting_dj, sizeof(lcm_initialization_setting_dj) / sizeof(struct LCM_setting_table), 1);

#ifdef BUILD_LK
    printf("ili9806_dj init end \n");
#else
    printk("ili9806_dj inint end \n");
#endif

}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting)/sizeof(struct LCM_setting_table), 1);
	//SET_RESET_PIN(1);
    //MDELAY(1);
	//SET_RESET_PIN(0);
	//MDELAY(5);
	//SET_RESET_PIN(1);
	//MDELAY(20);
}


static void lcm_resume(void)
{
	//lcm_init();
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


#ifdef BUILD_LK
static unsigned int lcm_compare_id(void)
{
	int array[4];
	char buffer[5];
	//char id_high=0;
	char id_midd=0;
	char id_low=0;
	int id=0;
	int lcm_id=0;
	

	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);

	array[0]=0x00063902;
	array[1]=0x0698ffff;
	array[2]=0x00000104;
	dsi_set_cmdq(array, 3, 1);
	MDELAY(10);

	array[0] = 0x00023700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x01, &buffer[1],1);

	id_midd = buffer[1]; 
	
	read_reg_v2(0x02, &buffer[2],1);

	id_low = buffer[2]; 

	id = (id_midd << 8) | id_low;

	printf("%s, LK DJ_HSD ILI9806E debug: ILI9806E_DJ id = 0x%x\n", __func__, id);
	
	if(LCM_ID_ILI9806E == id) {
		mt_set_gpio_mode(GPIO_LCM_ID, GPIO_LCM_ID_M_GPIO);
		mt_set_gpio_dir(GPIO_LCM_ID, GPIO_DIR_IN);
		lcm_id = mt_get_gpio_in(GPIO_LCM_ID);

		printf("ili9806_dj compare id value is = %d\n",lcm_id);

		if(lcm_id)
			return 1;
		else
			return 0;
	} else
		return 0;
}
#endif

LCM_DRIVER ili9806e_dsi_vdo_dj_fwvga_drv = 
{
	.name		= "ili9806e_dj_hsd",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#ifdef BUILD_LK
	.compare_id    = lcm_compare_id,
#endif
};

