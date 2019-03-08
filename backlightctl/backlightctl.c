/*
===============================================================================
Driver Name		:		baclightctl
Author			:		GOKUL
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
 */

#include"backlightctl.h"

#define BACLIGHTCTL_N_MINORS 1
#define BACLIGHTCTL_FIRST_MINOR 0
#define BACLIGHTCTL_NODE_NAME "backlightctl"
#define BACLIGHTCTL_BUFF_SIZE 1024
#define DATA_BUFF_SIZE  16

static unsigned char langprintdata[DATA_BUFF_SIZE];
int i, j, count=0;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GOKUL");

int baclightctl_major=0;

dev_t baclightctl_device_num;

struct class *baclightctl_class;

typedef struct privatedata {
	int nMinor;

	char buff[BACLIGHTCTL_BUFF_SIZE];

	struct cdev cdev;

	atomic_t singlepro;		

	struct device *baclightctl_device;

} baclightctl_private;

baclightctl_private devices[BACLIGHTCTL_N_MINORS];

static int baclightctl_open(struct inode *inode,struct file *filp)
{
	/* TODO Auto-generated Function */

	baclightctl_private *priv = container_of(inode->i_cdev ,
			baclightctl_private ,cdev);
	filp->private_data = priv;

	PINFO("In char driver open() function\n");

	if(!atomic_dec_and_test(&priv->singlepro)) {
		atomic_inc(&priv->singlepro);
		return -EBUSY;
	}

	return 0;
}					

static int baclightctl_release(struct inode *inode,struct file *filp)
{
	/* TODO Auto-generated Function */

	baclightctl_private *priv;

	priv=filp->private_data;

	PINFO("In char driver release() function\n");

	atomic_inc(&priv->singlepro);

	return 0;
}

static ssize_t baclightctl_read(struct file *filp, 
		char __user *ubuff,size_t count,loff_t *offp)
{
	/* TODO Auto-generated Function */

	int n=0;

	baclightctl_private *priv;

	priv = filp->private_data;

	PINFO("In char driver read() function\n");

	return n;
}

static ssize_t baclightctl_write(struct file *file, const char __user *buffer, size_t count, loff_t *off) {
	memcpy(langprintdata, buffer, DATA_BUFF_SIZE);
	copy_from_user(langprintdata,buffer,count);
	//	for(i=0;i<count;i++) {
	//		printk("%c",langprintdata[i]);
	//	}

	if(count==2){
		switch(langprintdata[0]){
		case '0':
			printk("Backlight Level 0\n");
			gpio_direction_output(34,0);
			printk("Pulse Given: %d\n",j);
			break;
		case '1':
			printk("Backlight Level 1\n");
			gpio_direction_output(34,0);
			for(j=0;j<12;j++){
				gpio_direction_output(34,1);
				udelay(2);
				gpio_direction_output(34,0);
				udelay(1);
			}
			gpio_direction_output(34,1);
			udelay(500);
			printk("Pulse Given: %d\n",j);
			break;
		case '2':
			printk("Backlight Level 2\n");
			gpio_direction_output(34,0);
			for( j=0;j<9;j++){
				gpio_direction_output(34,1);
				udelay(2);
				gpio_direction_output(34,0);
				udelay(1);
			}
			gpio_direction_output(34,1);
			udelay(500);
			printk("Pulse Given: %d\n",j);
			break;
		case '3':
			printk("Backlight Level 3\n");
			gpio_direction_output(34,0);
			for( j=0;j<6;j++){
				gpio_direction_output(34,1);
				udelay(2);
				gpio_direction_output(34,0);
				udelay(1);
			}
			gpio_direction_output(34,1);
			udelay(500);
			printk("Pulse Given: %d\n",j);
			break;
		case '4':
			printk("Backlight Level 4\n");
			gpio_direction_output(34,0);
			for( j=0;j<3;j++){
				gpio_direction_output(34,1);
				udelay(2);
				gpio_direction_output(34,0);
				udelay(1);
			}
			gpio_direction_output(34,1);
			udelay(500);
			printk("Pulse Given: %d\n",j);
			break;
		case '5':
			printk("Backlight Level 5\n");
			gpio_direction_output(34,0);
			for( j=0;j<1;j++){
				gpio_direction_output(34,1);
				udelay(2);
				gpio_direction_output(34,0);
				udelay(1);
			}
			gpio_direction_output(34,1);
			udelay(500);
			printk("Pulse Given: %d\n",j);
			break;
		default:
			printk("Invalid Value: Backlight value 0 to 5 \n");
			break;
		}
	}else {
		printk("Invalid Value: Backlight value 0 to 5 \n");
	}
	return count;
}

