#include	<general.h>
#include	"sound.h"
#include	"snail.h"
#include	"vga13h.h"
#include	"bitmaps.h"
#include	"text.h"
#include	"mouse.h"
#include	"cge.h"
#include	<dos.h>
#include	<alloc.h>
#include	<mem.h>
#include	<stdio.h>
#include	<stdlib.h>

#include	"keybd.h"

	int		MaxCave		=  0;

	SCB		Scb		= { NULL, 0, NULL };
	Boolean		Flag[4];
	Boolean		Dark 		= FALSE;
	Boolean		Game		= FALSE;
	int		Now		=  1;
	int		Lev		= -1;
	SNAIL		Snail		= FALSE;
	SNAIL		Snail_		= TRUE;

extern	SPRITE		PocLight;

//-------------------------------------------------------------------------
//	SPRITE *	Pocket[POCKET_NX]={ NULL, NULL, NULL, NULL,
//					    NULL, NULL, NULL, NULL, };
//	int		PocPtr		=  0;
//-------------------------------------------------------------------------
extern	SPRITE *	Pocket[];
extern	int		PocPtr;
//-------------------------------------------------------------------------

extern	DAC far *	SysPal;
extern	MOUSE		Mouse;



//-------------------------------------------------------------------------


static void SNGame (SPRITE * spr, int num)
{
  switch (num)
    {
      //--------------------------------------------------------------------
      case 1 :
	{
	  #define STAGES 8
	  #define DRESSED 3
	  static SPRITE * dup[3] = { NULL, NULL, NULL };
	  int buref;
	  int Stage;

	  for (dup[0] = VGA::ShowQ.First(); dup[0]; dup[0] = dup[0]->Next)
	    {
	      buref = dup[0]->Ref;
	      if (buref / 1000 == 16 && buref % 100 == 6)
		{
		  Stage = (buref / 100) % 10;
		  break;
		}
	    }
	  if (dup[1] == NULL)
	    {
	      dup[1] = VGA::ShowQ.Locate(16003);    // pan
	      dup[2] = VGA::ShowQ.Locate(16004);    // pani
	    }

	  if (Game) // continue game
	    {
	      int i = random(3), hand = (dup[0]->ShpCnt == 6);
	      ++ Stage;
	      if (hand && Stage > DRESSED) ++ hand;
	      if (
		  Debug( i >= 0 || )
		  dup[i] == spr && random(3) == 0)
		{
		  SNPOST(SNSEQ, -1, 3, dup[0]); // yes
		  SNPOST(SNSEQ, -1, 3, dup[1]); // yes
		  SNPOST(SNSEQ, -1, 3, dup[2]); // yes
		  SNPOST(SNTNEXT, -1, 0, dup[0]); // reset Take
		  SNPOST(SNTNEXT, -1, 0, dup[1]); // reset Take
		  SNPOST(SNTNEXT, -1, 0, dup[2]); // reset Take
		  SNPOST(SNNNEXT, -1, 0, dup[0]); // reset Near
		  SNPOST(SNPAUSE, -1, 72, NULL); // little rest
		  SNPOST(SNSAY, 1, 16009, NULL); // hura
		  SNPOST(SNSAY, buref, 16010, NULL); // siadaj
		  SNPOST(SNSAY, 1, 16011, NULL); // postoj�

		  if (hand)
		    {
		      SNPOST(SNSEND, 16060+hand, 16, NULL); // dawaj r�k�
		      SNPOST(SNSEQ, buref, 4, NULL); // zdejmowanie
		      SNPOST(SNSEQ, 16060+hand, 1, NULL); // ruch
		      SNPOST(SNSOUND, 16060+hand, 16002, NULL); // szelest
		      SNPOST(SNWAIT, 16060+hand, 3, NULL); // podniesie
		      SNPOST(SNSWAP, buref, buref+100, NULL); // rozdziana
		      SNPOST(SNSEQ, 16016, Stage, NULL); // ro�nie kupa
		      SNPOST(SNSEND, 16060+hand, -1, NULL); // chowaj r�k�
		      SNPOST(SNWAIT, 16060+hand, -1, NULL); // r�ka zamar�a
		    }
		  else
		    {
		      SNPOST(SNSEQ, buref, 4, NULL); // zdejmowanie
		      SNPOST(SNSOUND, 16060+hand, 16002, NULL); // szelest
		      SNPOST(SNWAIT, buref, -1, NULL); // zdejmie
		      SNPOST(SNSWAP, buref, buref+100, NULL); // rozdziana
		      SNPOST(SNSEQ, 16016, Stage, NULL); // ro�nie kupa
		    }
		  //SNPOST(SNSEQ, buref+100, 0, NULL); // reset
		  SNPOST(SNPAUSE, -1, 72, NULL); // chwilk�...

		  SNPOST(SNSEQ, -1, 0, dup[1]); // odstaw Go
		  SNPOST(SNSETXY, -1, 203 + SCR_WID * 49, dup[1]);
		  SNPOST(SNSETZ, -1, 7, dup[1]);

		  SNPOST(SNSEQ, -1, 0, dup[2]); // odstaw J�
		  SNPOST(SNSETXY, -1, 182 + SCR_WID * 62, dup[2]);
		  SNPOST(SNSETZ, -1, 9, dup[2]);
		  Game = 0;
		  return;
		}
	      else
		{
		  SNPOST(SNSEQ, -1, 2, dup[0]); // no
		  SNPOST(SNSEQ, -1, 2, dup[1]); // no
		  SNPOST(SNSEQ, -1, 2, dup[2]); // no
		  SNPOST(SNPAUSE, -1, 72, NULL); // 1 sec
		}
	    }
	  SNPOST(SNWALK, 198, 134, NULL);    // na miejsce
	  SNPOST(SNWAIT, 1, -1, NULL);       // stoi
	  SNPOST(SNCOVER, 1, 16101, NULL);   // ch�op do bicia
	  SNPOST(SNSEQ, 16101, 1, NULL);     // wystaw
	  SNPOST(SNWAIT, 16101, 5, NULL);    // czekaj
	  SNPOST(SNPAUSE, 16101, 24, NULL);  // czekaj chwil�
	  SNPOST(SNSEQ, 16040, 1, NULL);     // plask
	  SNPOST(SNSOUND, 16101, 16001, NULL); // plask!
	  SNPOST(SNPAUSE, 16101, 24, NULL);  // czekaj chwil�
	  SNPOST(SNSEQ, 16040, 0, NULL);     // schowaj plask
	  SNPOST(SNWAIT, 16101, -1, NULL);   // stoi
	  SNPOST(SNUNCOVER, 1, 16101, NULL); // SDS
	  if (! Game)
	    {
	      SNPOST(SNSAY, buref, 16008, NULL); // zgadnij!
	      Game = TRUE;
	    }
	  #undef STEPS
	  #undef DRESSED
	} break;
      //--------------------------------------------------------------------
      case 2 :
	{
	  static SPRITE * k = NULL, * k1, * k2, * k3;
	  static int count = 0;
	  Boolean hit;

	  if (k == NULL)
	    {
	      k  = VGA::ShowQ.Locate(20700);
	      k1 = VGA::ShowQ.Locate(20701);
	      k2 = VGA::ShowQ.Locate(20702);
	      k3 = VGA::ShowQ.Locate(20703);
	    }

	  if (! Game) // init
	    {
	      SNPOST(SNGAME, 20002, 2, NULL);
	      Game = TRUE;
	    }
	  else // cont
	    {
	      k1->Step(random(6));
	      k2->Step(random(6));
	      k3->Step(random(6));
	      ///--------------------
	      if (spr->Ref == 1 && KEYBOARD::Key[ALT])
		{
		  k1->Step(5);
		  k2->Step(5);
		  k3->Step(5);
		}
	      ///--------------------
	      SNPOST(SNSETZ, 20700, 0, NULL);
	      hit = (k1->SeqPtr + k2->SeqPtr + k3->SeqPtr == 15);
	      if (hit)
		{
		  if (spr->Ref == 1)
		    {
		      SNPOST(SNSAY,      1, 20003, NULL); // hura!
		      SNPOST(SNSEQ,  20011,     2, NULL); // kamera won
		      SNPOST(SNSEND, 20701,    -1, NULL); // k1 won
		      SNPOST(SNSEND, 20702,    -1, NULL); // k2 won
		      SNPOST(SNSEND, 20703,    -1, NULL); // k3 won
		      SNPOST(SNSEND, 20700,    -1, NULL); // tv won
		      SNPOST(SNKEEP, 20007,     0, NULL); // do kieszeni
		      SNPOST(SNSEND, 20006,    20, NULL); // bilon
		      SNPOST(SNSOUND,20006, 20002, NULL); // bilon!
		      SNPOST(SNSAY,  20002, 20004, NULL);
		      SNPOST(SNSEND, 20010,    20, NULL); // papier
		      SNPOST(SNSOUND,20010, 20003, NULL); // papier!
		      SNPOST(SNSAY,  20001, 20005, NULL);
		      Game = FALSE;
		      return;
		    }
		  else k3->Step(random(5));
		}
	      if (count < 100)
		{
		  switch (count)
		    {
		      case 15 : SNPOST(SNSAY, 20003, 20021, NULL); break;
		      case 30 :
		      case 45 :
		      case 60 :
		      case 75 : SNPOST(SNSAY, 20003, 20022, NULL); break;
		    }
		  ++ count;
		}
	      switch (spr->Ref)
		{
		  case     1 : SNPOST(SNSAY,  20001, 20011, NULL); // zapro
			       SNPOST(SNSEQ,  20001, 1, NULL);     // rzu�
			       SNPOST(SNWAIT, 20001, 1, NULL);     // czekaj
			       SNPOST(SNSETZ, 20700, 2, NULL);     // skryj k
			       SNPOST(SNHIDE, 20007, 1, NULL);     // skryj k
			       SNPOST(SNWAIT, 20001, 16, NULL);    // czekaj
			       SNPOST(SNSEQ,  20007, 1, NULL);     // lec�
			       SNPOST(SNHIDE, 20007, 0, NULL);     // poka�
			       SNPOST(SNSOUND,20007, 20001, NULL); // grzech
			       SNPOST(SNWAIT, 20007, -1, NULL);    // koniec
			       SNPOST(SNGAME, 20001, 2, NULL);     // again!
			       break;
		  case 20001 : SNPOST(SNSAY, 20002, 20012, NULL); // zapro
			       SNPOST(SNSEQ, 20002, 1, NULL);     // rzu�
			       SNPOST(SNWAIT, 20002, 3, NULL);    // czekaj
			       SNPOST(SNSETZ, 20700, 2, NULL);    // skryj k
			       SNPOST(SNHIDE, 20007, 1, NULL);    // skryj k
			       SNPOST(SNWAIT, 20002, 10, NULL);   // czekaj
			       SNPOST(SNSEQ, 20007, 2, NULL);     // lec�
			       SNPOST(SNHIDE, 20007, 0, NULL);    // poka�
			       SNPOST(SNSOUND,20007, 20001, NULL); // grzech
			       SNPOST(SNWAIT, 20007, -1, NULL);   // koniec
			       SNPOST(SNGAME, 20002, 2, NULL);    // again!
			       break;
		  case 20002 : SNPOST(SNSAY, 20002, 20010, NULL); // zapro
			       SNPOST(SNWALK, 20005, -1, NULL);   // do stol
			       SNPOST(SNWAIT, 1, -1, NULL);       // stoi
			       SNPOST(SNCOVER, 1, 20101, NULL);   // grasol
			       SNPOST(SNSEQ, 20101, 1, NULL);     // rzu�
			       SNPOST(SNWAIT, 20101, 5, NULL);    // czekaj
			       SNPOST(SNSETZ, 20700, 2, NULL);    // skryj k
			       SNPOST(SNHIDE, 20007, 1, NULL);    // skryj k
			       SNPOST(SNWAIT, 20101, 15, NULL);   // czekaj
			       SNPOST(SNSEQ, 20007, 1, NULL);     // lec�
			       SNPOST(SNHIDE, 20007, 0, NULL);    // poka�
			       SNPOST(SNSOUND,20007, 20001, NULL); // grzech
			       SNPOST(SNWAIT, 20101, -1, NULL);   // koniec
			       SNPOST(SNUNCOVER, 1, 20101, NULL); // SDS
			       SNPOST(SNGAME, 1, 2, NULL);        // again!
			       break;
		}
	    }
	} break;
      //--------------------------------------------------------------------
    }
}


