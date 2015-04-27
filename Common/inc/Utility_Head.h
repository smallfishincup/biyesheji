#ifndef _UTILITY_HEAD_H
#define _UTILITY_HEAD_H

//////////////////////////////////////////////////////////////////////////////////////////////

#include "Winsock2.h"
#include <afxsock.h>		// MFC socket extensions
#include "afxtempl.h"

#include <iostream>
#include <vector>
using namespace std;

#include "dt.h"						// 调试工具
#include "Mmsystem.h"				// 声音播放类

#include "AutoLock.h"

#include "UtilityStruct.h"
#include "UtilityMacro.h"

#include "ThreadEx.h"				// 线程基类
#include "RegisterKey.h"
#include "SystemInfo.h"
#include "SystemUtility.h"
#include "ValidFunction.h"
#include "SkinIniFile.h"

#include "avifile.h"

#include "hi_play_audioout.h"
#include "hi_play_audioin.h"

#include "ListenConnectionThread.h"
#include "DealCMDThread.h"
#include "ReceiveCameraThread.h"
#include "ReceiveAudioRenderThread.h"
#include "ReceiveScreenThread.h"
#include "ReceiveFileThread.h"
#include "SendFileThread.h"
#include "CMDShellThread.h"
#include "ScreenSnap.h"
#include "VideoCodec.h"

#include "XPListCtrl.h"
#include "SkinWin.h"
#include "XPButton.h"
#include "XPEdit.h"
#include "SkinComboBox.h"
#include "PictureWnd.h"
#include "ntray.h"
#include "XPCheckBox.h"
#include "XPTreeCtrl.h"
#include "ColorProgress.h"
#include "WZDTLTIP.H"
#include "HyperLink.H"

#endif