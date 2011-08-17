#include	<general.h>
#include	"vga13h.h"
#include	"bitmap.h"
#include	"vol.h"
#include	"text.h"
#include	<alloc.h>
#include	<conio.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<dos.h>
#include	<dir.h>
#include	<fcntl.h>
#include	<bios.h>
#include	<io.h>

#ifdef DEBUG
#define		REPORT
#endif

#define		OK(f)		((f).Error==0)
#define		FADE_STEP	2

#define		TMR_DIV		((0x8000/TMR_RATE)*2)


//--------------------------------------------------------------------------

#ifdef	REPORT
static char Report[] = "NearHeap=.....  FarHeap=......\n";
#define	NREP		 9
#define	FREP		24
#endif



static	VgaRegBlk VideoMode[] = {

		    { 0x04, VGASEQ, 0x08, 0x04 },	// memory mode

		    { 0x03, VGAGRA, 0xFF, 0x00 },	// data rotate = 0
		    { 0x05, VGAGRA, 0x03, 0x00 },	// R/W mode = 0
		    { 0x06, VGAGRA, 0x02, 0x00 },	// misc

		    { 0x14, VGACRT, 0x40, 0x00 },	// underline
		    { 0x13, VGACRT, 0xFF, 0x28 },	// screen width
		    { 0x17, VGACRT, 0xFF, 0xC3 },	// mode control

		    { 0x11, VGACRT, 0x80, 0x00 },	// vert retrace end
		    { 0x09, VGACRT, 0xEF, 0x01 },	// max scan line

		    { 0x30, VGAATR, 0x00, 0x20 },	// 256 color mode

//		    { 0x12, VGACRT, 0xFF, 0x6E },	// vert display end
//		    { 0x15, VGACRT, 0xFF, 0x7F },	// start vb
//		    { 0x10, VGACRT, 0xFF, 0x94 },	// start vr

		    { 0x00                     } };


	Boolean		SpeedTest	= FALSE;
	SEQ		Seq1[] = { { 0, 0, 0, 0, 0 } };
	SEQ		Seq2[] = { { 0, 1, 0, 0, 0 }, { 1, 0, 0, 0, 0 } };
	SPRITE *	Sys		= NULL;

extern "C"	void	SNDMIDIPlay	(void);






char * NumStr (char * str, int num)
{
  char * p = strchr(str, '#');
  if (p) wtom(num, p, 10, 5);
  return str;
}








static void Video (void)
{
  static word SP_S;

  asm	push	bx
  asm	push	bp
  asm	push	si
  asm	push	di
  asm	push	es
  asm	xor	bx,bx		// video page #0
  SP_S = _SP;
  asm	int	VIDEO
  _SP = SP_S;
  asm	pop	es
  asm	pop	di
  asm	pop	si
  asm	pop	bp
  asm	pop	bx
}






word far * SaveScreen (void)
{
  word cxy, cur, siz, far * scr = NULL, far * sav;

  // horizontal size of text mode screen
  asm	mov	ah,0x0F		// get current video mode
  Video();			// BIOS video service
  asm	xchg	ah,al		// answer in ah
  asm	push	ax		// preserve width

  // vertical size of text mode screen
  asm	mov	dl,24		// last row on std screen
  asm	xor	bx,bx		// valid request in BH
  asm	mov	ax,0x1130	// get EGA's last row #
  Video();			// BIOS video service
  asm	inc	dl		// # of rows = last+1

  // compute screen size in words
  asm	pop	ax		// restore width
  asm	mul	dl		// width * height

  siz = _AX;

  asm	mov	ax,0x40		// system data segment
  asm	mov	es,ax
  asm	mov	ax,0B000H	// Mono
  asm	cmp	byte ptr es:[0x49],0x07
  asm	je	sto
  asm	mov	ax,0B800H	// Color
  sto:				// store screen address
  asm	mov	word ptr scr+2,ax

  _AH = 0x0F; Video();		// active page

  // take cursor shape
  _AH = 0x03; Video();		// get cursor size
  cur = _CX;

  // take cursor position
  _DH = 0;
  _AH = 0x03; Video();		// get cursor
  cxy = _DX;

  sav = farnew(word, siz+3);	// +3 extra words for size and cursor
  if (sav)
    {
      sav[0] = siz;
      sav[1] = cur;
      sav[2] = cxy;
      _fmemcpy(sav+3, scr, siz * 2);
    }
  return sav;
}





void RestoreScreen (word far * &sav)
{
  word far * scr = NULL;

  asm	mov	ax,0x40		// system data segment
  asm	mov	es,ax
  asm	mov	ax,0B000H	// Mono
  asm	cmp	byte ptr es:[0x49],0x07
  asm	je	sto
  asm	mov	ax,0B800H	// Color
  sto:				// store screen address
  asm	mov	word ptr scr+2,ax

  _fmemcpy(scr, sav+3, sav[0] * 2);

  _AH = 0x0F; Video();		// active page

  // set cursor shape
  _CX = sav[1];
  _AH = 0x01; Video();		// set cursor size

  // set cursor position
  _DX = sav[2];
  _AH = 0x02; Video();		// set cursor

  farfree(sav);
  sav = NULL;
}