//-------------------------------------------------------------------------




void ExpandSprite (SPRITE * spr)
{
  if (spr) VGA::ShowQ.Insert(VGA::SpareQ.Remove(spr));
}





void ContractSprite (SPRITE * spr)
{
  if (spr) VGA::SpareQ.Append(VGA::ShowQ.Remove(spr));
}







int FindPocket (SPRITE * spr)
{
  int i;
  for (i = 0; i < POCKET_NX; i ++) if (Pocket[i] == spr) return i;
  return -1;
}





void SelectPocket (int n)
{
  if (n < 0 || (PocLight.SeqPtr && PocPtr == n))
    {
      PocLight.Step(0);
      n = FindPocket(NULL);
      if (n >= 0) PocPtr = n;
    }
  else
    {
      if (Pocket[n] != NULL)
	{
	  PocPtr = n;
	  PocLight.Step(1);
	}
    }
  PocLight.Goto(POCKET_X+PocPtr*POCKET_DX+POCKET_SX, POCKET_Y+POCKET_SY);
}





void PocFul (void)
{
  Hero->Park();
  SNPOST(SNWAIT, -1, -1, Hero);
  SNPOST(SNSEQ, -1, POC_FUL, Hero);
  SNPOST(SNSOUND, -1, 2, Hero);
  SNPOST(SNWAIT, -1, -1, Hero);
  SNPOST(SNSAY,  1, POC_FUL_TEXT, Hero);
}




