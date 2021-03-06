#ifdef BUILD_LK
#else
#include <linux/string.h>
#endif
#include "lcm_drv.h"
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER

#define LCM_ID_ili9881	0x89

#define LCM_DSI_CMD_MODE									0


#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif

//static unsigned int lcm_esd_test = FALSE; 

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

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

{0xFF,3,{0x98,0x81,0x03}},
{0x01,1,{0x00}},
{0x02,1,{0x00}},
{0x03,1,{0x73}},
{0x04,1,{0x00}},
{0x05,1,{0x00}},
{0x06,1,{0x0C}},
{0x07,1,{0x00}},
{0x08,1,{0x00}},
{0x09,1,{0x19}},
{0x0a,1,{0x01}},
{0x0b,1,{0x01}},
{0x0c,1,{0x0B}},
{0x0d,1,{0x01}},
{0x0e,1,{0x01}},
{0x0f,1,{0x26}},
{0x10,1,{0x26}},
{0x11,1,{0x00}},
{0x12,1,{0x00}},
{0x13,1,{0x02}},
{0x14,1,{0x00}},
{0x15,1,{0x00}},
{0x16,1,{0x00}},
{0x17,1,{0x00}},
{0x18,1,{0x00}},
{0x19,1,{0x00}},
{0x1a,1,{0x00}},
{0x1b,1,{0x00}},
{0x1c,1,{0x00}},
{0x1d,1,{0x00}},
{0x1e,1,{0x44}},
{0x1f,1,{0xC0}},
{0x20,1,{0x0A}},
{0x21,1,{0x03}},
{0x22,1,{0x0A}},
{0x23,1,{0x00}},
{0x24,1,{0x8C}},
{0x25,1,{0x8C}},
{0x26,1,{0x00}},
{0x27,1,{0x00}},
{0x28,1,{0x3B}},
{0x29,1,{0x03}},
{0x2a,1,{0x00}},
{0x2b,1,{0x00}},
{0x2c,1,{0x00}},
{0x2d,1,{0x00}},
{0x2e,1,{0x00}},
{0x2f,1,{0x00}},
{0x30,1,{0x00}},
{0x31,1,{0x00}},
{0x32,1,{0x00}},
{0x33,1,{0x00}},
{0x34,1,{0x00}},
{0x35,1,{0x00}},
{0x36,1,{0x00}},
{0x37,1,{0x00}},
{0x38,1,{0x00}},
{0x39,1,{0x00}},
{0x3a,1,{0x00}},
{0x3b,1,{0x00}},
{0x3c,1,{0x00}},
{0x3d,1,{0x00}},
{0x3e,1,{0x00}},
{0x3f,1,{0x00}},
{0x40,1,{0x00}},
{0x41,1,{0x00}},
{0x42,1,{0x00}},
{0x43,1,{0x00}},
{0x44,1,{0x00}},

//GIP_2
{0x50,1,{0x01}},
{0x51,1,{0x23}},
{0x52,1,{0x45}},
{0x53,1,{0x67}},
{0x54,1,{0x89}},
{0x55,1,{0xab}},
{0x56,1,{0x01}},
{0x57,1,{0x23}},
{0x58,1,{0x45}},
{0x59,1,{0x67}},
{0x5a,1,{0x89}},
{0x5b,1,{0xab}},
{0x5c,1,{0xcd}},
{0x5d,1,{0xef}},

//GIP_3
{0x5e,1,{0x11}},
{0x5f,1,{0x02}},
{0x60,1,{0x00}},
{0x61,1,{0x0C}},
{0x62,1,{0x0D}},
{0x63,1,{0x0E}},
{0x64,1,{0x0F}},
{0x65,1,{0x02}},
{0x66,1,{0x02}},
{0x67,1,{0x02}},
{0x68,1,{0x02}},
{0x69,1,{0x02}},
{0x6a,1,{0x02}},
{0x6b,1,{0x02}},
{0x6c,1,{0x02}},
{0x6d,1,{0x02}},
{0x6e,1,{0x05}},
{0x6f,1,{0x05}},
{0x70,1,{0x05}},
{0x71,1,{0x05}},
{0x72,1,{0x01}},
{0x73,1,{0x06}},
{0x74,1,{0x07}},
{0x75,1,{0x02}},
{0x76,1,{0x00}},
{0x77,1,{0x0C}},
{0x78,1,{0x0D}},
{0x79,1,{0x0E}},
{0x7a,1,{0x0F}},
{0x7b,1,{0x02}},
{0x7c,1,{0x02}},
{0x7d,1,{0x02}},
{0x7e,1,{0x02}},
{0x7f,1,{0x02}},
{0x80,1,{0x02}},
{0x81,1,{0x02}},
{0x82,1,{0x02}},
{0x83,1,{0x02}},
{0x84,1,{0x05}},
{0x85,1,{0x05}},
{0x86,1,{0x05}},
{0x87,1,{0x05}},
{0x88,1,{0x01}},
{0x89,1,{0x06}},
{0x8A,1,{0x07}},