DAC MkDAC (byte r, byte g, byte b)
{
  static DAC x;
  x.R = r;
  x.G = g;
  x.B = b;
  return x;
}




RGB MkRGB (byte r, byte g, byte b)
{
  static TRGB x;
  x.dac.R = r;
  x.dac.G = g;
  x.dac.B = b;
  return x.rgb;
}






SPRITE * Locate (int ref)
{
  SPRITE * spr = VGA::ShowQ.Locate(ref);
  return (spr) ? spr : VGA::SpareQ.Locate(ref);
}





//--------------------------------------------------------------------------


Boolean		HEART::Enable = FALSE;
word *		HEART::XTimer = NULL;




HEART::HEART (void)
: ENGINE(TMR_DIV)
{
}


/*
extern "C" void TimerProc (void)
{
  static SPRITE * spr;
  static byte run = 0;

  // decrement external timer word
  if (HEART::XTimer)
    if (*HEART::XTimer) -- *HEART::XTimer;
    else HEART::XTimer = NULL;

  if (! run && HEART::Enable)		// check overrun flag
    {
      static word oldSP, oldSS;

      ++ run;			// disable 2nd call until current lasts
      asm	mov	ax,ds
      asm	mov	oldSS,ss
      asm	mov	oldSP,sp
      asm	mov	ss,ax
      asm	mov	sp,0xFF80

      // system pseudo-sprite
      if (Sys) if (Sys->Time) if (-- Sys->Time == 0) Sys->Tick();

      for (spr = VGA::ShowQ.First(); spr; spr = spr->Next)
	{
	  if (spr->Time) if (!spr->Flags.Hide) if (-- spr->Time == 0) spr->Tick();
	}
      asm	mov	ss,oldSS
      asm	mov	sp,oldSP
      -- run;
    }
}
*/


void interrupt ENGINE::NewTimer (...)
{
  static SPRITE * spr;
  static byte run = 0, cntr1 = TMR_RATE1, cntr2 = TMR_RATE2;

  ___1152_Hz___:

  SNDMIDIPlay();
  asm	dec	cntr1
  asm	jz	___72_Hz___
  asm	mov	al,0x20		// send...
  asm	out	0x020,al	// ...e-o-i
  return;

  ___72_Hz___:

  asm	mov	cntr1,TMR_RATE1
  asm	dec	cntr2
  asm	jnz	my_eoi

  ___18_Hz___:

  OldTimer();
  asm	mov	cntr2,TMR_RATE2
  asm	jmp	short my_int

  // send E-O-I
  my_eoi:
  asm	mov	al,0x20
  asm	out	0x020,al
  asm	sti			//  enable interrupts

  my_int: //------72Hz-------//

  // decrement external timer word
  if (HEART::XTimer)
    if (*HEART::XTimer) -- *HEART::XTimer;
    else HEART::XTimer = NULL;

  if (! run && HEART::Enable)	// check overrun flag
    {
      static word oldSP, oldSS;

      ++ run;			// disable 2nd call until current lasts
      asm	mov	ax,ds
      asm	mov	oldSS,ss
      asm	mov	oldSP,sp
      asm	mov	ss,ax
      asm	mov	sp,0xFF80

      // system pseudo-sprite
      if (Sys) if (Sys->Time) if (-- Sys->Time == 0) Sys->Tick();

      for (spr = VGA::ShowQ.First(); spr; spr = spr->Next)
	{
	  if (spr->Time) if (!spr->Flags.Hide) if (-- spr->Time == 0) spr->Tick();
	}
      asm	mov	ss,oldSS
      asm	mov	sp,oldSP
      -- run;
    }
}





void HEART::SetXTimer (word * ptr)
{
  if (XTimer && ptr != XTimer) *XTimer = 0;
  XTimer = ptr;
}




void HEART::SetXTimer (word * ptr, word time)
{
  SetXTimer(ptr);
  *ptr = time;
}




//--------------------------------------------------------------------------







SPRITE::SPRITE (BMP_PTR * shp)
: X(0), Y(0), Z(0), NearPtr(0), TakePtr(0),
  Next(NULL), Prev(NULL), SeqPtr(NO_SEQ), Time(0), //Delay(0),
  Ext(NULL), Ref(-1), Cave(0)
{
  memset(File, 0, sizeof(File));
  *((word *)&Flags) = 0;
  SetShapeList(shp);
}






SPRITE::~SPRITE (void)
{
  Contract();
}




BMP_PTR SPRITE::Shp (void)
{
  register SPREXT * e = Ext;
  if (e) if (e->Seq)
    {
      int i = e->Seq[SeqPtr].Now;
      #ifdef DEBUG
	if (i >= ShpCnt)
	  {
	    //char s[256];
	    //sprintf(s, "Seq=%p ShpCnt=%d SeqPtr=%d Now=%d Next=%d",
	    //	    Seq, ShpCnt, SeqPtr, Seq[SeqPtr].Now, Seq[SeqPtr].Next);
	    //VGA::Exit(s, File);
	    VGA::Exit("Invalid PHASE in SPRITE::Shp()", File);
	  }
      #endif
      return e->ShpList[i];
    }
  return NULL;
}