void Hide1 (SPRITE * spr)
{
  SNPOST_(SNGHOST, -1, 0, spr->Ghost());
}




void SNGhost (BITMAP * bmp)
{
  bmp->Hide(FP_OFF(bmp->M), FP_SEG(bmp->M));
  bmp->M = NULL;
  delete bmp;
}




void FeedSnail (SPRITE * spr, SNLIST snq)
{
  if (spr) if (spr->Active())
    {
      byte ptr = (snq == TAKE) ? spr->TakePtr : spr->NearPtr;

      if (ptr != NO_PTR)
	{
	  SNAIL::COM * comtab = spr->SnList(snq);
	  SNAIL::COM * c = comtab + ptr;

	  if (FindPocket(NULL) < 0) // no empty pockets?
	    {
	      SNAIL::COM * p;
	      for (p = c; p->Com != SNNEXT; p ++) // find KEEP command
		{
		  if (p->Com == SNKEEP)
		    {
		      PocFul();
		      return;
		    }
		  if (p->Ptr) break;
		}
	    }
	  while (TRUE)
	    {
	      if (c->Com == SNTALK)
		{
		  if ((Snail.TalkEnable = (c->Val != 0)) == FALSE) KillText();
		}
	      if (c->Com == SNNEXT)
		{
		  SPRITE * s = (c->Ref < 0) ? spr : Locate(c->Ref);
		  if (s)
		    {
		      byte * idx = (snq == TAKE) ? &s->TakePtr : &s->NearPtr;
		      if (*idx != NO_PTR)
			{
			  int v;
			  switch (c->Val)
			    {
			      case -1 : v = c - comtab + 1; break;
			      case -2 : v = c - comtab; break;
			      case -3 : v = -1; break;
			      default : v = c->Val; break;
			    }
			  if (v >= 0) *idx = v;
			}
		    }
		  if (s == spr) break;
		}
	      if (c->Com == SNIF)
		{
		  SPRITE * s = (c->Ref < 0) ? spr : Locate(c->Ref);
		  if (s) // sprite extsts
		    {
		      if (! s->SeqTest(-1)) c = comtab + c->Val; // not parked
		      else ++ c;
		    }
		  else ++ c;
		}
	      else
		{
		  SNPOST(c->Com, c->Ref, c->Val, spr);
		  if (c->Ptr) break;
		  else ++ c;
		}
	    }
	}
    }
}






