/* Prototype module for second mandatory DM510 assignment */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#include "../Headers/morse_led.h"
#include "../Headers/morse_buffer.h"
#include "../Headers/morse_thread.h"

/* Prototypes - this would normally go in a .h file */
int morse_init_module(void);
void morse_cleanup_module(void);
static int morse_open(struct inode*, struct file*);
static int morse_release(struct inode*, struct file*);
static ssize_t morse_write(struct file*, const char __user*, size_t, loff_t*);
static long morse_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

#define DEVICE_NAME "morse_dev" /* Dev name as it appears in /proc/devices */
#define MAJOR_NUMBER 255

/* Global state */

////makes sure only one person can access the device ata time
static int device_open_count = 0;
static struct mutex open_mutex;

//////shared circular buffer used by the thread
static struct morse_buffer *shared_ev_buff = NULL;

/* file operations struct */
static struct file_operations morse_fops = {
	.owner   = THIS_MODULE,
	.write   = morse_write,
	.open    = morse_open,
	.release = morse_release,
    .unlocked_ioctl = morse_ioctl
};

/* called when module is loaded */
int morse_init_module(void) {

	int result;

	/* initialization code belongs here */
/////registers the character device
	result = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &morse_fops);
	if (result < 0) {
		printk(KERN_ALERT "Morse: Failed to register device\n");
		return result;
	}
////puts the synchronization into place
	mutex_init(&open_mutex);
	led_init();

  ////allocates the (circle) buffer  
	shared_ev_buff = kmalloc(sizeof(struct morse_buffer), GFP_KERNEL);
	if (!shared_ev_buff) {
		unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
		return -ENOMEM;
	}

	circlebuff_init(shared_ev_buff, MORSE_BUFFER_MAX);

	///start kernel thread 
	morse_thread_start(shared_ev_buff);

	printk(KERN_INFO "Morse: Module loaded\n");
	return 0;
}

/* Called when module is unloaded */
void morse_cleanup_module(void) {

	/* clean up code belongs here */

	morse_thread_stop();
	led_cleanup();


  ///free buffer
	if (shared_ev_buff) {
		kfree(shared_ev_buff);
	}

	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);

	printk(KERN_INFO "Morse: Module unloaded.\n");
}

/* Called when a process tries to open the device file */
static int morse_open(struct inode *inode, struct file *filp) {
	
	/* device claiming code belongs here */


  /////exclusice access;
	mutex_lock(&open_mutex);

	if (device_open_count > 0) {
		mutex_unlock(&open_mutex);
		return -EBUSY;
	}

	device_open_count++;

	mutex_unlock(&open_mutex);
	return 0;
}

/* Called when a process closes the device file. */
static int morse_release(struct inode *inode, struct file *filp) {

	/* device release code belongs here */


  ///release exclusive access
	mutex_lock(&open_mutex);
	device_open_count--;
	mutex_unlock(&open_mutex);
		
	return 0;
}

/* Called when a process writes to dev file */
static ssize_t morse_write(struct file *filp,
    const char __user *buf, /* The buffer to get data from      */
    size_t count,           /* The max number of bytes to write */
    loff_t *f_pos )         /* The offset in the file           */
{
	char *k_buf;
	int i;

	extern struct mutex morse_buff_mutex;
	extern wait_queue_head_t morse_wait_queue;

	/* write code belongs here */	

	if (count == 0)
		return 0;


  //////allocate the ((kernel)) buffer
	k_buf = kmalloc(count, GFP_KERNEL);
	if (!k_buf)
		return -ENOMEM;

	if (copy_from_user(k_buf, buf, count)) {
		kfree(k_buf);
		return -EFAULT;
	}


  ////insert characters into the circle buffer
	mutex_lock(&morse_buff_mutex);
	for (i = 0; i < count; i++) {
		circlebuff_add(shared_ev_buff, k_buf[i]);
	}
	mutex_unlock(&morse_buff_mutex);

	/* wake up thread */
	wake_up_interruptible(&morse_wait_queue);

	kfree(k_buf);
	return count;
}

/* called by system call ioctl */ 
static long morse_ioctl(struct file *filp, 
    unsigned int cmd,   /* command passed from the user */
    unsigned long arg ) /* argument of the command */
{
	/* ioctl code belongs here */

	if (cmd == 0) {
		morse_thread_set_unit((int)arg);
		return 0;
	}

	return -ENOTTY;
}

module_init(morse_init_module);
module_exit(morse_cleanup_module);

MODULE_AUTHOR("...Your names here. Do not delete the three dots in the beginning.");
MODULE_LICENSE("GPL");
