#ifdef WIN32
#include <windows.h>
#define malloc(s) HeapAlloc(GetProcessHeap(), 0, s)
#define free(p) HeapFree(GetProcessHeap(), 0, p)
#else
#include <stdlib.h>
#include <string.h>
#endif
//---------------------------------------------------------------------------

#define MAXBITS			15
#define MINOFFSET		0x01
#define MINMATCH		0x03
#define MAXMATCH		((1 << 24) + MINMATCH)
#define MAXWND			(1 << MAXBITS)
#define NIL				0xffff
#define M				3
#define MAX(a, b)		((a) > (b) ? (a) : (b))
#define MIN(a, b)		((a) < (b) ? (a) : (b))
//---------------------------------------------------------------------------

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

typedef struct _LZ77_MATCHINFO
{
	ULONG len;
	ULONG off;
} LZ77_MATCHINFO;

typedef struct _LZ77_RUNSTATE
{
	ULONG wsize;
	UCHAR *pwnd;
	ULONG confine;
	USHORT *head;
	USHORT *prev;
	ULONG nice;
} LZ77_RUNSTATE;

typedef struct _LZ77_IOSTATE
{
	UCHAR *pbuf;
	ULONG bytenum;
	UCHAR bitnum;
	UCHAR codelen;
} LZ77_INPUTS, LZ77_OUTPUTS;
//---------------------------------------------------------------------------

static UCHAR log2(ULONG n)
{
	UCHAR c, i;

	if (n > 0xffff)
	{
		for (i = 16; n > ((ULONG)-1 >> (sizeof(ULONG) * 8 - i)); i++);
		return i;
	}

	if (n & 0xff00)
	{
		if (n & 0xf000)
		{
			if (n & 0xc000)
			{
				if (n & 0x8000)
				{
					c = 16;
				}
				else
				{
					c = 15;
				}
			}
			else
			{
				if (n & 0x2000)
				{
					c = 14;
				}
				else
				{
					c = 13;
				}
			}
		}
		else
		{
			if (n & 0x0c00)
			{
				if (n & 0x0800)
				{
					c = 12;
				}
				else
				{
					c = 11;
				}
			}
			else
			{
				if (n & 0x0200)
				{
					c = 10;
				}
				else
				{
					c = 9;
				}
			}
		}
	}
	else
	{
		if (n & 0x00f0)
		{
			if (n & 0x00c0)
			{
				if (n & 0x0080)
				{
					c = 8;
				}
				else
				{
					c = 7;
				}
			}
			else
			{
				if (n & 0x0020)
				{
					c = 6;
				}
				else
				{
					c = 5;
				}
			}
		}
		else
		{
			if (n & 0x000c)
			{
				if (n & 0x0008)
				{
					c = 4;
				}
				else
				{
					c = 3;
				}
			}
			else
			{
				if (n & 0x0002)
				{
					c = 2;
				}
				else
				{
					c = 1;
				}
			}
		}
	}

	return c;
}
//---------------------------------------------------------------------------

static void PutBits(LZ77_OUTPUTS *out, ULONG v, int num)
{
	UCHAR *s = out->pbuf + out->bytenum;
	ULONG i = 0;
	ULONG temp = v & ~(-1 << num);

	do
	{
		s[i] &= ~(-1 << out->bitnum);
		s[i] |= (UCHAR)(temp << out->bitnum);
		s[i + 1] = (UCHAR)(temp >> (8 - out->bitnum));
		temp >>= 8;
	} while ((++i << 3) < (ULONG)num);

	out->bitnum += (UCHAR)num;
	out->bytenum += out->bitnum >> 3;
	out->bitnum &= 7;
}
//---------------------------------------------------------------------------

static ULONG GetBits(LZ77_INPUTS *in, int num)
{
	UCHAR *s = in->pbuf + in->bytenum;
	ULONG i = 0, v = 0;

	do
	{
		v |= (s[i] >> in->bitnum) << (i << 3);
		v |= (s[i + 1] << (8 - in->bitnum)) << (i << 3);
	} while ((++i << 3) < (ULONG)num);

	in->bitnum += (UCHAR)num;
	in->bytenum += in->bitnum >> 3;
	in->bitnum &= 7;

	return v & ~(-1 << num);
}
//---------------------------------------------------------------------------

