/*
Copyright (C) 2016 Daniel Kamil Kozar

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*!
 * \file <dr_1f.h>
 * \author Daniel Kamil Kozar <dkk089@gmail.com>
 * \brief Application interface for the FMC descriptor decoder and generator.
 *
 * Application interface for the FMC descriptor decoder and generator. This
 * descriptor's definition can be found in ISO/IEC 13818-1 revision 2014/10
 * section 2.6.44.
 */

#ifndef _DVBPSI_DR_1F_H_
#define _DVBPSI_DR_1F_H_

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \struct dvbpsi_mpeg_fmc_dr_s
 * \brief FMC descriptor structure.
 *
 * This structure is used to store a decoded FMC descriptor. (ISO/IEC 13818-1
 * section 2.6.44).
 */

/*!
 * \typedef struct dvbpsi_fmc_s dvbpsi_fmc_t
 * \brief dvbpsi_fmc_t type definition.
 */
typedef struct dvbpsi_fmc_s {
    uint16_t    i_es_id; /*< ES_ID */
    uint8_t     i_flex_mux_channel; /*< FlexMuxChannel */
} dvbpsi_fmc_t;

/*!
 * \typedef struct dvbpsi_mpeg_fmc_dr_s dvbpsi_mpeg_fmc_dr_t
 * \brief dvbpsi_mpeg_fmc_dr_t type definition.
 */
typedef struct dvbpsi_mpeg_fmc_dr_s
{
    uint8_t i_num_fmc;
    dvbpsi_fmc_t p_fmc[84]; /* 253 (max payload) / 3 (sizeof(fmc)) = 84 */
} dvbpsi_mpeg_fmc_dr_t;

/*!
 * \brief FMC descriptor decoder.
 * \param p_descriptor pointer to the descriptor structure
 * \return A pointer to a new FMC descriptor structure which contains the
 * decoded data.
 */
dvbpsi_mpeg_fmc_dr_t* dvbpsi_decode_mpeg_fmc_dr(
                                      dvbpsi_descriptor_t * p_descriptor);

/*!
 * \brief FMC descriptor generator.
 * \param p_decoded pointer to a decoded FMC descriptor structure.
 * \return a pointer to a new descriptor structure which contains encoded data.
 */
dvbpsi_descriptor_t * dvbpsi_gen_mpeg_fmc_dr(
                                      dvbpsi_mpeg_fmc_dr_t * p_decoded);

#ifdef __cplusplus
}
#endif

#else
#error "Multiple inclusions of dr_1f.h"
#endif
