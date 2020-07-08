#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "pimage.h"

static processimage process;

int
libsu_system(char *cmd) {
	int ret = -1;
	pid_t child;
	
	switch((child = fork())) {
		case -1:
			return -1;
			
		case 0:
			return execlp("su", "su", "-c", cmd, NULL);
			
		default:
			waitpid(child, &ret, 0);
	}
	
	return ret;
}

bool libsu_sudo(const char * cmd) {
        size_t len = strlen(cmd);
	bool ret = true;
	if(process.pid) {
		if (write(process.infd, cmd, len) != len) ret = JNI_FALSE;
		if (write(process.infd, "\n", 1) != 1) ret = JNI_FALSE;
	} else if(libsu_system(cmd)) ret = JNI_FALSE;
	(*env)->ReleaseStringUTFChars(env, xmd, cmd);
	return ret;
}

void libsu_startshell(JNIEnv *env, jobject obj) {
	if(!process.pid)
		mkprocess("su", &process, 1, 0, 0);
}

void libsu_closeshell(JNIEnv *env, jobject obj) {
	if(process.pid) {
		rmprocess(&process);
	}
}