BMP_PTR * SPRITE::SetShapeList (BMP_PTR * shp)
{
  BMP_PTR * r = (Ext) ? Ext->ShpList : NULL;

  ShpCnt = 0;
  W = 0;
  H = 0;

  if (shp)
    {
      BMP_PTR * p;
      for (p = shp; *p; p ++)
	{
	  BMP_PTR b = (*p); // ->Code();
	  if (b->W > W) W = b->W;
	  if (b->H > H) H = b->H;
	  ++ ShpCnt;
	}
      Expand();
      Ext->ShpList = shp;
      if (! Ext->Seq) SetSeq((ShpCnt < 2) ? Seq1 : Seq2);
    }
  return r;
}





void SPRITE::MoveShapes (byte far * buf)
{
  BMP_PTR * p;
  for (p = Ext->ShpList; *p; p ++)
    {
      buf += (*p)->MoveVmap(buf);
    }
}





Boolean SPRITE::Works (SPRITE * spr)
{
  if (spr) if (spr->Ext)
    {
      SNAIL::COM * c = spr->Ext->Take;
      if (c != NULL)
	{
	  c += spr->TakePtr;
	  if (c->Ref == Ref)
	    if (c->Com != SNLABEL || (c->Val == 0 || c->Val == Now))
	      return TRUE;
	}
    }
  return FALSE;
}





SEQ * SPRITE::SetSeq (SEQ * seq)
{
  Expand();
  register SEQ * s = Ext->Seq;
  Ext->Seq = seq;
  if (SeqPtr == NO_SEQ) Step(0);
  else
    if (Time == 0) Step(SeqPtr);
  return s;
}






Boolean SPRITE::SeqTest (int n)
{
  if (n >= 0) return (SeqPtr == n);
  if (Ext) return (Ext->Seq[SeqPtr].Next == SeqPtr);
  return TRUE;
}






SNAIL::COM * SPRITE::SnList (SNLIST type)
{
  register SPREXT * e = Ext;
  if (e) return (type == NEAR) ? e->Near : e->Take;
  return NULL;
}






void SPRITE::SetName (char * n)
{
  if (Ext)
    {
      if (Ext->Name)
	{
	  delete[] Ext->Name;
	  Ext->Name = NULL;
	}
      if (n)
	{
	  if ((Ext->Name = new char[strlen(n)+1]) != NULL) strcpy(Ext->Name, n);
	  else VGA::Exit("No core", n);
	}
    }
}





SPRITE * SPRITE::Expand (void)
{
  if (! Ext)
    {
      Boolean enbl = HEART::Enable;
      HEART::Enable = FALSE;
      if ((Ext = new SPREXT) == NULL) DROP("No core", NULL);
      if (*File)
	{
	  static char * Comd[] = { "Name", "Phase", "Seq", "Near", "Take", NULL };
	  char line[LINE_MAX], fname[MAXPATH];
	  BMP_PTR * shplist = new BMP_PTR [ShpCnt+1];
	  SEQ * seq = NULL;
	  int shpcnt = 0,
	      seqcnt = 0,
	      neacnt = 0,
	      takcnt = 0,
	      maxnow = 0,
	      maxnxt = 0,
	      lcnt   = 0,
	      len;

	  SNAIL::COM * nea = NULL;
	  SNAIL::COM * tak = NULL;
	  MergeExt(fname, File, SPR_EXT);
	  if (INI_FILE::Exist(fname))	// sprite description file exist
	    {
	      INI_FILE sprf(fname);
	      if (! OK(sprf))
		{
		  VGA::Exit("Bad SPR", fname);
		}

	      while ((len = sprf.Read(line)) != 0)
		{
		  ++ lcnt;
		  if (len && line[len-1] == '\n') line[-- len] = '\0';
		  if (len == 0 || *line == '.') continue;

		  switch (TakeEnum(Comd, strtok(line, " =\t")))
		    {
		      case 0 : // Name
			       SetName(strtok(NULL, "")); break;
		      case 1 : // Phase
			       shplist[shpcnt ++] = new BITMAP(strtok(NULL, " \t,;/"));
			       break;
		      case 2 : // Seq
			       seq = (SEQ *) realloc(seq, (seqcnt + 1) * sizeof(*seq));
			       if (seq == NULL)
				 VGA::Exit("No core", fname);
			       SEQ * s = &seq[seqcnt ++];
			       s->Now  = atoi(strtok(NULL, " \t,;/"));
			       if (s->Now > maxnow) maxnow = s->Now;
			       s->Next = atoi(strtok(NULL, " \t,;/"));
			       switch (s->Next)
				 {
				   case 0xFF : s->Next = seqcnt; break;
				   case 0xFE : s->Next = seqcnt-1; break;
				 }
			       if (s->Next > maxnxt) maxnxt = s->Next;
			       s->Dx   = atoi(strtok(NULL, " \t,;/"));
			       s->Dy   = atoi(strtok(NULL, " \t,;/"));
			       s->Dly  = atoi(strtok(NULL, " \t,;/"));
			       break;
		      case 3 : // Near
			       if (NearPtr != NO_PTR)
				 {
				   nea = (SNAIL::COM *) realloc(nea, (neacnt + 1) * sizeof(*nea));
				   if (nea == NULL) VGA::Exit("No core", fname);
				   else
				     {
				       SNAIL::COM * c = &nea[neacnt ++];
				       if ((c->Com = (SNCOM) TakeEnum(SNAIL::ComTxt, strtok(NULL, " \t,;/"))) < 0)
					 VGA::Exit(NumStr("Bad NEAR in ######", lcnt), fname);
				       c->Ref = atoi(strtok(NULL, " \t,;/"));
				       c->Val = atoi(strtok(NULL, " \t,;/"));
				       c->Ptr = NULL;
				     }
				 }
			       break;
		      case 4 : // Take
			       if (TakePtr != NO_PTR)
				 {
				   tak = (SNAIL::COM *) realloc(tak, (takcnt + 1) * sizeof(*tak));
				   if (tak == NULL) VGA::Exit("No core", fname);
				   else
				     {
				       SNAIL::COM * c = &tak[takcnt ++];
				       if ((c->Com = (SNCOM) TakeEnum(SNAIL::ComTxt, strtok(NULL, " \t,;/"))) < 0)
					 VGA::Exit(NumStr("Bad NEAR in ######", lcnt), fname);
				       c->Ref = atoi(strtok(NULL, " \t,;/"));
				       c->Val = atoi(strtok(NULL, " \t,;/"));
				       c->Ptr = NULL;
				     }
				 }
			       break;
		    }
		}
	    }
	  else	// no sprite description: try to read immediately from .BMP
	    {
	      shplist[shpcnt ++] = new BITMAP(File);
	    }
	  shplist[shpcnt] = NULL;
	  if (seq)
	    {
	      if (maxnow >= shpcnt) VGA::Exit("Bad PHASE in SEQ", fname);
	      if (maxnxt >= seqcnt) VGA::Exit("Bad JUMP in SEQ", fname);
	      SetSeq(seq);
	    }
	  else SetSeq((ShpCnt == 1) ? Seq1 : Seq2);
	  disable();

	  SetShapeList(shplist);
	  enable();
	  if (nea) nea[neacnt-1].Ptr = Ext->Near = nea; else NearPtr = NO_PTR;
	  if (tak) tak[takcnt-1].Ptr = Ext->Take = tak; else TakePtr = NO_PTR;
	}
      HEART::Enable = enbl;
    }
  return this;
}