{0xFF,3,{0x98,0x81,0x04}},
{0x00,1,{0x00}},
{0x6C,1,{0x15}},                
{0x6E,1,{0x1A}},                //di_pwr_reg=0 VGH clamp 
{0x6F,1,{0x25}},                // reg vcl + VGH pumping ratio 3x VGL=-2x
{0x3A,1,{0xA4}},                
{0x8D,1,{0x20}},                //VGL clamp -10V
{0x87,1,{0xBA}},                
{0x26,1,{0x76}},                //5.2 none
{0xB2,1,{0xD1}},                //5.2 none
//{0x3B,1,{0xC0}},              // HIMAX POWER IC
//{0x7A,1,{0x10}},              //DI_REG_REG1_EN_CAP=0,   bypass VREG1/2 to pad
//{0x71,1,{0xB0}},              //default setting

//CMD_Page 1
{0xFF,3,{0x98,0x81,0x01}},
{0x22,1,{0x0A}},               //BGR, SS
{0x31,1,{0x00}},               //column inversion
{0x53,1,{0x82}},               //VCOM1 7D
{0x55,1,{0x86}},               //VCOM2
{0x50,1,{0x75}},               //VREG1OUT=4.104V
{0x51,1,{0x71}},               //VREG2OUT=-4.106V
//{0x40,1,{0x53}},             // HIMAX POWER IC
//{0x43,1,{0x66}},
{0x60,1,{0x1B}},               //SDT
{0x61,1,{0x01}},
{0x62,1,{0x0C}},
{0x63,1,{0x00}},
{0xA0,1,{0x08}},               //VP255 Gamma P
{0xA1,1,{0x17}},               //VP251
{0xA2,1,{0x24}},               //VP247
{0xA3,1,{0x10}},               //VP243
{0xA4,1,{0x0A}},               //VP239
{0xA5,1,{0x25}},               //VP231
{0xA6,1,{0x1A}},               //VP219
{0xA7,1,{0x1C}},               //VP203
{0xA8,1,{0x7B}},               //VP175
{0xA9,1,{0x1F}},               //VP144
{0xAA,1,{0x2A}},               //VP111
{0xAB,1,{0x6A}},               //VP80
{0xAC,1,{0x1B}},               //VP52
{0xAD,1,{0x17}},               //VP36
{0xAE,1,{0x48}},               //VP24
{0xAF,1,{0x1C}},               //VP16
{0xB0,1,{0x27}},               //VP12
{0xB1,1,{0x49}},               //VP8
{0xB2,1,{0x58}},               //VP4
{0xB3,1,{0x39}},               //VP0 00

{0xC0,1,{0x08}},               //VN255 GAMMA N
{0xC1,1,{0x17}},               //VN251
{0xC2,1,{0x24}},               //VN247
{0xC3,1,{0x10}},               //VN243
{0xC4,1,{0x08}},               //VN239
{0xC5,1,{0x25}},               //VN231
{0xC6,1,{0x1A}},               //VN219
{0xC7,1,{0x1B}},               //VN203
{0xC8,1,{0x7C}},               //VN175
{0xC9,1,{0x1D}},               //VN144
{0xCA,1,{0x28}},               //VN111
{0xCB,1,{0x6A}},               //VN80
{0xCC,1,{0x16}},               //VN52
{0xCD,1,{0x15}},               //VN36
{0xCE,1,{0x4A}},               //VN24
{0xCF,1,{0x21}},               //VN16
{0xD0,1,{0x26}},               //VN12
{0xD1,1,{0x51}},               //VN8
{0xD2,1,{0x65}},               //VN4
{0xD3,1,{0x39}},               //VN0 00

///////////////////////////////////////////////////            
{0xFF,3,{0x98,0x81,0x00}},
{0x11,1,{00}},                 // Sleep-Out
{REGFLAG_DELAY, 120, {}},
{0x29,1,{00}},                 // Display On
{REGFLAG_DELAY, 10, {}},

{REGFLAG_END_OF_TABLE, 0x00, {}}
};


