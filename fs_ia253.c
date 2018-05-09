#include <linux/mm.h>
#include <linux/ramfs.h>
#include <linux/sched.h>
#include <linux/aio.h>
#include <linux/backing-dev.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/highmem.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/magic.h>
#include <linux/module.h>
#include <linux/mpage.h>
#include <linux/pagemap.h>
#include <linux/parser.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include "fs_ia253.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ISKANDAR ASKAROV");
MODULE_DESCRIPTION("CS680 Homework-9");
MODULE_VERSION("0.01");

struct file_system_type fs_ia253_type = {
	      .owner = THIS_MODULE,
	      .name = "fs_ia253",
	      .mount = fs_mount,
	      .kill_sb = kill_block_super,
	      .fs_flags = FS_REQUIRES_DEV,
};

struct super_operations fs_super_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= fs_show_proc_mode,

};

struct dentry *fs_mount(struct file_system_type *pa_fs_type, int aflags, const char *pa_dev_name, void *pa_data){
	return mount_nodev(pa_fs_type, aflags, pa_data, ia253_init_super);
}
EXPORT_SYMBOL(fs_mount);


struct file_operations fs_file_ops = {
		.read_iter	= generic_file_read_iter,
		.write_iter	= generic_file_write_iter,
		.mmap		= generic_file_mmap,
		.fsync		= noop_fsync,
		.splice_read	= generic_file_splice_read,
		.splice_write	= iter_file_splice_write,
		.llseek		= generic_file_llseek,
		.get_unmapped_area	= fs_mmu_get_unmapped_area
};


struct inode_operations file_inode_ops = {
	.setattr	= simple_setattr,
	.getattr	= simple_getattr,
};


struct address_space_operations fs_addr_ops = {
	.readpage	= simple_readpage,
	.write_begin	= simple_write_begin,
	.write_end	= simple_write_end,
	.set_page_dirty	= set_page_dirty,
};


struct inode_operations fs_dir_inode_ops = {
	.create		= fs_create,
	.lookup		= simple_lookup,
	.link		= simple_link,
	.unlink		= simple_unlink,
	.symlink	= fs_symlink,
	.mkdir		= fs_mkdir,
	.rmdir		= simple_rmdir,
	.mknod		= fs_mk_inode,
	.rename		= simple_rename,
};


int fs_symlink(struct inode * pa_dir, struct dentry *pa_dentry, const char * pasymname){

	struct inode *p_inode;
	int error = -ENOSPC;

	p_inode = fs_get_inode(pa_dir->i_sb, pa_dir, S_IFLNK|S_IRWXUGO, 0);
	if (p_inode) {
		int l = strlen(pasymname)+1;
		error = page_symlink(p_inode, pasymname, l);
		if (!error) {
			d_instantiate(pa_dentry, p_inode);
			dget(pa_dentry);
			pa_dir->i_mtime = pa_dir->i_ctime = current_time(pa_dir);
		} else
			iput(p_inode);
	}
	return error;
}
EXPORT_SYMBOL(fs_symlink);

int fs_mkdir(struct inode * pa_dir, struct dentry * pa_dentry, umode_t amode){
	int retval = fs_mk_inode(pa_dir, pa_dentry, amode | S_IFDIR, 0);
	if (!retval)
		inc_nlink(pa_dir);
	return retval;
}
EXPORT_SYMBOL(fs_mkdir);

int fs_mk_inode(struct inode *pa_dir, struct dentry *pa_dentry, umode_t amode, dev_t adev){

	struct inode * p_inode = fs_get_inode(pa_dir->i_sb, pa_dir, amode, adev);
	int error = -ENOSPC;

	if (p_inode) {
		d_instantiate(pa_dentry, p_inode);
		dget(pa_dentry);
		error = 0;
		pa_dir->i_mtime = pa_dir->i_ctime = current_time(pa_dir);
	}
	return error;
}
EXPORT_SYMBOL(fs_mk_inode);

unsigned long fs_mmu_get_unmapped_area(struct file *file, unsigned long addr, unsigned long len, unsigned long pgoff,unsigned long flags){
	return current->mm->get_unmapped_area(file, addr, len, pgoff, flags);
}
EXPORT_SYMBOL(fs_mmu_get_unmapped_area);


struct inode *fs_get_inode(struct super_block *pa_sb, const struct inode *pa_dir, umode_t amode, dev_t adev){

	struct inode * p_inode = new_inode(pa_sb);

