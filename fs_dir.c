#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/backing-dev.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/magic.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "fs_ia253.h"

fs_dir_inode_ops = {
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


static int fs_create(struct inode *pa_dir, struct dentry *pa_dentry, umode_t amode, bool aexcl){
	return fs_mk_inod(pa_dir, pa_dentry, amode | S_IFREG, 0);
}


static int fs_symlink(struct inode * pa_dir, struct dentry *pa_dentry, const char * pasymname){

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


static int fs_mkdir(struct inode * pa_dir, struct dentry * pa_dentry, umode_t amode){
	int retval = fs_mk_inode(pa_dir, pa_dentry, amode | S_IFDIR, 0);
	if (!retval)
		inc_nlink(pa_dir);
	return retval;
}


static int fs_mk_inode(struct inode *pa_dir, struct dentry *pa_dentry, umode_t amode, dev_t adev){

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