/*static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0xFF,3,{0x98,0x81,0x00}}, 

	
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};*/


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {

	// Display off sequence
	{0xFF,3,{0x98,0x81,0x00}}, 

	
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_comare_id_setting[] = {
    {0xFF,	3,	{0x98,0x81,0x01}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};
/*static struct LCM_setting_table lcm_comare_check[] = {
    {0xFF,	3,	{0x98,0x81,0x00}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};*/
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
	 
	 params->type	= LCM_TYPE_DSI;
	 
	 params->width	= FRAME_WIDTH;
	 params->height = FRAME_HEIGHT;
	    // enable tearing-free
    params->dbi.te_mode				= LCM_DBI_TE_MODE_DISABLED;
//    params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
	 params->dsi.mode	=  SYNC_PULSE_VDO_MODE;  
	 // DSI
	 /* Command mode setting */
	 params->dsi.LANE_NUM				 = LCM_THREE_LANE;
	 
	 //The following defined the fomat for data coming from LCD engine.
	 params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	 params->dsi.data_format.trans_seq	 = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	 params->dsi.data_format.padding	 = LCM_DSI_PADDING_ON_LSB;
	 params->dsi.data_format.format    = LCM_DSI_FORMAT_RGB888;
	 
	 // Video mode setting		 
	 params->dsi.intermediat_buffer_num =0;
	 
	 params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	 
	 params->dsi.word_count=720*3;	 //DSI CMD mode need set these two bellow params, different to 6577
	 params->dsi.vertical_active_line=1280;
	
	 params->dsi.vertical_sync_active				 = 15;//4
	 params->dsi.vertical_backporch 				 = 20;//16
	 params->dsi.vertical_frontporch				 = 20;//20
	 params->dsi.vertical_active_line				 = FRAME_HEIGHT;
	 
	 params->dsi.horizontal_sync_active 			 = 50;//10
	 params->dsi.horizontal_backporch				 = 50;//70;50
	 params->dsi.horizontal_frontporch				 = 50;//80;60
	 params->dsi.horizontal_blanking_pixel			 = 60;//100;80
	 params->dsi.horizontal_active_pixel			 = FRAME_WIDTH;
	 params->dsi.HS_TRAIL                             = 12;	 
	// Bit rate calculation
	//params->dsi.LPX=8; 

	params->dsi.PLL_CLOCK=280;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;

	params->dsi.lcm_esd_check_table[0].cmd = 0x09; 
	params->dsi.lcm_esd_check_table[0].count = 3; 
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x80; 
	params->dsi.lcm_esd_check_table[0].para_list[1] = 0x03; 
	params->dsi.lcm_esd_check_table[0].para_list[2] = 0x04;

	params->dsi.ssc_range=8;
	params->dsi.ssc_disable=0;
                
	// Non-continuous clock 
	params->dsi.noncont_clock = TRUE; 
	params->dsi.noncont_clock_period = 2; // Unit : frames	
}

static void lcm_init(void)
{
    SET_RESET_PIN(1);
    MDELAY(10); //20
    SET_RESET_PIN(0);
    MDELAY(20); //20
    SET_RESET_PIN(1);
    MDELAY(150);//150

    #ifdef BUILD_LK
		printf("ili9881 init start \n");
   #else
		printk("ili9881 inint start \n");
   #endif
push_table(lcm_initialization_setting_dj, sizeof(lcm_initialization_setting_dj) / sizeof(struct LCM_setting_table), 1);
    #ifdef BUILD_LK
		printf("ili9881 init end \n");
   #else
		printk("ili9881 inint end \n");
   #endif
}


static void lcm_suspend(void)
{

	/*SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);*/

	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting)/sizeof(struct LCM_setting_table), 1);
	SET_RESET_PIN(1);
        MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(5);
	SET_RESET_PIN(1);
	MDELAY(20);
}


static void lcm_resume(void)
{
	lcm_init();
//	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);

}

#define OTM8018B_ILI9806E_BOOT_ADC_PARASE_VAL	2047