static const struct file_operations baclightctl_fops= {
		.owner				= THIS_MODULE,
		.open				= baclightctl_open,
		.release			= baclightctl_release,
		.read				= baclightctl_read,
		.write				= baclightctl_write,
};

static int __init baclightctl_init(void)
{
	/* TODO Auto-generated Function Stub */

	int i;
	int res;

	int ret_request = gpio_request(34,"Backlight Control");
	int ret_export = gpio_export(34, true);

	printk("Return Request: %d\n", ret_request);
	printk("Return Export: %d\n", ret_export);

	if(ret_request<0 && ret_export<0){
	int i;

	PINFO("EXIT\n");

	for(i=0;i<BACLIGHTCTL_N_MINORS;i++) {
		baclightctl_device_num= MKDEV(baclightctl_major ,BACLIGHTCTL_FIRST_MINOR+i);

		cdev_del(&devices[i].cdev);

		device_destroy(baclightctl_class ,baclightctl_device_num);

	}

	class_destroy(baclightctl_class);

	unregister_chrdev_region(baclightctl_device_num ,BACLIGHTCTL_N_MINORS);	
		PINFO("Unable to INIT\n");
	} else {

	res = alloc_chrdev_region(&baclightctl_device_num,BACLIGHTCTL_FIRST_MINOR,BACLIGHTCTL_N_MINORS ,DRIVER_NAME);
	if(res) {
		PERR("register device no failed\n");
		return -1;
	}
	baclightctl_major = MAJOR(baclightctl_device_num);

	baclightctl_class = class_create(THIS_MODULE , DRIVER_NAME);
	if(!baclightctl_class) {
		PERR("class creation failed\n");
		return -1;
	}

	for(i=0;i<BACLIGHTCTL_N_MINORS;i++) {
		baclightctl_device_num= MKDEV(baclightctl_major ,BACLIGHTCTL_FIRST_MINOR+i);
		cdev_init(&devices[i].cdev , &baclightctl_fops);
		cdev_add(&devices[i].cdev,baclightctl_device_num,1);

		devices[i].baclightctl_device  = 
				device_create(baclightctl_class , NULL ,baclightctl_device_num ,
						NULL ,BACLIGHTCTL_NODE_NAME"%d",BACLIGHTCTL_FIRST_MINOR+i);
		if(!devices[i].baclightctl_device) {
			class_destroy(baclightctl_class);
			PERR("device creation failed\n");
			return -1;
		}

		devices[i].singlepro = (atomic_t)ATOMIC_INIT(1);

		devices[i].nMinor = BACLIGHTCTL_FIRST_MINOR+i;
	}
	PINFO("INIT\n");

	}


	

	return 0;
}

static void __exit baclightctl_exit(void)
{	
	/* TODO Auto-generated Function Stub */

	int i;

	PINFO("EXIT\n");

	for(i=0;i<BACLIGHTCTL_N_MINORS;i++) {
		baclightctl_device_num= MKDEV(baclightctl_major ,BACLIGHTCTL_FIRST_MINOR+i);

		cdev_del(&devices[i].cdev);

		device_destroy(baclightctl_class ,baclightctl_device_num);

	}

	class_destroy(baclightctl_class);

	unregister_chrdev_region(baclightctl_device_num ,BACLIGHTCTL_N_MINORS);	

}

module_init(baclightctl_init);
module_exit(baclightctl_exit);

