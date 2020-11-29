#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

/* Proxy routine having the same arguments as actual sys_openat() routine */
int jdo_openat(int dfd, const char *filename, int flags, mode_t mode)
{
	char s[1024];

	sprintf(s,"[OPEN] dfd :%d,flags :%d,mode :%x,filename:%s \n", dfd, flags, mode, filename);
        printk(s);

	/* Always end with a call to jprobe_return(). */
	jprobe_return();
	return 0;
}

static struct jprobe jprobe_openat = {
	.entry			= jdo_openat,
	.kp = {
		.symbol_name	= "sys_openat",
	},
};


static int __init jprobe_init(void)
{
	int ret;

	ret = register_jprobe(&jprobe_openat);
	if (ret < 0) {
		pr_info("register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	pr_info("Planted jprobe at %p, handler addr %p\n",
	       jprobe_openat.kp.addr, jprobe_openat.entry);
	return 0;
}

static void __exit jprobe_exit(void)
{
	unregister_jprobe(&jprobe_openat);
	pr_info("Unregistered openat jprobe\n");
}

module_init(jprobe_init)
module_exit(jprobe_exit)
MODULE_LICENSE("GPL");
