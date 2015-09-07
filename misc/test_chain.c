/*****************************************************************************
 * chain.c: decoder chain test
 *----------------------------------------------------------------------------
 * Copyright (C) 2015 VideoLAN
 * $Id: $
 *
 * Authors: Jean-Paul Saman <jpsaman@videolan.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *----------------------------------------------------------------------------
 *
 *****************************************************************************/


#include "config.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

/* the libdvbpsi distribution defines DVBPSI_DIST */
#ifdef DVBPSI_DIST
#include "../src/dvbpsi.h"
#include "../src/psi.h"
#include "../src/chain.h"
#else
#include <dvbpsi/dvbpsi.h>
#include <dvbpsi/psi.h>
#include <dvbpsi/chain.h>
#endif

#define TEST_PASSED(msg) fprintf(stderr, "test %s -- PASSED\n", (msg));
#define TEST_FAILED(msg) fprintf(stderr, "test %s -- FAILED\n", (msg));

/* debug */
//#define _TEST_CHAIN_DEBUG
#ifdef _TEST_CHAIN_DEBUG /* debug */
static void dvbpsi_decoder_chain_dump(dvbpsi_t *p_dvbpsi)
{
    dvbpsi_decoder_t *p = (dvbpsi_decoder_t *)p_dvbpsi->p_decoder;
    while (p) {
        dvbpsi_debug(p_dvbpsi, "dump chain", "found decoder %d:%d",
                     p->i_table_id, p->i_extension);
        p = p->p_next;
    }
}
#endif

static void message(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
{
    switch(level)
    {
        case DVBPSI_MSG_ERROR: fprintf(stderr, "Error: "); break;
        case DVBPSI_MSG_WARN:  fprintf(stderr, "Warning: "); break;
        case DVBPSI_MSG_DEBUG: fprintf(stderr, "Debug: "); break;
        default: /* do nothing */
            return;
    }
    fprintf(stderr, "%s\n", msg);
}

static bool chain_add(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        dvbpsi_decoder_t *p_dec = calloc(1,sizeof(dvbpsi_decoder_t));
        if (p_dec == NULL) {
            fprintf(stderr, "out of memory\n");
            return false;
        }
        p_dec->i_table_id = i;
        p_dec->i_extension = i;

        if (!dvbpsi_decoder_chain_add(p_dvbpsi, p_dec)) {
            fprintf(stderr, "failed to add decoder to chain\n");
            return false;
        }

        assert(p_dvbpsi->p_decoder);
        /* verify adding to chain */
        p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i);
        if (p_dec == NULL) {
            fprintf(stderr, "could not find decoder (%d:%d)\n", i, i);
            return false;
        }
    }
    return true;
}

static bool chain_add_table_extension(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        dvbpsi_decoder_t *p_dec = calloc(1,sizeof(dvbpsi_decoder_t));
        if (p_dec == NULL) {
            fprintf(stderr, "out of memory\n");
            return false;
        }
        const int i_extension = i + 1;
        p_dec->i_table_id = i;
        p_dec->i_extension = i_extension;

        if (!dvbpsi_decoder_chain_add(p_dvbpsi, p_dec)) {
            fprintf(stderr, "failed to add decoder to chain\n");
            return false;
        }

        assert(p_dvbpsi->p_decoder);
        /* verify adding to chain */
        p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i_extension);
        if (p_dec == NULL) {
            fprintf(stderr, "could not find decoder (%d:%d)\n", i, i_extension);
            return false;
        }
    }
    return true;
}

static bool chain_find(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i);
        if (p_dec == NULL) {
            fprintf(stderr, "could not find decoder (%d:%d)\n", i, i);
            return false;
        }
    }
    return true;
}

