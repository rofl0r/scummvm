#include	"vmenu.h"
#include	"mouse.h"
#include	<string.h>
#include	<alloc.h>

//--------------------------------------------------------------------------


#define		RELIEF		1
#if	RELIEF
  #define	MB_LT		LGRAY
  #define	MB_RB		DGRAY
#else
  #define	MB_LT		DGRAY
  #define	MB_RB		LGRAY
#endif




MENU_BAR::MENU_BAR (word w)
{
  int h = FONT_HIG + 2 * MB_VM, i = (w += 2 * MB_HM) * h;
  byte far * p = farnew(byte, i), far * p1, far * p2;

  _fmemset(p+w, TRANS, i-2*w);
  _fmemset(p, MB_LT, w);
  _fmemset(p+i-w, MB_RB, w);
  p1 = p;
  p2 = p+i-1;
  for (i = 0; i < h; i ++)
    {
      * p1 = MB_LT;
      * p2 = MB_RB;
      p1 += w;
      p2 -= w;
    }
  TS[0] = new BITMAP(w, h, p);
  SetShapeList(TS);
  Flags.Slav = TRUE;
  Flags.Tran = TRUE;
  Flags.Kill = TRUE;
  Flags.BDel = TRUE;
}



//--------------------------------------------------------------------------

static	char *	vmgt;



char * VMGather (CHOICE * list)
{
  CHOICE * cp;
  int len = 0, h = 0;

  for (cp = list; cp->Text; cp ++)
    {
      len += strlen(cp->Text);
      ++ h;
    }
  vmgt = new byte[len+h];
  if (vmgt)
    {
      *vmgt = '\0';
      for (cp = list; cp->Text; cp ++)
	{
	  if (*vmgt) strcat(vmgt, "|");
	  strcat(vmgt, cp->Text);
	  ++ h;
	}
    }
  return vmgt;
}




VMENU *		VMENU::Addr	= NULL;
int		VMENU::Recent	= -1;




VMENU::VMENU (CHOICE * list, int x, int y)
: TALK(VMGather(list), RECT), Menu(list), Bar(NULL)
{
  CHOICE * cp;

  Addr = this;
  delete[] vmgt;
  Items = 0;
  for (cp = list; cp->Text; cp ++) ++ Items;
  Flags.BDel = TRUE;
  Flags.Kill = TRUE;
  if (x < 0 || y < 0) Center();
  else Goto(x - W / 2, y - (TEXT_VM + FONT_HIG / 2));
  VGA::ShowQ.Insert(this, VGA::ShowQ.Last());
  Bar = new MENU_BAR(W - 2 * TEXT_HM);
  Bar->Goto(X + TEXT_HM - MB_HM, Y + TEXT_VM - MB_VM);
  VGA::ShowQ.Insert(Bar, VGA::ShowQ.Last());
}




VMENU::~VMENU (void)
{
  Addr = NULL;
}




void VMENU::Touch (word mask, int x, int y)
{
#define h (FONT_HIG+TEXT_LS)
  int n = 0;
  Boolean ok = FALSE;

  if (Items)
    {
      SPRITE::Touch(mask, x, y);

      y -= TEXT_VM-1;
      //if
      if (y >= 0)
	{
	  n = y / h;
	  if (n < Items) ok = (x >= TEXT_HM && x < W - TEXT_HM/* && y % h < FONT_HIG*/);
	  else n = Items-1;
	}

      Bar->Goto(X + TEXT_HM - MB_HM, Y + TEXT_VM + n * h - MB_VM);

      if (ok && (mask & L_UP))
	{
	  Items = 0;
	  SNPOST_(SNKILL, -1, 0, this);
	  Menu[Recent = n].Proc();
	}
    }
#undef h
}


