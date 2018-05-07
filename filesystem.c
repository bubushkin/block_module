#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include "fs_ia253.h"


fs_ia253_type = {
      .owner = THIS_MODULE,
      .name = "fs_ia253",
      .mount = fs_mount,
      .kill_sb = kill_block_super,
      .fs_flags = FS_REQUIRES_DEV,
};

struct dentry *fs_mount(struct file_system_type *pa_type, int aflags, char const *padev, void *padata){
        
	struct dentry *p_dentry = mount_bdev(pa_type, aflags, padev, padata, aufs_fill_sb);

        if (IS_ERR(entry)){
                pr_err("%s mounting failed\n", &p_fs_params->module_name);
	}
        else{
                pr_debug("%s successfully mounted\n", &p_fs_params->module_name);
        }
	return p_dentry;
}

static void fs_release_all(void)
{
    rcu_barrier();
    kmem_cache_destroy(inode_kcache);
}