static bool chain_release(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i);
        if (p_dec == NULL) {
            fprintf(stderr, "failed to find decoder (%d:%d) in chain\n", i, i);
            return false;
        }
        if (!dvbpsi_decoder_chain_del(p_dvbpsi, p_dec)) {
            fprintf(stderr, "failed to delete decoder (%d:%d) from chain\n", i, i);
            return false;
        }
        /* NOTE: normally we would call dvbpsi_decoder_delete(p_dec) for
         * a PSI table decoder allocated with dvbpsi_decoder_new(). However
         * for this test the contents of the decoder does not really matter
         * and therefor we allocate with calloc() and free with free(). */
        free(p_dec);
        p_dec = NULL;
    }
    return true;
}

static bool chain_release_with_extension(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        const int i_extension = i + 1;
        dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i_extension);
        if (p_dec == NULL) {
            fprintf(stderr, "failed to find decoder (%d:%d) in chain\n", i, i_extension);
            return false;
        }
        if (!dvbpsi_decoder_chain_del(p_dvbpsi, p_dec)) {
            fprintf(stderr, "failed to delete decoder (%d:%d) from chain\n", i, i_extension);
            return false;
        }
        /* NOTE: normally we would call dvbpsi_decoder_delete(p_dec) for
         * a PSI table decoder allocated with dvbpsi_decoder_new(). However
         * for this test the contents of the decoder does not really matter
         * and therefor we allocate with calloc() and free with free(). */
        free(p_dec);
        p_dec = NULL;
    }
    return true;
}

/*****************************************************************************
 * main
 *****************************************************************************/
#define CHAIN_DECODERS (20)
int main(int i_argc, char* pa_argv[])
{
  dvbpsi_t *p_dvbpsi = dvbpsi_new(&message, DVBPSI_MSG_DEBUG);
  if (p_dvbpsi == NULL)
      return 1;

  /* Test dvbpsi_decoder_chain_add() */
  if (!chain_add(p_dvbpsi, CHAIN_DECODERS)) {
      TEST_FAILED("dvbpsi_decoder_chain_add");
      goto error;
  }
  TEST_PASSED("dvbpsi_decoder_chain_add");

  /* Test dvbpsi_decoder_chain_get() */
  if (!chain_find(p_dvbpsi, CHAIN_DECODERS)) {
     TEST_FAILED("dvbpsi_decoder_chain_get");
     goto error;
  }
  TEST_PASSED("dvbpsi_decoder_chain_get");

  /* Test dvbpsi_decoder_chain_add() with table extensions */
  if (!chain_add_table_extension(p_dvbpsi, CHAIN_DECODERS)) {
      TEST_FAILED("dvbpsi_decoder_chain_add with extensions");
      goto error;
  }
  TEST_PASSED("dvbpsi_decoder_chain_add with extensions");

#ifdef _TEST_CHAIN_DEBUG
  dvbpsi_decoder_chain_dump(p_dvbpsi);
#endif

  /* Test dvbpsi_decoder_chain_del() */
  if (!chain_release(p_dvbpsi, CHAIN_DECODERS)) {
      TEST_FAILED("dvbpsi_decoder_chain_del");
      dvbpsi_delete(p_dvbpsi);
      return 1;
  }
  TEST_PASSED("dvbpsi_decoder_chain_del");

  /* Test dvbpsi_decoder_chain_del() */
  if (!chain_release_with_extension(p_dvbpsi, CHAIN_DECODERS)) {
      TEST_FAILED("dvbpsi_decoder_chain_del with extensions");
      dvbpsi_delete(p_dvbpsi);
      return 1;
  }
  TEST_PASSED("dvbpsi_decoder_chain_del with extensions");

  p_dvbpsi->p_decoder = NULL;
  dvbpsi_delete(p_dvbpsi);
  fprintf(stderr, "ALL CHAIN TESTS PASSED\n");
  return 0;

error:
  /* cleanup */
  if (!chain_release(p_dvbpsi, CHAIN_DECODERS))
      fprintf(stderr, "failed to cleanup after errors\n");
  p_dvbpsi->p_decoder = NULL;
  dvbpsi_delete(p_dvbpsi);
  return 1;
}