static void insert(LZ77_RUNSTATE *rs, ULONG at, ULONG len)
{
	ULONG ins_h, ins_t;

	if (len == 1)
	{
		ins_h = *(USHORT *)(rs->pwnd + at);
		rs->prev[at] = rs->head[ins_h];
		rs->head[ins_h] = (USHORT)at;
		return;
	}

	if ((at + len) < MAXWND)
	{
		ins_t = -1;
		len += at--;

		while (++at != len)
		{
			ins_h = *(USHORT *)(rs->pwnd + at);
			if ((ins_t - rs->head[ins_h]) <= 2)
				continue;
			ins_t = at;
			rs->prev[at] = rs->head[ins_h];
			rs->head[ins_h] = (USHORT)at;
		}
	}
}
//---------------------------------------------------------------------------

#define CHARBITS1 4
#define CHARBITS2 7
#define CHARBITS3 16
#define CHARNUMS0 7
#define CHARNUMS1 ((1 << CHARBITS1) - 1 + (CHARNUMS0 + 1) - 2)
#define CHARNUMS2 ((1 << CHARBITS2) - 1 + (CHARNUMS1 + 1))
#define CHARNUMS3 ((1 << CHARBITS3) - 1 + (CHARNUMS2 + 1))
//---------------------------------------------------------------------------
//标志位定义：
//长度：1，值：0，表示后面有一字节未压缩数据
//长度：2，值：10，表示后面有一个匹配（变长偏移+变长长度）
//长度：3，值：110，表示后面有一个匹配（7位偏移+1位长度，偏移为128时表示压缩结束）
//长度：3，值：111，表示后面有多个未压缩字节
//---------------------------------------------------------------------------

static void outcodec(LZ77_OUTPUTS *out, UCHAR *buffer, ULONG length)
{
	ULONG i, temp;

	if (length <= CHARNUMS0)
	{
		for (i = 0; i < length; i++)
		{
			//逐字符输出，额外输出位(length)
			temp = 0x00 | (buffer[i] << 1);
			PutBits(out, temp, 1 + 8);			//标志位(1)，数据位(8)
		}
	}
	else
	{
		if (length <= CHARNUMS1)
		{
			//输出(0-13)表示有(0-13)+8个连续字符未压缩，额外输出位(7)
			temp = 0x07 | ((length - CHARNUMS0 - 1) << 3);
			PutBits(out, temp, 3 + CHARBITS1);	//标志位(3)，数据位(4)
		}
		else if (length <= CHARNUMS1 * 2)
		{
			//优化输出，最大额外输出位(14)
			outcodec(out, buffer, CHARNUMS1);
			outcodec(out, buffer + CHARNUMS1, length - CHARNUMS1);
			return;
		}
		else if (length <= CHARNUMS2)
		{
			//输出(14)表示未压缩字符数由后面7位决定，额外输出位(14)
			temp = 0x07 | (14 << 3);
			PutBits(out, temp, 3 + CHARBITS1);	//标志位(3)，数据位(4)

			temp = length - CHARNUMS1 - 1;
			PutBits(out, temp, CHARBITS2);		//数据位(7)
		}
		else if (length <= CHARNUMS2 + CHARNUMS1)
		{
			//优化输出，最大额外输出位(21)
			outcodec(out, buffer, CHARNUMS2);
			outcodec(out, buffer + CHARNUMS2, length - CHARNUMS2);
			return;
		}
		else
		{
			//输出(15)表示未压缩字符数由后面两字节决定，额外输出位(23)
			temp = 0x07 | (15 << 3);
			PutBits(out, temp, 3 + CHARBITS1);	//标志位(3)，数据位(4)

			//输出(0-65535)+18个连续字符未压缩
			temp = length - CHARNUMS2 - 1;
			PutBits(out, temp, CHARBITS3);		//数据位(16)
		}

		{
			UCHAR *s = out->pbuf + out->bytenum;
			UCHAR x = out->bitnum;

			temp = buffer[0];
			PutBits(out, temp, 8 - x);

			//拷贝连续的未压缩字符
			for (i = 1; i < length; i++)
			{
				s[i] = buffer[i];
			}

			temp >>= 8 - x;
			out->bytenum += length - 1;
			PutBits(out, temp, x);
		}
	}
}
//---------------------------------------------------------------------------

