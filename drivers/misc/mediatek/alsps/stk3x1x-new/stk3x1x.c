/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.  
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/* drivers/hwmon/mt6516/amit/stk3x1x.c - stk3x1x ALS/PS driver
 * 
 * Author: MingHsien Hsieh <minghsien.hsieh@mediatek.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/gpio.h>

#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>
#include <linux/version.h>
#include <linux/fs.h>   
#include <linux/wakelock.h> 
#include <asm/io.h>
#include <linux/module.h>
#include "cust_alsps.h"  //wangxiqiang
#include "alsps.h"

//#include <linux/hwmsen_helper.h>
//#include "cust_eint.h"
//#include <linux/hwmsensor.h>
//#include <linux/sensors_io.h>
//#include <linux/hwmsen_dev.h>
//#include <stk_cust_alsps.h>
#include "stk3x1x.h"
#define DRIVER_VERSION          "3.5.2 20151230"
//#define STK_PS_POLLING_LOG
//#define STK_PS_DEBUG
//#define STK_ALS_FIR
#define HQ_PSENSOR_CALIBRATION
#define MAX_PS_NOISE 560
#define AVG_PS_NOISE 10

/////
//extern struct alsps_hw* get_cust_alsps_hw(void);


#if ((defined MT6573) || (defined MT6575) || (defined MT6577) || (defined MT6589)  || (defined MT6572))	
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);
#else
    //#include <mach/eint.h>
	// extern void mt_eint_mask(unsigned int eint_num);
	// extern void mt_eint_unmask(unsigned int eint_num);
	// extern void mt_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
	// extern void mt_eint_set_polarity(unsigned int eint_num, unsigned int pol);
	// extern unsigned int mt_eint_set_sens(unsigned int eint_num, unsigned int sens);
	// extern void mt_eint_registration(unsigned int eint_num, unsigned int flow, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);
	// extern void mt_eint_print_status(void);
#endif

/******************************************************************************
 * configuration
*******************************************************************************/

/*----------------------------------------------------------------------------*/
#define stk3x1x_DEV_NAME     "stk3x1x"
/*----------------------------------------------------------------------------*/
#define APS_TAG                  "[ALS/PS] "
/*
#define APS_FUN(f)               printk(KERN_ERR APS_TAG"%s\n", __FUNCTION__)
#define APS_ERR(fmt, args...)    printk(KERN_ERR  APS_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define APS_LOG(fmt, args...)    printk(KERN_ERR APS_TAG fmt, ##args)
#define APS_DBG(fmt, args...)    printk(KERN_ERR fmt, ##args) 
*/
#define APS_FUN(f)               pr_debug(APS_TAG"%s\n", __func__)
#define APS_ERR(fmt, args...)    pr_err(APS_TAG"%s %d : "fmt, __func__, __LINE__, ##args)
#define APS_LOG(fmt, args...)    pr_err(APS_TAG fmt, ##args)
#define APS_DBG(fmt, args...)    pr_err(APS_TAG fmt, ##args)                
/******************************************************************************
 * extern functions
*******************************************************************************/
/*----------------------------------------------------------------------------*/
#define mt6516_I2C_DATA_PORT        ((base) + 0x0000)
#define mt6516_I2C_SLAVE_ADDR       ((base) + 0x0004)
#define mt6516_I2C_INTR_MASK        ((base) + 0x0008)
#define mt6516_I2C_INTR_STAT        ((base) + 0x000c)
#define mt6516_I2C_CONTROL          ((base) + 0x0010)
#define mt6516_I2C_TRANSFER_LEN     ((base) + 0x0014)
#define mt6516_I2C_TRANSAC_LEN      ((base) + 0x0018)
#define mt6516_I2C_DELAY_LEN        ((base) + 0x001c)
#define mt6516_I2C_TIMING           ((base) + 0x0020)
#define mt6516_I2C_START            ((base) + 0x0024)
#define mt6516_I2C_FIFO_STAT        ((base) + 0x0030)
#define mt6516_I2C_FIFO_THRESH      ((base) + 0x0034)
#define mt6516_I2C_FIFO_ADDR_CLR    ((base) + 0x0038)
#define mt6516_I2C_IO_CONFIG        ((base) + 0x0040)
#define mt6516_I2C_DEBUG            ((base) + 0x0044)
#define mt6516_I2C_HS               ((base) + 0x0048)
#define mt6516_I2C_DEBUGSTAT        ((base) + 0x0064)
#define mt6516_I2C_DEBUGCTRL        ((base) + 0x0068)
/*----------------------------------------------------------------------------*/

#define STK_IRC_MAX_ALS_CODE		20000
#define STK_IRC_MIN_ALS_CODE		25
#define STK_IRC_MIN_IR_CODE		50
#define STK_IRC_ALS_DENOMI		2		
#define STK_IRC_ALS_NUMERA		5
#define STK_IRC_ALS_CORREC		748

#define STK_IRS_IT_REDUCE				2
#define STK_ALS_READ_IRS_IT_REDUCE	5
#define STK_ALS_THRESHOLD			30

#define STK3310SA_PID		0x17
#define STK3311SA_PID		0x1E
#define STK3311WV_PID	0x1D
#define MTK_AUTO_DETECT_ALSPS
/*----------------------------------------------------------------------------*/
static struct i2c_client *stk3x1x_i2c_client = NULL;
/*----------------------------------------------------------------------------*/
static const struct i2c_device_id stk3x1x_i2c_id[] = {{stk3x1x_DEV_NAME,0},{}};
//static struct regulator *stk3x1x_regulator;

/*the adapter id & i2c address will be available in customization*/
//static unsigned short stk3x1x_force[] = {0x00, 0x00, I2C_CLIENT_END, I2C_CLIENT_END};
//static const unsigned short *const stk3x1x_forces[] = { stk3x1x_force, NULL };
//static struct i2c_client_address_data stk3x1x_addr_data = { .forces = stk3x1x_forces,};

/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id); 
static int stk3x1x_i2c_remove(struct i2c_client *client);
static int stk3x1x_i2c_detect(struct i2c_client *client, struct i2c_board_info *info);
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_suspend(struct i2c_client *client, pm_message_t msg);
static int stk3x1x_i2c_resume(struct i2c_client *client);
//static struct stk3x1x_priv *g_stk3x1x_ptr = NULL;
#ifdef HQ_PSENSOR_CALIBRATION
static int stk3x1x_psensor_calibrate(void);
static int stk3x1x_get_init_noise(void);
#endif

static int	stk3x1x_init_flag = -1;	// 0<==>OK -1 <==> fail
static int  stk3x1x_local_init(void);
static int  stk3x1x_local_uninit(void);
//static struct sensor_init_info stk3x1x_init_info = {
static struct alsps_init_info stk3x1x_init_info = {
		.name = "stk3x1x",
		.init = stk3x1x_local_init,
		.uninit = stk3x1x_local_uninit,
	
};

static struct alsps_hw alsps_cust;
static struct alsps_hw *hw = &alsps_cust;
static struct platform_device *alspsPltFmDev;
/* For alsp driver get cust info */
/*static struct alsps_hw *get_cust_alsps(void)
{
	return &alsps_cust;
}*/
/*----------------------------------------------------------------------------*/
typedef enum {
    STK_TRC_ALS_DATA= 0x0001,
    STK_TRC_PS_DATA = 0x0002,
    STK_TRC_EINT    = 0x0004,
    STK_TRC_IOCTL   = 0x0008,
    STK_TRC_I2C     = 0x0010,
    STK_TRC_CVT_ALS = 0x0020,
    STK_TRC_CVT_PS  = 0x0040,
    STK_TRC_DEBUG   = 0x8000,
} STK_TRC;
/*----------------------------------------------------------------------------*/
typedef enum {
    STK_BIT_ALS    = 1,
    STK_BIT_PS     = 2,
} STK_BIT;
/*----------------------------------------------------------------------------*/
struct stk3x1x_i2c_addr {    
/*define a series of i2c slave address*/
    u8  state;      	/* enable/disable state */
    u8  psctrl;     	/* PS control */
    u8  alsctrl;    	/* ALS control */
    u8  ledctrl;   		/* LED control */
    u8  intmode;    	/* INT mode */
    u8  wait;     		/* wait time */
    u8  thdh1_ps;   	/* PS INT threshold high 1 */
	u8	thdh2_ps;		/* PS INT threshold high 2 */
    u8  thdl1_ps;   	/* PS INT threshold low 1 */
	u8  thdl2_ps;   	/* PS INT threshold low 2 */
    u8  thdh1_als;   	/* ALS INT threshold high 1 */
	u8	thdh2_als;		/* ALS INT threshold high 2 */
    u8  thdl1_als;   	/* ALS INT threshold low 1 */
	u8  thdl2_als;   	/* ALS INT threshold low 2 */	
	u8  flag;			/* int flag */
	u8  data1_ps;		/* ps data1 */
	u8  data2_ps;		/* ps data2 */
	u8  data1_als;		/* als data1 */
	u8  data2_als;		/* als data2 */
	u8  data1_offset;	/* offset data1 */
	u8  data2_offset;	/* offset data2 */
	u8  data1_ir;		/* ir data1 */
	u8  data2_ir;		/* ir data2 */
	u8  soft_reset;		/* software reset */
};
/*----------------------------------------------------------------------------*/
#ifdef STK_ALS_FIR
	#define STK_FIR_LEN	8
	#define MAX_FIR_LEN 32
struct data_filter {
    u16 raw[MAX_FIR_LEN];
    int sum;
    int num;
    int idx;
};
#endif


struct stk3x1x_priv {
    struct alsps_hw  *hw;
    struct i2c_client *client;
    struct delayed_work  eint_work;

    /*i2c address group*/
    struct stk3x1x_i2c_addr  addr;
    
    /*misc*/
    atomic_t    trace;
    atomic_t    i2c_retry;
    atomic_t    als_suspend;
    atomic_t    als_debounce;   /*debounce time after enabling als*/
    atomic_t    als_deb_on;     /*indicates if the debounce is on*/
    atomic_t    als_deb_end;    /*the jiffies representing the end of debounce*/
    atomic_t    ps_mask;        /*mask ps: always return far away*/
    atomic_t    ps_debounce;    /*debounce time after enabling ps*/
    atomic_t    ps_deb_on;      /*indicates if the debounce is on*/
    atomic_t    ps_deb_end;     /*the jiffies representing the end of debounce*/
    atomic_t    ps_suspend;
	atomic_t    init_done;
	struct      device_node *irq_node;
	int         irq;

    /*data*/
    u16         als;
    u16         ps;
    u8          _align;
    u16         als_level_num;
    u16         als_value_num;
    u32         als_level[C_CUST_ALS_LEVEL-1];
    u32         als_value[C_CUST_ALS_LEVEL];

	atomic_t	state_val;
	atomic_t 	psctrl_val;
	atomic_t 	alsctrl_val;
	u8 			wait_val;
	u8		 	ledctrl_val;
	u8		 	int_val;
	
    atomic_t    ps_high_thd_val;     /*the cmd value can't be read, stored in ram*/
    atomic_t    ps_low_thd_val;     /*the cmd value can't be read, stored in ram*/
	int 		init_noise;
	int 		current_xtalk;
    ulong       enable;         /*enable mask*/
    ulong       pending_intr;   /*pending interrupt*/
	atomic_t	recv_reg;
    /*early suspend*/    
	bool first_boot;
#ifdef STK_ALS_FIR
	struct data_filter      fir;
	atomic_t                firlength;		
#endif
	uint16_t ir_code;
	uint16_t als_correct_factor;	
	u16 als_last;
	bool re_enable_ps;
	bool re_enable_als;
	u16 ps_cali;		
	
	uint8_t pid;
	uint8_t	p_wv_r_bd_with_co;
	uint16_t p_wv_r_bd_ratio;
	uint32_t als_code_last;	
};
/*----------------------------------------------------------------------------*/
#ifdef CONFIG_OF
static const struct of_device_id alsps_of_match[] = {
	{.compatible = "mediatek,alsps"},
	{},
};
#endif

static struct i2c_driver stk3x1x_i2c_driver = {	
	.probe      = stk3x1x_i2c_probe,
	.remove     = stk3x1x_i2c_remove,
	.detect     = stk3x1x_i2c_detect,
	.suspend    = stk3x1x_i2c_suspend,
	.resume     = stk3x1x_i2c_resume,
	.id_table   = stk3x1x_i2c_id,
	.driver = {
		.name           = stk3x1x_DEV_NAME,
#ifdef CONFIG_OF
		.of_match_table = alsps_of_match,
#endif
	},
};

