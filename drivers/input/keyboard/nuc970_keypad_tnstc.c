/* linux/driver/input/nuc970_keypad.c
 *
 * Copyright (c) 2017 Nuvoton technology corporation
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Changelog:
 *
 */

#include <linux/init.h>
#include <linux/slab.h>

#include <linux/input.h>
#include <linux/device.h>

#include <asm/errno.h>
#include <asm/delay.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/completion.h>

#include <linux/platform_device.h>

#include <mach/map.h>
#include <mach/mfp.h>

#include <mach/gpio.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#include <mach/map.h>
#include <mach/regs-gpio.h>
#include <mach/regs-clock.h>
#include <mach/regs-gcr.h>
#include <mach/regs-aic.h>

#include <mach/irqs.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/platform_data/keypad-nuc970.h>

#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fcntl.h>
#include <linux/mm.h>

#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>




#undef BIT
#include <linux/input.h>
#define BIT(x)  (1UL<<((x)%BITS_PER_LONG))

#define DEF_KPD_DELAY           HZ/100

#define KEY_COUNT		32

#if defined CONFIG_OF
#define CONFIG_NUC970_KEYPAD_PH 1
#endif

static struct input_dev *nuc970_keypad_input_dev;
static struct timer_list kpd_timer;
static char timer_active = 0;

static u32 old_key;
static u32 new_key;
static u32 open_cnt = 0;

/******************************Decleration*****************************************/
static int reset_lock=0, rotate_handler=0;
static int backspace=0, shift_press=0,rot_a=0,rot_d=0,rot_g=0,rot_j=0,rot_m=0,rot_p=0,rot_t=0,rot_w=0;

u32 nuc970_key_pressing = 0;
EXPORT_SYMBOL(nuc970_key_pressing);

static void touch_file(char *filename);
static void reset_key(void);
static void reset_not(char data);
static void KEY_RELEASE_DEF(int i);
static void KEY_PRESS_DEF(int i);
static void timeout_handler(unsigned long data);
static void alpha_reset_key(unsigned long data);

static unsigned long onesec;
DEFINE_TIMER(tcatimer, timeout_handler, 0, 0);
DEFINE_TIMER(tcatimer_alpha, alpha_reset_key, 0, 0);

/******************************Decleration*****************************************/

/******************************Proc Stat**********************/

#define STR_PRINTF_RET(len, str, args...) len += seq_printf(m, str, ## args)

static struct proc_dir_entry *parent, *file;
static int KEYPADMODE_state = 0, KEYPADMODE_buzzer_state = 1;

static ssize_t time_write(struct file *file, const char __user *buffer, size_t count, loff_t *off)
{
    char kbuf[2];
    if (count > 2)
        return count;
    if (copy_from_user(kbuf, buffer, count))
    {
        return -EFAULT;
    }
    if ((count == 2) && (kbuf[1] != '\n'))
        return count;
    if ((kbuf[0] < '0') || ('9' < kbuf[0]))
        return count;
    KEYPADMODE_state = kbuf[0] - '0';
    return count;
}

static ssize_t time_write1(struct file *file, const char __user *buffer, size_t count, loff_t *off)
{
    char kbuf[2];
    if (count > 2)
        return count;
    if (copy_from_user(kbuf, buffer, count))
    {
        return -EFAULT;
    }
    if ((count == 2) && (kbuf[1] != '\n'))
        return count;
    if ((kbuf[0] < '0') || ('9' < kbuf[0]))
        return count;
    KEYPADMODE_buzzer_state = kbuf[0] - '0';
    return count;
}

static int time_open(struct inode *inode, struct file *file)
{
    //    return single_open(file, time_read, NULL);
    return 0;
}

static int time_open1(struct inode *inode, struct file *file)
{
    //    return single_open(file, time_read, NULL);
    return 0;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = time_open,
    //    .read = seq_read,
    .write = time_write,
};

static struct file_operations fops1 =
{
    .owner = THIS_MODULE,
    .open = time_open1,
    //    .read = seq_read,
    .write = time_write1,
};

static int __init proc_win_init(void)
{
    if ((parent = proc_mkdir("keypad", NULL)) == NULL)
    {
        return -1;
    }
    if ((file = proc_create("KEYPAD_mode", 0666, parent, &fops)) == NULL)
    {
        remove_proc_entry("keypad", NULL);
        return -1;
    }
    if ((file = proc_create("KEYPAD_buzzer", 0666, parent, &fops1)) == NULL)
    {
        remove_proc_entry("keypad",NULL);
        return -1;
    }
    return 0;
}

static void __exit proc_win_exit(void)
{
    remove_proc_entry("KEYPAD_mode",parent);
    remove_proc_entry("KEYPAD_buzzer",parent);
    remove_proc_entry("keypad", NULL);
}

/*********************************Proc Stat**********************************/