//--------------------------------------------------------------------------

char *		SNAIL::ComTxt[] = { "LABEL", "PAUSE", "WAIT", "LEVEL",
				    "HIDE", "SAY", "INF", "TIME",
				    "CAVE", "KILL", "RSEQ",
				    "SEQ", "SEND", "SWAP", "KEEP", "GIVE",
				    "IF", "GAME", "SETX0", "SETY0", "SLAVE",
				    "SETXY", "RELX", "RELY", "RELZ",
				    "SETX", "SETY", "SETZ", "TRANS", "PORT",
				    "NEXT","NNEXT", "TNEXT", "RNNEXT", "RTNEXT",
				    "RMNEAR", "RMTAKE", "FLAG", "SETREF",
				    "BACKPT", "FLASH", "LIGHT",
				    "SETHB", "SETVB",
				    "WALK", "REACH", "COVER", "UNCOVER",
				    "CLEAR", "TALK", "MOUSE",
				    "SOUND", "COUNT",
				    NULL };



SNAIL::SNAIL (Boolean turbo)
: Turbo(turbo), Busy(FALSE), TextDelay(FALSE),
  Pause(0), TalkEnable(TRUE),
  Head(0), Tail(0), SNList(farnew(COM, 256))
{
}






SNAIL::~SNAIL (void)
{
  if (SNList) farfree(SNList);
}






void SNAIL::AddCom (SNCOM com, int ref, int val, void * ptr)
{
  _disable();
  COM far * snc = &SNList[Head ++];
  snc->Com = com;
  snc->Ref = ref;
  snc->Val = val;
  snc->Ptr = ptr;
  if (com == SNCLEAR)
    {
      Tail = Head;
      KillText();
      Pause = 0;
    }
  _enable();
}




