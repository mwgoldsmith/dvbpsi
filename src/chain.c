/*****************************************************************************
 * chain.c: DVB subtables decoder chain functions
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
#include <stdlib.h>
#include <stdbool.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include <assert.h>

#include "dvbpsi.h"
#include "dvbpsi_private.h"
#include "psi.h"
#include "chain.h"

/*****************************************************************************
 * dvbpsi_decoder_chain_add
 *****************************************************************************
 * Add decoder to chain
 *****************************************************************************/
bool dvbpsi_decoder_chain_add(dvbpsi_t *p_dvbpsi, dvbpsi_decoder_t *p_decoder)
{
    assert(p_decoder);
    assert(!p_decoder->p_next);

    dvbpsi_decoder_t *p_list = (dvbpsi_decoder_t *) p_dvbpsi->p_decoder;
    while (p_list) {
        if (p_decoder->i_table_id == p_list->i_table_id) {
            if (p_decoder->i_extension == p_list->i_extension) {
                dvbpsi_error(p_dvbpsi, "chain", "decoder (%d:%d) already present",
                             p_decoder->i_table_id, p_decoder->i_extension);
                return false;
            }
            /* insert in reverse sequential order based on i_extension */
            if (p_decoder->i_extension > p_list->i_extension) {
                p_decoder->p_next = p_list->p_next;
                p_list->p_next = p_decoder;
                return true;
            }
        }
        /* append to end of list */
        if (p_list->p_next == NULL) {
            p_list->p_next = p_decoder;
            return true;
        }
        /* insert in sequential order based on i_table_id */
        if (p_decoder->i_table_id < p_list->p_next->i_table_id) {
            p_decoder->p_next = p_list->p_next;
            p_list->p_next = p_decoder;
            return true;
        }
        p_list = p_list->p_next;
    }
    /* empty list */
    p_dvbpsi->p_decoder = p_decoder;
    return true;
}

/*****************************************************************************
 * dvbpsi_decoder_chain_del
 *****************************************************************************
 * Remove decoder from chain
*****************************************************************************/
bool dvbpsi_decoder_chain_del(dvbpsi_t *p_dvbpsi, const dvbpsi_decoder_t *p_decoder)
{
    if (!p_decoder) return false;

    dvbpsi_decoder_t **pp_prev = &p_dvbpsi->p_decoder;
    while (pp_prev) {
        if ((p_decoder->i_table_id == (*pp_prev)->i_table_id) &&
            (p_decoder->i_extension == (*pp_prev)->i_extension)) {
            *pp_prev = p_decoder->p_next;
            /* NOTE: caller must call dvbpsi_decoder_delete(p_decoder) */
            return true;
        }
        pp_prev = &(*pp_prev)->p_next;
    }

    dvbpsi_warning(p_dvbpsi, "chain", "decoder (table id: %u, extension: %u) not found",
                   p_decoder->i_table_id, p_decoder->i_extension);
    return false;
}

/*****************************************************************************
 * dvbpsi_decoder_chain_get
 *****************************************************************************
 * Return pointer to decoder in chain
 *****************************************************************************/
dvbpsi_decoder_t *dvbpsi_decoder_chain_get(dvbpsi_t *p_dvbpsi, const uint16_t table_id, const uint16_t extension)
{
    dvbpsi_decoder_t *p = (dvbpsi_decoder_t *)p_dvbpsi->p_decoder;
    while (p) {
        if ((table_id == p->i_table_id) &&
            (extension == p->i_extension))
            return p;
        p = p->p_next;
    }

    dvbpsi_error(p_dvbpsi, "chain", "decoder not found");
    return NULL;
}