static struct stk3x1x_priv *stk3x1x_obj = NULL;
static int stk3x1x_get_ps_value(struct stk3x1x_priv *obj, u16 ps);
//static int stk3x1x_get_ps_value_only(struct stk3x1x_priv *obj, u16 ps);
static int stk3x1x_get_als_value(struct stk3x1x_priv *obj, u16 als);
static int stk3x1x_read_als(struct i2c_client *client, u16 *data);
static int stk3x1x_read_ps(struct i2c_client *client, u16 *data);
//static int stk3x1x_set_als_int_thd(struct i2c_client *client, u16 als_data_reg);
static int32_t stk3x1x_get_ir_value(struct stk3x1x_priv *obj, int32_t als_it_reduce);
static int stk3x1x_init_client(struct i2c_client *client);
//struct wake_lock ps_lock;
/*----------------------------------------------------------------------------*/
int stk3x1x_get_addr(struct alsps_hw *hw, struct stk3x1x_i2c_addr *addr)
{
	if(!hw || !addr)
	{
		return -EFAULT;
	}
	addr->state   = STK_STATE_REG; 
	addr->psctrl   = STK_PSCTRL_REG;         
	addr->alsctrl  = STK_ALSCTRL_REG;
	addr->ledctrl  = STK_LEDCTRL_REG;
	addr->intmode    = STK_INT_REG;
	addr->wait    = STK_WAIT_REG;
	addr->thdh1_ps    = STK_THDH1_PS_REG;
	addr->thdh2_ps    = STK_THDH2_PS_REG;
	addr->thdl1_ps = STK_THDL1_PS_REG;
	addr->thdl2_ps = STK_THDL2_PS_REG;
	addr->thdh1_als    = STK_THDH1_ALS_REG;
	addr->thdh2_als    = STK_THDH2_ALS_REG;
	addr->thdl1_als = STK_THDL1_ALS_REG ;
	addr->thdl2_als = STK_THDL2_ALS_REG;
	addr->flag = STK_FLAG_REG;	
	addr->data1_ps = STK_DATA1_PS_REG;
	addr->data2_ps = STK_DATA2_PS_REG;
	addr->data1_als = STK_DATA1_ALS_REG;	
	addr->data2_als = STK_DATA2_ALS_REG;	
	addr->data1_offset = STK_DATA1_OFFSET_REG;
	addr->data2_offset = STK_DATA2_OFFSET_REG;
	addr->data1_ir = STK_DATA1_IR_REG;	
	addr->data2_ir = STK_DATA2_IR_REG;		
	addr->soft_reset = STK_SW_RESET_REG;	
	
	return 0;
}
/*----------------------------------------------------------------------------*/
int stk3x1x_hwmsen_read_block(struct i2c_client *client, u8 addr, u8 *data, u8 len)
{
	u8 beg = addr; 
	struct i2c_msg msgs[2] = 
	{
		{
			.addr = client->addr,	 
			.flags = 0,
			.len = 1,				 
			.buf= &beg
		},
		{
			.addr = client->addr,	 
			.flags = I2C_M_RD,
			.len = len, 			 
			.buf = data,
		}
	};
	int err;

	if (!client)
		return -EINVAL;
	else if (len > C_I2C_FIFO_SIZE) 
	{		 
		APS_LOG(" length %d exceeds %d\n", len, C_I2C_FIFO_SIZE);
		return -EINVAL;
	}

	err = i2c_transfer(client->adapter, msgs, sizeof(msgs)/sizeof(msgs[0]));
	if (err != 2) 
	{
		APS_LOG("i2c_transfer error: (%d %p %d) %d\n", addr, data, len, err);
		err = -EIO;
	}
	else 
	{
		err = 0;/*no error*/
	}
	return err;
}
/*----------------------------------------------------------------------------*/
int stk3x1x_get_timing(void)
{
	return 200;
/*
	u32 base = I2C2_BASE; 
	return (__raw_readw(mt6516_I2C_HS) << 16) | (__raw_readw(mt6516_I2C_TIMING));
*/
}

