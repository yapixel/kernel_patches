#ifndef KSU_SUSFS_DEF_H
#define KSU_SUSFS_DEF_H

#include <linux/bits.h>
#include <linux/cred.h>
#include <linux/sched.h>

/********/
/* ENUM */
/********/
/* Shared with userspace ksu_susfs tool - The command "switchboard" */
#define SUSFS_MAGIC 0xFAFAFAFA
#define CMD_SUSFS_ADD_SUS_PATH 0x55550
#define CMD_SUSFS_ADD_SUS_PATH_LOOP 0x55553
#define CMD_SUSFS_HIDE_SUS_MNTS_FOR_NON_SU_PROCS 0x55561
#define CMD_SUSFS_ADD_SUS_KSTAT 0x55570
#define CMD_SUSFS_UPDATE_SUS_KSTAT 0x55571
#define CMD_SUSFS_ADD_SUS_KSTAT_STATICALLY 0x55572
#define CMD_SUSFS_SET_UNAME 0x55590
#define CMD_SUSFS_ENABLE_LOG 0x555a0
#define CMD_SUSFS_SET_CMDLINE_OR_BOOTCONFIG 0x555b0
#define CMD_SUSFS_ADD_OPEN_REDIRECT 0x555c0
#define CMD_SUSFS_SHOW_VERSION 0x555e1
#define CMD_SUSFS_SHOW_ENABLED_FEATURES 0x555e2
#define CMD_SUSFS_SHOW_VARIANT 0x555e3
#define CMD_SUSFS_ENABLE_AVC_LOG_SPOOFING 0x60010
#define CMD_SUSFS_ADD_SUS_MAP 0x60020

/* Buffer Limits */
#define SUSFS_MAX_LEN_PATHNAME 256
#define SUSFS_FAKE_CMDLINE_OR_BOOTCONFIG_SIZE 8192
#define SUSFS_ENABLED_FEATURES_SIZE 8192
#define SUSFS_MAX_VERSION_BUFSIZE 16
#define SUSFS_MAX_VARIANT_BUFSIZE 16

/* Mount Management Constants */
#define TRY_UMOUNT_DEFAULT 0
#define TRY_UMOUNT_DETACH 1
#define DEFAULT_KSU_MNT_ID 500000 
#define DEFAULT_SUS_MNT_ID_FOR_KSU_PROC_UNSHARE 1000000 
#define DEFAULT_KSU_MNT_GROUP_ID 5000 
#define VFSMOUNT_MNT_FLAGS_KSU_UNSHARED_MNT 0x80000000

/* * Bit Definitions for i_state and thread_info.
 * Using high bits (33+) ensures zero conflict with Sultan's 6.1 standard flags.
 */
#define TIF_PROC_UMOUNTED 33

#define AS_FLAGS_SUS_PATH 33
#define AS_FLAGS_SUS_MOUNT 34
#define AS_FLAGS_SUS_KSTAT 35
#define AS_FLAGS_OPEN_REDIRECT 36
#define AS_FLAGS_ANDROID_DATA_ROOT_DIR 37
#define AS_FLAGS_SDCARD_ROOT_DIR 38
#define AS_FLAGS_SUS_MAP 39

#define BIT_SUS_PATH BIT(33)
#define BIT_SUS_MOUNT BIT(34)
#define BIT_SUS_KSTAT BIT(35)
#define BIT_OPEN_REDIRECT BIT(36)
#define BIT_ANDROID_DATA_ROOT_DIR BIT(37)
#define BIT_ANDROID_SDCARD_ROOT_DIR BIT(38)
#define BIT_SUS_MAPS BIT(39)

/* Nameidata (nd) state and flags */
#define ND_STATE_LOOKUP_LAST 32
#define ND_STATE_OPEN_LAST 64
#define ND_STATE_LAST_SDCARD_SUS_PATH 128
#define ND_FLAGS_LOOKUP_LAST 0x2000000
 
#define MAGIC_MOUNT_WORKDIR "/debug_ramdisk/workdir"

/*******************/
/* INLINE HELPERS  */
/*******************/

/* Checks if the current process is already unmounted (Stealth Mode) */
static inline bool susfs_is_current_proc_umounted(void) {
	return test_ti_thread_flag(&current->thread_info, TIF_PROC_UMOUNTED);
}

static inline void susfs_set_current_proc_umounted(void) {
	set_ti_thread_flag(&current->thread_info, TIF_PROC_UMOUNTED);
}

/* Specifically checks for apps (UID >= 10000) that are umounted */
static inline bool susfs_is_current_proc_umounted_app(void) {
	return (test_ti_thread_flag(&current->thread_info, TIF_PROC_UMOUNTED) &&
			current_uid().val >= 10000);
}

/* * Fast-path macros for the De-Inlined hooks in namei.c and open.c.
 * These use atomic bit tests for maximum performance.
 */
#define PRE_CHECK_OPEN_REDIRECT_WITHOUT_UID_CHECK(inode) \
		(inode && unlikely(test_bit(AS_FLAGS_OPEN_REDIRECT, &inode->i_state)))

#define PRE_CHECK_OPEN_REDIRECT(inode) \
		(inode && unlikely(test_bit(AS_FLAGS_OPEN_REDIRECT, &inode->i_state)) && \
		susfs_is_current_proc_umounted_app())

#endif /* KSU_SUSFS_DEF_H */
