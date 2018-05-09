#include <linux/fs.h>
#include <linux/parser.h>
#include "fs_ia253.h"

fs_super_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= fs_show_proc_mode,
};

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

static int fs_parse_options(char *adata, struct fs_mount_opts *pa_opts){

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
