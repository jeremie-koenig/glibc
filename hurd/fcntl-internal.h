/* Copyright (C) 2008 Free Software Foundation, Inc.

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


#include <fcntl.h>
#include <sys/socket.h>
#include <verify.h>

/* Do some compile-time checks for the SOCK_* constants, which we rely on.  */
verify (SOCK_CLOEXEC == O_CLOEXEC);
verify ((SOCK_MAX | SOCK_TYPE_MASK) == SOCK_TYPE_MASK);
verify ((SOCK_CLOEXEC & SOCK_TYPE_MASK) == 0);
verify ((SOCK_NONBLOCK & SOCK_TYPE_MASK) == 0);


/* Helper functions for translating between O_* and SOCK_* flags.  */

__extern_always_inline
int
sock_to_o_flags (int in)
{
  int out = 0;

  if (in & SOCK_NONBLOCK)
    out |= O_NONBLOCK;
  /* Others are passed through unfiltered.  */
  out |= in & ~(SOCK_NONBLOCK);

  return out;
}

__extern_always_inline
int
o_to_sock_flags (int in)
{
  int out = 0;

  if (in & O_NONBLOCK)
    out |= SOCK_NONBLOCK;
  /* Others are passed through unfiltered.  */
  out |= in & ~(O_NONBLOCK);

  return out;
}
