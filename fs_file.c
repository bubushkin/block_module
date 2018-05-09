#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/ramfs.h>
#include <linux/sched.h>

#include "fs_ia253.h"

fs_file_ops = {
		.read_iter	= generic_file_read_iter,
		.write_iter	= generic_file_write_iter,
		.mmap		= generic_file_mmap,
		.fsync		= noop_fsync,
		.splice_read	= generic_file_splice_read,
		.splice_write	= iter_file_splice_write,
		.llseek		= generic_file_llseek,
		.get_unmapped_area	= fs_mmu_get_unmapped_area
};

static unsigned long fs_mmu_get_unmapped_area(struct file *file, unsigned long addr, unsigned long len, unsigned long pgoff,unsigned long flags){
	return current->mm->get_unmapped_area(file, addr, len, pgoff, flags);
}
