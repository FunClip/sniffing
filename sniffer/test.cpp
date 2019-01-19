#define APP_NAME		"sniffex"
#define APP_DESC		"Sniffer example using libpcap"
#define APP_COPYRIGHT	"Copyright (c) 2005 The Tcpdump Group"
#define APP_DISCLAIMER	"THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM."
#include <pcap.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <time.h>
#include <syslog.h>
#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;


/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

/* Ethernet header */
struct sniff_ethernet {
        u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
        u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
        u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
        u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
        u_char  ip_tos;                 /* type of service */
        u_short ip_len;                 /* total length */
        u_short ip_id;                  /* identification */
        u_short ip_off;                 /* fragment offset field */
        #define IP_RF 0x8000            /* reserved fragment flag */
        #define IP_DF 0x4000            /* dont fragment flag */
        #define IP_MF 0x2000            /* more fragments flag */
        #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
        u_char  ip_ttl;                 /* time to live */
        u_char  ip_p;                   /* protocol */
        u_short ip_sum;                 /* checksum */
        struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        tcp_seq th_seq;                 /* sequence number */
        tcp_seq th_ack;                 /* acknowledgement number */
        u_char  th_offx2;               /* data offset, rsvd */
        #define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
        u_char  th_flags;
        #define TH_FIN  0x01
        #define TH_SYN  0x02
        #define TH_RST  0x04
        #define TH_PUSH 0x08
        #define TH_ACK  0x10
        #define TH_URG  0x20
        #define TH_ECE  0x40
        #define TH_CWR  0x80
        #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
};


int init_daemon(void) 
{ 
	int pid; 
	int i; 
 
	//忽略终端I/O信号，STOP信号
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	signal(SIGHUP,SIG_IGN);
	
	pid = fork();
	if(pid > 0) {
		exit(0); //结束父进程，使得子进程成为后台进程
	}
	else if(pid < 0) { 
		return -1;
	}
 
	//建立一个新的进程组,在这个新的进程组中,子进程成为这个进程组的首进程,以使该进程脱离所有终端
	setsid();
 
	//再次新建一个子进程，退出父进程，保证该进程不是进程组长，同时让该进程无法再打开一个新的终端
	pid=fork();
	if( pid > 0) {
		exit(0);
	}
	else if( pid< 0) {
		return -1;
	}
 
	//关闭所有从父进程继承的不再需要的文件描述符
	for(i=0;i< NOFILE;close(i++));
 
	//改变工作目录，使得进程不与任何文件系统联系
	chdir("/");
 
	//将文件当时创建屏蔽字设置为0
	umask(0);
 
	//忽略SIGCHLD信号
	signal(SIGCHLD,SIG_IGN); 
	
	return 0;
}


void
got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void
print_payload(const u_char *payload, int len);

void
print_hex_ascii_line(const u_char *payload, int len, int offset);

void
print_app_banner(void);

void
print_app_usage(void);

void
mysql_connection_header(string id,string value1,string value2);

void
mysql_connection_ip(string id,string value1,string value2,string url);


/*
 * app name/banner
 */
void
print_app_banner(void)
{

	printf("%s - %s\n", APP_NAME, APP_DESC);
	printf("%s\n", APP_COPYRIGHT);
	printf("%s\n", APP_DISCLAIMER);
	printf("\n");

return;
}

/*
 * print help text
 */
void
print_app_usage(void)
{

	printf("Usage: %s [interface]\n", APP_NAME);
	printf("\n");
	printf("Options:\n");
	printf("    interface    Listen on <interface> for packets.\n");
	printf("\n");

return;
}

/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */

/*
 * dissect/print packet
 */
