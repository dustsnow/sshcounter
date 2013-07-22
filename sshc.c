#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth.h>
#include <rfcomm.h>
#include <hci.h>
#include <hci_lib.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
int main(int argc, char** argv){
	fd_set writeFDs;
	struct sockaddr_rc addr={0};
	char dest[] = "00:12:02:09:04:90";
	openlog("sshcounter",LOG_PID,LOG_USER);
	pid_t pid,sid;
	pid = fork();
	if(pid < 0){
		syslog(LOG_INFO,"Fork Failture");
		exit(EXIT_FAILURE);	
	}
	if(pid > 0){
		exit(EXIT_SUCCESS);	
	}
	umask(0);
	sid = setsid();
	if(sid < 0){
		syslog(LOG_INFO,"Seting Session ID Failure");
		exit(EXIT_FAILURE);	
	}
	if((chdir("/")) < 0){
		syslog(LOG_INFO,"Change Directory Failure");
		exit(EXIT_FAILURE);	
	}
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	int btSocket = socket(AF_BLUETOOTH, SOCK_STREAM|SOCK_NONBLOCK ,BTPROTO_RFCOMM);
	if(btSocket < 0){
		syslog(LOG_INFO,"Socket Creating Failure");
		exit(EXIT_FAILURE);	
	}
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t)1;
	str2ba(dest,&addr.rc_bdaddr);
	int rc;
	int	status = connect(btSocket,(struct sockaddr*)&addr,sizeof(addr));
	//if(errno == EINPROGRESS){
	//	for(;;){
	//		FD_ZERO(&writeFDs);
	//		FD_SET(btSocket,&writeFDs);
	//		rc = select(btSocket+1,NULL,&writeFDs,NULL,NULL);
	//		if(rc > 0 && FD_ISSET(btSocket,&writeFDs)){
	//				break;	
	//		}
	//	}
	//}

	char numConnections;
	while(1){
		FILE* file = popen("netstat -an | grep -E '\\:22[ \\t]+' | grep ESTABLISHED | wc -l","r");
		if(file == NULL){
			syslog(LOG_INFO,"POPEN Failure");
			exit(EXIT_FAILURE);	
		}
		fscanf(file, "%c",&numConnections);
		fclose(file);
		write(btSocket, &numConnections, 1);
		sleep(1);
	}
	close(btSocket);
	exit(EXIT_SUCCESS);
}
