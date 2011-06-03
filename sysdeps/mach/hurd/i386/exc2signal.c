/* Translate Mach exception codes into signal numbers.  i386 version.
   Copyright (C) 1991,1992,1994,1996,1997,2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <hurd.h>
#include <hurd/signal.h>
#include <mach/exception.h>

/* Translate the Mach exception codes, as received in an `exception_raise' RPC,
   into a signal number and signal subcode.  */

void
_hurd_exception2signal (struct hurd_signal_detail *detail, int *signo)
{
  detail->error = 0;

  switch (detail->exc)
    {
    default:
      *signo = SIGIOT;
      detail->code = detail->exc;
      break;

    case EXC_BAD_ACCESS:
      switch (detail->exc_code)
        {
	case KERN_INVALID_ADDRESS:
	case KERN_MEMORY_FAILURE:
	  *signo = SIGSEGV;
	  detail->code = SEGV_MAPERR;
	  break;

	case KERN_PROTECTION_FAILURE:
	case KERN_WRITE_PROTECTION_FAILURE:
	  *signo = SIGSEGV;
	  detail->code = SEGV_ACCERR;
	  break;

	default:
	  *signo = SIGBUS;
	  detail->code = 0;
	  break;
	}
      detail->error = detail->exc_code;
      break;

    case EXC_BAD_INSTRUCTION:
      *signo = SIGILL;
      switch (detail->exc_code)
        {
	case EXC_I386_INVOPFLT:
	  detail->code = ILL_ILLOPC;
	  break;

	case EXC_I386_STKFLT:
	  detail->code = ILL_BADSTK;
	  break;

	case EXC_I386_INVOP:
	default:
	  detail->code = 0;
	  break;
	}
      break;

    case EXC_ARITHMETIC:
      *signo = SIGFPE;
      switch (detail->exc_code)
	{
	case EXC_I386_DIV:	/* integer divide by zero */
	  detail->code = FPE_INTDIV;
	  break;

	case EXC_I386_INTO:	/* integer overflow */
	  detail->code = FPE_INTOVF;
	  break;

	  /* These aren't anywhere documented or used in Mach 3.0.  */
	case EXC_I386_NOEXT:
	case EXC_I386_EXTOVR:
	default:
	  detail->code = 0;
	  break;

	case EXC_I386_EXTERR:
	  /* Subcode is the fp_status word saved by the hardware.
	     Give an error code corresponding to the first bit set.  */
	  if (detail->exc_subcode & FPS_IE)
	    {
	      detail->code = FPE_FLTINV;
	    }
	  else if (detail->exc_subcode & (FPS_DE | FPS_UE))
	    {
	      detail->code = FPE_FLTUND;
	    }
	  else if (detail->exc_subcode & FPS_ZE)
	    {
	      detail->code = FPE_FLTDIV;
	    }
	  else if (detail->exc_subcode & FPS_OE)
	    {
	      detail->code = FPE_FLTOVF;
	    }
	  else if (detail->exc_subcode & FPS_PE)
	    {
	      detail->code = FPE_FLTRES;
	    }
	  else
	    {
	      detail->code = 0;
	    }
	  break;

	  /* These two can only be arithmetic exceptions if we
	     are in V86 mode.  (See Mach 3.0 i386/trap.c.)  */
	case EXC_I386_EMERR:
	  detail->code = 0;
	  break;
	case EXC_I386_BOUND:
	  detail->code = FPE_FLTSUB;
	  break;
	}
      break;

    case EXC_EMULATION:
      /* 3.0 doesn't give this one, why, I don't know.  */
      *signo = SIGEMT;
      detail->code = 0;
      break;

    case EXC_SOFTWARE:
      /* The only time we get this in Mach 3.0
	 is for an out of bounds trap.  */
      if (detail->exc_code == EXC_I386_BOUND)
	{
	  *signo = SIGFPE;
	  detail->code = FPE_FLTSUB;
	}
      else
	{
	  *signo = SIGEMT;
	  detail->code = 0;
	}
      break;

    case EXC_BREAKPOINT:
      *signo = SIGTRAP;
      switch (detail->exc_code)
        {
	case EXC_I386_SGL:
	case EXC_I386_BPT:
	  detail->code = TRAP_BRKPT;
	  break;

	default:
	  detail->code = 0;
	  break;
	}
      break;
    }
}
