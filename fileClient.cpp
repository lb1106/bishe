#include "common.h"
int main(int argc,char **argv){
    int clientfd;
    if (argc != 2)
    {
		fprintf(stderr,"Usage: ./fileClient <IP address>");
		return 1;
    }
    struct sockaddr_in clientaddr;
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = htons(INADDR_ANY);  //Communicate with any host
    clientaddr.sin_port = htons(0);                  //htons to transfer little endian to big endian,ports to monitor
    clientfd=socket(AF_INET,SOCK_STREAM,0);        //SOCK_STREAM : TCP protocol
	if (clientfd < 0)
	{
		perror("socket:");
		return 1;
	}
	int opt=1;
	setsockopt(clientfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	if(bind(clientfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 0)
	{
		perror("bind");
		return 1;
    }
    struct sockaddr_in svraddr;  
    bzero(&svraddr,sizeof(svraddr));  
    if(inet_aton(argv[1],&svraddr.sin_addr)==0)  
    {  
        perror("inet_aton");  
        exit(1);  
    }  
    svraddr.sin_family=AF_INET;  
    svraddr.sin_port=htons(PORT);  
      
    socklen_t svraddrlen=sizeof(svraddr);  
    if(connect(clientfd,(struct sockaddr*)&svraddr,svraddrlen)<0)      
    {  
        perror("connect");  
        exit(1);  
    }  
    else
    printf("connect succeed");
    //recv file imformation  
    char buff[BUFFSIZE];  
    char filename[FILE_NAME_MAX_SIZE] = "hah.py"; 
    int count;  
    bzero(buff,BUFFSIZE);  
	
    count=recv(clientfd,buff,BUFFSIZE,0);  
    if(count<0)  
    {  
        perror("recv");  
        exit(1);  
    }  
    //strncpy(filename,buff,strlen(buff)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buff));  
  
    printf("Preparing recv file : %s from %s \n",filename,argv[1]);   
  
    //recv file  
    FILE *fd=fopen(filename,"wb+");  
    if(NULL==fd)  
    {  
        perror("open");  
        exit(1);  
    }  
    bzero(buff,BUFFSIZE);  
  
    int length=0;  
    while(length=recv(clientfd,buff,BUFFSIZE,0))  
    {  
        if(length<0)  
        {  
            perror("recv");  
            exit(1);  
        }  
        int writelen=fwrite(buff,sizeof(char),length,fd);  
        if(writelen<length)  
        {  
            perror("write");  
            exit(1);  
        }  
        bzero(buff,BUFFSIZE);  
    }  
    printf("Receieved file:%s from %s finished!\n",filename,argv[1]);  
    fclose(fd);  
    close(clientfd);  
    return 0;  
}

