// SPDX-FileCopyrightText: 2002, 2013 Niels Möller
// SPDX-License-Identifier: LGPL-3.0-only

/* aes192-encrypt.c

   Encryption function for the aes/rijndael block cipher.

   Copyright (C) 2002, 2013 Niels Möller

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>

#include "aes-internal.h"

/* For fat builds */
#if HAVE_NATIVE_aes192_encrypt
void
_nettle_aes192_encrypt_c(const struct aes192_ctx *ctx,
	       size_t length, uint8_t *dst,
	       const uint8_t *src);
# define nettle_aes192_encrypt _nettle_aes192_encrypt_c
#endif

void
nettle_aes192_encrypt(const struct aes192_ctx *ctx,
	       size_t length, uint8_t *dst,
	       const uint8_t *src)
{
  assert(!(length % AES_BLOCK_SIZE) );
  _nettle_aes_encrypt(_AES192_ROUNDS, ctx->keys, &_nettle_aes_encrypt_table,
		      length, dst, src);
}
