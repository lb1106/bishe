#include "common.h"
#include <iostream>
#include <string>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <linux/tcp.h>
	
using namespace std;


int view_pcd_file(std::string filename)
{
	typedef pcl::PointXYZRGBA PointT;
	pcl::PointCloud<PointT>::Ptr cloud (new pcl::PointCloud<PointT>);

	std::string dir = "/home/liubin/TCPFile/recv/";

	if (pcl::io::loadPCDFile<PointT> ((dir+filename), *cloud) == -1){
//* load the file
	PCL_ERROR ("Couldn't read PCD file \n");
	return (-1);
	}
	printf("Loaded %d data points from PCD\n",
	cloud->width * cloud->height);

	for (size_t i = 0; i < cloud->points.size (); i+=10000)
		printf("%8.3f %8.3f %8.3f %5d %5d %5d %5d\n",
		cloud->points[i].x,
		cloud->points[i].y,
		cloud->points[i].z,
		cloud->points[i].r,
		cloud->points[i].g,
		cloud->points[i].b,
		cloud->points[i].a
		);

	pcl::visualization::PCLVisualizer viewer("Cloud viewer");
	viewer.setCameraPosition(0,0,-3.0,0,-1,0);
	viewer.addCoordinateSystem(0.3);
	viewer.addPointCloud(cloud);
	while(!viewer.wasStopped())
		viewer.spinOnce(100);
	return 0;
}


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
    
    
    int rcv_buf_size=1024*1024;
    /*if(setsockopt(clientfd,SOL_SOCKET,SO_RCVBUF,&rcv_buf_size,sizeof(int))<0)
		perror("setsockopt error:");
	*/	
    socklen_t opt1=4;
    if(getsockopt(clientfd,SOL_SOCKET,SO_RCVBUF,&rcv_buf_size,&opt1)<0)
		perror("getsockopt error:");
    printf("rcv buf size is %d\n",rcv_buf_size);
    int flag=1;
	if(setsockopt(clientfd,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(int))<0)
		perror("disable nagle error:");
      
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
    char filename[FILE_NAME_MAX_SIZE] = "/home/liubin/TCPFile/recv/"; 
    int count;  
    char temp_filename[FILE_NAME_MAX_SIZE];
    bzero(buff,BUFFSIZE);  
	
    count=recv(clientfd,buff,BUFFSIZE,0);  
    if(count<0)  
    {  
        perror("recv");  
        exit(1);  
    }  
    strcpy(temp_filename,buff+1); 
    printf("buff is *%s*  temp_filename is %s,size is %d\n",buff+1,temp_filename,buff[0]);
	strcat(filename,temp_filename);
	printf("Preparing recv file : %s from %s  %d\n",filename,argv[1],strlen(temp_filename));
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
    printf("Receieved file:%s from %s finished!\n",temp_filename,argv[1]); 
    view_pcd_file(temp_filename); 
    fclose(fd);  
    close(clientfd);  
    return 0;  
}

