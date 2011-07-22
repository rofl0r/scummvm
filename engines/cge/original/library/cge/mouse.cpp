#include	"mouse.h"
#include	"text.h"
#include	<dos.h>



	EVENT		Evt[EVT_MAX];

	word		EvtHead = 0, EvtTail = 0;
//--------------------------------------------------------------------------

MOUSE_FUN *	MOUSE::OldMouseFun	= NULL;
word		MOUSE::OldMouseMask	= 0;



//--------------------------------------------------------------------------





MOUSE::MOUSE (BITMAP ** shpl)
 : SPRITE(shpl), Busy(NULL), Hold(NULL), hx(0)
{
  static SEQ ms[] = { { 0,0,0,0,1 }, { 1,1,0,0,1 } };
  SetSeq(ms);

  // Mouse reset
  _AX = 0x0000; // soft & hard reset (0x0021 soft reset does not work)
  __int__(0x33);
  Exist = (_AX != 0);
  Buttons = _BX;

  Goto(SCR_WID/2, SCR_HIG/2);
  Z = 127;
  Step(1);
}




MOUSE::~MOUSE (void)
{
  Off();
}





//void MOUSE::SetFun (void)
//{
//}





void MOUSE::On (void)
{
  if (SeqPtr && Exist)
    {
      _CX = X + X;	// horizontal position
      _DX = Y;		// vertical position
      _AX = 0x0004;	// Set Mouse Position
      __int__(0x33);
      // set new mouse fun
      _ES = FP_SEG(NewMouseFun);
      _DX = FP_OFF(NewMouseFun);
      _CX = 0x001F;	// 11111b = all events
      _AX = 0x0014;	// Swap User-Interrupt Vector
      __int__(0x33);
      // save old mouse fun
      OldMouseMask = _CX;
      OldMouseFun = (MOUSE_FUN *) MK_FP(_ES, _DX);

      // set X bounds
      _DX = (SCR_WID - W) * 2;		// right limit
      _CX = 0;				// left limit
      _AX = 0x0007;			// note: each pixel = 2
      __int__(0x33);

      // set Y bounds
      _DX = SCR_HIG - H;		// bottom limit
      _CX = 0;				// top limit
      _AX = 0x0008;
      __int__(0x33);

      Step(0);
      if (Busy) Busy->Step(0);
    }
}






void MOUSE::Off (void)
{
  if (SeqPtr == 0)
    {
      if (Exist)
	{
	  // bring back old mouse fun
	  _ES = FP_SEG(OldMouseFun);
	  _DX = FP_OFF(OldMouseFun);
	  _CX = OldMouseMask;
	  _AX = 0x0014;		// Swap User-Interrupt Vector
	  __int__(0x33);
	}
      Step(1);
      if (Busy) Busy->Step(1);
    }
}






void MOUSE::ClrEvt (SPRITE * spr)
{
  if (spr)
    {
      word e;
      for (e = EvtTail; e != EvtHead; e = (e + 1) % EVT_MAX)
	if (Evt[e].Ptr == spr) Evt[e].Msk = 0;
    }
  else EvtTail = EvtHead;
}






void MOUSE::Tick (void)
{
  Step();
  while (EvtTail != EvtHead)
    {
      EVENT e = Evt[EvtTail];
      if (e.Msk)
	{
	  if (Hold && e.Ptr != Hold)
	    {
	      Hold->Touch(e.Msk | ATTN, e.X - Hold->X, e.Y - Hold->Y);
	    }

	  // update mouse cursor position
	  if (e.Msk & ROLL)
	    {
	      Goto(e.X, e.Y);
	    }

	  // activate current touched SPRITE
	  if (e.Ptr)
	    {
	      if (e.Msk & KEYB) e.Ptr->Touch(e.Msk, e.X, e.Y);
	      else e.Ptr->Touch(e.Msk, e.X - e.Ptr->X, e.Y - e.Ptr->Y);
	    }
	  else if (Sys) Sys->Touch(e.Msk, e.X, e.Y);

	  if (e.Msk & L_DN)
	    {
	      Hold = e.Ptr;
	      if (Hold)
		{
		  Hold->Flags.Hold = TRUE;
		  #ifndef DEBUG
		  if (Hold->Flags.Drag)
		  #endif
		    {
		      hx = e.X - Hold->X;
		      hy = e.Y - Hold->Y;
		    }
		}
	    }

	  if (e.Msk & L_UP)
	    {
	      if (Hold)
		{
		  Hold->Flags.Hold = FALSE;
		  Hold = NULL;
		}
	    }
	  ///Touched = e.Ptr;

	  // discard Text if button released
	  if (e.Msk & (L_UP | R_UP)) KillText();
	}
      EvtTail = (EvtTail + 1) % EVT_MAX;
    }
  if (Hold)
    #ifndef DEBUG
    if (Hold->Flags.Drag)
    #endif
      Hold->Goto(X-hx, Y-hy);
}



//--------------------------------------------------------------------------