/*----------------------------------------------------------------------------*/
int stk3x1x_master_recv(struct i2c_client *client, u16 addr, u8 *buf ,int count)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0, retry = 0;
	int trc = atomic_read(&obj->trace);
	int max_try = atomic_read(&obj->i2c_retry);

	while(retry++ < max_try)
	{
		ret = stk3x1x_hwmsen_read_block(client, addr, buf, count);
		if(ret == 0)
            break;
		udelay(100);
	}

	if(unlikely(trc))
	{
		if((retry != 1) && (trc & STK_TRC_DEBUG))
		{
			APS_LOG("(recv) %d/%d\n", retry-1, max_try); 

		}
	}

	/* If everything went ok (i.e. 1 msg transmitted), return #bytes
	transmitted, else error code. */
	return (ret == 0) ? count : ret;
}
/*----------------------------------------------------------------------------*/
int stk3x1x_master_send(struct i2c_client *client, u16 addr, u8 *buf ,int count)
{
	int ret = 0, retry = 0;
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int trc = atomic_read(&obj->trace);
	int max_try = atomic_read(&obj->i2c_retry);


	while(retry++ < max_try)
	{
		ret = hwmsen_write_block(client, addr, buf, count);
		if (ret == 0)
		    break;
		udelay(100);
	}

	if(unlikely(trc))
	{
		if((retry != 1) && (trc & STK_TRC_DEBUG))
		{
			APS_LOG("(send) %d/%d\n", retry-1, max_try);
		}
	}
	/* If everything went ok (i.e. 1 msg transmitted), return #bytes
	transmitted, else error code. */
	return (ret == 0) ? count : ret;
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_led(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int ret = 0;
    
    ret = stk3x1x_master_send(client, obj->addr.ledctrl, &data, 1);
	if(ret < 0)
	{
		APS_ERR("write led = %d\n", ret);
		return -EFAULT;
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_read_als(struct i2c_client *client, u16 *data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	u8 buf[2];
	int32_t als_comperator;	
	u16 als_data;
	int32_t ir_data, tmp;
	u32 als_data_u32;

#ifdef STK_ALS_FIR
	int idx;   
	int firlen = atomic_read(&obj->firlength);   	
#endif
	if(NULL == client)
	{
		return -EINVAL;
	}	
	ret = stk3x1x_master_recv(client, obj->addr.data1_als, buf, 0x02);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}

	als_data = (buf[0] << 8) | (buf[1]);
	
	if((obj->p_wv_r_bd_with_co & 0b101) == 0b101)
	{
		tmp = als_data * obj->p_wv_r_bd_ratio / 1024;
		if(tmp > 65535)
			als_data = 65535;
		else
			als_data = tmp;
	}
	
	if(obj->p_wv_r_bd_with_co & 0b010)
	{
		if(als_data < STK_ALS_THRESHOLD && obj->als_code_last > 10000)
		{
			ir_data = stk3x1x_get_ir_value(obj, STK_ALS_READ_IRS_IT_REDUCE);
			APS_LOG("%s: als_data=%d, als_code_last=%d,ir_data=%d\n", 
					__func__, als_data, obj->als_code_last, ir_data);	
			if(ir_data > 1000)
			{
				als_data = obj->als_code_last;
			}
		}
	}
	obj->als_code_last = als_data;
	
#ifdef STK_ALS_FIR
	if(obj->fir.num < firlen)
	{                
		obj->fir.raw[obj->fir.num] = als_data;
		obj->fir.sum += als_data;
		obj->fir.num++;
		obj->fir.idx++;
	}
	else
	{
		idx = obj->fir.idx % firlen;
		obj->fir.sum -= obj->fir.raw[idx];
		obj->fir.raw[idx] = als_data;
		obj->fir.sum += als_data;
		obj->fir.idx++;
		als_data = (obj->fir.sum / firlen);
	}	
#endif
	
	
	if(obj->ir_code)
	{
		obj->als_correct_factor = 1000;
		if(als_data < STK_IRC_MAX_ALS_CODE && als_data > STK_IRC_MIN_ALS_CODE && 
			obj->ir_code > STK_IRC_MIN_IR_CODE)
		{
			als_comperator = als_data * STK_IRC_ALS_NUMERA / STK_IRC_ALS_DENOMI;
			if(obj->ir_code > als_comperator)
				obj->als_correct_factor = STK_IRC_ALS_CORREC;
		}
		APS_LOG("%s: als=%d, ir=%d, als_correct_factor=%d", __func__, als_data, obj->ir_code, obj->als_correct_factor);
		obj->ir_code = 0;
	}	
	als_data_u32 = als_data;
	als_data_u32 = als_data_u32 * obj->als_correct_factor / 1000;
	*data = (u16)als_data_u32;
	
	if(atomic_read(&obj->trace) & STK_TRC_ALS_DATA)
	{
		APS_DBG("ALS: 0x%04X\n", (u32)(*data));
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_als(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int ret = 0;
    
    ret = stk3x1x_master_send(client, obj->addr.alsctrl, &data, 1);
	if(ret < 0)
	{
		APS_ERR("write als = %d\n", ret);
		return -EFAULT;
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_read_state(struct i2c_client *client, u8 *data)
{
	//struct stk3x1x_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	u8 buf;
	
	if(NULL == client)
	{
		return -EINVAL;
	}	
	ret = stk3x1x_master_recv(client, STK_STATE_REG, &buf, 0x01);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	else
	{
		*data = buf;
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_read_flag(struct i2c_client *client, u8 *data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	u8 buf;
	
	if(NULL == client)
	{
		return -EINVAL;
	}	
	ret = stk3x1x_master_recv(client, obj->addr.flag, &buf, 0x01);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	else
	{
		*data = buf;
	}
	
	if(atomic_read(&obj->trace) & STK_TRC_ALS_DATA)
	{
		APS_DBG("PS NF flag: 0x%04X\n", (u32)(*data));
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
static int stk3x1x_read_otp25(struct i2c_client *client)
{
	// struct stk3x1x_priv *obj = i2c_get_clientdata(client);   
	int ret, otp25;
	u8 data;

	data = 0x2;
    ret = stk3x1x_master_send(client, 0x0, &data, 1);
	if (ret < 0)	
	{
		APS_ERR("write 0x0 = %d\n", ret);
		return -EFAULT;
	} 
	
	data = 0x25;
    ret = stk3x1x_master_send(client, 0x90, &data, 1);
	if (ret < 0)	
	{
		APS_ERR("write 0x90 = %d\n", ret);
		return -EFAULT;
	} 

	data = 0x82;
    ret = stk3x1x_master_send(client, 0x92, &data, 1);
	if (ret < 0)	
	{
		APS_ERR("write 0x0 = %d\n", ret);
		return -EFAULT;
	} 
	usleep_range(1000, 5000);

	ret = stk3x1x_master_recv(client, 0x91, &data, 1);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	otp25 = data;
	
	data = 0x0;
    ret = stk3x1x_master_send(client, 0x0, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write 0x0 = %d\n", ret);
		return -EFAULT;
	} 	
	
	APS_LOG("%s: otp25=0x%x\n", __func__, otp25);
	if(otp25 & 0x80)
		return 1;
	return 0;
}

/*----------------------------------------------------------------------------*/
int stk3x1x_read_id(struct i2c_client *client)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);	
	int ret = 0;
	u8 buf[2];
	u8 pid_msb;
	
	if(NULL == client)
	{
		return -EINVAL;
	}	
	obj->p_wv_r_bd_with_co = 0;
	obj->p_wv_r_bd_ratio = 1024;	
	
	ret = stk3x1x_master_recv(client, STK_PDT_ID_REG, buf, 0x02);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	obj->pid = buf[0];
	APS_LOG("%s: PID=0x%x, VID=0x%x\n", __func__, buf[0], buf[1]);
	
	if(obj->pid == STK3310SA_PID || obj->pid == STK3311SA_PID)
		obj->ledctrl_val &= 0xFF;
	
	if(buf[0] == STK3311WV_PID)
		obj->p_wv_r_bd_with_co |= 0b100;
	if(buf[1] == 0xC3)
		obj->p_wv_r_bd_with_co |= 0b010;
		
	if(stk3x1x_read_otp25(client) == 1)
	{
		obj->p_wv_r_bd_with_co |= 0b001;
		obj->p_wv_r_bd_ratio = 1024;
	}
	APS_LOG("%s: p_wv_r_bd_with_co = 0x%x\n", __func__, obj->p_wv_r_bd_with_co);	
	
	if(buf[0] == 0)
	{
		APS_ERR( "PID=0x0, please make sure the chip is stk3x1x!\n");
		return -2;			
	}		
	
	pid_msb = buf[0] & 0xF0;
	switch(pid_msb)
	{
	case 0x10:
	case 0x20:
	case 0x30:
		return 0;
	default:
		APS_ERR( "invalid PID(%#x)\n", buf[0]);	
		return -1;
	}	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_read_ps(struct i2c_client *client, u16 *data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	u8 buf[2];
	
	if(NULL == client)
	{
		APS_ERR("i2c client is NULL\n");
		return -EINVAL;
	}	
	ret = stk3x1x_master_recv(client, obj->addr.data1_ps, buf, 0x02);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	else
	{
		*data = (buf[0] << 8) | (buf[1]);
	}
	
	if(atomic_read(&obj->trace) & STK_TRC_ALS_DATA)
	{
		APS_DBG("PS: 0x%04X\n", (u32)(*data));
	}
	
	return 0;     
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_ps(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.psctrl, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write ps = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}

/*----------------------------------------------------------------------------*/
int stk3x1x_write_wait(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.wait, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write wait = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}

/*----------------------------------------------------------------------------*/
int stk3x1x_write_int(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.intmode, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write intmode = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int stk3x1x_write_state(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.state, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write state = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_flag(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.flag, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write ps = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int stk3x1x_write_sw_reset(struct i2c_client *client)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf = 0, r_buf = 0;	
	int ret = 0;

	buf = 0x7F;
    ret = stk3x1x_master_send(client, obj->addr.wait, (char*)&buf, sizeof(buf));
	if (ret < 0)
	{
		APS_ERR("addr:%x,i2c write test error = %d\n",client->addr,ret);
		return -EFAULT;
	} 	
	
    ret = stk3x1x_master_recv(client, obj->addr.wait, &r_buf, 1);
	if (ret < 0)
	{
		APS_ERR("i2c read test error = %d\n", ret);
		return -EFAULT;
	}	
	
	if(buf != r_buf)
	{
        APS_ERR("i2c r/w test error, read-back value is not the same, write=0x%x, read=0x%x\n", buf, r_buf);		
		return -EIO;
	}
	
	buf = 0;
    ret = stk3x1x_master_send(client, obj->addr.soft_reset, (char*)&buf, sizeof(buf));
	if (ret < 0)
	{
		APS_ERR("write software reset error = %d\n", ret);
		return -EFAULT;
	} 
	msleep(13);
	return 0;    
}

/*----------------------------------------------------------------------------*/
int stk3x1x_write_ps_high_thd(struct i2c_client *client, u16 thd)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & thd) >> 8);
    buf[1] = (u8) (0x00FF & thd);	

    ret = stk3x1x_master_send(client, obj->addr.thdh1_ps, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %d\n",  ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.thdh2_ps, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %d\n", ret);
		return -EFAULT;
	}	
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_ps_low_thd(struct i2c_client *client, u16 thd)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & thd) >> 8);
    buf[1] = (u8) (0x00FF & thd);	
    ret = stk3x1x_master_send(client, obj->addr.thdl1_ps, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.thdl2_ps, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_als_high_thd(struct i2c_client *client, u16 thd)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & thd) >> 8);
    buf[1] = (u8) (0x00FF & thd);	
    ret = stk3x1x_master_send(client, obj->addr.thdh1_als, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.thdh2_als, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_als_low_thd(struct i2c_client *client, u16 thd)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & thd) >> 8);
    buf[1] = (u8) (0x00FF & thd);	
    ret = stk3x1x_master_send(client, obj->addr.thdl1_als, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.thdl2_als, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
#if 0
int stk3x1x_write_foffset(struct i2c_client *client, u16 ofset)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & ofset) >> 8);
    buf[1] = (u8) (0x00FF & ofset);	
    ret = stk3x1x_master_send(client, obj->addr.data1_offset, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.data2_offset, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	
	return 0;   	
}

/*----------------------------------------------------------------------------*/

int stk3x1x_write_aoffset(struct i2c_client *client,  u16 ofset)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	u8 s_buf = 0, re_en;
    ret = stk3x1x_master_recv(client, obj->addr.state, &s_buf, 1);
	if (ret < 0)
	{
		APS_ERR("i2c read state error = %d\n", ret);
		return -EFAULT;
	}		
	re_en = (s_buf & STK_STATE_EN_AK_MASK) ? 1: 0;
	if(re_en)
	{
		s_buf &= (~STK_STATE_EN_AK_MASK); 		
		ret = stk3x1x_master_send(client, obj->addr.state, &s_buf, 1);
		if (ret < 0)
		{
			APS_ERR("write state = %d\n", ret);
			return -EFAULT;
		} 			
		msleep(3);		
	}	

    buf[0] = (u8) ((0xFF00 & ofset) >> 8);
    buf[1] = (u8) (0x00FF & ofset);	
    ret = stk3x1x_master_send(client, 0x0E, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, 0x0F, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	if(!re_en)
		return 0;
	s_buf |= STK_STATE_EN_AK_MASK; 		
	ret = stk3x1x_master_send(client, obj->addr.state, &s_buf, 1);
	if (ret < 0)
	{
		APS_ERR("write state = %d\n", ret);
		return -EFAULT;
	} 			
	return 0;  	
}
#endif
/*----------------------------------------------------------------------------*/
static void stk3x1x_power(struct alsps_hw *hw, unsigned int on) 
{
#if 0
	int ret;
	if(on == 1){
		ret = regulator_enable(stk3x1x_regulator);	/*enable regulator*/
		if (ret)
			printk("regulator_enable() failed!\n");
	}else{
		ret = regulator_disable(stk3x1x_regulator);       /*enable regulator*/
         if (ret)
            printk("regulator_disable() failed!\n");
	}
#endif
}

/*----------------------------------------------------------------------------*/
static int stk3x1x_enable_als(struct i2c_client *client, int enable)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int err, cur = 0, old = atomic_read(&obj->state_val);
	int trc = atomic_read(&obj->trace);

//	APS_LOG("%s: enable=%d\n", __func__, enable);
	
	cur = old & (~(STK_STATE_EN_ALS_MASK | STK_STATE_EN_WAIT_MASK)); 
	if(enable)
	{
		cur |= STK_STATE_EN_ALS_MASK;
	}
	else if (old & STK_STATE_EN_PS_MASK)
	{
		cur |= STK_STATE_EN_WAIT_MASK;   
	}
	if(trc & STK_TRC_DEBUG)
	{
		APS_LOG("%s: %08X, %08X, %d\n", __func__, cur, old, enable);
	}
	
	if(0 == (cur ^ old))
	{
		return 0;
	}
				
	if(enable && obj->hw->polling_mode_als == 0)
	{
		stk3x1x_write_als_high_thd(client, 0x0);
		stk3x1x_write_als_low_thd(client, 0xFFFF);
	}
	err = stk3x1x_write_state(client, cur);
	if(err < 0)
		return err;
	else
		atomic_set(&obj->state_val, cur);
	
	if(enable)
	{
		
		obj->als_last = 0;
		if(obj->hw->polling_mode_als)
		{
			atomic_set(&obj->als_deb_on, 1);
			atomic_set(&obj->als_deb_end, jiffies+atomic_read(&obj->als_debounce)*HZ/1000);
		}
		else
		{
			//set_bit(STK_BIT_ALS,  &obj->pending_intr);
			schedule_delayed_work(&obj->eint_work,220*HZ/1000); 
		}
	}

	if(trc & STK_TRC_DEBUG)
	{
		APS_LOG("enable als (%d)\n", enable);
	}

	return err;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_enable_ps(struct i2c_client *client, int enable, int validate_reg)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int err, cur = 0, old = atomic_read(&obj->state_val);
	int trc = atomic_read(&obj->trace);
//	hwm_sensor_data sensor_data;

//	APS_LOG("%s: enable=%d\n", __FUNCTION__, enable);	

	if(obj->first_boot == true)
	{		
		obj->first_boot = false;
	}


	cur = old;		
	cur &= (~(0x45)); 
	if(enable)
	{
		cur |= (STK_STATE_EN_PS_MASK);
		if(!(old & STK_STATE_EN_ALS_MASK))
			cur |= STK_STATE_EN_WAIT_MASK;
		if(1 == obj->hw->polling_mode_ps)
		{
		//	APS_LOG("%s: line=%d\n", __FUNCTION__, __LINE__);
		//	wake_lock(&ps_lock);
		}
	}
	else
	{
		if(1 == obj->hw->polling_mode_ps)
		{		
		//	APS_LOG("%s: line=%d\n", __FUNCTION__, __LINE__);
		//	wake_unlock(&ps_lock);
		}
	}
	
	if(trc & STK_TRC_DEBUG)
	{
		APS_LOG("%s: %08X, %08X, %d\n", __func__, cur, old, enable);
	}
	
	if(0 == (cur ^ old))
	{
		return 0;
	}
//	APS_LOG("%s: line=%d\n", __FUNCTION__, __LINE__);
	err = stk3x1x_write_state(client, cur);
	if(err < 0)
		return err;
	else
		atomic_set(&obj->state_val, cur);
	
	return 0;
}

/*----------------------------------------------------------------------------*/
static int stk3x1x_check_intr(struct i2c_client *client, u8 *status) 
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int err;

	//if (mt_get_gpio_in(GPIO_ALS_EINT_PIN) == 1) /*skip if no interrupt*/  
	//    return 0;

	err = stk3x1x_read_flag(client, status);	
	if (err < 0)
	{
		APS_ERR("WARNING: read flag reg error: %d\n", err);
		return -EFAULT;
	}
	APS_LOG("%s: read status reg: 0x%x\n", __func__, *status);
    
	if(*status & STK_FLG_ALSINT_MASK)
	{
		set_bit(STK_BIT_ALS, &obj->pending_intr);
	}
	else
	{
	   clear_bit(STK_BIT_ALS, &obj->pending_intr);
	}
	
	if(*status & STK_FLG_PSINT_MASK)
	{
		set_bit(STK_BIT_PS,  &obj->pending_intr);
	}
	else
	{
	    clear_bit(STK_BIT_PS, &obj->pending_intr);
	}
	
	if(atomic_read(&obj->trace) & STK_TRC_DEBUG)
	{
		APS_LOG("check intr: 0x%02X => 0x%08lX\n", *status, obj->pending_intr);
	}

	return 0;
}


static int stk3x1x_clear_intr(struct i2c_client *client, u8 status, u8 disable_flag) 
{
    int err = 0;

    status = status | (STK_FLG_ALSINT_MASK | STK_FLG_PSINT_MASK | STK_FLG_OUI_MASK | STK_FLG_IR_RDY_MASK);
    status &= (~disable_flag);
	//APS_LOG(" set flag reg: 0x%x\n", status);
	if((err = stk3x1x_write_flag(client, status)))
		APS_ERR("stk3x1x_write_flag failed, err=%d\n", err);
    return err;
}

/*----------------------------------------------------------------------------*/
void stk3x1x_eint_func(void)
{
	struct stk3x1x_priv *obj = stk3x1x_obj;
	APS_LOG(" interrupt fuc\n");
	if(!obj)
	{
		return;
	}
	//schedule_work(&obj->eint_work);
	if(obj->hw->polling_mode_ps == 0 || obj->hw->polling_mode_als == 0)
		schedule_delayed_work(&obj->eint_work,0);
	if(atomic_read(&obj->trace) & STK_TRC_EINT)
	{
		APS_LOG("eint: als/ps intrs\n");
	}
}
/*----------------------------------------------------------------------------*/
static void stk3x1x_eint_work(struct work_struct *work)
{
	struct stk3x1x_priv *obj = stk3x1x_obj;
	int err;
	int report_value = 1;
	//hwm_sensor_data sensor_data;
	u8 flag_reg;
	u8 disable_flag=0;
	//memset(&sensor_data, 0, sizeof(sensor_data));

	APS_LOG(" eint work\n");
	
	if((err = stk3x1x_check_intr(obj->client, &flag_reg)))
	{
		APS_ERR("stk3x1x_check_intr fail: %d\n", err);
		goto err_i2c_rw;
	}

    APS_LOG(" &obj->pending_intr =%lx\n",obj->pending_intr);
	
#ifdef HQ_PSENSOR_CALIBRATION
	APS_DBG("zqq int:init=%d,noise=%d,low=%d,high=%d\n",obj->init_noise,obj->current_xtalk,\
		atomic_read(&obj->ps_low_thd_val),atomic_read(&obj->ps_high_thd_val));
#endif
#if 1	
	if(((1<<STK_BIT_PS) &  obj->pending_intr) && (obj->hw->polling_mode_ps == 0))
	{
		APS_LOG("stk ps change\n");
		disable_flag |= STK_FLG_PSINT_MASK;
		
		if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
		{
			APS_ERR("stk3x1x read ps data: %d\n", err);
			goto err_i2c_rw;
		}
		
		report_value = (flag_reg & STK_FLG_NF_MASK)? 1 : 0;

		APS_LOG("%s:ps raw 0x%x -> value 0x%x \n",__FUNCTION__, obj->ps, report_value);
		//let up layer to know
/*
		if((err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
		{	
			APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
		}
*/
		if( 0 == stk3x1x_init_flag){
			if(ps_report_interrupt_data(report_value))
			{	
				APS_ERR("call ps_report_interrupt_data fail\n");
			} 
		}
	}
	if(disable_flag)
	{
		if((err = stk3x1x_clear_intr(obj->client, flag_reg, disable_flag)))
		{
			APS_ERR("fail: %d\n", err);
			goto err_i2c_rw;
		}		
	}
		
	msleep(1);

#if defined(CONFIG_OF)
	enable_irq(obj->irq);
#endif
#endif

	return;

err_i2c_rw:	
	msleep(30);
#if defined(CONFIG_OF)
	enable_irq(obj->irq);
#endif
	return;
}


#if defined(CONFIG_OF)
static irqreturn_t stk3x1x_eint_handler(int irq, void *desc)
{
	//printk("----------------stk3x1x_eint_handler-----------------------\n");
	disable_irq_nosync(stk3x1x_obj->irq);
	stk3x1x_eint_func();

	return IRQ_HANDLED;
}
#endif


/*----------------------------------------------------------------------------*/
int stk3x1x_setup_eint(struct i2c_client *client)
{
	int ret;
	struct pinctrl *pinctrl;
	struct pinctrl_state *pins_default;
	struct pinctrl_state *pins_cfg;

	u32 ints[2] = {0, 0};

	alspsPltFmDev = get_alsps_platformdev();
/* gpio setting */
	pinctrl = devm_pinctrl_get(&alspsPltFmDev->dev);
	if (IS_ERR(pinctrl)) {
		ret = PTR_ERR(pinctrl);
		APS_ERR("Cannot find alsps pinctrl!\n");
	}
	pins_default = pinctrl_lookup_state(pinctrl, "pin_default");
	if (IS_ERR(pins_default)) {
		ret = PTR_ERR(pins_default);
		APS_ERR("Cannot find alsps pinctrl default!\n");

	}

	pins_cfg = pinctrl_lookup_state(pinctrl, "pin_cfg");
	if (IS_ERR(pins_cfg)) {
		ret = PTR_ERR(pins_cfg);
		APS_ERR("Cannot find alsps pinctrl pin_cfg!\n");
	 }
/* eint request */
	if (stk3x1x_obj->irq_node) {
		//node = of_find_matching_node(node, touch_of_match);

		of_property_read_u32_array(stk3x1x_obj->irq_node, "debounce", ints, ARRAY_SIZE(ints));
		gpio_request(ints[0], "p-sensor");
		gpio_set_debounce(ints[0], ints[1]);
		pinctrl_select_state(pinctrl, pins_cfg);
		APS_LOG("ints[0] = %d, ints[1] = %d!!\n", ints[0], ints[1]);

		stk3x1x_obj->irq = irq_of_parse_and_map(stk3x1x_obj->irq_node, 0);
		APS_LOG("stk3x1x_obj->irq = %d\n", stk3x1x_obj->irq);
		if (!stk3x1x_obj->irq) {
			APS_ERR("irq_of_parse_and_map fail!!\n");
			return -EINVAL;
		}
		if (request_irq(stk3x1x_obj->irq, stk3x1x_eint_handler,IRQF_TRIGGER_FALLING, "stk3-eint", NULL)) {
			APS_ERR("IRQ LINE NOT AVAILABLE!!\n");
			return -EINVAL;
		}
		enable_irq(stk3x1x_obj->irq);
	} else {
		APS_ERR("null irq node!!\n");
		return -EINVAL;
	}
	
    return 0;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_init_client(struct i2c_client *client)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int err;
	int ps_ctrl;
	//u8 int_status;
	
	if((err = stk3x1x_write_sw_reset(client)))
	{
		APS_ERR("software reset error, err=%d", err);
		return err;
	}

	if((err = stk3x1x_read_id(client)))
	{
		APS_ERR("stk3x1x_read_id error, err=%d", err);
		return err;
	}		
	
	if(obj->first_boot == true)
	{	
		if(obj->hw->polling_mode_ps == 0 || obj->hw->polling_mode_als == 0)
		{
			if((err = stk3x1x_setup_eint(client)))
			{
				APS_ERR("setup eint error: %d\n", err);
				//return err;
			}
		}
	}
	if((err = stk3x1x_write_state(client, atomic_read(&obj->state_val))))
	{
		APS_ERR("write stete error: %d\n", err);
		return err;        
	}	
	
/*	
	if((err = stk3x1x_check_intr(client, &int_status)))
	{
		APS_ERR("check intr error: %d\n", err);
		return err;
	}
	
	if((err = stk3x1x_clear_intr(client, int_status, STK_FLG_PSINT_MASK | STK_FLG_ALSINT_MASK)))
	{
		APS_ERR("clear intr error: %d\n", err);	
		return err;
	}
*/	
	ps_ctrl = atomic_read(&obj->psctrl_val);
	if(obj->hw->polling_mode_ps == 1)
		ps_ctrl &= 0x3F;
	
	if((err = stk3x1x_write_ps(client, ps_ctrl)))
	{
		APS_ERR("write ps error: %d\n", err);
		return err;        
	}
	
	if((err = stk3x1x_write_als(client, atomic_read(&obj->alsctrl_val))))
	{
		APS_ERR("write als error: %d\n", err);
		return err;
	}	
	
	if((err = stk3x1x_write_led(client, obj->ledctrl_val)))
	{
		APS_ERR("write led error: %d\n", err);
		return err;
	}	
	
	if((err = stk3x1x_write_wait(client, obj->wait_val)))
	{
		APS_ERR("write wait error: %d\n", err);
		return err;
	}	
	if((err = stk3x1x_write_ps_high_thd(client, atomic_read(&obj->ps_high_thd_val))))
	{
		APS_ERR("write high thd error: %d\n", err);
		return err;        
	}
	
	if((err = stk3x1x_write_ps_low_thd(client, atomic_read(&obj->ps_low_thd_val))))
	{
		APS_ERR("write low thd error: %d\n", err);
		return err;        
	}
	if((err = stk3x1x_write_int(client, obj->int_val)))
	{
		APS_ERR("write int mode error: %d\n", err);
		return err;        
	}		
	
	/*
	u8 data;
	data = 0x60;
    err = stk3x1x_master_send(client, 0x87, &data, 1);
	if (err < 0)
	{
		APS_ERR("write 0x87 = %d\n", err);
		return -EFAULT;
	} 
	*/
#ifdef STK_ALS_FIR
	memset(&obj->fir, 0x00, sizeof(obj->fir));  
#endif
	obj->re_enable_ps = false;
	obj->re_enable_als = false;
	obj->als_code_last = 500;
	
	return 0;
}

/******************************************************************************
 * Sysfs attributes
*******************************************************************************/
static ssize_t stk3x1x_show_config(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	res = scnprintf(buf, PAGE_SIZE, "(%d %d %d %d %d %d)\n", 
		atomic_read(&stk3x1x_obj->i2c_retry), atomic_read(&stk3x1x_obj->als_debounce), 
		atomic_read(&stk3x1x_obj->ps_mask), atomic_read(&stk3x1x_obj->ps_high_thd_val),atomic_read(&stk3x1x_obj->ps_low_thd_val), atomic_read(&stk3x1x_obj->ps_debounce));     
	return res;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_config(struct device_driver *ddri, const char *buf, size_t count)
{
	int retry, als_deb, ps_deb, mask, hthres, lthres, err;
	struct i2c_client *client;
	client = stk3x1x_i2c_client;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if(6 == sscanf(buf, "%d %d %d %d %d %d", &retry, &als_deb, &mask, &hthres, &lthres, &ps_deb))
	{ 
		atomic_set(&stk3x1x_obj->i2c_retry, retry);
		atomic_set(&stk3x1x_obj->als_debounce, als_deb);
		atomic_set(&stk3x1x_obj->ps_mask, mask);
		atomic_set(&stk3x1x_obj->ps_high_thd_val, hthres);    
		atomic_set(&stk3x1x_obj->ps_low_thd_val, lthres);        
		atomic_set(&stk3x1x_obj->ps_debounce, ps_deb);

		if((err = stk3x1x_write_ps_high_thd(client, atomic_read(&stk3x1x_obj->ps_high_thd_val))))
		{
			APS_ERR("write high thd error: %d\n", err);
			return err;        
		}
		
		if((err = stk3x1x_write_ps_low_thd(client, atomic_read(&stk3x1x_obj->ps_low_thd_val))))
		{
			APS_ERR("write low thd error: %d\n", err);
			return err;        
		}
	}
	else
	{
		APS_ERR("invalid content: '%s'\n", buf);
	}
	return count;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_trace(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}

	res = scnprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&stk3x1x_obj->trace));     
	return res;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_trace(struct device_driver *ddri, const char *buf, size_t count)
{
    int trace;
    if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if(1 == sscanf(buf, "0x%x", &trace))
	{
		atomic_set(&stk3x1x_obj->trace, trace);
	}
	else 
	{
		APS_ERR("invalid content: '%s', length = %d\n", buf, (int)count);
	}
	return count;    
}

/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_ir(struct device_driver *ddri, char *buf)
{
    int32_t reading;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
    reading = stk3x1x_get_ir_value(stk3x1x_obj, STK_IRS_IT_REDUCE);
	if(reading < 0)
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", reading);

	stk3x1x_obj->ir_code = reading;
	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", stk3x1x_obj->ir_code);     
}

/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_als(struct device_driver *ddri, char *buf)
{
	int res;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if(stk3x1x_obj->hw->polling_mode_als == 0)
	{
		if((res = stk3x1x_read_als(stk3x1x_obj->client, &stk3x1x_obj->als)))
			return sprintf(buf,"ERROR: %d\n", res);
		else
			return sprintf(buf,"%d\n", stk3x1x_obj->als);     
	}
	return sprintf(buf,"0x%04X\n", stk3x1x_obj->als_code_last);     
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_ps(struct device_driver *ddri, char *buf)
{
	int res;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if((res = stk3x1x_read_ps(stk3x1x_obj->client, &stk3x1x_obj->ps)))
	{
		return sprintf(buf,"ERROR: %d\n", res);
	}
	else
	{
		if (stk3x1x_obj->ps > MAX_PS_NOISE)
			return sprintf(buf,"device_name:%s @FAIL@ psensor_noise = %d\n", stk3x1x_DEV_NAME, stk3x1x_obj->ps);
		else
			return sprintf(buf,"device_name:%s @PASS@ psensor_noise = %d\n", stk3x1x_DEV_NAME, stk3x1x_obj->ps);
	}
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_reg(struct device_driver *ddri, char *buf)
{
	u8 int_status;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	/*read*/
	stk3x1x_check_intr(stk3x1x_obj->client, &int_status);
	//stk3x1x_clear_intr(stk3x1x_obj->client, int_status, 0x0);
	stk3x1x_read_ps(stk3x1x_obj->client, &stk3x1x_obj->ps);
	stk3x1x_read_als(stk3x1x_obj->client, &stk3x1x_obj->als);
	/*write*/
	stk3x1x_write_als(stk3x1x_obj->client, atomic_read(&stk3x1x_obj->alsctrl_val));
	stk3x1x_write_ps(stk3x1x_obj->client, atomic_read(&stk3x1x_obj->psctrl_val)); 
	stk3x1x_write_ps_high_thd(stk3x1x_obj->client, atomic_read(&stk3x1x_obj->ps_high_thd_val));
	stk3x1x_write_ps_low_thd(stk3x1x_obj->client, atomic_read(&stk3x1x_obj->ps_low_thd_val));

	return 0;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_send(struct device_driver *ddri, char *buf)
{
    return 0;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_send(struct device_driver *ddri, const char *buf, size_t count)
{
	int addr, cmd;
	u8 dat;

	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(2 != sscanf(buf, "%x %x", &addr, &cmd))
	{
		APS_ERR("invalid format: '%s'\n", buf);
		return 0;
	}

	dat = (u8)cmd;
	APS_LOG("send(%02X, %02X) = %d\n", addr, cmd, 
	stk3x1x_master_send(stk3x1x_obj->client, (u16)addr, &dat, sizeof(dat)));
	
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_recv(struct device_driver *ddri, char *buf)
{
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&stk3x1x_obj->recv_reg));     	
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_recv(struct device_driver *ddri, const char *buf, size_t count)
{
	int addr;
	u8 dat;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(1 != sscanf(buf, "%x", &addr))
	{
		APS_ERR("invalid format: '%s'\n", buf);
		return 0;
	}

	APS_LOG("recv(%02X) = %d, 0x%02X\n", addr, 
	stk3x1x_master_recv(stk3x1x_obj->client, (u16)addr, (char*)&dat, sizeof(dat)), dat);
	atomic_set(&stk3x1x_obj->recv_reg, dat);	
	return count;
}
/*----------------------------------------------------------------------------*/
#ifdef STK_PS_DEBUG
static int show_allreg(void)
{
	int ret = 0;
	u8 rbuf[0x22];
	int cnt;	
	int len = 0;
	
	memset(rbuf, 0, sizeof(rbuf));
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 0, &rbuf[0], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 7, &rbuf[7], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 14, &rbuf[14], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 21, &rbuf[21], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 28, &rbuf[28], 4);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, STK_PDT_ID_REG, &rbuf[32], 2);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	
	for(cnt=0;cnt<0x20;cnt++)
	{
		APS_LOG("reg[0x%x]=0x%x\n", cnt, rbuf[cnt]);
	}	
	APS_LOG("reg[0x3E]=0x%x\n", rbuf[cnt]);
	APS_LOG("reg[0x3F]=0x%x\n", rbuf[cnt++]);

	return 0;
}
#endif
static ssize_t stk3x1x_show_allreg(struct device_driver *ddri, char *buf)
{
	int ret = 0;
	u8 rbuf[0x22];
	int cnt;	
	int len = 0;
	
	memset(rbuf, 0, sizeof(rbuf));
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 0, &rbuf[0], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 7, &rbuf[7], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 14, &rbuf[14], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 21, &rbuf[21], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 28, &rbuf[28], 4);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, STK_PDT_ID_REG, &rbuf[32], 2);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	
	for(cnt=0;cnt<0x20;cnt++)
	{
		APS_LOG("reg[0x%x]=0x%x\n", cnt, rbuf[cnt]);
		len += scnprintf(buf+len, PAGE_SIZE-len, "[%2X]%2X,", cnt, rbuf[cnt]);
	}	
	APS_LOG("reg[0x3E]=0x%x\n", rbuf[cnt]);
	APS_LOG("reg[0x3F]=0x%x\n", rbuf[cnt++]);
	len += scnprintf(buf+len, PAGE_SIZE-len, "[0x3E]%2X,[0x3F]%2X\n", rbuf[cnt-1], rbuf[cnt]);	
	return len;
	/*
    return scnprintf(buf, PAGE_SIZE, "[0]%2X [1]%2X [2]%2X [3]%2X [4]%2X [5]%2X [6/7 HTHD]%2X,%2X [8/9 LTHD]%2X, %2X [A]%2X [B]%2X [C]%2X [D]%2X [E/F Aoff]%2X,%2X,[10]%2X [11/12 PS]%2X,%2X [13]%2X [14]%2X [15/16 Foff]%2X,%2X [17]%2X [18]%2X [3E]%2X [3F]%2X\n", 	
		rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5], rbuf[6], rbuf[7], rbuf[8], 
		rbuf[9], rbuf[10], rbuf[11], rbuf[12], rbuf[13], rbuf[14], rbuf[15], rbuf[16], rbuf[17], 
		rbuf[18], rbuf[19], rbuf[20], rbuf[21], rbuf[22], rbuf[23], rbuf[24], rbuf[25], rbuf[26]);	
	*/
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_status(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	u8 rbuf[25];
	int ret = 0;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if(stk3x1x_obj->hw)
	{
		len += scnprintf(buf+len, PAGE_SIZE-len, "CUST: %d, (%d %d) (%02X) (%02X %02X %02X) (%02X %02X %02X %02X)\n", 
			stk3x1x_obj->hw->i2c_num, stk3x1x_obj->hw->power_id, stk3x1x_obj->hw->power_vol, stk3x1x_obj->addr.flag, 
			stk3x1x_obj->addr.alsctrl, stk3x1x_obj->addr.data1_als, stk3x1x_obj->addr.data2_als, stk3x1x_obj->addr.psctrl, 
			stk3x1x_obj->addr.data1_ps, stk3x1x_obj->addr.data2_ps, stk3x1x_obj->addr.thdh1_ps);
	}
	else
	{
		len += scnprintf(buf+len, PAGE_SIZE-len, "CUST: NULL\n");
	}
	
	len += scnprintf(buf+len, PAGE_SIZE-len, "REGS: %02X %02X %02X %02X %02X %02X %d %d %02lX %02lX\n", 
				atomic_read(&stk3x1x_obj->state_val), atomic_read(&stk3x1x_obj->psctrl_val), atomic_read(&stk3x1x_obj->alsctrl_val), 
				stk3x1x_obj->ledctrl_val, stk3x1x_obj->int_val, stk3x1x_obj->wait_val, 
				atomic_read(&stk3x1x_obj->ps_high_thd_val), atomic_read(&stk3x1x_obj->ps_low_thd_val),stk3x1x_obj->enable, stk3x1x_obj->pending_intr);

	len += scnprintf(buf+len, PAGE_SIZE-len, "MISC: %d %d\n", atomic_read(&stk3x1x_obj->als_suspend), atomic_read(&stk3x1x_obj->ps_suspend));	
	len += scnprintf(buf+len, PAGE_SIZE-len, "VER.: %s\n", DRIVER_VERSION);
	
	memset(rbuf, 0, sizeof(rbuf));	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 0, &rbuf[0], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 7, &rbuf[7], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 14, &rbuf[14], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}		
	/*
	ret = stk3x1x_master_recv(stk3x1x_obj->client, 21, &rbuf[21], 4);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	*/	
    len += scnprintf(buf+len, PAGE_SIZE-len, "[PS=%2X] [ALS=%2X] [WAIT=%4Xms] [EN_ASO=%2X] [EN_AK=%2X] [NEAR/FAR=%2X] [FLAG_OUI=%2X] [FLAG_PSINT=%2X] [FLAG_ALSINT=%2X]\n", 
		rbuf[0]&0x01,(rbuf[0]&0x02)>>1,((rbuf[0]&0x04)>>2)*rbuf[5]*6,(rbuf[0]&0x20)>>5,
		(rbuf[0]&0x40)>>6,rbuf[16]&0x01,(rbuf[16]&0x04)>>2,(rbuf[16]&0x10)>>4,(rbuf[16]&0x20)>>5);		
	
	return len;
}
/*----------------------------------------------------------------------------*/
#define IS_SPACE(CH) (((CH) == ' ') || ((CH) == '\n'))
/*----------------------------------------------------------------------------*/
static int read_int_from_buf(struct stk3x1x_priv *obj, const char* buf, size_t count,
                             u32 data[], int len)
{
	int idx = 0;
	char *cur = (char*)buf, *end = (char*)(buf+count);

	while(idx < len)
	{
		while((cur < end) && IS_SPACE(*cur))
		{
			cur++;        
		}

		if(1 != sscanf(cur, "%d", &data[idx]))
		{
			break;
		}

		idx++; 
		while((cur < end) && !IS_SPACE(*cur))
		{
			cur++;
		}
	}
	return idx;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_alslv(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	int idx;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	for(idx = 0; idx < stk3x1x_obj->als_level_num; idx++)
	{
		len += scnprintf(buf+len, PAGE_SIZE-len, "%d ", stk3x1x_obj->hw->als_level[idx]);
	}
	len += scnprintf(buf+len, PAGE_SIZE-len, "\n");
	return len;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_alslv(struct device_driver *ddri, const char *buf, size_t count)
{
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(!strcmp(buf, "def"))
	{
		memcpy(stk3x1x_obj->als_level, stk3x1x_obj->hw->als_level, sizeof(stk3x1x_obj->als_level));
	}
	else if(stk3x1x_obj->als_level_num != read_int_from_buf(stk3x1x_obj, buf, count, 
			stk3x1x_obj->hw->als_level, stk3x1x_obj->als_level_num))
	{
		APS_ERR("invalid format: '%s'\n", buf);
	}    
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_alsval(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	int idx;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	for(idx = 0; idx < stk3x1x_obj->als_value_num; idx++)
	{
		len += scnprintf(buf+len, PAGE_SIZE-len, "%d ", stk3x1x_obj->hw->als_value[idx]);
	}
	len += scnprintf(buf+len, PAGE_SIZE-len, "\n");
	return len;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_alsval(struct device_driver *ddri, const char *buf, size_t count)
{
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(!strcmp(buf, "def"))
	{
		memcpy(stk3x1x_obj->als_value, stk3x1x_obj->hw->als_value, sizeof(stk3x1x_obj->als_value));
	}
	else if(stk3x1x_obj->als_value_num != read_int_from_buf(stk3x1x_obj, buf, count, 
			stk3x1x_obj->hw->als_value, stk3x1x_obj->als_value_num))
	{
		APS_ERR("invalid format: '%s'\n", buf);
	}    
	return count;
}

#ifdef STK_ALS_FIR
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_firlen(struct device_driver *ddri, char *buf)
{
	int len = atomic_read(&stk3x1x_obj->firlength);
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
		
	APS_LOG("%s: len = %2d, idx = %2d\n", __func__, len, stk3x1x_obj->fir.idx);			
	APS_LOG("%s: sum = %5d, ave = %5d\n", __func__, stk3x1x_obj->fir.sum, stk3x1x_obj->fir.sum/len);
	
	return scnprintf(buf, PAGE_SIZE, "%d\n", len);		
}

/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_firlen(struct device_driver *ddri, const char *buf, size_t count)
{
	int value;

	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(1 != sscanf(buf, "%d", &value))
	{
		APS_ERR("invalid format: '%s'\n", buf);
		return 0;
	}
		
	if(value > MAX_FIR_LEN)
	{
		APS_ERR("%s: firlen exceed maximum filter length\n", __func__);
	}
	else if (value < 1)
	{
		atomic_set(&stk3x1x_obj->firlength, 1);
		memset(&stk3x1x_obj->fir, 0x00, sizeof(stk3x1x_obj->fir));
	}
	else
	{ 
		atomic_set(&stk3x1x_obj->firlength, value);
		memset(&stk3x1x_obj->fir, 0x00, sizeof(stk3x1x_obj->fir));
	}
	
	return count;
}
#endif /* #ifdef STK_ALS_FIR */
/*----------------------------------------------------------------------------*/
static DRIVER_ATTR(als,     0444, stk3x1x_show_als,   NULL);
static DRIVER_ATTR(Huaqin_show_val,0444, stk3x1x_show_ps,    NULL);
static DRIVER_ATTR(ir,      0444, stk3x1x_show_ir,    NULL);
static DRIVER_ATTR(config,  0664, stk3x1x_show_config,stk3x1x_store_config);
static DRIVER_ATTR(alslv,   0664, stk3x1x_show_alslv, stk3x1x_store_alslv);
static DRIVER_ATTR(alsval,  0664, stk3x1x_show_alsval,stk3x1x_store_alsval);
static DRIVER_ATTR(trace,   0664, stk3x1x_show_trace, stk3x1x_store_trace);
static DRIVER_ATTR(status,  0444, stk3x1x_show_status,  NULL);
static DRIVER_ATTR(send,    0664, stk3x1x_show_send,  stk3x1x_store_send);
static DRIVER_ATTR(recv,    0664, stk3x1x_show_recv,  stk3x1x_store_recv);
static DRIVER_ATTR(reg,     0444, stk3x1x_show_reg,   NULL);
static DRIVER_ATTR(allreg,  0444, stk3x1x_show_allreg,   NULL);
#ifdef STK_ALS_FIR
static DRIVER_ATTR(firlen,    0664, stk3x1x_show_firlen,  stk3x1x_store_firlen);
#endif
/*----------------------------------------------------------------------------*/
static struct driver_attribute *stk3x1x_attr_list[] = {
    &driver_attr_als,
    &driver_attr_Huaqin_show_val,    
    &driver_attr_ir,    
    &driver_attr_trace,        /*trace log*/
    &driver_attr_config,
    &driver_attr_alslv,
    &driver_attr_alsval,
    &driver_attr_status,
    &driver_attr_send,
    &driver_attr_recv,
    &driver_attr_allreg,
//    &driver_attr_i2c,
    &driver_attr_reg,
#ifdef STK_ALS_FIR
    &driver_attr_firlen,
#endif	
};

/*----------------------------------------------------------------------------*/
static int stk3x1x_create_attr(struct device_driver *driver) 
{
	int idx, err = 0;
	int num = (int)(sizeof(stk3x1x_attr_list)/sizeof(stk3x1x_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		if((err = driver_create_file(driver, stk3x1x_attr_list[idx])))
		{            
			APS_ERR("driver_create_file (%s) = %d\n", stk3x1x_attr_list[idx]->attr.name, err);
			break;
		}
	}    
	return err;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_delete_attr(struct device_driver *driver)
{
	int idx ,err = 0;
	int num = (int)(sizeof(stk3x1x_attr_list)/sizeof(stk3x1x_attr_list[0]));

	if (!driver)
	return -EINVAL;

	for (idx = 0; idx < num; idx++) 
	{
		driver_remove_file(driver, stk3x1x_attr_list[idx]);
	}
	
	return err;
}


/****************************************************************************** 
 * Function Configuration
******************************************************************************/

static int stk3x1x_get_als_value(struct stk3x1x_priv *obj, u16 als)
{
	int idx;
	int invalid = 0;
	for(idx = 0; idx < obj->als_level_num; idx++)
	{
		if(als < obj->hw->als_level[idx])
		{
			break;
		}
	}
	
	if(idx >= obj->als_value_num)
	{
		APS_ERR("exceed range\n"); 
		idx = obj->als_value_num - 1;
	}
	
	if(1 == atomic_read(&obj->als_deb_on))
	{
		unsigned long endt = atomic_read(&obj->als_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->als_deb_on, 0);
		}
		
		if(1 == atomic_read(&obj->als_deb_on))
		{
			invalid = 1;
		}
	}

	if(!invalid)
	{
		if (atomic_read(&obj->trace) & STK_TRC_CVT_ALS)
		{
			APS_DBG("ALS: %05d => %05d\n", als, obj->hw->als_value[idx]);
		}
		
		return obj->hw->als_value[idx];
	}
	else
	{
		if(atomic_read(&obj->trace) & STK_TRC_CVT_ALS)
		{
			APS_DBG("ALS: %05d => %05d (-1)\n", als, obj->hw->als_value[idx]);    
		}
		return -1;
	}
}

/*----------------------------------------------------------------------------*/
static int stk3x1x_get_ps_value(struct stk3x1x_priv *obj, u16 ps)
{
	int mask = atomic_read(&obj->ps_mask);
	int invalid = 0, val;
	int err;
	u8 flag;

	err = stk3x1x_read_flag(obj->client, &flag);
	if(err)
		return err;
	
	val = (flag & STK_FLG_NF_MASK)? 1 : 0;	
	
	APS_LOG("%s: read_flag = 0x%x, val = %d\n", __FUNCTION__, flag, val);
	
/*	if((err = stk3x1x_clear_intr(obj->client, flag, STK_FLG_OUI_MASK)))
	{
		APS_ERR("fail: %d\n", err);
		return err;
	}	*/

	if(atomic_read(&obj->ps_suspend))
	{
		invalid = 1;
	}
	else if(1 == atomic_read(&obj->ps_deb_on))
	{
		unsigned long endt = atomic_read(&obj->ps_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->ps_deb_on, 0);
		}
		
		if (1 == atomic_read(&obj->ps_deb_on))
		{
			invalid = 1;
		}
	}
		
	
	if(!invalid)
	{
		if(unlikely(atomic_read(&obj->trace) & STK_TRC_CVT_PS))
		{
			if(mask)
			{
				APS_DBG("PS:  %05d => %05d [M] \n", ps, val);
			}
			else
			{
				APS_DBG("PS:  %05d => %05d\n", ps, val);
			}
		}
		return val;
		
	}	
	else
	{
		APS_ERR(" ps value is invalid, PS:  %05d => %05d\n", ps, val);
		if(unlikely(atomic_read(&obj->trace) & STK_TRC_CVT_PS))
		{
			APS_DBG("PS:  %05d => %05d (-1)\n", ps, val);    
		}
		return -1;
	}	
}

/*----------------------------------------------------------------------------*/

static int32_t stk3x1x_set_irs_it_slp(struct stk3x1x_priv *obj, uint16_t *slp_time, int32_t ials_it_reduce)
{
	uint8_t irs_alsctrl;
	int32_t ret;
		
	irs_alsctrl = (atomic_read(&obj->alsctrl_val) & 0x0F) - ials_it_reduce;		
	switch(irs_alsctrl)
	{
	case 2:
		*slp_time = 1;
		break;			
	case 3:
		*slp_time = 2;
		break;	
	case 4:
		*slp_time = 3;
		break;	
	case 5:
		*slp_time = 6;
		break;	
	case 6:
		*slp_time = 12;
		break;
	case 7:
		*slp_time = 24;			
		break;
	case 8:
		*slp_time = 48;			
		break;
	case 9:
		*slp_time = 96;			
		break;				
	default:
		APS_ERR( "%s: unknown ALS IT=0x%x\n", __func__, irs_alsctrl);
		ret = -EINVAL;	
		return ret;
	}
	irs_alsctrl |= (atomic_read(&obj->alsctrl_val) & 0xF0);
	ret = i2c_smbus_write_byte_data(obj->client, STK_ALSCTRL_REG, irs_alsctrl);
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __func__);
		return ret;		
	}		
	return 0;
}

static int32_t stk3x1x_get_ir_value(struct stk3x1x_priv *obj, int32_t als_it_reduce)
{
    int32_t word_data, ret;
	uint8_t w_reg, retry = 0;	
	uint16_t irs_slp_time = 100;
	//bool re_enable_ps = false;
	u8 flag;
	u8 buf[2];
	
/*	re_enable_ps = (atomic_read(&obj->state_val) & STK_STATE_EN_PS_MASK) ? true : false;	
	if(re_enable_ps)
	{
		stk3x1x_enable_ps(obj->client, 0, 1);
	}
*/	
	ret = stk3x1x_set_irs_it_slp(obj, &irs_slp_time, als_it_reduce);
	if(ret < 0)
		goto irs_err_i2c_rw;
		
	w_reg = atomic_read(&obj->state_val) | STK_STATE_EN_IRS_MASK;		
    ret = i2c_smbus_write_byte_data(obj->client, STK_STATE_REG, w_reg);
    if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __func__);
		goto irs_err_i2c_rw;
	}	
	msleep(irs_slp_time);	
	
	do
	{
		msleep(3);		
		ret = stk3x1x_read_flag(obj->client, &flag);	
		if (ret < 0)
		{
			APS_ERR("WARNING: read flag reg error: %d\n", ret);
			goto irs_err_i2c_rw;
		}	
		retry++;
	}while(retry < 10 && ((flag&STK_FLG_IR_RDY_MASK) == 0));
	
	if(retry == 10)
	{
		APS_ERR( "%s: ir data is not ready for a long time\n", __func__);
		ret = -EINVAL;
		goto irs_err_i2c_rw;
	}

	ret = stk3x1x_clear_intr(obj->client, flag, STK_FLG_IR_RDY_MASK);	
    if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __func__);
		goto irs_err_i2c_rw;
	}		
	
	ret = stk3x1x_master_recv(obj->client, STK_DATA1_IR_REG, buf, 2);
	if(ret < 0)	
	{
		APS_ERR( "%s fail, ret=0x%x", __func__, ret); 
		goto irs_err_i2c_rw;		
	}
	word_data =  (buf[0] << 8) | buf[1];

	ret = i2c_smbus_write_byte_data(obj->client, STK_ALSCTRL_REG, atomic_read(&obj->alsctrl_val));
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __func__);
		goto irs_err_i2c_rw;
	}
	//if(re_enable_ps)
		//stk3x1x_enable_ps(obj->client, 1, 0);		
	return word_data;

