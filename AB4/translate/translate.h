
#ifndef TRANSLATE_H_
#define TRANSLATE_H_
#include <pthread.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

/* #include <asm/system.h>		/\* cli(), *_flags *\/ */
#include <asm/uaccess.h>	/* copy_*_user */

#define BUFFERSIZE	40

struct translator {
	char * buffer[BUFFERSIZE];
	pthread_mutex_t

};

translate_open()

#endif /* TRANSLATE_H_ */
