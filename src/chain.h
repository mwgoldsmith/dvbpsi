/*****************************************************************************
 * chain.h
 *
 * Copyright (C) 2015 VideoLAN
 * $Id$
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
 *****************************************************************************/

/*!
 * \file <chain.h>
 * \author Jean-Paul Saman <jpsaman@videolan.org>
 * \brief Chain PSI table decoders based on table_id and extension.
 *
 * Chain PSI table decoders based on table_id and extension. The
 * decoder chain is kept inside the dvbpsi_t handle at the
 * dvbpsi_t::dvbpsi_decoder_t member variable.
 */

#ifndef _DVBPSI_CHAIN_H_
#define _DVBPSI_CHAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * dvbpsi_decoder_chain_add
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_decoder_chain_add(dvbpsi_t *p_dvbpsi, dvbpsi_decoder_t *p_decoder)
 * \brief Adds decoder to the chain in handle 'p_dvbpsi' at dvbpsi_t::dvbpsi_decoder_t
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \param p_decoder pointer to dvbpsi_decoder_t for adding to chain
 * \return true on success, false on failure
 */
bool dvbpsi_decoder_chain_add(dvbpsi_t *p_dvbpsi, dvbpsi_decoder_t *p_decoder);

/*****************************************************************************
 * dvbpsi_decoder_chain_del
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_decoder_chain_del(dvbpsi_t *p_dvbpsi, const dvbpsi_decoder_t *p_decoder)
 * \brief Deletes decoder from the chain in handle 'p_dvbpsi' at dvbpsi_t::dvbpsi_decoder_t
 * \note Use @see dvbpsi_decoder_chain_get() to find the decoder pointer, then call
 * dvbpsi_decoder_chain_del() to remove the pointer from the chain. The caller is
 * responsible for freeing the associated memory of the just removed decoder pointer
 * and needs to call decoder_delete(p_decoder).
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \param p_decoder pointer to dvbpsi_decoder_t for deletion from chain
 * \return true on success, false on failure
 */
bool dvbpsi_decoder_chain_del(dvbpsi_t *p_dvbpsi, const dvbpsi_decoder_t *p_decoder);

/*****************************************************************************
 * dvbpsi_decoder_chain_get
 *****************************************************************************/
/*!
 * \fn dvbpsi_decoder_t *dvbpsi_decoder_chain_get(dvbpsi_t *p_dvbpsi, const uint16_t table_id, const uint16_t extension)
 * \brief Gets decoder from the chain based on given table_id and extension.
 * If extension is 0, then the search is performed on table_id only and the first match is returned.
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \param table_id PSI table id to get
 * \param extension PSI subtable id to get
 * \return true on success, false on failure
 */
dvbpsi_decoder_t *dvbpsi_decoder_chain_get(dvbpsi_t *p_dvbpsi, const uint16_t table_id, const uint16_t extension);

#if 0 /* debug code */
void dvbpsi_decoder_chain_dump(dvbpsi_t *p_dvbpsi);
#endif

#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of chain.h"
#endif
