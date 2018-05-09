#include <linux/fs.h>
#include "fs_ia253.h"

struct dentry *fs_mount(struct file_system_type *pa_fs_type, int aflags, const char *pa_dev_name, void *pa_data){
	return mount_nodev(pa_fs_type, aflags, pa_data, ia253_init_super);
}

void fs_kill_sb(struct super_block *pa_sb){
	kfree(pa_sb->s_fs_info);
	kill_litter_super(pa_sb);
}
