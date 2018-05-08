#ifndef __FSIA253_H__
#define __FSIA253_H__

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/types.h>

const unsigned long FSIA253_MAGIC_NUM = 0xBBBBCCCC9999

struct file_system_type fs_ia253_type;
struct file_operations fs_ia253_file_ops;

struct kmem_cache *inode_kcache;


struct meta_disk_inode {
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		__le32	e_i_first;
		__le32	e_i_blocks;
		__le32	e_i_size;
		__le32	e_i_gid;
		__le32	e_i_uid;
		__le32	e_i_mode;
		__le64	e_i_ctime;
	#else
		__be32	e_i_first;
		__be32	e_i_blocks;
		__be32	e_i_size;
		__be32	e_i_gid;
		__be32	e_i_uid;
		__be32	e_i_mode;
		__be64	e_i_ctime;
	#endif
};


/*
	filesystem
*/
void fs_inode_init(void *);
struct dentry *fs_mount(struct file_system_type *, int, char const *, void *);

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

/*
	superblock
*/

struct super_operations ia253_super_ops;

int init_superblock(struct super_block *, void *, int)
struct fs_ia253_sb *init_logical_block(struct super_block *);

struct meta_disk_sb {
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		__le32	e_magic_num;
		__le32	e_block_size;
		__le32	e_root_inode;
		__le32	e_inode_blocks;
	#else
		__be32	e_magic_num;
		__be32	e_block_size;
		__be32	e_root_inode;
		__be32	e_inode_blocks;
	#endif
};

struct fs_ia253_sb { //logical block structure!

	unsigned long inode_blocks;
	unsigned long block_size;
	unsigned long magic_num;
	unsigned long root;
	unsigned long inode_blocks;
};

/*
	cache initializers/destructors
*/
int inode_cache_alloc(void);
void fs_release_all(void);

#endif
