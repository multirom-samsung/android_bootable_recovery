#ifndef RECOVERY_TWINSTALL_H_
#define RECOVERY_TWINSTALL_H_

int TWinstall_zip(const char* path, int* wipe_cache, bool check_for_digest = false, int mr_skip_comp_verify = 0);

#endif  // RECOVERY_TWINSTALL_H_