irs_err_i2c_rw:	
	//if(re_enable_ps)
	//	stk3x1x_enable_ps(obj->client, 1, 0);	
	return ret;
}

/****************************************************************************** 
 * Function Configuration
******************************************************************************/
static int stk3x1x_open(struct inode *inode, struct file *file)
{
	file->private_data = stk3x1x_i2c_client;

	if (!file->private_data)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}
	
	return nonseekable_open(inode, file);
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*----------------------------------------------------------------------------*/
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
static long stk3x1x_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#else
static int stk3x1x_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)      
#endif
{
	struct i2c_client *client = (struct i2c_client*)file->private_data;
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);  
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
	long err = 0;
#else
	int err = 0;
#endif
	void __user *ptr = (void __user*) arg;
	int dat;
	uint32_t enable;
	int ps_result, ps_cali_result;

	switch (cmd)
	{
		case ALSPS_SET_PS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				if((err = stk3x1x_enable_ps(obj->client, 1, 1)))
				{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
					APS_ERR("enable ps fail: %ld\n", err); 
#else
					APS_ERR("enable ps fail: %d\n", err); 
#endif
					goto err_out;
				}
				
				set_bit(STK_BIT_PS, &obj->enable);
			}
			else
			{
				if((err = stk3x1x_enable_ps(obj->client, 0, 1)))
				{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
					APS_ERR("disable ps fail: %ld\n", err); 
#else
					APS_ERR("disable ps fail: %d\n", err); 
#endif
	
					goto err_out;
				}
				
				clear_bit(STK_BIT_PS, &obj->enable);
			}
			break;

		case ALSPS_GET_PS_MODE:
			enable = test_bit(STK_BIT_PS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_PS_DATA:    
			if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
			{
				goto err_out;
			}
			
			dat = stk3x1x_get_ps_value(obj, obj->ps);
			if(dat < 0)
			{
				err = dat;
				goto err_out;
			}
#ifdef STK_PS_POLLING_LOG	
			APS_LOG("%s:ps raw 0x%x -> value 0x%x \n",__FUNCTION__, obj->ps, dat);			
#endif			
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
			break;

		case ALSPS_GET_PS_RAW_DATA:    
			if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
			{
				goto err_out;
			}
			
			dat = obj->ps;
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
			break;            

		case ALSPS_SET_ALS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				if((err = stk3x1x_enable_als(obj->client, 1)))
				{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
					APS_ERR("enable als fail: %ld\n", err); 
#else
					APS_ERR("enable als fail: %d\n", err); 
#endif

					goto err_out;
				}
				set_bit(STK_BIT_ALS, &obj->enable);
			}
			else
			{
				if((err = stk3x1x_enable_als(obj->client, 0)))
				{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
					APS_ERR("disable als fail: %ld\n", err); 
#else
					APS_ERR("disable als fail: %d\n", err); 
#endif

					goto err_out;
				}
				clear_bit(STK_BIT_ALS, &obj->enable);
			}
			break;

		case ALSPS_GET_ALS_MODE:
			enable = test_bit(STK_BIT_ALS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_ALS_DATA: 
			if((err = stk3x1x_read_als(obj->client, &obj->als)))
			{
				goto err_out;
			}

			dat = stk3x1x_get_als_value(obj, obj->als);
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}              
			break;

		case ALSPS_GET_ALS_RAW_DATA:    
			if((err = stk3x1x_read_als(obj->client, &obj->als)))
			{
				goto err_out;
			}

			dat = obj->als;
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}              
			break;
		
		case ALSPS_GET_PS_THRESHOLD_HIGH:
			dat = atomic_read(&obj->ps_high_thd_val);
			APS_LOG("%s:ps_high_thd_val:%d\n",__func__,dat);
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;
		case ALSPS_GET_PS_THRESHOLD_LOW:
			dat = atomic_read(&obj->ps_low_thd_val);
			APS_LOG("%s:ps_low_thd_val:%d\n",__func__,dat);			
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;		
			
		case ALSPS_GET_PS_TEST_RESULT: 
			if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
			{
				goto err_out;
			}

			if(obj->ps > atomic_read(&obj->ps_high_thd_val))
				ps_result = 0;
			else	
				ps_result = 1;
			APS_LOG("ALSPS_GET_PS_TEST_RESULT : ps_result = %d\n",ps_result); 				
			if(copy_to_user(ptr, &ps_result, sizeof(ps_result)))
			{
				err = -EFAULT;
				goto err_out;
			}			   
			break;
			
		case ALSPS_IOCTL_CLR_CALI: 
			if(copy_from_user(&dat, ptr, sizeof(dat))) 
			{ 
				err = -EFAULT; 
				goto err_out; 
			} 
			obj->ps_cali = 0; 
			atomic_set(&obj->ps_high_thd_val,  obj->hw->ps_threshold_high); 
			atomic_set(&obj->ps_low_thd_val,  obj->hw->ps_threshold_low);  //obj->hw->ps_low_thd_val
			APS_LOG("ALSPS_IOCTL_CLR_CALI : obj->ps_cali:%d high:%d low:%d\n", obj->ps_cali, 
													atomic_read(&obj->ps_high_thd_val), 
													atomic_read(&obj->ps_low_thd_val)); 			
			break;
			
		case ALSPS_IOCTL_GET_CALI: 
			ps_cali_result = obj->ps_cali; 
			APS_LOG("ALSPS_IOCTL_GET_CALI : ps_cali = %d\n",obj->ps_cali); 
			if(copy_to_user(ptr, &ps_cali_result, sizeof(ps_cali_result))) 
			{ 
				err = -EFAULT; 
				goto err_out; 
			} 
			break; 
			
		case ALSPS_IOCTL_SET_CALI:                 
			//1. libhwm.so calc value store in ps_cali; 
			//2. nvram_daemon update ps_cali in driver 
			if(copy_from_user(&ps_cali_result, ptr, sizeof(ps_cali_result))) 
			{ 
				err = -EFAULT; 
				goto err_out; 
			} 
			obj->ps_cali = ps_cali_result; 
			atomic_set(&obj->ps_high_thd_val, obj->hw->ps_threshold_high); 
			atomic_set(&obj->ps_low_thd_val,  obj->hw->ps_threshold_low); 
			if((err = stk3x1x_write_ps_high_thd(obj->client, atomic_read(&obj->ps_high_thd_val))))
				goto err_out;       
			if((err = stk3x1x_write_ps_low_thd(obj->client, atomic_read(&obj->ps_low_thd_val))))
				goto err_out;    
			APS_LOG("ALSPS_IOCTL_SET_CALI : ps_cali_result = %d\n",ps_cali_result); 
			APS_LOG("ALSPS_IOCTL_SET_CALI : obj->ps_cali:%d high:%d low:%d\n", obj->ps_cali, 
													atomic_read(&obj->ps_high_thd_val), 
													atomic_read(&obj->ps_low_thd_val)); 
			break;					
			
		default:
			APS_ERR("%s not supported = 0x%04x", __FUNCTION__, cmd);
			err = -ENOIOCTLCMD;
			break;
	}

	err_out:
	return err;    
}
/*----------------------------------------------------------------------------*/
static struct file_operations stk3x1x_fops = {
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))
	.owner = THIS_MODULE,