static int nuc970_keymap[32] = {
    // (0,0) 0          (0,1) 1         (0,2) 2         (0,3) 3
    KEY_RESERVED,	KEY_ESC, 	KEY_F5,         KEY_DOWN,
    // (0,4) 4          (0,5)  5        (0,6)  6        (0,7)  7
    KEY_UP,	        KEY_F14,         KEY_F1,	KEY_F6,

    // (1,0) 8          (1,1)  9        (1,2)  10       (1,3)  11
    KEY_RESERVED,       KEY_F13,	KEY_15		KEY_F2

    // (1,4) 12         (1,5) 13       (1,6)  14        (1,7)  15
    KEY_F7,	        KEY_1,          KEY_2,	        KEY_3,

    // (2,0) 16         (2,1) 17        (2,2)   18      (2,3)  19
    KEY_F11,           KEY_F3,   	KEY_F8,         KEY_4,
    // (2,4) 20         (2,5)  21       (2,6)  22       (2,7)  23
    KEY_5,              KEY_6,          KEY_F4,   	KEY_F9,

    // (3,0)  24        (3,1)  25       (3,2)  26       (3,3)   27
    KEY_RESERVED,	KEY_7,          KEY_8,	        KEY_9,
    // (3,4)  28        (3,5)   29      (3,6)   30      (3,7)   31
    KEY_F10,		KEY_F12,        KEY_0,	        KEY_ENTER,
};

static int nuc970_keymap1[32] = {
    // (0,0) 0          (0,1) 1         (0,2) 2         (0,3) 3
    KEY_RESERVED,	KEY_F8, 	KEY_F5,         KEY_F9,
    // (0,4) 4          (0,5)  5        (0,6)  6        (0,7)  7
    KEY_UP,	        KEY_F7,         KEY_TAB,	KEY_F1,

    // (1,0) 8          (1,1)  9        (1,2)  10       (1,3)  11
    KEY_ESC,            KEY_DOWN,	KEY_F10,	KEY_F6,
    // (1,4) 12         (1,5) 13       (1,6)  14        (1,7)  15
    KEY_F2,	        KEY_1,          KEY_2,	        KEY_3,

    // (2,0) 16         (2,1) 17        (2,2)   18     (2,3)  19
    KEY_F11,           KEY_F12,   KEY_F3,        KEY_4,
    // (2,4) 20         (2,5)  21       (2,6)  22      (2,7)  23
    KEY_5,              KEY_6,          KEY_F13,   KEY_F4,

    // (3,0)  24        (3,1)  25       (3,2)  26       (3,3)   27
    KEY_RESERVED,	KEY_7,          KEY_8,	        KEY_9,
    // (3,4)  28        (3,5)   29      (3,6)   30      (3,7)   31
    KEY_BACKSPACE,	KEY_DOT,        KEY_0,	        KEY_ENTER,
};


#if 0
// arg = 0, from isr, 1 from timer.
static void nuc970_check_ghost_state(void)
{
    int i,j;
    u32 check = 0;
    u32 col, check_col, cmp_col;

    for (i = 0; i < ROW_CNT; i++)
    {
        col = (new_key >> (i*COL_CNT)) & COL_MASK;

        if ((col & check) && hweight8(col) > 1)
        {
            for(j=0; j<ROW_CNT; j++)
            {
                check_col = (new_key >> (j*COL_CNT)) & COL_MASK;
                if((col & check_col) != 0)
                {
                    cmp_col = (old_key >> (j*COL_CNT)) & COL_MASK;
                    new_key = new_key & ~((cmp_col ^ check_col) << (j*COL_CNT));
                }
            }
        }

        check |= col;
    }

}
#endif