static void outcodex(LZ77_OUTPUTS *out, ULONG offset, ULONG length)
{
	UCHAR i = 0;
	ULONG temp, m, n;

	switch (length)
	{
//	case 1:
//		temp = 0x03 | ((offset - MINOFFSET) << 3);
//		PutBits(out, temp, 3 + 4);				//标志位(3)，数据位(4)
//		return;

	case 3:
		if (offset > 127)
			break;

	case 2:
		temp = 0x03 | ((offset - MINOFFSET) << 3);	//短匹配优化
		temp |= (length - 2) << (3 + 7);
		PutBits(out, temp, 3 + 7 + 1);			//标志位(3)，数据位(1+7)
		return;
	}

	//写入变长匹配偏移
	temp = 0x01 | ((offset - MINOFFSET) << 2);
	PutBits(out, temp, 2 + out->codelen);		//标志位(2)，数据位(log2(数据))

	length -= MINMATCH;
	m = 1 << (M - 1);

	//计算匹配长度最少占用多少位
	do
	{
		n = ~(-1 << i++) << M;
		m <<= 1;
	} while ((m + n) <= length);

	//写入匹配长度位数
	temp = ~(-1 << (i - 1));
	PutBits(out, temp, i);

	//写入变长匹配长度
	temp = length - n;
	PutBits(out, temp, i + 3 - 1);
}//*/
//---------------------------------------------------------------------------

static int match(LZ77_RUNSTATE *rs, ULONG strat, LZ77_MATCHINFO *mi)
{
	UCHAR *src, *s, *d, *c, *t;
	USHORT index, *prev;
	ULONG i, m = 0, n, nice, flag = 0;

	src = rs->pwnd;
	index = rs->head[*(USHORT *)(src + strat)];

	if (NIL != index)
	{
		c = src + MIN(rs->confine, MAXMATCH);
		t = src + strat;
		m = MINMATCH - 1;
		prev = rs->prev;
		nice = rs->nice;

		do
		{
			s = t;
			d = src + index;

			while (*(USHORT *)(s += 2) == *(USHORT *)(d += 2)
				&& *(USHORT *)(s += 2) == *(USHORT *)(d += 2)
				&& *(USHORT *)(s += 2) == *(USHORT *)(d += 2)
				&& *(USHORT *)(s += 2) == *(USHORT *)(d += 2)
				&& s < c);
			if (*s == *d) s++;

			if (s >= c)
			{
				m = c - t;
				n = index;
				break;
			}
			i = s - t;

			if (m < i)
			{
				m = i;
				n = index;
				if (m > nice)
					flag = 1;
			}
			else if (flag)
				break;
			index = prev[index];
		} while (NIL != index);
	}

	if (MINMATCH <= m)
	{
		mi->len = m;
		mi->off = strat - n;
		return 1;
	}
	else
	{
		index = rs->head[*(USHORT *)(src + strat)];

		if (strat - index <= 127)
		{
			mi->len = 2;
			mi->off = strat - index;
			return 1;
		}/*
		else
		{
			//从前面16字节中查找1字节匹配
			for (i = 16; i > 0; i--)
			{
				if (*(src + strat - i) == *(src + strat))
				{
					mi->len = 1;
					mi->off = i;
					return 1;
				}
			}
		}//*/
	}

	return 0;
}
//---------------------------------------------------------------------------

static ULONG deflate(LZ77_RUNSTATE rs, UCHAR *dst, ULONG *inbytes)
{
	LZ77_MATCHINFO mi;
	LZ77_OUTPUTS out;
	ULONG strstart = 0, count = 0;
	LZ77_OUTPUTS prev_out;
	ULONG prev_start = 0;

	out.pbuf = dst;
	out.bytenum = 0;
	out.bitnum = 0;
	out.codelen = 1;
	prev_out = out;

	memset(rs.head, NIL, sizeof(USHORT) * 65536);

	do
	{
		if (match(&rs, strstart, &mi))
		{
			if (count > 0)
			{
				outcodec(&out, rs.pwnd + strstart - count, count);		//输出无匹配字符
				count = 0;
			}

			if ((ULONG)(1 << out.codelen) < strstart)
				out.codelen = log2(strstart - MINOFFSET);

			insert(&rs, strstart, mi.len);						//更新字典记录
			outcodex(&out, mi.off, mi.len);						//输出压缩代码
			strstart += mi.len;
		}
		else
		{
			insert(&rs, strstart, 1);							//更新字典记录
			count++;
			strstart += 1;
		}

		if (strstart - prev_start > 0x1000)						//跟踪压缩率变化
		{
			if (strstart - prev_start < out.bytenum - prev_out.bytenum - 4)
			{
				out = prev_out;
				outcodec(&out, rs.pwnd + prev_start, strstart - prev_start);	//处理无法压缩的块
			}

			prev_out = out;
			prev_start = strstart;
		}
	} while (strstart < rs.wsize);

	if (count > 0)
	{
		outcodec(&out, rs.pwnd + strstart - count, count);		//输出无匹配字符
		count = 0;
	}

	if (strstart - prev_start < out.bytenum - prev_out.bytenum - 4)
	{
		out = prev_out;
		outcodec(&out, rs.pwnd + prev_start, strstart - prev_start);	//处理无法压缩的块
	}

	outcodex(&out, 128, 2);		//输出压缩结束标记

	if (out.bitnum)
		out.bytenum++;

	*inbytes = strstart;

	return out.bytenum;
}
//---------------------------------------------------------------------------