void SNAIL::InsCom (SNCOM com, int ref, int val, void * ptr)
{
  COM far * snc;

  _disable();
  if (Busy)
    {
      SNList[(Tail-1)&0xFF] = SNList[Tail];
      snc = &SNList[Tail];
    }
  else snc = &SNList[(Tail-1)&0xFF];
  -- Tail;
  snc->Com = com;
  snc->Ref = ref;
  snc->Val = val;
  snc->Ptr = ptr;
  if (com == SNCLEAR)
    {
      Tail = Head;
      KillText();
      Pause = 0;
    }
  _enable();
}







static void SNNNext(SPRITE * sprel, int p)
{
  if (sprel) if (sprel->NearPtr != NO_PTR) sprel->NearPtr = p;
}






static void SNTNext(SPRITE * sprel, int p)
{
  if (sprel) if (sprel->TakePtr != NO_PTR) sprel->TakePtr = p;
}






static void SNRNNext(SPRITE * sprel, int p)
{
  if (sprel) if (sprel->NearPtr != NO_PTR) sprel->NearPtr += p;
}






static void SNRTNext(SPRITE * sprel, int p)
{
  if (sprel) if (sprel->TakePtr != NO_PTR) sprel->TakePtr += p;
}






static void SNZTrim (SPRITE * spr)
{
  if (spr) if (spr->Active())
    {
      Boolean en = HEART::Enable;
      SPRITE * s;
      HEART::Enable = FALSE;
      s = (spr->Flags.Shad) ? spr->Prev : NULL;
      VGA::ShowQ.Insert(VGA::ShowQ.Remove(spr));
      if (s)
	{
	  s->Z = spr->Z;
	  VGA::ShowQ.Insert(VGA::ShowQ.Remove(s), spr);
	}
      HEART::Enable = en;
    }
}






static void SNHide (SPRITE * spr, int val)
{
  if (spr)
    {
      spr->Flags.Hide = (val >= 0) ? (val != 0) : (! spr->Flags.Hide);
      if (spr->Flags.Shad) spr->Prev->Flags.Hide = spr->Flags.Hide;
    }
}





static void SNRmNear (SPRITE * spr)
{
  if (spr) spr->NearPtr = NO_PTR;
}





static void SNRmTake (SPRITE * spr)
{
  if (spr) spr->TakePtr = NO_PTR;
}





void SNSeq (SPRITE * spr, int val)
{
  if (spr)
    {
      if (spr == Hero && val == 0) Hero->Park();
      else spr->Step(val);
    }
}





void SNRSeq (SPRITE * spr, int val)
{
  if (spr) SNSeq(spr, spr->SeqPtr + val);
}





void SNSend (SPRITE * spr, int val)
{
  if (spr)
    {
      int was = spr->Cave;
      Boolean was1 = (was == 0 || was == Now);
      Boolean val1 = (val == 0 || val == Now);
      spr->Cave = val;
      if (val1 != was1)
	{
	  if (was1)
	    {
	      if (spr->Flags.Kept)
		{
		  int n = FindPocket(spr);
		  if (n >= 0) Pocket[n] = NULL;
		}
	      Hide1(spr);
	      ContractSprite(spr);
	      spr->Flags.Slav = FALSE;
	    }
	  else
	    {
	      if (spr->Ref % 1000 == 0) BITMAP::Pal = SysPal;
	      if (spr->Flags.Back) spr->BackShow(TRUE);
	      else ExpandSprite(spr);
	      BITMAP::Pal = NULL;
	    }
	}
    }
}





void SNSwap (SPRITE * spr, int xref)
{
  SPRITE * xspr = Locate(xref);
  if (spr && xspr)
    {
      int was = spr->Cave;
      int xwas = xspr->Cave;
      Boolean was1 = (was == 0 || was == Now);
      Boolean xwas1 = (xwas == 0 || xwas == Now);

      Swap(spr->Cave, xspr->Cave);
      Swap(spr->X, xspr->X);
      Swap(spr->Y, xspr->Y);
      Swap(spr->Z, xspr->Z);
      if (spr->Flags.Kept)
	{
	  int n = FindPocket(spr);
	  if (n >= 0) Pocket[n] = xspr;
	  xspr->Flags.Kept = TRUE;
	  xspr->Flags.Port = FALSE;
	}
      if (xwas1 != was1)
	{
	  if (was1)
	    {
	      Hide1(spr);
	      ContractSprite(spr);
	    }
	  else ExpandSprite(spr);
	  if (xwas1)
	    {
	      Hide1(xspr);
	      ContractSprite(xspr);
	    }
	  else ExpandSprite(xspr);
	}
    }
}