static void read_key(unsigned long arg)
{
    u32 i;
    onesec = msecs_to_jiffies(500 * 1);
    mod_timer(&tcatimer_alpha, jiffies + onesec);

#if 1
    // ISR detect key press, disable irq, use timer to read following key press until released
    if (!timer_active) {
        nuc970_key_pressing = 1;

#if defined CONFIG_NUC970_KEYPAD_PH
        for(i = 0; i < NUC970_KPD_COL_NUMBER; i++)
        {
            disable_irq_nosync(gpio_to_irq(NUC970_PH8+i));
        }
#endif
    }
#else
    nuc970_key_pressing = 1;
#endif

#if defined CONFIG_NUC970_KEYPAD_PH
    new_key = readl(REG_GPIOH_DATAIN) & (((1 << NUC970_KPD_COL_NUMBER) - 1) << 8);

#endif

    if(KEYPADMODE_state==1 || KEYPADMODE_state==2)
    {
        //        printk("Alpha Small File present\n");
        shift_press=1;
        reset_lock=0;
    }
    else if (KEYPADMODE_state==3)
    {
        //        printk("Number File present\n");
        shift_press=0;
        reset_lock=0;
    }

    /******************************Shift Press************************************/
    if(KEYPADMODE_state==0)
    {
        if(reset_lock==0)
        {
            shift_press=0;
            reset_lock=1;
        }
    }
    /******************************Shift Press************************************/

    if ((new_key & (((1 << NUC970_KPD_COL_NUMBER) - 1) << 8)) == (((1 << NUC970_KPD_COL_NUMBER) - 1) << 8)) { // all key released

        for (i = 0; i < KEY_COUNT; i++) {
            if (old_key & (1 << i)) {
                KEY_RELEASE_DEF(i);//Key release Definitions or down here
                input_event(nuc970_keypad_input_dev, EV_MSC, MSC_SCAN, i);
                //printk("=== key up1 code[%d] 0x%x 0x%x \n", i, nuc970_keymap[i], new_key);
                input_report_key(nuc970_keypad_input_dev, nuc970_keymap[i], 0);     //key up
                input_sync(nuc970_keypad_input_dev);
            }
        }
        old_key = 0;
        del_timer(&kpd_timer);
        timer_active = 0;

#if defined CONFIG_NUC970_KEYPAD_PH
        for(i = 0; i < NUC970_KPD_COL_NUMBER; i++)
        {
            enable_irq(gpio_to_irq(NUC970_PH8+i));
        }

#endif

        nuc970_key_pressing = 0;
        return;
    }

#if defined CONFIG_NUC970_KEYPAD_PH
    // scan key
    new_key = 0;
    for(i = 0; i < NUC970_KPD_ROW_NUMBER; i++)
    {
        writel((readl(REG_GPIOH_DIR) & ~( ((1 << NUC970_KPD_ROW_NUMBER) - 1) << 4 ) ), REG_GPIOH_DIR);
        writel((readl(REG_GPIOH_DIR) | (1 << (4+i))), REG_GPIOH_DIR);
        udelay(100);
        new_key |= ( (~(readl(REG_GPIOH_DATAIN) >> 8) & ((1 << NUC970_KPD_COL_NUMBER)- 1)) << (8*i) );
    }

    writel( ( readl(REG_GPIOH_DIR) | ( ((1 << NUC970_KPD_ROW_NUMBER) - 1) << 4 ) ), REG_GPIOH_DIR);

#endif

    for (i = 0; i < KEY_COUNT; i++) {

        if ((new_key ^ old_key) & (1 << i)) {// key state change
            if (new_key & (1 << i)) {
                //key down
                //printk("=== key down code[%d], 0x%x, 0x%x \n", i, new_key, nuc970_keymap[i]);
                KEY_PRESS_DEF(i); //Key Press Definitions or done here
                if(KEYPADMODE_state==2 || shift_press==2) {
                    input_event(nuc970_keypad_input_dev, EV_MSC, MSC_SCAN, 0);
                    input_report_key(nuc970_keypad_input_dev, KEY_LEFTSHIFT, 1);//LeftShift
                    input_sync(nuc970_keypad_input_dev);
                }
                if(backspace==1) {
                    input_event(nuc970_keypad_input_dev, EV_MSC, MSC_SCAN, 28);
                    input_report_key(nuc970_keypad_input_dev, KEY_BACKSPACE, 1);//Backspace
                    input_report_key(nuc970_keypad_input_dev, KEY_BACKSPACE, 0);//Backspace
                    input_sync(nuc970_keypad_input_dev);
                }
                input_event(nuc970_keypad_input_dev, EV_MSC, MSC_SCAN, i);
                input_report_key(nuc970_keypad_input_dev, nuc970_keymap[i], 1);
                input_sync(nuc970_keypad_input_dev);
                if(KEYPADMODE_state==2 || shift_press==2) {
                    input_event(nuc970_keypad_input_dev, EV_MSC, MSC_SCAN, 0);
                    input_report_key(nuc970_keypad_input_dev, KEY_LEFTSHIFT, 0);//LeftShift
                    input_sync(nuc970_keypad_input_dev);
                }
            } else {

                KEY_RELEASE_DEF(i);//Key release Definitions or down here
                //key up
                //printk("=== key up code[%d] 0x%x, 0x%x \n", i, nuc970_keymap[i], new_key);
                input_event(nuc970_keypad_input_dev, EV_MSC, MSC_SCAN, i);
                input_report_key(nuc970_keypad_input_dev, nuc970_keymap[i], 0);
                input_sync(nuc970_keypad_input_dev);
            }

        }

    }

    old_key = new_key;

    timer_active = 1;
    if ( arg == 0 )
        mod_timer(&kpd_timer, jiffies + DEF_KPD_DELAY*1); //### to avoid key too sensitive
    else
        mod_timer(&kpd_timer, jiffies + DEF_KPD_DELAY);
    return;

}

