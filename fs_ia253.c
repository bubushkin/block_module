/*
===============================================================================
Driver Name		:		fa_ia253
Author			:		ISKANDAR ASKAROV
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/
#include "fs_ia253.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ISKANDAR ASKAROV");

struct fs_meta_param p_fsmeta = {
    .fs_name = "fs_ia253",
    .kcache_name = "ia253_inode",
    .module_name = "fs_ia253",

};

fs_ia253_type = {
      .owner = THIS_MODULE,
      .name = &p_fsmeta->fs_name,
      .mount = fs_mount,
      .kill_sb = fs_kill_sb,
      .fs_flags = FS_USERNS_MOUNT,
};


static int __init fa_ia253_init(void){

	/* TODO Auto-generated Function Stub */

	int retcode = 0;

	retcode = register_filesystem(&fs_ia253_type);
	if(retcode != 0){
		PERR("Failed to register filesystem %s with kernel\n", &p_fsmeta->fs_name);
		return retcode;
	}

	PINFO("Successfuly registered filesystem %s. Enjoy!\n", &p_fsmeta->fs_name);

	return retcode;
}

static void __exit fa_ia253_exit(void){

	/* TODO Auto-generated Function Stub */

	int ret = unregister_filesystem(&fs_ia253_type);

    if (ret != 0)
        pr_err("cannot unregister filesystem\n");

    aufs_inode_cache_destroy();

    PDEBUG("%s module unloaded\n", &p_fsmeta->module_name);

}

module_init(fa_ia253_init);
module_exit(fa_ia253_exit);

