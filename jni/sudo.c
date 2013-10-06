#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <jni.h>

#include "pimage.h"

static processimage process;

int
sustem(char *cmd) {
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

jboolean
Java_koneu_rootapp_MainActivity_sudo(JNIEnv *env, jobject obj, jstring xmd) {
	char const * const cmd = (*env)->GetStringUTFChars(env, xmd, 0), *rmd;
	jboolean ret = JNI_TRUE;
	if(process.pid) {
		for(rmd = cmd; *rmd; ++rmd) {
			if(write(process.infd, rmd, 1) != 1)
				ret = JNI_FALSE;
		}
		if(write(process.infd, "\n", 1) != 1)
			ret = JNI_FALSE;
	} else if(sustem(cmd))
			ret = JNI_FALSE;
	
	(*env)->ReleaseStringUTFChars(env, xmd, cmd);
	return ret;
}

void
Java_koneu_rootapp_MainActivity_startshell(JNIEnv *env, jobject obj) {
	if(!process.pid)
		mkprocess("su", &process, 1, 0, 0);
}

void
Java_koneu_rootapp_MainActivity_closeshell(JNIEnv *env, jobject obj) {
	if(process.pid) {
		rmprocess(&process);
	}
}