void SNCover (SPRITE * spr, int xref)
{
  SPRITE * xspr = Locate(xref);
  if (spr && xspr)
    {
      spr->Flags.Hide = TRUE;
      xspr->Z = spr->Z;
      xspr->Cave = spr->Cave;
      xspr->Goto(spr->X, spr->Y);
      ExpandSprite(xspr);
      if ((xspr->Flags.Shad = spr->Flags.Shad) == TRUE)
	{
	  VGA::ShowQ.Insert(VGA::ShowQ.Remove(spr->Prev), xspr);
	  spr->Flags.Shad = FALSE;
	}
      FeedSnail(xspr, NEAR);
    }
}





void SNUncover (SPRITE * spr, SPRITE * xspr)
{
  if (spr && xspr)
    {
      spr->Flags.Hide = FALSE;
      spr->Cave = xspr->Cave;
      spr->Goto(xspr->X, xspr->Y);
      if ((spr->Flags.Shad = xspr->Flags.Shad) == TRUE)
	{
	  VGA::ShowQ.Insert(VGA::ShowQ.Remove(xspr->Prev), spr);
	  xspr->Flags.Shad = FALSE;
	}
      spr->Z = xspr->Z;
      SNSend(xspr, -1);
      if (spr->Time == 0) ++ spr->Time;
    }
}





void SNSetX0 (int cav, int x0)
{
  HeroXY[cav-1].X = x0;
}





void SNSetY0 (int cav, int y0)
{
  HeroXY[cav-1].Y = y0;
}





void SNSetXY (SPRITE * spr, word xy)
{
  if (spr)
    {
      spr->Goto(xy % SCR_WID, xy / SCR_WID);
    }
}





void SNRelX (SPRITE * spr, int x)
{
  if (spr && Hero)
    {
      spr->Goto(Hero->X + x, spr->Y);
    }
}





void SNRelY (SPRITE * spr, int y)
{
  if (spr && Hero)
    {
      spr->Goto(spr->X, Hero->Y + y);
    }
}





void SNRelZ (SPRITE * spr, int z)
{
  if (spr && Hero)
    {
      spr->Z = Hero->Z + z;
      SNZTrim(spr);
    }
}





void SNSetX (SPRITE * spr, int x)
{
  if (spr)
    {
      spr->Goto(x, spr->Y);
    }
}





void SNSetY (SPRITE * spr, int y)
{
  if (spr)
    {
      spr->Goto(spr->X, y);
    }
}





void SNSetZ (SPRITE * spr, int z)
{
  if (spr)
    {
      spr->Z = z;
      //SNPOST_(SNZTRIM, -1, 0, spr);
      SNZTrim(spr);
    }
}





void SNSlave (SPRITE * spr, int ref)
{
  SPRITE * slv = Locate(ref);
  if (spr && slv)
    {
      if (spr->Active())
	{
	  SNSend(slv, spr->Cave);
	  slv->Flags.Slav = TRUE;
	  slv->Z = spr->Z;
	  VGA::ShowQ.Insert(VGA::ShowQ.Remove(slv), spr->Next);
	}
    }
}





void SNTrans (SPRITE * spr, int trans)
{
  if (spr)
    {
      spr->Flags.Tran = (trans < 0) ? !spr->Flags.Tran : (trans != 0);
    }
}





void SNPort (SPRITE * spr, int port)
{
  if (spr)
    {
      spr->Flags.Port = (port < 0) ? !spr->Flags.Port : (port != 0);
    }
}





void SNKill (SPRITE * spr)
{
  if (spr)
    {
      if (spr->Flags.Kept)
	{
	  int n = FindPocket(spr);
	  if (n >= 0) Pocket[n] = NULL;
	}
      SPRITE * nx = spr->Next;
      Hide1(spr);
      VGA::ShowQ.Remove(spr);
      MOUSE::ClrEvt(spr);
      if (spr->Flags.Kill) delete spr;
      else
	{
	  spr->Cave = -1;
	  VGA::SpareQ.Append(spr);
	}
      if (nx) if (nx->Flags.Slav) SNKill(nx);
    }
}





static void SNSound (SPRITE * spr, int wav, int cnt)
{
  if (SNDDrvInfo.DDEV)
    {
      if (wav == -1) Sound.Stop();
      else
	Sound.Play(Fx[wav], (spr) ? ((spr->X+spr->W/2)/(SCR_WID/16)) : 8, cnt);
    }
}