SPRITE * SPRITE::Contract (void)
{
  register SPREXT * e = Ext;
  if (e)
    {
      if (e->Name) delete[] e->Name;
      if (Flags.BDel && e->ShpList)
	{
	  int i;
	  for (i = 0; e->ShpList[i]; i ++) delete e->ShpList[i];
	  if (MemType(e->ShpList) == NEAR_MEM) delete[] e->ShpList;
	}
      if (MemType(e->Seq) == NEAR_MEM) free(e->Seq);
      if (e->Near) free(e->Near);
      if (e->Take) free(e->Take);
      delete e;
      Ext = NULL;
    }
  return this;
}






SPRITE * SPRITE::BackShow (Boolean fast)
{
  Expand();
  Show(2);
  Show(1);
  if (fast) Show(0);
  Contract();
  return this;
}








void SPRITE::Step (int nr)
{
  if (nr >= 0) SeqPtr = nr;
  if (Ext)
    {
      SEQ * seq;
      if (nr < 0) SeqPtr = Ext->Seq[SeqPtr].Next;
      seq = Ext->Seq + SeqPtr;
      if (seq->Dly >= 0)
	{
	  Goto(X + (seq->Dx), Y + (seq->Dy));
	  Time = seq->Dly;
	}
    }
}






void SPRITE::Tick (void)
{
  Step();
}





void SPRITE::MakeXlat (byte far * x)
{
  if (Ext)
    {
      BMP_PTR * b;

      if (Flags.Xlat) KillXlat();
      for (b = Ext->ShpList; *b; b ++) (*b)->M = x;
      Flags.Xlat = TRUE;
    }
}





void SPRITE::KillXlat (void)
{
  if (Flags.Xlat && Ext)
    {
      BMP_PTR * b;
      byte far * m = (*Ext->ShpList)->M;

      switch (MemType(m))
	{
	  case NEAR_MEM : delete[] (byte *) m; break;
	  case FAR_MEM  : farfree(m); break;
	}
      for (b = Ext->ShpList; *b; b ++) (*b)->M = NULL;
      Flags.Xlat = FALSE;
    }
}





void SPRITE::Goto (int x, int y)
{
  int xo = X, yo = Y;
  if (W < SCR_WID)
    {
      if (x < 0) x = 0;
      if (x + W > SCR_WID) x = (SCR_WID - W);
      X = x;
    }
  if (H < SCR_HIG)
    {
      if (y < 0) y = 0;
      if (y + H > SCR_HIG) y = (SCR_HIG - H);
      Y = y;
    }
  if (Next) if (Next->Flags.Slav) Next->Goto(Next->X-xo+X, Next->Y-yo+Y);
  if (Flags.Shad) Prev->Goto(Prev->X-xo+X, Prev->Y-yo+Y);
}