static void KEY_PRESS_DEF(int i) {
    /**********Buzzer Enable*********************/
    if(KEYPADMODE_buzzer_state == 1) {
        gpio_request(195,"Buzzer");
        gpio_direction_output(195,1);
        mdelay(20);
        gpio_direction_output(195,0);
    }
    /*******************************************/
    switch(i) {
    case 0:
        nuc970_keymap[i]=KEY_RESERVED;
        backspace=0;
        reset_key();
        break;
    case 1:
        nuc970_keymap[i]=KEY_ESC;
        backspace=0;
        reset_key();
        break;
    case 2:
        nuc970_keymap[i]=KEY_F5;
        backspace=0;
        reset_key();
        break;
    case 3:
        nuc970_keymap[i]=KEY_DOWN;
        backspace=0;
        reset_key();
        break;
    case 4:
        nuc970_keymap[i]=KEY_UP;
        backspace=0;
        reset_key();
        break;
    case 5:
        nuc970_keymap[i]=KEY_F14;
        backspace=0;
        reset_key();
        break;
    case 6:
        nuc970_keymap[i]=KEY_F1;
        backspace=0;
        reset_key();
        break;
    case 7:
        nuc970_keymap[i]=KEY_F6;
        backspace=0;
        reset_key();
        break;
    case 8:
        nuc970_keymap[i]=KEY_F11;
        backspace=0;
        reset_key();
        onesec = msecs_to_jiffies(1200 * 1);
        mod_timer(&tcatimer, jiffies + onesec);
        break;
    case 9:
        nuc970_keymap[i]=KEY_F13;
        backspace=0;
        reset_key();
        break;
    case 10:
        nuc970_keymap[i]=KEY_F15;
        backspace=0;
        reset_key();
        break;
    case 11:
        nuc970_keymap[i]=KEY_F2;
        backspace=0;
        reset_key();
        break;
    case 12:
        nuc970_keymap[i]=KEY_F7;
        backspace=0;
        reset_key();
        break;
    case 13:
        backspace=0;
        if(shift_press==0){
            nuc970_keymap[i]=KEY_1;
        } else {
            nuc970_keymap[i]=KEY_DOT;
        }
        break;
    case 14:
            rotate_handler=1;
            reset_not('a');
            if(rot_a==0){
                backspace=0;
                nuc970_keymap[i]=KEY_2;
            } else if(rot_a==1) {
                backspace=1;
                nuc970_keymap[i]=KEY_A;
            } else if(rot_a==2) {
                backspace=1;
                nuc970_keymap[i]=KEY_B;
            } else if(rot_a==3) {
                backspace=1;
                nuc970_keymap[i]=KEY_C;
            } else if(rot_a==4) {
		backspace=1;
		nuc970_keymap[i]=KEY_2;
	    }
        break;
    case 15:
            rotate_handler=1;
            reset_not('d');
            if(rot_d==0) {
                backspace=0;
                nuc970_keymap[i]=KEY_3;
            } else if(rot_d==1) {
                backspace=1;
                nuc970_keymap[i]=KEY_D;
            } else if(rot_d==2) {
                backspace=1;
                nuc970_keymap[i]=KEY_E;
            } else if(rot_d==3) {
                backspace=1;
                nuc970_keymap[i]=KEY_F;
            } else if(rot_d==4) {
		backspace=1;
		nuc970_keymap[i]=KEY_3;
	    }
        break;
    case 16:
        backspace=0;
        reset_key();
        nuc970_keymap[i]=KEY_F11;
        break;
    case 17:
        backspace=0;
        reset_key();
        nuc970_keymap[i]=KEY_F12;
        break;
    case 18:
        backspace=0;
        reset_key();
        nuc970_keymap[i]=KEY_F3;
        break;
    case 19:
        if(shift_press==0) {
            backspace=0;
            nuc970_keymap[i]=KEY_4;
        } else {
            rotate_handler=1;
            reset_not('g');
            if(rot_g==0) {
                backspace=0;
                nuc970_keymap[i]=KEY_G;
            } else if(rot_g==1) {
                backspace=1;
                nuc970_keymap[i]=KEY_H;
            } else if(rot_g==2) {
                backspace=1;
                nuc970_keymap[i]=KEY_I;
            } else if(rot_g==3) {
                backspace=1;
                nuc970_keymap[i]=KEY_G;
            }
        }
        break;
    case 20:
        if(shift_press==0) {
            backspace=0;
            nuc970_keymap[i]=KEY_5;
        } else {
            rotate_handler=1;
            reset_not('j');
            if(rot_j==0) {
                backspace=0;
                nuc970_keymap[i]=KEY_J;
            } else if(rot_j==1) {
                backspace=1;
                nuc970_keymap[i]=KEY_K;
            } else if(rot_j==2) {
                backspace=1;
                nuc970_keymap[i]=KEY_L;
            } else if(rot_j==3) {
                backspace=1;
                nuc970_keymap[i]=KEY_J;
            }
        }
        break;
    case 21:
        if(shift_press==0) {
            backspace=0;
            nuc970_keymap[i]=KEY_6;
        } else {
            rotate_handler=1;
            reset_not('m');
            if(rot_m==0) {
                backspace=0;
                nuc970_keymap[i]=KEY_M;
            } else if(rot_m==1) {
                backspace=1;
                nuc970_keymap[i]=KEY_N;
            } else if(rot_m==2) {
                backspace=1;
                nuc970_keymap[i]=KEY_O;
            } else if(rot_m==3) {
                backspace=1;
                nuc970_keymap[i]=KEY_M;
            }
        }
        break;
    case 22:
        backspace=0;
        reset_key();
        nuc970_keymap[i]=KEY_F13;
        if(shift_press==0) {
            shift_press++;
            touch_file("/usr/share/status/Asmall_status");
        } else if(shift_press==1) {
            shift_press++;
            touch_file("/usr/share/status/Abig_status");
        } else if(shift_press==2) {
            shift_press=0;
            touch_file("/usr/share/status/Numeric_status");
        }
        break;
    case 23:
        nuc970_keymap[i]=KEY_F4;
        backspace=0;
        reset_key();
        break;
    case 25:
        if(shift_press==0) {
            backspace=0;
            nuc970_keymap[i]=KEY_7;
        } else {
            rotate_handler=1;
            reset_not('p');
            if(rot_p==0) {
                backspace=0;
                nuc970_keymap[i]=KEY_P;
            } else if(rot_p==1) {
                backspace=1;
                nuc970_keymap[i]=KEY_Q;
            } else if(rot_p==2) {
                backspace=1;
                nuc970_keymap[i]=KEY_R;
            } else if(rot_p==3) {
                backspace=1;
                nuc970_keymap[i]=KEY_S;
            } else if(rot_p==4) {
                backspace=1;
                nuc970_keymap[i]=KEY_P;
            }
        }
        break;
    case 26:
        if(shift_press==0) {
            backspace=0;
            nuc970_keymap[i]=KEY_8;
        } else {
            rotate_handler=1;
            reset_not('t');
            if(rot_t==0) {
                backspace=0;
                nuc970_keymap[i]=KEY_T;
            } else if(rot_t==1) {
                backspace=1;
                nuc970_keymap[i]=KEY_U;
            } else if(rot_t==2) {
                backspace=1;
                nuc970_keymap[i]=KEY_V;
            } else if(rot_t==3) {
                backspace=1;
                nuc970_keymap[i]=KEY_T;
            }
        }
        break;
    case 27:
        if(shift_press==0) {
            backspace=0;
            nuc970_keymap[i]=KEY_9;
        } else {
            rotate_handler=1;
            reset_not('w');
            if(rot_w==0) {
                backspace=0;
                nuc970_keymap[i]=KEY_W;
            } else if(rot_w==1) {
                backspace=1;
                nuc970_keymap[i]=KEY_X;
            } else if(rot_w==2) {
                backspace=1;
                nuc970_keymap[i]=KEY_Y;
            } else if(rot_w==3) {
                backspace=1;
                nuc970_keymap[i]=KEY_Z;
            } else if(rot_w==4) {
                backspace=1;
                nuc970_keymap[i]=KEY_W;
            }
        }
        break;
    case 28:
        nuc970_keymap[i]=KEY_BACKSPACE;
        backspace=0;
        reset_key();
        break;
    case 29:
        nuc970_keymap[i]=KEY_DOT;
        backspace=0;
        reset_key();
        break;
    case 30:
        backspace=0;
        reset_key();
        if(shift_press==0) {
            nuc970_keymap[i]=KEY_0;
        } else {
            nuc970_keymap[i]=KEY_SPACE;
        }
        break;
    case 31:
        nuc970_keymap[i]=KEY_ENTER;
        backspace=0;
        reset_key();
        break;
    }
}