#endif
	.open = stk3x1x_open,
	.release = stk3x1x_release,
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
	.unlocked_ioctl = stk3x1x_unlocked_ioctl,
#else
	.ioctl = stk3x1x_ioctl,
#endif

};
/*----------------------------------------------------------------------------*/
static struct miscdevice stk3x1x_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "als_ps",
	.fops = &stk3x1x_fops,
};
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_suspend(struct i2c_client *client, pm_message_t msg) 
{
	APS_FUN();    
/*
	if(msg.event == PM_EVENT_SUSPEND)
	{   
		if(!obj)
		{
			APS_ERR("null pointer!!\n");
			return -EINVAL;
		}
		
		atomic_set(&obj->als_suspend, 1);
		if((err = stk3x1x_enable_als(client, 0)))
		{
			APS_ERR("disable als: %d\n", err);
			return err;
		}

		atomic_set(&obj->ps_suspend, 1);
		if((err = stk3x1x_enable_ps(client, 0, 1)))
		{
			APS_ERR("disable ps:  %d\n", err);
			return err;
		}
		
		stk3x1x_power(obj->hw, 0);
	}
	
*/
	return 0;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_resume(struct i2c_client *client)
{
	APS_FUN();
/*
	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}

	stk3x1x_power(obj->hw, 1);
	if((err = stk3x1x_init_client(client)))
	{
		APS_ERR("initialize client fail!!\n");
		return err;        
	}
	atomic_set(&obj->als_suspend, 0);
	if(test_bit(STK_BIT_ALS, &obj->enable))
	{
		if((err = stk3x1x_enable_als(client, 1)))
		{
			APS_ERR("enable als fail: %d\n", err);        
		}
	}
	atomic_set(&obj->ps_suspend, 0);
	if(test_bit(STK_BIT_PS,  &obj->enable))
	{
		if((err = stk3x1x_enable_ps(client, 1, 1)))
		{
			APS_ERR("enable ps fail: %d\n", err);                
		}
	}
*/
	return 0;
}

/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_detect(struct i2c_client *client, struct i2c_board_info *info) 
{    
	strcpy(info->type, stk3x1x_DEV_NAME);
	return 0;
}

/*----------------------------------------------------------------------------*/	
static int als_open_report_data(int open)
{
	//should queuq work to report event if  is_report_input_direct=true
	return 0;
}

// if use  this typ of enable , Gsensor only enabled but not report inputEvent to HAL

static int als_enable_nodata(int en)
{
	int res = 0;
#ifdef CUSTOM_KERNEL_SENSORHUB
    SCP_SENSOR_HUB_DATA req;
    int len;
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

 //   APS_LOG("stk3x1x_obj als enable value = %d\n", en);

#ifdef CUSTOM_KERNEL_SENSORHUB
    req.activate_req.sensorType = ID_LIGHT;
    req.activate_req.action = SENSOR_HUB_ACTIVATE;
    req.activate_req.enable = en;
    len = sizeof(req.activate_req);
    res = SCP_sensorHub_req_send(&req, &len, 1);
#else //#ifdef CUSTOM_KERNEL_SENSORHUB
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return -1;
	}
	res=	stk3x1x_enable_als(stk3x1x_obj->client, en);
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB
	if(res){
		APS_ERR("als_enable_nodata is failed!!\n");
		return -1;
	}
	return 0;
}