void SNKeep (SPRITE * spr, int stp)
{
  SelectPocket(-1);
  if (spr && ! spr->Flags.Kept && Pocket[PocPtr] == NULL)
    {
      SNSound(spr, 3, 1);
      Pocket[PocPtr] = spr;
      spr->Cave = 0;
      spr->Flags.Kept = TRUE;
      spr->Goto(POCKET_X + POCKET_DX*PocPtr + POCKET_DX/2 - spr->W/2,
		POCKET_Y + POCKET_DY/2 - spr->H/2);
      if (stp >= 0) spr->Step(stp);
    }
  SelectPocket(-1);
}






void SNGive (SPRITE * spr, int stp)
{
  if (spr)
    {
      int p = FindPocket(spr);
      if (p >= 0)
	{
	  Pocket[p] = NULL;
	  spr->Cave = Now;
	  spr->Flags.Kept = FALSE;
	  if (stp >= 0) spr->Step(stp);
	}
    }
  SelectPocket(-1);
}




static void SNBackPt (SPRITE * spr, int stp)
{
  if (spr)
    {
      if (stp >= 0) spr->Step(stp);
      spr->BackShow(TRUE);
    }
}





static void SNLevel (SPRITE * spr, int lev)
{
  #ifdef	DEMO
  static int maxcav[] = { CAVE_MAX };
  #else
  static int maxcav[] = { 1, 8, 16, 23, 24 };
  #endif
  while (Lev < lev)
    {
      SPRITE * spr;
      ++ Lev;
      spr = VGA::SpareQ.Locate(100+Lev);
      if (spr)
	{
	  spr->BackShow(TRUE);
	  spr->Cave = 0;
	}
    }
  MaxCave = maxcav[Lev];
  if (spr) spr->Flags.Hide = FALSE;
}






static void SNFlag (int fn, Boolean v)
{
  Flag[fn] = v;
}






static void SNSetRef (SPRITE * spr, int nr)
{
  if (spr)
    {
      spr->Ref = nr;
    }
}




void SNFlash (Boolean on)
{
  if (on)
    {
      DAC far * pal = farnew(DAC, PAL_CNT);
      if (pal)
	{
	  int i;
	  _fmemcpy(pal, SysPal, PAL_SIZ);
	  for (i = 0; i < PAL_CNT; i ++)
	    {
	      register int c;
	      c = pal[i].R << 1; pal[i].R = (c < 64) ? c : 63;
	      c = pal[i].G << 1; pal[i].G = (c < 64) ? c : 63;
	      c = pal[i].B << 1; pal[i].B = (c < 64) ? c : 63;
	    }
	  VGA::SetColors(pal, 64);
	}
    }
  else VGA::SetColors(SysPal, 64);
  Dark = FALSE;
}





static void SNLight (Boolean in)
{
  if (in) VGA::Sunrise(SysPal);
  else    VGA::Sunset();
  Dark = ! in;
}





static void SNBarrier (int cav, int bar, Boolean horz)
{
  ((byte *) (Barriers + ((cav > 0) ? cav : Now)))[horz] = bar;
}





static void SNWalk (SPRITE * spr, int x, int y)
{
  if (Hero)
    {
      if (spr && y < 0) Hero->FindWay(spr);
      else Hero->FindWay(XZ(x, y));
    }
}





static void SNReach (SPRITE * spr, int mode)
{
  if (Hero) Hero->Reach(spr, mode);
}






static void SNMouse (Boolean on)
{
  if (on)    Mouse.On();
  else       Mouse.Off();
}