static void alpha_reset_key(unsigned long data)
{
    rot_a=0,rot_d=0,rot_g=0,rot_j=0,rot_m=0,rot_p=0,rot_t=0,rot_w=0;
}

static void reset_key(void)
{
    rot_a=0,rot_d=0,rot_g=0,rot_j=0,rot_m=0,rot_p=0,rot_t=0,rot_w=0;
}

static void KEY_RELEASE_DEF(int i) {
    switch(i) {
    case 0:
        reset_key();
        nuc970_keymap[i]=KEY_RESERVED;
        break;
    case 1:
        reset_key();
        nuc970_keymap[i]=KEY_F8;
        break;
    case 2:
        reset_key();
        nuc970_keymap[i]=KEY_F5;
        break;
    case 3:
        reset_key();
        nuc970_keymap[i]=KEY_F9;
        break;
    case 4:
        reset_key();
        nuc970_keymap[i]=KEY_UP;
        break;
    case 5:
        reset_key();
        nuc970_keymap[i]=KEY_F7;
        break;
    case 6:
        reset_key();
        nuc970_keymap[i]=KEY_TAB;
        break;
    case 7:
        reset_key();
        nuc970_keymap[i]=KEY_F1;
        break;
    case 8:
        reset_key();
        del_timer(&tcatimer); //Esc Key
        nuc970_keymap[i]=KEY_ESC;
        break;
    case 9:
        reset_key();
        nuc970_keymap[i]=KEY_DOWN;
        break;
    case 10:
        reset_key();
        nuc970_keymap[i]=KEY_F10;
        break;
    case 11:
        reset_key();
        nuc970_keymap[i]=KEY_F6;
        break;
    case 12:
        reset_key();
        nuc970_keymap[i]=KEY_F2;
        break;
    case 13:
        if(shift_press==0) {
            nuc970_keymap[i]= KEY_1;
        } else {
            nuc970_keymap[i]= KEY_COMMA;
        }
        break;
    case 14:
        if(shift_press==0) {
            nuc970_keymap[i]= KEY_2;
        } else {
            rotate_handler=1;
            if(rot_a==0) {
                nuc970_keymap[i]=KEY_A;
                rot_a++;
            } else if(rot_a==1) {
                nuc970_keymap[i]=KEY_B;
                rot_a++;
            } else if(rot_a==2) {
                nuc970_keymap[i]=KEY_C;
                rot_a++;
            } else if(rot_a==3) {
                nuc970_keymap[i]=KEY_A;
                rot_a=1;
            }
        }
        break;
    case 15:
        if(shift_press==0) {
            nuc970_keymap[i]=KEY_3;
        } else {
            if(rot_d==0) {
                nuc970_keymap[i]=KEY_D;
                rot_d++;
            } else if(rot_d==1) {
                nuc970_keymap[i]=KEY_E;
                rot_d++;
            } else if(rot_d==2) {
                nuc970_keymap[i]=KEY_F;
                rot_d++;
            } else if(rot_d==3) {
                nuc970_keymap[i]=KEY_D;
                rot_d=1;
            }
        }
        break;
    case 16:
        reset_key();
        nuc970_keymap[i]=KEY_F11;
        break;
    case 17:
        reset_key();
        nuc970_keymap[i]=KEY_F12;
        break;
    case 18:
        reset_key();
        nuc970_keymap[i]=KEY_F3;
        break;
    case 19:
        if(shift_press==0) {
            nuc970_keymap[i]=KEY_4;
        } else {
            rotate_handler=1;
            if(rot_g==0) {
                nuc970_keymap[i]=KEY_G;
                rot_g++;
            } else if(rot_g==1) {
                nuc970_keymap[i]=KEY_H;
                rot_g++;
            } else if(rot_g==2) {
                nuc970_keymap[i]=KEY_I;
                rot_g++;
            } else if(rot_g==3) {
                nuc970_keymap[i]=KEY_G;
                rot_g=1;
            }
        }
        break;
    case 20:
        if(shift_press==0) {
            nuc970_keymap[i]=KEY_5;
        } else {
            rotate_handler=1;
            if(rot_j==0) {
                nuc970_keymap[i]=KEY_J;
                rot_j++;
            } else if(rot_j==1) {
                nuc970_keymap[i]=KEY_K;
                rot_j++;
            } else if(rot_j==2) {
                nuc970_keymap[i]=KEY_L;
                rot_j++;
            } else if(rot_j==3) {
                nuc970_keymap[i]=KEY_J;
                rot_j=1;
            }
        }
        break;
    case 21:
        if(shift_press==0) {
            nuc970_keymap[i]=KEY_6;
        } else {
            rotate_handler=1;
            if(rot_m==0) {
                nuc970_keymap[i]=KEY_M;
                rot_m++;
            } else if(rot_m==1) {
                nuc970_keymap[i]=KEY_N;
                rot_m++;
            } else if(rot_m==2) {
                nuc970_keymap[i]=KEY_O;
                rot_m++;
            } else if(rot_m==3) {
                nuc970_keymap[i]=KEY_M;
                rot_m=1;
            }
        }
        break;
    case 22:
        nuc970_keymap[i]=KEY_F13;
        break;
    case 23:
        reset_key();
        nuc970_keymap[i]=KEY_F4;
        break;
    case 24:
        reset_key();
        nuc970_keymap[i]=KEY_RESERVED;
        break;
    case 25:
        if(shift_press==0) {
            nuc970_keymap[i]=KEY_7;
        } else {
            rotate_handler=1;
            if(rot_p==0) {
                nuc970_keymap[i]=KEY_P;
                rot_p++;
            } else if(rot_p==1) {
                nuc970_keymap[i]=KEY_Q;
                rot_p++;
            } else if(rot_p==2) {
                nuc970_keymap[i]=KEY_R;
                rot_p++;
            } else if(rot_p==3) {
                nuc970_keymap[i]=KEY_S;
                rot_p++;
            } else if(rot_p==4) {
                nuc970_keymap[i]=KEY_P;
                rot_p=1;
            }
        }
        break;
    case 26:
        if(shift_press==0) {
            nuc970_keymap[i]=KEY_8;
        } else {
            rotate_handler=1;
            if(rot_t==0) {
                nuc970_keymap[i]=KEY_T;
                rot_t++;
            } else if(rot_t==1) {
                nuc970_keymap[i]=KEY_U;
                rot_t++;
            } else if(rot_t==2) {
                nuc970_keymap[i]=KEY_V;
                rot_t++;
            } else if(rot_t==3) {
                nuc970_keymap[i]=KEY_T;
                rot_t=1;
            }
        }
        break;
    case 27:
        if(shift_press==0) {
            nuc970_keymap[i]=KEY_9;
        } else {
            rotate_handler=1;
            if(rot_w==0) {
                nuc970_keymap[i]=KEY_W;
                rot_w++;
            } else if(rot_w==1) {
                nuc970_keymap[i]=KEY_X;
                rot_w++;
            } else if(rot_w==2) {
                nuc970_keymap[i]=KEY_Y;
                rot_w++;
            } else if(rot_w==3) {
                nuc970_keymap[i]=KEY_Z;
                rot_w++;
            } else if(rot_w==4) {
                nuc970_keymap[i]=KEY_W;
                rot_w=1;
            }
        }
        break;
    case 28:
        reset_key();
        nuc970_keymap[i]=KEY_BACKSPACE;
        break;
    case 29:
        reset_key();
        nuc970_keymap[i]=KEY_DOT;
        break;
    case 30:
        rotate_handler=1;
        reset_key();
        if(shift_press == 1 || shift_press == 2) {
            nuc970_keymap[i]=KEY_SPACE;
        } else if(shift_press==0) {
            nuc970_keymap[i]=KEY_0;
        }
        break;
    case 31:
        reset_key();
        nuc970_keymap[i]=KEY_ENTER;
        break;
    }
}