static int als_set_delay(u64 ns)
{
	return 0;
}

static int als_get_data(int* value, int* status)
{
	int err = 0;
#ifdef CUSTOM_KERNEL_SENSORHUB
    SCP_SENSOR_HUB_DATA req;
    int len;
#else
    struct stk3x1x_priv *obj = NULL;
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

#ifdef CUSTOM_KERNEL_SENSORHUB
    req.get_data_req.sensorType = ID_LIGHT;
    req.get_data_req.action = SENSOR_HUB_GET_DATA;
    len = sizeof(req.get_data_req);
    err = SCP_sensorHub_req_send(&req, &len, 1);
    if (err)
    {
        APS_ERR("SCP_sensorHub_req_send fail!\n");
    }
    else
    {
        *value = req.get_data_rsp.int16_Data[0];
        *status = SENSOR_STATUS_ACCURACY_MEDIUM;
    }

    if(atomic_read(&stk3x1x_obj->trace) & CMC_TRC_PS_DATA)
	{
        APS_LOG("value = %d\n", *value);
        //show data
	}
#else //#ifdef CUSTOM_KERNEL_SENSORHUB
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return -1;
	}
	obj = stk3x1x_obj;
	if((err = stk3x1x_read_als(obj->client, &obj->als)))
	{
		err = -1;
	}
	else
	{
		if(obj->als < 3)
		{
			obj->als_last = obj->als;
			*value = stk3x1x_get_als_value(obj, 0);
		}
		else if(abs(obj->als - obj->als_last) >= STK_ALS_CODE_CHANGE_THD)
		{
			obj->als_last = obj->als;
			*value = stk3x1x_get_als_value(obj, obj->als);
		}
		else
		{
			*value = stk3x1x_get_als_value(obj, obj->als_last);
		}					
	
		*status = SENSOR_STATUS_ACCURACY_MEDIUM;
	}
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

	return err;
}

