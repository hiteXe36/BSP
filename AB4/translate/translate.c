#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>        // Required for the copy to user function
#include <linux/moduleparam.h>
#include <linux/slab.h>		
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>

/* #include <asm/system.h>		/\* cli(), *_flags *\/ */
#include <asm/uaccess.h>	/* copy_*_user */

#include <linux/cdev.h>             //cdev representiert char devices intern 

#define DEV_NAME "trans"    ///< The device will appear at /dev/trans using this value
#define DEV_NO 2
#define BUFFERSIZE 40

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("PM, DS");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character Device Driver Task for OS MODULE HAW Hamburg");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    shift = 3;
static int    majorNumber = 0;                  ///< Stores the device number -- determined automatically

struct trans_device {
    struct cdev myChrDev;
    int    minorNumber;
    char   message[BUFFERSIZE];
    //TODO mutex
};

struct trans_device *trans0, *trans1;

// The prototype functions for the character driver -- must come before the struct definition
static int     trans_open(struct inode *, struct file *);
static int     trans_release(struct inode *, struct file *);
static ssize_t trans_read(struct file *, char *, size_t, loff_t *);
static ssize_t trans_write(struct file *, const char *, size_t, loff_t *);

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
    .open = NULL,
    .read = NULL,
    .write = NULL,
    .release = NULL,
};

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void transCharD_exit(void){
    printk(KERN_INFO "EXIT/CLEANUP FUNCTION");
    cdev_del(&trans0->myChrDev);
    cdev_del(&trans1->myChrDev);
    
    kfree(trans0);
    kfree(trans1);
    
    unregister_chrdev_region(MKDEV(majorNumber,0), DEV_NO);
    
    //TODO 
    printk(KERN_INFO "TransChar: Goodbye from the LKM!\n");
}

int initTransDev(struct trans_device * dev, int minorNumber)
{
    int err, devno;
    //TODO: MUTEX
    dev->minorNumber = minorNumber;
    err = devno = MKDEV(majorNumber, minorNumber);
    
    cdev_init(&dev->myChrDev, &fops);
    dev->myChrDev.owner = THIS_MODULE;
    dev->myChrDev.ops = &fops;
    err = cdev_add(&dev->myChrDev, devno, 1);
    
    /* Fail gracefully if need be */
    if (err)
        printk(KERN_NOTICE "Error %d adding device %d", err, minorNumber);
}

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int transCharD_init(void){
    dev_t dev = 0;
    int res;
    printk(KERN_INFO "Initializing the LKM\n");

   if(majorNumber)
   {
       dev = MKDEV(majorNumber, 0);
       res = register_chrdev_region(dev, DEV_NO, DEV_NAME);
   } else {
       res = alloc_chrdev_region(&dev, 0, DEV_NO, DEV_NAME);
       majorNumber = MAJOR(dev);
   }
   
   if(res < 0)
   {
       printk(KERN_WARNING "trans: cant get a majorNumber %d\n", majorNumber);
       return res;
   }
   
   trans0 = (struct trans_device *) kmalloc(sizeof(struct trans_device), GFP_KERNEL);
   trans1 = (struct trans_device *) kmalloc(sizeof(struct trans_device), GFP_KERNEL);
   
   if (!trans0 || !trans1) {
      res = -ENOMEM;
      transCharD_exit();
      return(res);
   }
   
   initTransDev(trans0, 0);
   initTransDev(trans1, 1);

   
   return 0;
}


/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(transCharD_init);
module_exit(transCharD_exit);
