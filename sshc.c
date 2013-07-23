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
	fd_set readFDs;
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

	//// Device Search
	//int max_rsp,num_rsp,len,flags,dev_id,i;
	//char buf[1024] = {0};
	//int found = 0;
	//inquiry_info *ii = NULL;
	//len = 8;
	//max_rsp = 255;
	//flags = IREQ_CACHE_FLUSH;
	//dev_id = hci_get_route(NULL);
	//ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
	//while(1){
	//	num_rsp = hci_inquiry(dev_id,len,max_rsp,NULL,&ii,flags);
	//	syslog(LOG_INFO,"num_rsp: %d",num_rsp);
	//	sleep(1);
	//	for(i = 0; i < num_rsp; i++){
	//		ba2str(&(ii+i)->bdaddr,buf);
	//		syslog(LOG_INFO,"bdaddr: %s",buf);
	//		syslog(LOG_INFO,"dest: %s",dest);
	//		if(strcmp(dest,buf) == 0){
	//			found = 1;
	//			break;
	//		}
	//		sleep(1);
	//	}
	//	if(found){
	//		break;
	//	}
	//	sleep(5);
	//}



	syslog(LOG_INFO,"Socket Connect ");
	int rc;
	int	status = connect(btSocket,(struct sockaddr*)&addr,sizeof(addr));

	syslog(LOG_INFO,"Socket status: %d",status);
	if(status<0){
		syslog(LOG_INFO,"Socket Connect Failure");
		exit(EXIT_FAILURE);	
	}
	//if(errno == EINPROGRESS){
	//	syslog(LOG_INFO,"EINPROGRESS");
	//	for(;;){
	//		FD_ZERO(&writeFDs);
	//		FD_ZERO(&readFDs);
	//		FD_SET(btSocket,&writeFDs);
	//		FD_SET(btSocket,&readFDs);
	//		rc = select(btSocket+1,&readFDs,&writeFDs,NULL,NULL);

	//		if(FD_ISSET(btSocket,&writeFDs)){
	//			syslog(LOG_INFO,"Write");
	//			if(FD_ISSET(btSocket,&readFDs)){
	//				syslog(LOG_INFO,"Read and Write");
	//				sleep(5);
	//				btSocket = socket(AF_BLUETOOTH, SOCK_STREAM|SOCK_NONBLOCK ,BTPROTO_RFCOMM);
	//				status = connect(btSocket,(struct sockaddr*)&addr,sizeof(addr));
	//		    	continue;		
	//			} else{
	//				break;	
	//			}
	//		}
	//	}
	//}

	//syslog(LOG_INFO,"OUT OF EINPROGRESS");
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
