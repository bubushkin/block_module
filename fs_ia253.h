
#define DRIVER_NAME "fs_ia253"

#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/kdev_t.h>
#include<linux/module.h>
#include<linux/types.h>
#include<linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/types.h>

/*
 * helpers
 */
#define PDEBUG(fmt,args...) printk(KERN_DEBUG"%s:"fmt,DRIVER_NAME, ##args)
#define PERR(fmt,args...) printk(KERN_ERR"%s:"fmt,DRIVER_NAME,##args)
#define PINFO(fmt,args...) printk(KERN_INFO"%s:"fmt,DRIVER_NAME, ##args)
#define PCRIT(fmt,args...) printk(KERN_CRIT"%s:"fmt,DRIVER_NAME, ##args)

#define IA253_MAGIC_NUM 0xBBCC99
#define IA253_DEFAULT_MODE	0755

struct file_system_type fs_ia253_type;

enum {
	Opt_mode,
	Opt_err
};

static const match_table_t tokens = {
	{Opt_mode, "mode=%o"},
	{Opt_err, NULL}
};


struct fs_meta_param{

	char *fs_name;
	char *kcache_name;
	char *module_name;

};

/*
 * Super block
 */

struct super_operations fs_super_ops;

int ia253_init_super(struct super_block *, void *, int);
struct dentry *fs_mount(struct file_system_type *, int, const char *, void *);
void fs_kill_sb(struct super_block *);


struct fs_mount_opts {
	umode_t mode;
};

struct fs_fs_info {
	struct fs_mount_opts mount_opts;
};


int fs_show_proc_mode(struct seq_file *, struct dentry *);


struct address_space_operations fs_addr_ops;
struct inode_operations file_inode_ops;
struct file_operations fs_file_ops;
struct inode_operations fs_dir_inode_ops;
/*
 * inode
 */
struct inode *fs_get_inode(struct super_block *, const struct inode *, umode_t, dev_t);