static void reset_not(char data)
{
    switch(data) {
    case 'a':
        rot_d=0,rot_g=0,rot_j=0,rot_m=0,rot_p=0,rot_t=0,rot_w=0;
        break;
    case 'd':
        rot_a=0,rot_g=0,rot_j=0,rot_m=0,rot_p=0,rot_t=0,rot_w=0;
        break;
    case 'j':
        rot_a=0,rot_d=0,rot_g=0,rot_m=0,rot_p=0,rot_t=0,rot_w=0;
        break;
    case 'g':
        rot_a=0,rot_d=0,rot_j=0,rot_m=0,rot_p=0,rot_t=0,rot_w=0;
        break;
    case 'm':
        rot_a=0,rot_d=0,rot_g=0,rot_j=0,rot_p=0,rot_t=0,rot_w=0;
        break;
    case 'p':
        rot_a=0,rot_d=0,rot_g=0,rot_j=0,rot_m=0,rot_t=0,rot_w=0;
        break;
    case 't':
        rot_a=0,rot_d=0,rot_g=0,rot_j=0,rot_m=0,rot_p=0,rot_w=0;
        break;
    case 'w':
        rot_a=0,rot_d=0,rot_g=0,rot_j=0,rot_m=0,rot_p=0,rot_t=0;
        break;
    }
}