static int ps_open_report_data(int open)
{
	//should queuq work to report event if  is_report_input_direct=true
	return 0;
}

// if use  this typ of enable , Gsensor only enabled but not report inputEvent to HAL

static int ps_enable_nodata(int en)
{
	int res = 0;
	//struct stk3x1x_priv *obj = i2c_get_clientdata(client);
#ifdef CUSTOM_KERNEL_SENSORHUB
    SCP_SENSOR_HUB_DATA req;
    int len;
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

//    APS_LOG("stk3x1x_obj als enable value = %d\n", en);

#ifdef CUSTOM_KERNEL_SENSORHUB
    req.activate_req.sensorType = ID_PROXIMITY;
    req.activate_req.action = SENSOR_HUB_ACTIVATE;
    req.activate_req.enable = en;
    len = sizeof(req.activate_req);
    res = SCP_sensorHub_req_send(&req, &len, 1);
#else //#ifdef CUSTOM_KERNEL_SENSORHUB
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return -1;
	}
	res= stk3x1x_enable_ps(stk3x1x_obj->client, en,1);
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB
    
	if(res){
		APS_ERR("als_enable_nodata is failed!!\n");
		return -1;
	}

#ifdef HQ_PSENSOR_CALIBRATION
	if(en){
		stk3x1x_psensor_calibrate();
	}
#endif

	return 0;

}

static int ps_set_delay(u64 ns)
{
	return 0;
}

static int ps_get_data(int* value, int* status)
{
    int err = 0;
#ifdef CUSTOM_KERNEL_SENSORHUB
    SCP_SENSOR_HUB_DATA req;
    int len;
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB

#ifdef CUSTOM_KERNEL_SENSORHUB
    req.get_data_req.sensorType = ID_PROXIMITY;
    req.get_data_req.action = SENSOR_HUB_GET_DATA;
    len = sizeof(req.get_data_req);
    err = SCP_sensorHub_req_send(&req, &len, 1);
    if (err)
    {
        APS_ERR("SCP_sensorHub_req_send fail!\n");
    }
    else
    {
        *value = req.get_data_rsp.int16_Data[0];
        *status = SENSOR_STATUS_ACCURACY_MEDIUM;
    }

    if(atomic_read(&stk3x1x_obj->trace) & CMC_TRC_PS_DATA)
	{
        APS_LOG("value = %d\n", *value);
        //show data
	}
#else //#ifdef CUSTOM_KERNEL_SENSORHUB
    if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return -1;
	}
    
    if((err = stk3x1x_read_ps(stk3x1x_obj->client, &stk3x1x_obj->ps)))
    {
        err = -1;;
    }
    else
    {
        *value = stk3x1x_get_ps_value(stk3x1x_obj, stk3x1x_obj->ps);
        *status = SENSOR_STATUS_ACCURACY_MEDIUM;
    }
#endif //#ifdef CUSTOM_KERNEL_SENSORHUB
    
	return 0;
}

#ifdef HQ_PSENSOR_CALIBRATION
static int stk3x1x_psensor_calibrate(void)
{
	int i = 0;
	int j = 0;
	unsigned long sum = 0;
	struct stk3x1x_priv *obj = stk3x1x_obj;

//	APS_FUN(f);
	if (!obj) goto err;

	for (i = 0; i < 3; i++) {
		msleep(25);
				
		stk3x1x_read_ps(obj->client, &obj->ps);
		
		if(obj->ps != 0){
			j++;
		}	
		sum += obj->ps;
	}
	
	if(j>0)
		sum = sum/j;
	else
		sum = 0;
	
	if(sum < MAX_PS_NOISE){
		if(obj->init_noise == AVG_PS_NOISE)
			obj->init_noise = sum;
		
		obj->current_xtalk = sum;
	}else{
		obj->current_xtalk = obj->init_noise;
	}

	atomic_set(&obj->ps_high_thd_val,  obj->hw->ps_threshold_high+obj->current_xtalk); 
	atomic_set(&obj->ps_low_thd_val,  obj->hw->ps_threshold_low+obj->current_xtalk);  

//	APS_LOG("zqq calibrate:noise=%ld,low=%d ,high=%d\n",
//		sum,atomic_read(&obj->ps_high_thd_val),atomic_read(&obj->ps_low_thd_val));

	if(0 == obj->hw->polling_mode_ps)
	{	
		if((0 != stk3x1x_write_ps_high_thd(obj->client, atomic_read(&obj->ps_high_thd_val))))
			goto err;		
		if((0 != stk3x1x_write_ps_low_thd(obj->client, atomic_read(&obj->ps_low_thd_val))))
			goto err;	 
	}
	
	return 0;
err:
	APS_ERR("zqq stk3x1x_psensor_calibrate fail!!!\n");
	return -1;
}