static unsigned int lcm_compare_id(void)
{ 
	unsigned int id,id1=0;
	unsigned char buffer[2];
	unsigned int array[16];  
    int lcm_adc = 0;
    //int data[4] = {0,0,0,0};	

	SET_RESET_PIN(1);
	MDELAY(5); 
	SET_RESET_PIN(0);
	MDELAY(50); 
	SET_RESET_PIN(1);
	MDELAY(120); 
	
	push_table(lcm_comare_id_setting, sizeof(lcm_comare_id_setting) / sizeof(struct LCM_setting_table), 1);

	//array[0]=0x00063902;
	//array[1]=0x0698FFFF;// page enable
	//array[2]=0x00000104;
	//dsi_set_cmdq(array, 3, 1);
	//MDELAY(10); 
	array[0]=0x00023700;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(10); 
	//read_reg_v2(0x00, &buffer[0], 1);
	read_reg_v2(0x00, buffer, 1);
	id  =  buffer[0]; 	
	read_reg_v2(0x01, buffer, 1);
	id1  =  buffer[0]; 


	#if 0
	//read_reg_v2(0x00, &buffer[0], 1);
	read_reg_v2(0x01, &buffer[0], 1);
	read_reg_v2(0x02, &buffer[1], 1);
	id  =  buffer[1]; 
	id1 =  buffer[0];
	#endif
  //  IMM_GetOneChannelValue(1,&data,&lcm_adc); //read lcd _id
#ifdef BUILD_LK
	printf("[ili9881_dj]id:%x, id1:%x\n", id, id1);
	printf("id:%d, id1:%d\n", id, id1);
	printf("%s, id = 0x%08x id1=%x \n", __func__, id,id1);
	printf("ili9881 boot lcm_adc = %d\n", lcm_adc);
#else
	printk("buffer[0]:%x, buffer[1]:%x\n", buffer[0], buffer[1]);
	printk("[ili9881_dj]id:%x, id1:%x\n", id, id1);
	printk("ili9881 boot lcm_adc = %d\n", lcm_adc);
#endif

	//return (lcm_adc < OTM8018B_ILI9806E_BOOT_ADC_PARASE_VAL)? 1 : 0;	//yutianqiang 2014-4-28 18:40:05
      if(0x98 == id && 0x81 == id1){
#ifdef PAD_AUX_XP
		  int data[4] = {0, 0, 0,0};
		  int rawdata = 0;
		  int res = 0;
		  int lcm_vol = 0;
	 
		  res = IMM_GetOneChannelValue(PAD_AUX_XP,data,&rawdata);
		  if(res < 0)
		  {
#ifdef BUILD_LK
			  printf("[adc_uboot]: get data error\n");
#endif
			  return 0;
		  }
		  
		  lcm_vol = data[0] * 1000 + data[1] * 10;
			   
 #ifdef BUILD_LK
		  printf("[adc_uboot][ili9881_dj]: lcm_vol= %d\n", lcm_vol);
#else
		  printk("[adc_kernel][ili9881_dj]: lcm_vol = %d\n", lcm_vol);
 #endif
		  
		  if(lcm_vol >=0 && lcm_vol <=100)
				  return 0;
		  else
				  return 1;
#endif
      	}
      else
        return 0;
     //return (0xbb == id)?1 : 0;

}


/*static unsigned int lcm_esd_check(void)
{
 #ifndef BUILD_LK
// printk("luning ---lcm_esd_check\n");
	char  buffer[3];
	int   array[4];
	UDELAY(800) ;

	if(lcm_esd_test)
	{
		lcm_esd_test = FALSE;
		return TRUE;
	}
	push_table(lcm_comare_check, sizeof(lcm_comare_check) / sizeof(struct LCM_setting_table), 1);

	array[0] = 0x00033700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x09, buffer, 3);
	printk("%s, kernel\n", __func__);
	printk("%s, kernel ili9881 lcm_esd_check buffer[0] = %x, buffer[1] = %x, buffer[2] =%x\n", __func__, buffer[0], buffer[1], buffer[2]);
	if(buffer[0]==0x80 && buffer[1]==0x73 && buffer[2]==0x04)
	{
		printk("%s, kernel ili9881 lcm_esd_check ok!\n!", __func__);
		return FALSE;
	}
	else
	{	
	  printk("%s, kernel ili9881 lcm_esd_check failed!\n", __func__);		 
		return TRUE;
	}
	//return FALSE;
		
 #endif

}

static unsigned int lcm_esd_recover(void)
{
	
	lcm_init();
//	lcm_resume();
 #ifndef BUILD_LK
	printk("luning ---lcm_esd_recover\n");
 #endif

	return TRUE;
}*/
	
LCM_DRIVER ili9881c_dsi_vdo_hd720_dj_zal875_lcm_drv = 
{
	.name			= "ili9881c_boe_hd720_dj",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,	
//	.esd_check = lcm_esd_check,
//	.esd_recover = lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
	//.set_backlight	= lcm_setbacklight,
	//.update         = lcm_update,
#endif
};