	if (p_inode) {
		p_inode->i_ino = get_next_ino();
		inode_init_owner(p_inode, pa_dir, amode);
		p_inode->i_mapping->a_ops = &fs_addr_ops;
		mapping_set_gfp_mask(p_inode->i_mapping, GFP_HIGHUSER);
		mapping_set_unevictable(p_inode->i_mapping);
		p_inode->i_atime = p_inode->i_mtime = p_inode->i_ctime = current_time(p_inode);
		switch (amode & S_IFMT) {
		default:
			init_special_inode(p_inode, amode, adev);
			break;
		case S_IFREG:
			p_inode->i_op = &file_inode_ops;
			p_inode->i_fop = &fs_file_ops;
			break;
		case S_IFDIR:
			p_inode->i_op = &fs_dir_inode_ops;
			p_inode->i_fop = &simple_dir_operations;

			inc_nlink(p_inode);
			break;
		case S_IFLNK:
			p_inode->i_op = &page_symlink_inode_operations;
			inode_nohighmem(p_inode);
			break;
		}
	}
	return p_inode;
}


void fs_kill_sb(struct super_block *pa_sb){
	kfree(pa_sb->s_fs_info);
	kill_litter_super(pa_sb);
}
EXPORT_SYMBOL(fs_kill_sb);


int ia253_init_super(struct super_block *pa_sb, void *adata, int asilent){

	struct fs_fs_info *p_fsi;
	struct inode *p_inode;
	int err;

	p_fsi = kzalloc(sizeof(struct fs_fs_info), GFP_KERNEL);
	pa_sb->s_fs_info = p_fsi;
	if (!p_fsi)
		return -ENOMEM;

	err = fs_parse_options(adata, &p_fsi->mount_opts);
	if (err)
		return err;

	pa_sb->s_maxbytes		= MAX_LFS_FILESIZE;
	pa_sb->s_blocksize		= PAGE_SIZE;
	pa_sb->s_blocksize_bits	= PAGE_SHIFT;
	pa_sb->s_magic		= IA253_MAGIC_NUM;
	pa_sb->s_op		= &fs_super_ops;
	pa_sb->s_time_gran		= 1;

	p_inode = fs_get_inode(pa_sb, NULL, S_IFDIR | p_fsi->mount_opts.mode, 0);
	pa_sb->s_root = d_make_root(p_inode);
	if (!pa_sb->s_root)
		return -ENOMEM;

	return 0;
}

static const match_table_t tokens = {
	{Opt_mode, "mode=%o"},
	{Opt_err, NULL}
};


int fs_parse_options(char *adata, struct fs_mount_opts *pa_opts){

	substring_t args[MAX_OPT_ARGS];
	int option;
	int token;
	char *p;

	pa_opts->mode = IA253_DEFAULT_MODE;

	while ((p = strsep(&adata, ",")) != NULL) {
		if (!*p){
			continue;
		}
		token = match_token(p, tokens, args);

		switch (token) {
		case Opt_mode:
			if (match_octal(&args[0], &option))
				return -EINVAL;
			pa_opts->mode = option & S_IALLUGO;
			break;
		}
	}

	return 0;
}



int fs_show_proc_mode(struct seq_file *pa_m, struct dentry *pa_root){
	struct fs_fs_info *p_fsi = pa_root->d_sb->s_fs_info;

	if (p_fsi->mount_opts.mode != IA253_DEFAULT_MODE)
		seq_printf(pa_m, ",mode=%o", p_fsi->mount_opts.mode);
	return 0;
}
EXPORT_SYMBOL(fs_show_proc_mode);


int fs_create(struct inode *pa_dir, struct dentry *pa_dentry, umode_t amode, bool aexcl){
	return fs_mk_inode(pa_dir, pa_dentry, amode | S_IFREG, 0);
}
EXPORT_SYMBOL(fs_create);


static int __init fs_ia253_init(void) {
	/* TODO Auto-generated Function Stub */

	int retcode = 0;
	printk(KERN_INFO "Filesystem registration returned %d\n", retcode);
	retcode = register_filesystem(&fs_ia253_type);
	printk(KERN_INFO "Filesystem registration returned %d\n", retcode);
	if(retcode != 0){
		PDEBUG("Failed to register filesystem fs_ia253 with kernel\n");
		return retcode;
	}

	PINFO("Successfuly registered filesystem fs_ia253. Enjoy!\n");

	return retcode;
}

static void __exit fs_ia253_exit(void) {
	/* TODO Auto-generated Function Stub */

	int ret = unregister_filesystem(&fs_ia253_type);
	PDEBUG("Unloaded\n");
    if (ret != 0)
        pr_err("cannot unregister filesystem\n");

    PDEBUG("fs_ia253 module unloaded\n");
}
module_init(fs_ia253_init);
module_exit(fs_ia253_exit);
