#include "common.h"  
#include <stdlib.h>
#include <sys/time.h>
#include <linux/tcp.h>
int tim_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
	int nsec;
	if ( x->tv_sec > y->tv_sec )
		return   -1;
    if ((x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec))
		return   -1;
	result->tv_sec = ( y->tv_sec-x->tv_sec );
	result->tv_usec = ( y->tv_usec-x->tv_usec );
	if (result->tv_usec<0)
	{
		result->tv_sec--;
		result->tv_usec+=1000000;}
		return   0;
}
int main(int argc, char *argv[])  
{  
    //Input the file name  
    char filename[FILE_NAME_MAX_SIZE]="/home/liubin/TCPFile/";  
    char temp_filename[FILE_NAME_MAX_SIZE];
    bzero(filename,FILE_NAME_MAX_SIZE);  
    printf("Please input the file name you wana to send:");  
    scanf("%s",temp_filename);  
    strcat(filename,temp_filename);
    getchar();  
  
    //Create socket  
    int sockfd,connfd;  
    struct sockaddr_in svraddr,clientaddr;  
    bzero(&svraddr,sizeof(svraddr));  
      
    svraddr.sin_family=AF_INET;  
    svraddr.sin_addr.s_addr=htonl(INADDR_ANY);  
    svraddr.sin_port=htons(PORT);  
  
    sockfd=socket(AF_INET,SOCK_STREAM,0);  
    if(sockfd<0)  
    {  
        perror("socket");  
        exit(1);  
    }  
	int opt=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr))<0)  
    {  
        perror("bind");  
        exit(1);  
    }  
    
    
    
    int snd_buf_size=1024*1024;
    /*if(setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,&snd_buf_size,sizeof(int))<0)
		perror("setsockopt error:");
		*/
    socklen_t opt1=4;
    if(getsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,&snd_buf_size,&opt1)<0)
		perror("getsockopt error:");
	int flag=1;
	if(setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(int))<0)
		perror("disable nagle error:");
    printf("send buf size is %d\n",snd_buf_size);
    
    
    //listen  
    if(listen(sockfd,LISTENQ)<0)  
    {  
        perror("listen");  
        exit(1);  
    }  
  
    while(1)  
    {  
        socklen_t length=sizeof(clientaddr);  
  
        //accept  
        connfd=accept(sockfd,(struct sockaddr*)&clientaddr,&length);  
        if(connfd<0)  
        {  
            perror("connect");  
            exit(1);  
        }  
		struct timeval start,stop,diff;
		gettimeofday(&start,0);
        
        //send file imformation  
        char buff[BUFFSIZE];  
        int count;  
        bzero(buff,BUFFSIZE);  
        buff[0]=strlen(temp_filename);
        strncpy(buff+1,temp_filename,strlen(temp_filename)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(temp_filename));  
        count=send(connfd,buff,strlen(temp_filename)+1,0);  
        printf("count is :%d %d\n",count,strlen(filename)+1);
        if(count<0)  
        {  
            perror("Send file information");  
            exit(1);  
        }  
          
        //read file   
        FILE *fd=fopen(filename,"rb");  
        if(fd==NULL)  
        {  
            printf("File :%s not found!\n",filename);  
        }  
        else   
        {  
            bzero(buff,BUFFSIZE);  
            int file_block_length=0;  
            while((file_block_length=fread(buff,sizeof(char),BUFFSIZE,fd))>0)  
            {  
                printf("file_block_length:%d\n",file_block_length);  
                if(send(connfd,buff,file_block_length,0)<0)  
                {  
                    perror("Send");  
                    exit(1);  
                }  
                bzero(buff,BUFFSIZE);     
            }  
            fclose(fd);  
            printf("Transfer file finished !\n");  
            gettimeofday(&stop,0);
            tim_subtract(&diff,&start,&stop);
            printf("total time: %d(ms)\n",diff.tv_usec);
        }  
        close(connfd);  
    }  
    close(sockfd);  
    return 0;  
} 