void SPRITE::Center (void)
{
  Goto((SCR_WID - W) / 2, (SCR_HIG - H) / 2);
}








void SPRITE::Show (void)
{
  register SPREXT * e;
  asm	cli		// critic section...
  e = Ext;
  e->x0 = e->x1;
  e->y0 = e->y1;
  e->b0 = e->b1;
  e->x1 = X;
  e->y1 = Y;
  e->b1 = Shp();
  asm	sti		// ...done!
  if (! Flags.Hide)
    {
      if (Flags.Xlat) e->b1->XShow(e->x1, e->y1);
      else e->b1->Show(e->x1, e->y1);
    }
}







void SPRITE::Show (word pg)
{
  byte far * a = VGA::Page[1];
  VGA::Page[1] = VGA::Page[pg & 3];
  Shp()->Show(X, Y);
  VGA::Page[1] = a;
}








void SPRITE::Hide (void)
{
  register SPREXT * e = Ext;
  if (e->b0) e->b0->Hide(e->x0, e->y0);
}




BMP_PTR SPRITE::Ghost (void)
{
  register SPREXT * e = Ext;
  if (e->b1)
    {
      BMP_PTR bmp = new BITMAP(0, 0, (byte far *)NULL);
      if (bmp == NULL) VGA::Exit("No core");
      bmp->W = e->b1->W;
      bmp->H = e->b1->H;
      if ((bmp->B = farnew(HideDesc, bmp->H)) == NULL) VGA::Exit("No Core");
      bmp->V = (byte far *) _fmemcpy(bmp->B, e->b1->B, sizeof(HideDesc) * bmp->H);
      bmp->M = (byte far *) MK_FP(e->y1, e->x1);
      return bmp;
    }
  return NULL;
}






SPRITE * SpriteAt (int x, int y)
{
  SPRITE * spr = NULL, * tail = VGA::ShowQ.Last();
  if (tail)
    {
      for (spr = tail->Prev; spr; spr = spr->Prev)
	if (! spr->Flags.Hide && ! spr->Flags.Tran)
	  if (spr->Shp()->SolidAt(x-spr->X, y-spr->Y))
	    break;
    }
  return spr;
}





//--------------------------------------------------------------------------






QUEUE::QUEUE (Boolean show)
: Head(NULL), Tail(NULL), Show(show)
{
}




QUEUE::~QUEUE (void)
{
  Clear();
}




void QUEUE::Clear (void)
{
  while (Head)
    {
      SPRITE * s = Remove(Head);
      if (s->Flags.Kill) delete s;
    }
}




void QUEUE::ForAll (void (*fun)(SPRITE *))
{
  SPRITE * s = Head;
  while (s)
    {
      SPRITE * n = s->Next;
      fun(s);
      s = n;
    }
}





void QUEUE::Append (SPRITE * spr)
{
  if (Tail)
    {
      spr->Prev = Tail;
      Tail->Next = spr;
    }
  else Head = spr;
  Tail = spr;
  if (Show) spr->Expand();
  else spr->Contract();
}





void QUEUE::Insert (SPRITE * spr, SPRITE * nxt)
{
  if (nxt == Head)
    {
      spr->Next = Head;
      Head = spr;
      if (! Tail) Tail = spr;
    }
  else
    {
      spr->Next = nxt;
      spr->Prev = nxt->Prev;
      if (spr->Prev) spr->Prev->Next = spr;
    }
  if (spr->Next) spr->Next->Prev = spr;
  if (Show) spr->Expand();
  else spr->Contract();
}





void QUEUE::Insert (SPRITE * spr)
{
  SPRITE * s;
  for (s = Head; s; s = s->Next)
    if (s->Z > spr->Z)
      break;
  if (s) Insert(spr, s);
  else Append(spr);
  if (Show) spr->Expand();
  else spr->Contract();
}





SPRITE * QUEUE::Remove (SPRITE * spr)
{
  if (spr == Head) Head = spr->Next;
  if (spr == Tail) Tail = spr->Prev;
  if (spr->Next) spr->Next->Prev = spr->Prev;
  if (spr->Prev) spr->Prev->Next = spr->Next;
  spr->Prev = NULL;
  spr->Next = NULL;
  return spr;
}





SPRITE * QUEUE::Locate (int ref)
{
  SPRITE * spr;
  for (spr = Head; spr; spr = spr->Next) if (spr->Ref == ref) return spr;
  return NULL;
}





//--------------------------------------------------------------------------




word		VGA::StatAdr = VGAST1_;
word		VGA::OldMode = 0;
word far *	VGA::OldScreen = NULL;
const char *	VGA::Msg = NULL;
const char *	VGA::Nam = NULL;
DAC far *	VGA::OldColors = NULL;
DAC far *	VGA::NewColors = NULL;
Boolean		VGA::SetPal = FALSE;
int		VGA::Mono = 0;
QUEUE		VGA::ShowQ = TRUE, VGA::SpareQ = FALSE;
byte far *	VGA::Page[4] = { (byte far *) MK_FP(SCR_SEG, 0x0000),
				 (byte far *) MK_FP(SCR_SEG, 0x4000),
				 (byte far *) MK_FP(SCR_SEG, 0x8000),
				 (byte far *) MK_FP(SCR_SEG, 0xC000) };




