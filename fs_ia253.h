#ifndef __FSIA253_H__
#define __FSIA253_H__

#include <linux/fs.h>
#include <linux/slab.h>

const unsigned long FSIA253_MAGIC_NUM = 0xBBBBCCCC9999

struct file_system_type fs_ia253_type;
struct file_operations fs_ia253_file_ops;

struct kmem_cache *inode_kcache;


struct fs_param{

	char *fs_name;
	char *inode_name;
	char *module_name;
	char *module_author;
	char *author_email;

} p_fs_params;


struct fs_inode {
	struct inode fs_inode;
	unsigned long fsblock;
};

struct fs_ia253_sb {

	unsigned long inode_blocks;
	unsigned long block_size;
	unsigned long magic_num;
	unsigned long root;
	unsigned long inodes_per_blk;

};

/*
	filesystem
*/
void fs_inode_init(void *);
struct dentry *fs_mount(struct file_system_type *, int, char const *, void *);


/*
	superblock
*/

int init_superblock(struct super_block *, void *, int)



/*
	cache initializers/destructors
*/
int inode_cache_alloc(void);
void fs_release_all(void);


#endif
