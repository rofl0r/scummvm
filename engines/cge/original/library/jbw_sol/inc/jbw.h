#ifndef __JBW__
#define __JBW__

#define		BEL		 7
#define		BS		 8
#define		HT		 9
#define		LF		10
#define		FF		12
#define		CR		13

#define		NULL		0
#define		TRUE		(1==1)
#define		FALSE		(!TRUE)
#define		OFF		FALSE
#define		ON		TRUE

#define		IsWhite(c)	((c) == ' ' || (c) == '\t' || (c) == '\n')
#define		IsUpper(c)	((c) >= 'A' && (c) <= 'Z')
#define		IsLower(c)	((c) >= 'a' && (c) <= 'z')
#define         IsDigit(c)	((c) >= '0' && (c) <= '9')
#define		IsAlpha(c)	(IsLower(c) || IsUpper(c) || (c) == '_')
#define		IsAlNum(c)	(IsAlpha(c) || IsDigit(c))
#define		IsHxDig(c)	(IsDigit(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

#define		farnew(t,n)	((t far *) farmalloc(sizeof(t) * (n)))
#define		ArrayCount(a)	(sizeof(a)/sizeof((a)[0]))
#define		MAX_TIMER	0x1800B0L

typedef	unsigned char	BYTE;
typedef	unsigned int	WORD;
typedef	unsigned long	DWORD;

typedef	int		Boolean;
typedef unsigned char	byte;
typedef unsigned int	word;
typedef unsigned long	dword;
typedef	void (far _loadds MouseFunType)(void);

#define		Lo(d)		(((int *) &d)[0])
#define		Hi(d)		(((int *) &d)[1])
#define		LoWord(d)	((word) Lo(d))
#define		HiWord(d)	((word) Hi(d))
#define		K(n)		(1024*(n))
#define		MASK(n)		((1<<n)-1)

typedef enum
  {
    NoKey	= 0, CtrlA, CtrlB, CtrlC, CtrlD, CtrlE, CtrlF, CtrlG, CtrlH,
		     CtrlI, CtrlJ, CtrlK, CtrlL, CtrlM, CtrlN, CtrlO, CtrlP,
		     CtrlQ, CtrlR, CtrlS, CtrlT, CtrlU, CtrlV, CtrlW, CtrlX,
		     CtrlY, CtrlZ,
    BSp		= 8,
    Tab		= 9,
    Enter	= 13,
    Eof		= 26,
    Esc		= 27,
    AltQ	= 256+16, AltW, AltE, AltR, AltT, AltY, AltU, AltI, AltO, AltP,
    AltA	= 256+30, AltS, AltD, AltF, AltG, AltH, AltJ, AltK, AltL,
    AltZ	= 256+44, AltX, AltC, AltV, AltB, AltN, AltM,
    F11		= 256+87, F12,
    F1		= 256+59, F2, F3, F4, F5, F6, F7, F8, F9, F10,
    ShiftTab	= 256+15,
    ShiftF1	= 256+84, ShiftF2, ShiftF3, ShiftF4, ShiftF5,
			  ShiftF6, ShiftF7, ShiftF8, ShiftF9, ShiftF10,
    CtrlF1	= 256+94, CtrlF2,  CtrlF3,  CtrlF4,  CtrlF5,
			  CtrlF6,  CtrlF7,  CtrlF8,  CtrlF9,  CtrlF10,
    AltF1	= 256+104, AltF2,  AltF3,   AltF4,   AltF5,
			   AltF6,  AltF7,   AltF8,   AltF9,   AltF10,
    Home	= 256+71,
    Up,
    PgUp,
    Left	= 256+75,
    Ctr,
    Right,
    End		= 256+79,
    Down,
    PgDn,
    Ins,
    Del,
    CtrlLeft	= 256+115,
    CtrlRight,
    CtrlEnd,
    CtrlPgDn,
    CtrlHome,
    CtrlPgUp	= 256+132,

    MouseLeft	= 512+1,
    MouseRight,
    TwiceLeft	= 512+256+1,
    TwiceRight
  }  Keys;

struct	KeyStatStruct
	  {
	    int	RShift		: 1;
	    int	LShift		: 1;
	    int	Ctrl		: 1;
	    int	Alt		: 1;

	    int	ScrollLock	: 1;
	    int	NumLock		: 1;
	    int	CapsLock	: 1;
	    int	Ins		: 1;

	    int	LeftCtrl	: 1;
	    int	LeftAlt		: 1;
	    int	Unused		: 6;
	  };

#define		HGC_Cursor	0x0B0C
#define		CGA_Cursor	0x0607
#define		OFF_Cursor	0x2000

#define		TimerCount	(* ((volatile long far *) ((void _seg *) 0x40 + (void near *) 0x6C)))
#define		KeyStat		(* ((volatile struct KeyStatStruct far *) ((void _seg *) 0x40 + (void near *) 0x17)))
#define		BreakFlag	(* ((volatile byte far *) ((void _seg *) 0x40 + (void near *) 0x71)))
#define		PostFlag	(* ((volatile word far *) ((void _seg *) 0x40 + (void near *) 0x72)))
#define		POST		((void far (*)(void)) ((void _seg *) 0xF000 + (void near *) 0xFFF0))
#define		SLIF		if (KeyStat.ScrollLock)

#define		FOR(i,n)	for(i=0;i<(n);i++)

#define		TRAP(x)		{ if (x) asm { int 3 } }

#ifdef		DEBUG
  #define	Debug(x)	x
#else
  #define	Debug(x)
#endif

#ifdef		DEMO
  #define	Demo(x)		x
#else
  #define	Demo(x)
#endif


#ifdef	__cplusplus
  #define	EC		extern "C"
#else
  #define	EC
#endif


extern	word	_stklen;
extern	word	_heaplen;


#endif