VGA::VGA (int mode)
: FrmCnt(0)
{
  extern const char Copr[];
  Boolean std = TRUE;
  int i;
  for (i = 10; i < 20; i ++)
    {
      char * txt = Text[i];
      if (txt)
	{
	  puts(txt);
	  #ifndef DEBUG
	  std = FALSE;
	  #endif
	}
    }
  if (std) puts(Copr);
  SetStatAdr();
  if (StatAdr != VGAST1_) ++ Mono;
  if (IsVga())
    {
      OldColors = farnew(DAC, 256);
      NewColors = farnew(DAC, 256);
      OldScreen = SaveScreen();
      GetColors(OldColors);
      Sunset();
      OldMode = SetMode(mode);
      SetColors();
      Setup(VideoMode);
      Clear();
    }
}





VGA::~VGA (void)
{
  Mono = 0;
  if (IsVga())
    {
      Clear();
      SetMode(OldMode);
      SetColors();
      RestoreScreen(OldScreen);
      Sunrise(OldColors);
      if (OldColors) farfree(OldColors);
      if (NewColors) farfree(NewColors);
      if (Msg) fputs(Msg, stderr);
      if (Nam)
	{
	  fputs(" [", stderr);
	  fputs(Nam, stderr);
	  fputc(']', stderr);
	}
      if (Msg || Nam) fputc('\n', stderr);
      #ifdef REPORT
      fputs(Report, stderr);
      #endif
    }
}





void VGA::SetStatAdr (void)
{
  asm	mov	dx,VGAMIr_
  asm	in	al,dx
  asm	test	al,1		// CGA addressing mode flag
  asm	mov	ax,VGAST1_	// CGA addressing
  asm	jnz	set_mode_adr
  asm	xor	al,0x60		// MDA addressing
  set_mode_adr:
  StatAdr = _AX;
}






#pragma argsused
void VGA::WaitVR (Boolean on)
{
  _DX = StatAdr;
  _AH = (on) ? 0x00 : 0x08;

  asm	mov	cx,2
  // wait for vertical retrace on (off)
  wait:
  asm	in	al,dx
  asm	xor	al,ah
  asm	test	al,0x08
  asm	jnz	wait
  asm	xor	ah,0x08
  asm	loop	wait
}






void VGA::Setup (VgaRegBlk * vrb)
{
  WaitVR();			// *--LOOK!--* resets VGAATR logic
  asm	cld
  asm	mov	si, vrb		// take address of parameter table
  asm	mov	dh,0x03		// higher byte of I/O address is always 3

  s:
  asm	lodsw			// take lower byte of I/O address and index
  asm	or	ah,ah		// 0 = end of table
  asm	jz	xit		// no more: exit
  asm	or	al,al		// indexed register?
  asm	js	single		// 7th bit set means single register
  asm	mov	dl,ah		// complete I/O address
  asm	out	dx,al		// put index into control register
  asm	inc	dx		// data register is next to control
  asm	in	al,dx		// take old data

  write:
  asm	mov	cl,al		// preserve old data
  asm	lodsw			// take 2 masks from table
  asm	xor	al,0xFF		// invert mask bits
  asm	and	al,cl		// clear bits with "clr" mask
  asm	or	al,ah		// set bits with "set" mask
  asm	cmp	dl,0xC1		// special case?
  asm	jne	std2		// no: standard job, otherwise...
  asm	dec	dx		// data out reg shares address with index
  std2:
  asm	out	dx,al		// write new value to register
  asm	jmp	s

  single:			// read address in al, write address in ah
  asm	mov	dl,al		// complete I/O read address
  asm	in	al,dx		// take old data
  asm	mov	dl,ah		// complete I/O write address
  asm	jmp	write		// continue standard routine

  xit:
}






int VGA::SetMode (int mode)
{
  Clear();
  // get current mode
  asm	mov	ah,0x0F
  Video();			// BIOS video service
  asm	xor	ah,ah
  asm	push	ax

  // wait for v-retrace
  WaitVR();

  // set mode
  asm	xor	ah,ah
  asm	mov	al,byte ptr mode
  Video();			// BIOS video service
  SetStatAdr();
  // return previous mode
  asm	pop	ax
  return _AX;
}






void VGA::GetColors (DAC far * tab)
{
  asm	cld
  asm	les	di,tab		// color table
  asm	mov	dx,0x3C7	// PEL address read mode register
  asm	xor	al,al		// start from address 0
  asm	out	dx,al		// put address
  asm	mov	cx,256*3	// # of colors
  asm	mov	dl,0xC9		// PEL data register

//  asm	rep insb		// very fast!

  gc:				// much slower:
  asm	in	al,dx		// take 1 color
  asm	jmp	sto		// little delay
  sto:
  asm	stosb			// store 1 color
  asm	loop	gc		// next one?
}