void SNAIL::RunCom (void)
{
  static int count = 1;
  extern void SwitchCave(int);
  if (! Busy)
    {
      Busy = TRUE;
      byte tmphea = Head;
      while (Tail != tmphea)
	{
	  COM far * snc = &SNList[Tail];

	  if (! Turbo) // only for the slower one
	    {
	      if (Pause) break;
	      else
		{
		  if (TextDelay)
		    {
		      KillText();
		      TextDelay = FALSE;
		    }
		}
	      if (Talk && snc->Com != SNPAUSE) break;
	    }

	  SPRITE * sprel = ((snc->Ref >= 0) ? Locate(snc->Ref)
					    : ((SPRITE *) snc->Ptr));
	  switch (snc->Com)
	    {
	      case SNLABEL    : break;
	      case SNPAUSE    : HEART::SetXTimer(&Pause, snc->Val);
				if (Talk) TextDelay = TRUE; break;
	      case SNWAIT     : if (sprel)
				  {
				    if (sprel->SeqTest(snc->Val) &&
					(snc->Val >= 0 || sprel != Hero || Hero->TracePtr < 0))
				      {
					HEART::SetXTimer(&Pause, sprel->Time);
				      }
				    else goto xit;
				  }
				break;
	      case SNLEVEL    : SNLevel(sprel, snc->Val); break;
	      case SNHIDE     : SNHide(sprel, snc->Val); break;
	      case SNSAY      : if (sprel && TalkEnable)
				  {
				    if (sprel == Hero && sprel->SeqTest(-1))
				      sprel->Step(HTALK);
				    Say(Text[snc->Val], sprel);
				    SYSTEM::FunDel = HEROFUN0;
				  }
				break;
	      case SNINF      : if (TalkEnable)
				  {
				    Inf(Text[snc->Val]);
				    SYSTEM::FunDel = HEROFUN0;
				  }
				break;
	      case SNTIME     : if (sprel && TalkEnable)
				  {
				    if (sprel == Hero && sprel->SeqTest(-1))
				      sprel->Step(HTALK);
				    SayTime(sprel);
				  }
				break;
	      case SNCAVE     : SwitchCave(snc->Val); break;
	      case SNKILL     : SNKill(sprel); break;
	      case SNSEQ      : SNSeq(sprel, snc->Val); break;
	      case SNRSEQ     : SNRSeq(sprel, snc->Val); break;
	      case SNSEND     : SNSend(sprel, snc->Val); break;
	      case SNSWAP     : SNSwap(sprel, snc->Val); break;
	      case SNCOVER    : SNCover(sprel, snc->Val); break;
	      case SNUNCOVER  : SNUncover(sprel, (snc->Val >= 0) ? Locate(snc->Val)
								 : ((SPRITE *) snc->Ptr));
				break;
	      case SNKEEP     : SNKeep(sprel, snc->Val); break;
	      case SNGIVE     : SNGive(sprel, snc->Val); break;
	      case SNGAME     : SNGame(sprel, snc->Val); break;
	      case SNSETX0    : SNSetX0(snc->Ref, snc->Val); break;
	      case SNSETY0    : SNSetY0(snc->Ref, snc->Val); break;
	      case SNSETXY    : SNSetXY(sprel, snc->Val); break;
	      case SNRELX     : SNRelX(sprel, snc->Val); break;
	      case SNRELY     : SNRelY(sprel, snc->Val); break;
	      case SNRELZ     : SNRelZ(sprel, snc->Val); break;
	      case SNSETX     : SNSetX(sprel, snc->Val); break;
	      case SNSETY     : SNSetY(sprel, snc->Val); break;
	      case SNSETZ     : SNSetZ(sprel, snc->Val); break;
	      case SNSLAVE    : SNSlave(sprel, snc->Val); break;
	      case SNTRANS    : SNTrans(sprel, snc->Val); break;
	      case SNPORT     : SNPort(sprel, snc->Val); break;
	      case SNNEXT     : break;
	      case SNIF       : break;
	      case SNTALK     : break;
	      case SNMOUSE    : SNMouse(snc->Val != 0); break;
	      case SNNNEXT    : SNNNext(sprel, snc->Val); break;
	      case SNTNEXT    : SNTNext(sprel, snc->Val); break;
	      case SNRNNEXT   : SNRNNext(sprel, snc->Val); break;
	      case SNRTNEXT   : SNRTNext(sprel, snc->Val); break;
	      case SNRMNEAR   : SNRmNear(sprel); break;
	      case SNRMTAKE   : SNRmTake(sprel); break;
	      case SNFLAG     : SNFlag(snc->Ref & 3, snc->Val != 0); break;
	      case SNSETREF   : SNSetRef(sprel, snc->Val); break;
	      case SNBACKPT   : SNBackPt(sprel, snc->Val); break;
	      case SNFLASH    : SNFlash(snc->Val != 0); break;
	      case SNLIGHT    : SNLight(snc->Val != 0); break;
	      case SNSETHB    : SNBarrier(snc->Ref, snc->Val, TRUE); break;
	      case SNSETVB    : SNBarrier(snc->Ref, snc->Val, FALSE); break;
	      case SNWALK     : SNWalk(sprel, snc->Ref, snc->Val); break;
	      case SNREACH    : SNReach(sprel, snc->Val); break;
	      case SNSOUND    : SNSound(sprel, snc->Val, count); count = 1; break;
	      case SNCOUNT    : count = snc->Val; break;

	      case SNEXEC     : ((void(*)(int)) (snc->Ptr))(snc->Val); break;
	      case SNSTEP     : sprel->Step(); break;
	      case SNZTRIM    : SNZTrim(sprel); break;
	      case SNGHOST    : SNGhost((BITMAP *) snc->Ptr); break;
	    }
	  ++ Tail;
	  if (! Turbo) break;
	}
      xit:
      Busy = FALSE;
    }
}





Boolean SNAIL::Idle (void)
{
  return (Head == Tail);
}