static void timeout_handler(unsigned long data)
{
    touch_file("/usr/share/status/SHUTDOWN_status");
    gpio_request(195,"Buzzer");
    gpio_direction_output(195,1);
    mdelay(50);
    gpio_direction_output(195,0);
    mdelay(50);
    gpio_direction_output(195,1);
    mdelay(50);
    gpio_direction_output(195,0);
}

static void touch_file(char *filename)
{
    int fd;
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);

    fd = sys_open(filename, O_CREAT, 0666);

    if (fd >= 0) {
        sys_close(fd);
    }
    set_fs(old_fs);
}


static irqreturn_t nuc970_kpd_irq(int irq, void *dev_id) 
{
    read_key(0);

    return IRQ_HANDLED;
}

static irqreturn_t kpi_interrupr_handle__(int irq, void *dev_id)
{
    // clear ISR
    writel(readl(REG_GPIOH_ISR) | ( ((1 << 12) - 1) << 4), REG_GPIOH_ISR);
    
    return IRQ_HANDLED;
}


int nuc970_kpd_open(struct input_dev *dev)
{
    u32 i;
    int error = 0;

    if (open_cnt > 0) {
        goto exit;
    }

    new_key = old_key = 0;

    // init timer
    init_timer(&kpd_timer);
    kpd_timer.function = read_key;	/* timer handler */
    kpd_timer.data = 1;

#if defined CONFIG_NUC970_KEYPAD_PH
    writel(readl(REG_GPIOH_ISR), REG_GPIOH_ISR); // clear source

    for(i = 0; i < NUC970_KPD_COL_NUMBER; i++)
    {
        error =  request_irq((IRQ_GPIO_START+NUC970_PH8+i), nuc970_kpd_irq, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND, "Keypad",NULL);
        if (error) {
            printk("register the PH%d keypad_irq failed!  0x%x  0x%x \n", (8+i), (IRQ_GPIO_START+NUC970_PH8+i), NUC970_PH8);
            return -EAGAIN;
        }
    }

    for(i = 0; i < NUC970_KPD_ROW_NUMBER; i++)
    {
        if(request_irq((IRQ_GPIO_START+NUC970_PH4+i),kpi_interrupr_handle__, IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND, "Keypad",NULL) != 0){
            printk("register the PH%d keypad_irq failed!  0x%x \n", (4+i), (IRQ_GPIO_START+NUC970_PH4+i) );
            return -1;
        }

        enable_irq_wake((IRQ_GPIO_START+NUC970_PH4+i));
        disable_irq_nosync((IRQ_GPIO_START+NUC970_PH4+i));
    }

#endif

exit:
    open_cnt++;
    return 0;
}



void nuc970_kpd_close(struct input_dev *dev)
{
    u32 i;

    open_cnt--;
    if (open_cnt == 0)
    {
        //disable interrupt
#if defined CONFIG_NUC970_KEYPAD_PH
        for(i = 0; i < CONFIG_COL_NUMBER; i++)
        {
            free_irq(gpio_to_irq(NUC970_PH8+i),NULL);
        }

#endif
    }
    return;
}