void VGA::SetColors (DAC far * tab, int lum)
{
  DAC far * des = NewColors;
  asm	push	ds

  asm	les	di,des
  asm	lds	si,tab
  asm	mov	cx,256*3
  asm	xor	bx,bx
  asm	mov	dx,lum

  copcol:
  asm	mov	al,[si+bx]
  asm	mul	dl
  asm	shr	ax,6
  asm	mov	es:[di+bx],al
  asm	inc	bx
  asm	cmp	bx,cx
  asm	jb	copcol

  asm	pop	ds

  if (Mono)
    {
      asm	add	cx,di
      mono:
      asm	xor	dx,dx
      asm	mov	al,77	// 30% R
      asm	mul	byte ptr es:[di].0
      asm	add	dx,ax
      asm	mov	al,151	// 59% G
      asm	mul	byte ptr es:[di].1
      asm	add	dx,ax
      asm	mov	al,28	// 11% B
      asm	mul	byte ptr es:[di].2
      asm	add	dx,ax

      asm	mov	es:[di].0,dh
      asm	mov	es:[di].1,dh
      asm	mov	es:[di].2,dh

      asm	add	di,3
      asm	cmp	di,cx
      asm	jb	mono
    }
  SetPal = TRUE;
}






void VGA::SetColors (void)
{
  _fmemset(NewColors, 0, PAL_SIZ);
  UpdateColors();
}






void VGA::Sunrise (DAC far * tab)
{
  int i;
  for (i = 0; i <= 64; i += FADE_STEP)
    {
      SetColors(tab, i);
      WaitVR();
      UpdateColors();
    }
}






void VGA::Sunset (void)
{
  DAC tab[256];
  int i;
  GetColors(tab);
  for (i = 64; i >= 0; i -= FADE_STEP)
    {
      SetColors(tab, i);
      WaitVR();
      UpdateColors();
    }
}









void VGA::Show (void)
{
  SPRITE * spr = ShowQ.First();

  for (spr = ShowQ.First(); spr; spr = spr->Next) spr->Show();
  Update();
  for (spr = ShowQ.First(); spr; spr = spr->Next) spr->Hide();

  ++ FrmCnt;
}




void VGA::UpdateColors (void)
{
  DAC far * tab = NewColors;

  asm	push	ds
  asm	cld
  asm	lds	si,tab		// color table
  asm	mov	dx,0x3C8	// PEL address write mode register
  asm	xor	al,al		// start from address 0
  asm	out	dx,al		// put address
  asm	mov	cx,256*3	// # of colors
  asm	mov	dl,0xC9		// PEL data register

//  asm	rep outsb		// very fast!

 // the slower version of above:
  sc:
  asm	lodsb			// take 1/3 color
  asm	out	dx,al		// put 1/3 color
  asm	jmp	loop		// little delay
  loop:
  asm	loop	sc		// next one?


  asm	pop	ds
}







void VGA::Update (void)
{
  byte far * p = Page[1];
  Page[1] = Page[0];
  Page[0] = p;

  asm	mov	dx,VGACRT_
  asm	mov	al,0x0D
  asm	mov	ah,byte ptr p
  asm	out	dx,ax
  asm	dec	al
  asm	mov	ah,byte ptr p+1
  asm	out	dx,ax

  if (! SpeedTest) WaitVR();

  if (SetPal)
    {
      UpdateColors();
      SetPal = FALSE;
    }
}








void VGA::Clear (byte color)
{
  byte far * a = (byte far *) MK_FP(SCR_SEG, 0);

  asm	mov	dx,VGASEQ_
  asm	mov	ax,0x0F02	// map mask register - enable all planes
  asm	out	dx,ax
  asm	les	di,a
  asm	cld

  asm	mov	cx,0xFFFF
  asm	mov	al,color
  asm	rep stosb
  asm	stosb
}






void VGA::CopyPage (word d, word s)
{
  byte far * S = Page[s & 3], far * D = Page[d & 3];

  asm	mov	dx,VGAGRA_
  asm	mov	al,0x05		// R/W mode
  asm	out	dx,al
  asm	inc	dx
  asm	in	al,dx
  asm	and	al,0xF4
  asm	push	ax
  asm	push	dx
  asm	or	al,0x01
  asm	out	dx,al

  asm	mov	dx,VGASEQ_
  asm	mov	ax,0x0F02	// map mask register - enable all planes
  asm	out	dx,ax

  asm	push	ds

  asm	les	di,D
  asm	lds	si,S
  asm	cld
  asm	mov	cx,0x4000
  asm	rep movsb

  asm	pop	ds

  asm	pop	dx
  asm	pop	ax
  asm	out	dx,al		// end of copy mode
}







void VGA::Exit (const char * txt, const char * name)
{
  Msg = txt;
  Nam = name;
  #ifdef REPORT
    wtom(coreleft(), Report + NREP, 10, 5);
    dwtom(farcoreleft(), Report + FREP, 10, 6);
  #endif
  exit(0);
}




void VGA::Exit (int tref, const char * name)
{
  Exit(Text[tref], name);
}




//--------------------------------------------------------------------------