void
got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

	static int count = 1;                   /* packet counter */
	
    //if(count>500)exit(0);
	/* declare pointers to packet headers */
	const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
	const struct sniff_ip *ip;              /* The IP header */
	const struct sniff_tcp *tcp;            /* The TCP header */
	const char *payload_;                    /* Packet payload */

	int size_ip;
	int size_tcp;
	int size_payload;
	int count1=0;
	
	/* define ethernet header */
	ethernet = (struct sniff_ethernet*)(packet);
	
	/* define/compute ip header offset */
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

    tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if (size_tcp < 20) {
		printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
		return;
	}
    if(ntohs (tcp->th_dport) == 80)
    {
		payload_ = ((char *)(packet + SIZE_ETHERNET + size_ip + size_tcp));
	
		/* compute tcp payload (segment) size */
		size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
		string payload=payload_;
        
	/* define/compute tcp payload (segment) offset */
	
	/*
	 * Print payload data; it might be binary, so don't just
	 * treat it as a string.
	 */
	if (size_payload > 1) {
		printf("\nPacket number %d:\n", count);
	    
        printf("       From: %s\n", inet_ntoa(ip->ip_src));
	    printf("         To: %s\n", inet_ntoa(ip->ip_dst));
        stringstream ss;
    	ss<<count;
		string s1 = ss.str();
        printf("ip_src:%s,ip_dst:%s\n",inet_ntoa(ip->ip_src),inet_ntoa(ip->ip_dst));
        
        
	/* determine protocol */	
	switch(ip->ip_p) {
		case IPPROTO_TCP:
			printf("   Protocol: TCP\n");
			break;
		case IPPROTO_UDP:
			printf("   Protocol: UDP\n");
			return;
		case IPPROTO_ICMP:
			printf("   Protocol: ICMP\n");
			return;
		case IPPROTO_IP:
			printf("   Protocol: IP\n");
			return;
		default:
			printf("   Protocol: unknown\n");
			return;
	}
	
	
	printf("   Src port: %d\n", ntohs(tcp->th_sport));
	printf("   Dst port: %d\n", ntohs(tcp->th_dport));
	

		printf("   Payload (%d bytes):\n", size_payload);
		// print_payload(payload_, size_payload);
		//printf("payload_:%s\n",payload_);
		//printf("payload:%s\n",payload.c_str());
		vector<string> key, value;
		string url;
        ss.clear();
		ss.str(payload);
		string tmp;
		string uri;
		ss >> tmp; ss >> uri; ss.get();
		tmp.clear();
		while(getline(ss, tmp) && tmp != "\r") {
			tmp.erase(tmp.end()-1,tmp.end());
			//cout << tmp << endl;
			size_t pos = tmp.find(':');
			if(pos != tmp.npos) {
				key.push_back(tmp.substr(0, pos));
				if(pos >= tmp.size())
					value.push_back("");
				else
					value.push_back(tmp.substr(pos+2));
				if(tmp.substr(0, pos) == "Host")
					url = tmp.substr(pos+2) + uri;
				//cout << tmp.substr(0, pos) << tmp.substr(pos+2) << endl;
			}
			tmp.clear();
		}
		
		if(key.size()){
			mysql_connection_ip(s1,inet_ntoa(ip->ip_src),inet_ntoa(ip->ip_dst), url);
        count++;}
        for(int i=0;i<key.size();i++)
        { 	
            mysql_connection_header(s1,key[i],value[i]);
            printf("req_id:%d\nheader_key:%s\nheader_values:%s\n",count,key[i].c_str(),value[1].c_str());
        }
	}
	
    }

	/* print source and destination IP addresses */
return;
}

void mysql_connection_header(string id,string value1,string value2)
{
    MYSQL my_connection;

    int res;

    mysql_init(&my_connection);

      /*mysql_real_connect(&mysql,主机名,用户名,密码,数据库名,0,NULL,0) == NULL)*/
      /*mysql_real_connect(&mysql,host,user,passwd,dbname,0,NULL,0) == NULL)*/

    if (mysql_real_connect(&my_connection, "localhost", "root", "","n161002213",0,NULL,CLIENT_FOUND_ROWS))
    {
        printf("Connection success\n");
        string sql="insert into request_headers(req_id,header_key,header_value) values(";
        sql+=id+",'"+value1+"','"+value2+"')";

        res = mysql_query(&my_connection, sql.c_str());

        if (!res)
        {
            printf("Inserted %lu rows\n",(unsigned long)mysql_affected_rows(&my_connection));
        /*里头的函数返回受表中影响的行数*/
        }
        else
        {
        //分别打印出错误代码及详细信息
            fprintf(stderr, "Insert error %d: %s\n",mysql_errno(&my_connection),mysql_error(&my_connection));
         }
          mysql_close(&my_connection);
     }

      else
      {
          fprintf(stderr, "Connection failed\n");

          if (mysql_errno(&my_connection))
          {
            fprintf(stderr, "Connection error %d: %s\n",mysql_errno(&my_connection),mysql_error(&my_connection));
          }
      }
}