static int nuc970_keypad_probe(struct platform_device *pdev)
{
    int i, err;

#ifdef CONFIG_OF
    if (pdev->dev.of_node) {
        of_property_read_u32_array(pdev->dev.of_node, "row", &NUC970_KPD_ROW_NUMBER, 1);
        of_property_read_u32_array(pdev->dev.of_node, "col", &NUC970_KPD_COL_NUMBER, 1);
    }
#else
    NUC970_KPD_COL_NUMBER = CONFIG_COL_NUMBER;
    NUC970_KPD_ROW_NUMBER = CONFIG_ROW_NUMBER;
#endif

    if(NUC970_KPD_COL_NUMBER > 8)  NUC970_KPD_COL_NUMBER = 8;
    if(NUC970_KPD_COL_NUMBER > 4)  NUC970_KPD_ROW_NUMBER = 4;

    // init GPIO
#if defined CONFIG_NUC970_KEYPAD_PH
    // Set Column
    writel(readl(REG_GPIOH_DIR) & ~( ((1 << NUC970_KPD_COL_NUMBER) - 1) << 8), REG_GPIOH_DIR); // input
    writel(readl(REG_GPIOH_PUEN) | ( ((1 << NUC970_KPD_COL_NUMBER) - 1) << 8), REG_GPIOH_PUEN); // pull-up

    // Set Row
    writel(readl(REG_GPIOH_DIR) | ( ((1 << NUC970_KPD_ROW_NUMBER) - 1) << 4), REG_GPIOH_DIR);  // output
    writel(readl(REG_GPIOH_PUEN) | ( ((1 << NUC970_KPD_ROW_NUMBER) - 1) << 4), REG_GPIOH_PUEN); // pull up
    writel(readl(REG_GPIOH_DATAOUT) & ~( ((1 << NUC970_KPD_ROW_NUMBER) - 1) << 4), REG_GPIOH_DATAOUT); // low

    writel(readl(REG_GPIOH_DBEN) | ( ((1 << NUC970_KPD_COL_NUMBER) - 1) << 8), REG_GPIOH_DBEN);
#endif

    writel(0x2f, REG_GPIO_DBNCECON); // De-bounce sampling cycle select 32768 clock

    if (!(nuc970_keypad_input_dev = input_allocate_device())) {
        printk("NUC970 Keypad Drvier Allocate Memory Failed!\n");
        err = -ENOMEM;
        goto fail;
    }

    nuc970_keypad_input_dev->name = "nuc970-kpi"; //"NUC970_Keypad";
    nuc970_keypad_input_dev->phys = "input/event1";
    nuc970_keypad_input_dev->id.bustype = BUS_HOST;
    nuc970_keypad_input_dev->id.vendor  = 0x0005;
    nuc970_keypad_input_dev->id.product = 0x0001;
    nuc970_keypad_input_dev->id.version = 0x0100;

    nuc970_keypad_input_dev->open    = nuc970_kpd_open;
    nuc970_keypad_input_dev->close   = nuc970_kpd_close;

    nuc970_keypad_input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_SYN) |  BIT(EV_REP);

    for (i = 0; i < KEY_MAX; i++)
        set_bit(i+1, nuc970_keypad_input_dev->keybit);

    err = input_register_device(nuc970_keypad_input_dev);
    if (err) {
        input_free_device(nuc970_keypad_input_dev);
        return err;
    }
    // must set after input device register!!!
    nuc970_keypad_input_dev->rep[REP_DELAY] = 1000; //1000ms
    nuc970_keypad_input_dev->rep[REP_PERIOD] = 100; //ms

    proc_win_init();

    return 0;

fail:
    input_free_device(nuc970_keypad_input_dev);
    return err;
}

static int nuc970_keypad_remove(struct platform_device *pdev)
{
    u32 i;

#if defined CONFIG_NUC970_KEYPAD_PH
    for(i = 0; i < NUC970_KPD_COL_NUMBER; i++)
    {
        free_irq(gpio_to_irq(NUC970_PH8+i),NULL);
    }

#endif
    proc_win_exit();
    platform_set_drvdata(pdev, NULL);
    input_free_device(nuc970_keypad_input_dev);

    return 0;
}

#define nuc970_keypad_suspend 	NULL
#define nuc970_keypad_resume	NULL

static const struct of_device_id nuc970_kpi_of_match[] = {
{ .compatible = "nuvoton,nuc970-kpi" },
{},
};
MODULE_DEVICE_TABLE(of, nuc970_kpi_of_match);

static struct platform_driver nuc970_keypad_driver = {
    .probe		= nuc970_keypad_probe,
    .remove		= nuc970_keypad_remove,
    .suspend	= nuc970_keypad_suspend,
    .resume		= nuc970_keypad_resume,
    .driver		= {
        .name	= "nuc970-kpi",
        .owner	= THIS_MODULE,
        .of_match_table = of_match_ptr(nuc970_kpi_of_match),
    },
};
module_platform_driver(nuc970_keypad_driver);

MODULE_AUTHOR("nuvoton");
MODULE_DESCRIPTION("nuc970 keypad driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:nuc970-keypad");