static int stk3x1x_get_init_noise(void)
{
	int i = 0;
	int j = 0;
	u8 rev = 0;	
	u8 data = 0;
	unsigned long sum = 0;
	struct stk3x1x_priv *obj = stk3x1x_obj;

	APS_FUN(f);
	if (!obj) return -1;

	disable_irq_nosync(obj->irq);

	//enable
	stk3x1x_read_state(obj->client,&data);
	data |= STK_STATE_EN_PS_MASK;
	rev = stk3x1x_write_state(obj->client, data);
	if(rev < 0)
		goto err;

	for (i = 0; i < 5; i++) {
		msleep(30);
				
		stk3x1x_read_ps(obj->client, &obj->ps);
		
		if(obj->ps != 0){
			j++;
		}	
		sum += obj->ps;
	}
	
	if(j>0)
		sum = sum/j;
	else
		sum = 0;
	
	if(sum < MAX_PS_NOISE){
		obj->init_noise = sum;	
	}

	//disable 
	stk3x1x_read_state(obj->client,&data);
	data &= (~STK_STATE_EN_PS_MASK);
	rev = stk3x1x_write_state(obj->client, data);
	if(rev < 0)
		goto err;
	
	enable_irq(obj->irq);

	APS_DBG("zqq init_noise %d\n",obj->init_noise);
	
	return 0;
err:
	APS_ERR("zqq stk3x1x_get_init_noise fail!!!\n");
	enable_irq(obj->irq);
	return -1;
}
#endif

extern char *g_alsps_name;
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct stk3x1x_priv *obj;
	int err = 0;
	struct als_control_path als_ctl={0};
	struct als_data_path als_data={0};
	struct ps_control_path ps_ctl={0};
	struct ps_data_path ps_data={0};

	APS_LOG("%s: driver version: %s\n", __FUNCTION__, DRIVER_VERSION);

	if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL)))
	{
		err = -ENOMEM;
		goto exit;
	}
	memset(obj, 0, sizeof(*obj));
	stk3x1x_obj = obj;
	obj->hw = hw;
	stk3x1x_get_addr(obj->hw, &obj->addr);

	INIT_DELAYED_WORK(&obj->eint_work, stk3x1x_eint_work);
	printk("client->addr======%x\n",client->addr);
	client->addr = 0x48;
	printk("client->addr======%x\n",client->addr);
	obj->client = client;
	i2c_set_clientdata(client, obj);	
	atomic_set(&obj->als_debounce, 200);
	atomic_set(&obj->als_deb_on, 0);
	atomic_set(&obj->als_deb_end, 0);
	atomic_set(&obj->ps_debounce, 10);
	atomic_set(&obj->ps_deb_on, 0);
	atomic_set(&obj->ps_deb_end, 0);
	atomic_set(&obj->ps_mask, 0);
	atomic_set(&obj->trace, 0x00);
	atomic_set(&obj->als_suspend, 0);
	atomic_set(&obj->init_done, 0);
	//obj->irq_node = of_find_matching_node(obj->irq_node, alsps_of_match);
	obj->irq_node = of_find_compatible_node(NULL, NULL, "mediatek, als-eint");

	atomic_set(&obj->state_val, 0);
	atomic_set(&obj->psctrl_val, 0x21);
	atomic_set(&obj->alsctrl_val, 0x39);
	obj->ledctrl_val = 0xFF;	
	obj->wait_val = 0xF;
	obj->int_val = 0;
	obj->first_boot = true;			 
	obj->als_correct_factor = 1000;
	atomic_set(&obj->ps_high_thd_val, obj->hw->ps_threshold_high);//  obj->hw->ps_high_thd_val
	atomic_set(&obj->ps_low_thd_val, obj->hw->ps_threshold_low);	
	atomic_set(&obj->recv_reg, 0);  
#ifdef STK_ALS_FIR	
	atomic_set(&obj->firlength, STK_FIR_LEN);	
#endif	

	if(obj->hw->polling_mode_ps == 0)
	{
		APS_LOG("%s: enable PS interrupt\n", __FUNCTION__);
	}
	obj->int_val |= STK_INT_PS_MODE1;
	
	if(obj->hw->polling_mode_als == 0)
	{
	  obj->int_val |= STK_INT_ALS;		
	  APS_LOG("%s: enable ALS interrupt\n", __FUNCTION__);
	}	

	APS_LOG("%s: state_val=0x%x, psctrl_val=0x%x, alsctrl_val=0x%x, ledctrl_val=0x%x, wait_val=0x%x, int_val=0x%x\n", 
		__FUNCTION__, atomic_read(&obj->state_val), atomic_read(&obj->psctrl_val), atomic_read(&obj->alsctrl_val), 
		obj->ledctrl_val, obj->wait_val, obj->int_val);
	
	obj->enable = 0;
	obj->pending_intr = 0;
	obj->als_level_num = sizeof(obj->hw->als_level)/sizeof(obj->hw->als_level[0]);
	obj->als_value_num = sizeof(obj->hw->als_value)/sizeof(obj->hw->als_value[0]);   
	BUG_ON(sizeof(obj->als_level) != sizeof(obj->hw->als_level));
	memcpy(obj->als_level, obj->hw->als_level, sizeof(obj->als_level));
	BUG_ON(sizeof(obj->als_value) != sizeof(obj->hw->als_value));
	memcpy(obj->als_value, obj->hw->als_value, sizeof(obj->als_value));
	atomic_set(&obj->i2c_retry, 3);
	if(atomic_read(&obj->state_val) & STK_STATE_EN_ALS_MASK)
	{
		set_bit(STK_BIT_ALS, &obj->enable);
	}
	
	if(atomic_read(&obj->state_val) & STK_STATE_EN_PS_MASK)
	{
		set_bit(STK_BIT_PS, &obj->enable);
	}
	
	stk3x1x_i2c_client = client;

	if((err = stk3x1x_init_client(client)))
	{
		goto exit_init_failed;
	}
	
	if((err = misc_register(&stk3x1x_device)))
	{
		APS_ERR("stk3x1x_device register failed\n");
		goto exit_misc_device_register_failed;
	}

	if((err = stk3x1x_create_attr(&(stk3x1x_init_info.platform_diver_addr->driver))))
	//if((err = stk3x1x_create_attr(&stk3x1x_alsps_driver.driver)))
	{
		APS_ERR("create attribute err = %d\n", err);
		goto exit_create_attr_failed;
	}
		
	als_ctl.open_report_data= als_open_report_data;
	als_ctl.enable_nodata = als_enable_nodata;
	als_ctl.set_delay  = als_set_delay;
	als_ctl.is_report_input_direct = false;
#ifdef CUSTOM_KERNEL_SENSORHUB
	als_ctl.is_support_batch = obj->hw->is_batch_supported_als;
#else
    als_ctl.is_support_batch = false;
#endif
	
	err = als_register_control_path(&als_ctl);
	if(err)
	{
		APS_ERR("register fail = %d\n", err);
		goto exit_sensor_obj_attach_fail;
	}

	als_data.get_data = als_get_data;
	als_data.vender_div = 100;
	err = als_register_data_path(&als_data);	
	if(err)
	{
		APS_ERR("tregister fail = %d\n", err);
		goto exit_sensor_obj_attach_fail;
	}
	
	ps_ctl.open_report_data= ps_open_report_data;
	ps_ctl.enable_nodata = ps_enable_nodata;
	ps_ctl.set_delay  = ps_set_delay;
//	ps_ctl.is_report_input_direct = true;
#ifdef CUSTOM_KERNEL_SENSORHUB
	ps_ctl.is_support_batch = obj->hw->is_batch_supported_ps;
#else
    ps_ctl.is_support_batch = false;
#endif

	if(1 == obj->hw->polling_mode_ps)
	{
		//wake_lock_init(&ps_lock,WAKE_LOCK_SUSPEND,"ps wakelock");
		ps_ctl.is_polling_mode = 1;
		ps_ctl.is_report_input_direct = true;
	}
	else
	{
		ps_ctl.is_polling_mode = 0;//PS interrupt mode
		ps_ctl.is_report_input_direct = false;
	}
	
	err = ps_register_control_path(&ps_ctl);
	if(err)
	{
		APS_ERR("register fail = %d\n", err);
		goto exit_sensor_obj_attach_fail;
	}

	ps_data.get_data = ps_get_data;
	ps_data.vender_div = 100;
	err = ps_register_data_path(&ps_data);	
	if(err)
	{
		APS_ERR("tregister fail = %d\n", err);
		goto exit_sensor_obj_attach_fail;
	}

	err = batch_register_support_info(ID_LIGHT,als_ctl.is_support_batch, 100, 0);
	if(err)
	{
		APS_ERR("register light batch support err = %d\n", err);
	}
	
	err = batch_register_support_info(ID_PROXIMITY,ps_ctl.is_support_batch, 100, 0);
	if(err)
	{
		APS_ERR("register proximity batch support err = %d\n", err);
	}
	
#ifdef HQ_PSENSOR_CALIBRATION
	obj->init_noise = AVG_PS_NOISE; // avg noise
	stk3x1x_get_init_noise();
#endif

    stk3x1x_init_flag = 0;
	g_alsps_name = stk3x1x_DEV_NAME;
	
	
	APS_LOG("stk3x1x_i2c_probe() OK!\n");	

	return 0;
	
	exit_sensor_obj_attach_fail:
	exit_create_attr_failed:
	misc_deregister(&stk3x1x_device);
	exit_misc_device_register_failed:
	exit_init_failed:
	//i2c_detach_client(client);
//	exit_kfree:
	kfree(obj);
	exit:
	stk3x1x_i2c_client = NULL;           
    stk3x1x_init_flag = -1;

	APS_ERR("%s: err = %d\n", __FUNCTION__, err);
	return err;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_remove(struct i2c_client *client)
{
	int err;	
	//if((err = stk3x1x_delete_attr(&stk3x1x_i2c_driver.driver)))
	if((err = stk3x1x_delete_attr(&(stk3x1x_init_info.platform_diver_addr->driver))))
	{
		APS_ERR("stk3x1x_delete_attr fail: %d\n", err);
	} 

	if((err = misc_deregister(&stk3x1x_device)))
	{
		APS_ERR("misc_deregister fail: %d\n", err);    
	}
	
	stk3x1x_i2c_client = NULL;
	i2c_unregister_device(client);
	kfree(i2c_get_clientdata(client));

	return 0;
}

static int  stk3x1x_local_uninit(void)
{
//	struct alsps_hw *hw = get_cust_alsps_hw();
	APS_FUN();    
	stk3x1x_power(hw, 0);    
	i2c_del_driver(&stk3x1x_i2c_driver);
	stk3x1x_i2c_client = NULL;
	return 0;
}

/*----------------------------------------------------------------------------*/

static int stk3x1x_local_init(void) 
{
//	struct alsps_hw *hw = get_cust_alsps_hw();
	struct stk3x1x_i2c_addr addr;

	APS_FUN();    
	stk3x1x_power(hw, 1);    
	stk3x1x_get_addr(hw, &addr);
	if(i2c_add_driver(&stk3x1x_i2c_driver))
	{
		APS_ERR("add driver error\n");
		return -1;
	} 

    if(-1 == stk3x1x_init_flag)
    {
		APS_ERR("stk3x1x_local_init fail with stk3x1x_init_flag=%d\n", stk3x1x_init_flag);
        return -1;
    }
	return 0;
}

/*----------------------------------------------------------------------------*/
static int __init stk3x1x_init(void)
{
	const char *name = "mediatek,stk3x1x";

	APS_FUN();
	printk("stk3x1x_init In");

	hw = get_alsps_dts_func(name, hw);
	if (!hw)
		APS_ERR("get dts info fail\n");

   	alsps_driver_add(&stk3x1x_init_info);// hwmsen_alsps_add(&stk3x1x_init_info);

#if 0
	stk3x1x_regulator = regulator_get(&alspsPltFmDev->dev,"vstk3x1x");
	if(!stk3x1x_regulator)
		printk("ZYQ stk3x1x_regulator is NULL\n");
	ret = regulator_set_voltage(stk3x1x_regulator, 2800000, 2800000); /*set 2.8v*/
	if (ret){
		printk("regulator_set_voltage() failed!\n");
	}	
#endif
	
	printk("stk3x1x_init Out");

	return 0;
}
/*----------------------------------------------------------------------------*/
static void __exit stk3x1x_exit(void)
{
	APS_FUN();
}
/*----------------------------------------------------------------------------*/
module_init(stk3x1x_init);
module_exit(stk3x1x_exit);
/*----------------------------------------------------------------------------*/
MODULE_AUTHOR("MingHsien Hsieh");
MODULE_DESCRIPTION("SensorTek stk3x1x proximity and light sensor driver");
MODULE_LICENSE("GPL");