void mysql_connection_ip(string id,string value1,string value2,string url)
{
    MYSQL my_connection;

    int res;

    mysql_init(&my_connection);

      /*mysql_real_connect(&mysql,主机名,用户名,密码,数据库名,0,NULL,0) == NULL)*/
      /*mysql_real_connect(&mysql,host,user,passwd,dbname,0,NULL,0) == NULL)*/

    if (mysql_real_connect(&my_connection, "localhost", "root", "","n161002213",0,NULL,CLIENT_FOUND_ROWS))
    {
        printf("Connection success\n");
        string sql="insert into requests(id,src_ip,dest_ip,url,url_hash) values(";
        sql+=id+",'"+value1+"','"+value2+"','"+url+"',md5('"+url+"'))";

        res = mysql_query(&my_connection, sql.c_str());

        if (!res){
        /*里头的函数返回受表中影响的行数*/
        }
        else
        {
        //分别打印出错误代码及详细信息
            fprintf(stderr, "Insert error %d: %s\n",mysql_errno(&my_connection),mysql_error(&my_connection));
         }
          mysql_close(&my_connection);
     }

      else
      {
          fprintf(stderr, "Connection failed\n");

          if (mysql_errno(&my_connection))
          {
            fprintf(stderr, "Connection error %d: %s\n",mysql_errno(&my_connection),mysql_error(&my_connection));
          }
      }
}

int main(int argc, char **argv)
{
    init_daemon();  
	char *dev = NULL;			/* capture device name */
	char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
	pcap_t *handle;				/* packet capture handle */

	char filter_exp[] = "ip";		/* filter expression [3] */
	struct bpf_program fp;			/* compiled filter program (expression) */
	bpf_u_int32 mask;			/* subnet mask */
	bpf_u_int32 net;			/* ip */
	int num_packets = 500;			/* number of packets to capture */

	print_app_banner();

	/* check for capture device name on command-line */
	if (argc == 2) {
		dev = argv[1];
	}
	else if (argc > 2) {
		fprintf(stderr, "error: unrecognized command-line options\n\n");
		print_app_usage();
		exit(EXIT_FAILURE);
	}
	else {
		/* find a capture device if not specified on command-line */
		dev = pcap_lookupdev(errbuf);
		if (dev == NULL) {
			fprintf(stderr, "Couldn't find default device: %s\n",
			    errbuf);
			exit(EXIT_FAILURE);
		}
	}
	
	/* get network number and mask associated with capture device */
	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
		    dev, errbuf);
		net = 0;
		mask = 0;
	}

	/* print capture info */
	printf("Device: %s\n", dev);
	printf("Number of packets: %d\n", num_packets);
	printf("Filter expression: %s\n", filter_exp);

	/* open capture device */
	handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		exit(EXIT_FAILURE);
	}

	/* make sure we're capturing on an Ethernet device [2] */
	if (pcap_datalink(handle) != DLT_EN10MB) {
		fprintf(stderr, "%s is not an Ethernet\n", dev);
		exit(EXIT_FAILURE);
	}

	/* compile the filter expression */
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n",
		    filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}

	/* apply the compiled filter */
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n",
		    filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}

	/* now we can set our callback function */
	pcap_loop(handle, -1, got_packet, NULL);

	/* cleanup */
	pcap_freecode(&fp);
	pcap_close(handle);

	printf("\nCapture complete.\n");

return 0;
}