static ULONG inflate(UCHAR *src, UCHAR *dst, ULONG len, ULONG *inbytes)
{
	ULONG offset, length;
	UCHAR i, t;
	UCHAR *out, *s;
	LZ77_INPUTS in;

	in.pbuf = src;
	in.bytenum = 0;
	in.bitnum = 0;
	in.codelen = 1;
	out = dst;

	while (in.bytenum < len)
	{
		if (!GetBits(&in, 1))			//0 ?
		{
			*out++ = (UCHAR)GetBits(&in, 8);
		}
		else
		{
			if (!GetBits(&in, 1))		//10 ?
			{
				if ((1 << in.codelen) < out - dst)
					in.codelen = log2(out - dst - MINOFFSET);
				offset = GetBits(&in, in.codelen) + MINOFFSET;

				for (i = 0; GetBits(&in, 1); i++);	//计算匹配长度位数
				length = GetBits(&in, i + M);
				length += (~(-1 << i) << M) + MINMATCH;		//计算匹配长度

				do
				{
					*out++ = *(out - offset);
				} while (--length);
			}
			else
			{
				if (!GetBits(&in, 1))	//110 ?
				{
					offset = GetBits(&in, 7) + MINOFFSET;
					length = GetBits(&in, 1) + 2;

					if (offset == 128)	//解压结束？
						break;

					do
					{
						*out++ = *(out - offset);	//解压短匹配
					} while (--length);
				}
				else
				{
					length = GetBits(&in, CHARBITS1);

					switch (length)
					{
					case 14:
						length = GetBits(&in, CHARBITS2);
						length += CHARNUMS1 + 1;
						break;
					case 15:
						length = GetBits(&in, CHARBITS3);
						length += CHARNUMS2 + 1;
						break;
					default:
						length += CHARNUMS0 + 1;
						break;
					}

					s = in.pbuf + in.bytenum;
					offset = 1;
					i = in.bitnum;

					do
					{
						out[offset] = s[offset];	//还原字符串
					} while (++offset < length);

					t = (UCHAR)GetBits(&in, 8 - i);
					in.bytenum += length - 1;
					t |= (UCHAR)(GetBits(&in, i) << (8 - i));
					*out = t;
					out += length;
				}
			}
		}
	}

	*inbytes = in.bytenum;
	*inbytes += in.bitnum == 0 ? 0 : 1;

	return out - dst;
}
//---------------------------------------------------------------------------

static ULONG Lz77Compress(void *dst, void *src, ULONG len, int level)
{
	LZ77_RUNSTATE rs;
	ULONG m, n, count = 0;

	if (0 == len)
		return 0;

	if (!src || !dst)
		return 0;

	switch (level)
	{
	case 0:
		rs.nice = 3;
		break;
	case 1:
		rs.nice = 30;
		break;
	case 2:
		rs.nice = 70;
		break;
	case 3:
		rs.nice = 150;
		break;
	case 4:
		rs.nice = 0;
		break;
	}

	rs.prev = (USHORT *)malloc(sizeof(USHORT) * 65536);
	rs.head = (USHORT *)malloc(sizeof(USHORT) * 65536);

	if (!rs.prev || !rs.head)
	{
		free(rs.head);
		free(rs.prev);
		return 0;
	}

	do
	{
		rs.wsize = MIN(len, MAXWND);
		rs.pwnd  = (UCHAR*)src;
		//rs.confine = len;
		rs.confine = MIN(len, MAXWND);
		n = deflate(rs, (UCHAR*)dst, &m);
		len -= m;
		src = (UCHAR*)src + m;
		dst = (UCHAR*)dst + n;
		count += n;
	} while ((int)len > 0);

	free(rs.head);
	free(rs.prev);

	return count;
}
//---------------------------------------------------------------------------

static ULONG Lz77Decompress(void *dst, void *src, ULONG len)
{
	ULONG c = 0, i, o, a = 0;

	do
	{
		o = inflate((UCHAR*)src,(UCHAR*) dst, len, &i);
		if ((a += o) >= 0x88000)
		{int c = 0;}
		src = (UCHAR*)src + i;
		dst = (UCHAR*)dst + o;
		
		len -= i;
		c += o;
	} while (len);

	return c;
}
//---------------------------------------------------------------------------
