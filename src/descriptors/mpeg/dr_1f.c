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

#include "config.h"

#include <stdlib.h>
#include <stdbool.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include "../../dvbpsi.h"
#include "../../dvbpsi_private.h"
#include "../../descriptor.h"

#include "dr_1f.h"

#define FMC_PACKED_SIZE 3

dvbpsi_mpeg_fmc_dr_t* dvbpsi_decode_mpeg_fmc_dr(
                                      dvbpsi_descriptor_t * p_descriptor)
{
    dvbpsi_mpeg_fmc_dr_t * p_decoded;

    /* check the tag. */
    if (!dvbpsi_CanDecodeAsDescriptor(p_descriptor, 0x1f))
        return NULL;

    /* don't decode twice. */
    if (dvbpsi_IsDescriptorDecoded(p_descriptor))
        return p_descriptor->p_decoded;

    /* all descriptors of this type must carry an integral number of packed FMC
     * structures inside. */
    const uint8_t num_fmc = p_descriptor->i_length / FMC_PACKED_SIZE;
    if (p_descriptor->i_length % FMC_PACKED_SIZE
        || num_fmc > ARRAY_SIZE(p_decoded->p_fmc))
        return NULL;

    p_decoded = malloc(sizeof(*p_decoded));
    if (!p_decoded)
        return NULL;

    p_decoded->i_num_fmc = num_fmc;
    for(uint8_t i = 0 ; i < num_fmc ; ++i)
    {
        dvbpsi_fmc_t * pair = p_decoded->p_fmc + i;
        pair->i_es_id = ((p_descriptor->p_data[i*FMC_PACKED_SIZE] & 0xff) << 8)
                        | p_descriptor->p_data[i*FMC_PACKED_SIZE + 1];
        pair->i_flex_mux_channel = p_descriptor->p_data[i*FMC_PACKED_SIZE + 2];
    }

    p_descriptor->p_decoded = p_decoded;

    return p_decoded;
}

dvbpsi_descriptor_t * dvbpsi_gen_mpeg_fmc_dr(
                                      dvbpsi_mpeg_fmc_dr_t * p_decoded)
{
    if (p_decoded->i_num_fmc > ARRAY_SIZE(p_decoded->p_fmc))
        return NULL;

    dvbpsi_descriptor_t * p_descriptor = dvbpsi_NewDescriptor(
        0x1f, p_decoded->i_num_fmc * FMC_PACKED_SIZE, NULL);
    if (!p_descriptor)
        return NULL;

    /* encode the data. */
    for (uint8_t i = 0 ; i < p_decoded->i_num_fmc ; ++i)
    {
        dvbpsi_fmc_t * pair = p_decoded->p_fmc + i;
        p_descriptor->p_data[i*FMC_PACKED_SIZE]     = pair->i_es_id >> 8;
        p_descriptor->p_data[i*FMC_PACKED_SIZE + 1] = pair->i_es_id;
        p_descriptor->p_data[i*FMC_PACKED_SIZE + 2] = pair->i_flex_mux_channel;
    }

    return p_descriptor;
}
