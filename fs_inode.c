#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/aio.h>
#include <linux/buffer_head.h>
#include <linux/mpage.h>

#include "fs_ia253.h"

fs_addr_ops = {
	.readpage	= simple_readpage,
	.write_begin	= simple_write_begin,
	.write_end	= simple_write_end,
	.set_page_dirty	= __set_page_dirty_no_writeback,
};

file_inode_ops = {
	.setattr	= simple_setattr,
	.getattr	= simple_getattr,
};

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
