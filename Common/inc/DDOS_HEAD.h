#ifndef _DDOS_HEAD_H
#define _DDOS_HEAD_H

#ifndef PACKET_COUNT
#define PACKET_COUNT		4096
#endif
	
#ifndef SLEEP_TIME
#define SLEEP_TIME			10
#endif

#ifndef DATA_1K
#define DATA_1K				1024
#endif

#ifndef ICMP_ECHO
#define ICMP_ECHO           8
#endif

typedef struct  IP_HEADER
{
	unsigned char   h_verlen;
	unsigned char   tos;
	unsigned short  total_len;
	unsigned short  ident;
	unsigned short  frag_and_flags;
	unsigned char   ttl;
	unsigned char   proto;
	unsigned short  checksum;
	unsigned int    sourceIP;
	unsigned int    destIP;
}IP_HEADER;

typedef struct  TCP_HEADER
{
	unsigned short  th_sport;
	unsigned short  th_dport;
	unsigned int    th_seq;
	unsigned int    th_ack;
	unsigned char   th_lenres;
	unsigned char   th_flag;
	unsigned short  th_win;
	unsigned short  th_sum;
	unsigned short  th_urp;
}TCP_HEADER;

typedef struct PSD_HEADER
{ 
	unsigned long  saddr;
	unsigned long  daddr;
	char           mbz; 
	char           ptcl;
	unsigned short tcpl;
}PSD_HEADER; 

typedef struct UDP_HEADER			// UDP�ײ�
{
	unsigned short sourceport; 
	unsigned short destport; 
	unsigned short udp_length; 
	unsigned short udp_checksum; 
} UDP_HEADER;

typedef struct _icmphdr				//����ICMP�ײ�
{
	BYTE   i_type;					//8λ����
	BYTE   i_code;					//8λ����
	USHORT i_cksum;					//16λУ��� 
	USHORT i_id;					//ʶ��ţ�һ���ý��̺���Ϊʶ��ţ�
	USHORT i_seq;					//�������к�	
	ULONG  timestamp;				//ʱ���
}ICMP_HEADER;

#endif