void BITMAP::XShow (int x, int y)
{
  byte rmsk = x % 4,
       mask = 1 << rmsk,
       far * scr = VGA::Page[1] + y * (SCR_WID / 4) + x / 4;
  byte near * m = (char *) M;
  byte far  * v = V;

	asm	push	bx
	asm	push	si
	asm	push	ds

	asm	cld
	asm	les	di,scr
	asm	lds	si,v
	asm	mov	bx,m



	asm	mov	al,0x02		// map mask register
	asm	mov	ah,mask

  plane:
	// enable output plane
	asm	mov	dx,VGASEQ_
	asm	out	dx,ax
	asm	push	ax

	// select input plane
	asm	mov	dx,VGAGRA_
	asm	mov	al,0x04		// read map select register
	asm	mov	ah,rmsk
	asm	out	dx,ax

	asm	push	di

  block:
	asm	lodsw
	asm	mov	cx,ax
	asm	and	ch,0x3F
	asm	test	ah,0xC0
	asm	jz	endpl
	asm	jns	skip
	asm	jnp	incsi		// replicate?
	asm	add	si,cx		// skip over data block
	asm	dec	si		// fix it before following inc

  incsi:
	asm	inc	si
  tint:
	asm	mov	al,es:[di]
	//-----------------------------------------------
	// asm	xlat	ss:0	// unsupported with BASM!
	__emit__(0x36, 0xD7);	// this stands for above!
	//-----------------------------------------------
	asm	stosb
	asm	loop	tint
	asm	jmp	block

  skip:
	asm	add	di,cx
	asm	jmp	block

  endpl:
	asm	pop	di
	asm	pop	ax
	asm	inc	rmsk
	asm	shl	ah,1
	asm	test	ah,0x10
	asm	jz	x_chk
	asm	mov	ah,0x01
	asm	mov	rmsk,0
	asm	inc	di
  x_chk:
	asm	cmp	ah,mask
	asm	jne	plane
	asm	pop	ds
	asm	pop	si
	asm	pop	bx
}






void BITMAP::Show (int x, int y)
{
  byte mask = 1 << (x & 3),
       far * scr = VGA::Page[1] + y * (SCR_WID >> 2) + (x >> 2);
  byte far * v = V;

	asm	push	ds		// preserve DS

	asm	cld			// normal direction
	asm	les	di,scr		// screen address
	asm	lds	si,v		// picture address
	asm	mov	dx,VGASEQ_	// VGA reg
	asm	mov	al,0x02
	asm	mov	ah,mask

  plane:
	asm	out	dx,ax
	asm	push	ax
	asm	push	di

  block:
	asm	mov	cx,[si]		// with ADD faster then LODSW
	asm	add	si,2
	asm	test	ch,0xC0
	asm	jns	skip		// 1 (SKP) or 0 (EOI)
	asm	jpo	repeat		// 2 (REP)

  copy:					// 3 (CPY)
	asm	and	ch,0x3F
	asm	shr	cx,1
	asm	rep	movsw
	asm	jnc	block
	asm	movsb
	asm	jmp	block

  repeat:
	asm	and	ch,0x3F
	asm	mov	al,[si]
	asm	inc	si
	asm	mov	ah,al
	asm	shr	cx,1
	asm	rep stosw
	asm	jnc	block
	asm	mov	es:[di],al
	asm	inc	di
	asm	jmp	block

  skip:
	asm	jz	endpl
	asm	and	ch,0x3F
	asm	add	di,cx
	asm	jmp	block

  endpl:
	asm	pop	di
	asm	pop	ax
	asm	shl	ah,1
	asm	test	ah,0x10
	asm	jz	x_chk
	asm	mov	ah,0x01
	asm	inc	di
  x_chk:
	asm	cmp	ah,mask
	asm	jne	plane
	asm	pop	ds
}





void BITMAP::Hide (int x, int y)
{
  byte far * scr = VGA::Page[1] + y * (SCR_WID / 4) + x / 4;
  word d = FP_OFF(VGA::Page[2]) - FP_OFF(VGA::Page[1]);
  HideDesc far * b = B;
  word extra = ((x & 3) != 0);
  word h = H;

//	asm	push	bx
	asm	push	si
	asm	push	ds

	asm	cld
	asm	les	di,scr
	asm	mov	si,di
	asm	add	si,d		// take bytes from background page
	asm	lds	bx,b

	asm	mov	dx,VGAGRA_
	asm	mov	al,0x05		// R/W mode
	asm	out	dx,al
	asm	inc	dx
	asm	in	al,dx
	asm	and	al,0xF4
	asm	push	ax
	asm	push	dx
	asm	or	al,0x01
	asm	out	dx,al

	asm	mov	dx,VGASEQ_
	asm	mov	ax,0x0F02	// enable all planes
	asm	out	dx,ax

	asm	mov	dx,ds		// save DS

  row:
// skip block
	asm	mov	cx,[bx]
	asm	add	si,cx
	asm	add	di,cx
	asm	mov	cx,[bx+2]
	asm	add	bx,4
	asm	add	cx,extra

	asm	push	es
	asm	pop	ds		// set DS to video seg
	asm	rep movsb		// move bytes fast
	asm	sub	si,extra
	asm	sub	di,extra
	asm	mov	ds,dx		// restore DS

	asm	dec	h
	asm	jnz	row

	asm	pop	dx
	asm	pop	ax
	asm	out	dx,al		// end of copy mode


	asm	pop	ds
	asm	pop	si
//	asm	pop	bx
}







//--------------------------------------------------------------------------


