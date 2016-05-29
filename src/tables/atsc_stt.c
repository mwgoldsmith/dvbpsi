/*
Copyright (C) 2006-2012  Adam Charrett

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

stt.c

Decode PSIP System Time Table.

*/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include <assert.h>

#include "../dvbpsi.h"
#include "../dvbpsi_private.h"
#include "../psi.h"
#include "../descriptor.h"
#include "../chain.h"

#include "atsc_stt.h"

typedef struct dvbpsi_atsc_stt_decoder_s
{
     DVBPSI_DECODER_COMMON

     dvbpsi_atsc_stt_callback      pf_stt_callback;

     dvbpsi_atsc_stt_t             current_stt;
     dvbpsi_atsc_stt_t *           p_building_stt;

} dvbpsi_atsc_stt_decoder_t;

dvbpsi_descriptor_t *dvbpsi_atsc_STTAddDescriptor(dvbpsi_atsc_stt_t *p_stt,
                                               uint8_t i_tag, uint8_t i_length,
                                               uint8_t *p_data);

static void dvbpsi_atsc_GatherSTTSections(dvbpsi_t* p_dvbpsi,
                                          dvbpsi_psi_section_t* p_section);

static void dvbpsi_atsc_DecodeSTTSections(dvbpsi_atsc_stt_t* p_stt,
                                          dvbpsi_psi_section_t* p_section);

/*****************************************************************************
 * dvbpsi_atsc_stt_attach
 *****************************************************************************
 * Initialize a STT subtable decoder.
 *****************************************************************************/
bool dvbpsi_atsc_stt_attach(dvbpsi_t* p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,
                           dvbpsi_atsc_stt_callback pf_stt_callback, void* p_priv)
{
    assert(p_dvbpsi);

    dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i_table_id, i_extension);
    if (p_dec != NULL)
    {
        dvbpsi_error(p_dvbpsi, "ATSC STT decoder",
                               "Already a decoder for (table_id == 0x%02x)",
                                i_table_id);
        return false;
    }

    dvbpsi_atsc_stt_decoder_t*  p_stt_decoder;
    p_stt_decoder = (dvbpsi_atsc_stt_decoder_t*) dvbpsi_decoder_new(dvbpsi_atsc_GatherSTTSections,
                                                      4096, true, sizeof(dvbpsi_atsc_stt_decoder_t));
    if (p_stt_decoder == NULL)
        return false;

    /* STT decoder information */
    p_stt_decoder->pf_stt_callback = pf_stt_callback;
    p_stt_decoder->p_priv = p_priv;
    p_stt_decoder->p_building_stt = NULL;

    p_stt_decoder->i_table_id = i_table_id;
    p_stt_decoder->i_extension = i_extension;

    /* add sdt decoder to decoder chain */
    if (!dvbpsi_decoder_chain_add(p_dvbpsi, DVBPSI_DECODER(p_stt_decoder)))
    {
        dvbpsi_decoder_delete(DVBPSI_DECODER(p_stt_decoder));
        return false;
    }

    return true;
}

/*****************************************************************************
 * dvbpsi_atsc_stt_detach
 *****************************************************************************
 * Close a STT decoder.
 *****************************************************************************/
void dvbpsi_atsc_stt_detach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension)
{
    assert(p_dvbpsi);

    // NOTE: Seems wrong
    // i_extension = 0;
    dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i_table_id, i_extension);
    if (p_dec == NULL)
    {
        dvbpsi_error(p_dvbpsi, "ATSC STT Decoder",
                             "No such STT decoder (table_id == 0x%02x,"
                             "extension == 0x00)",
                            i_table_id);
        return;
    }

    /* Remove table decoder from decoder chain */
    if (!dvbpsi_decoder_chain_remove(p_dvbpsi, p_dec))
    {
        dvbpsi_error(p_dvbpsi, "ATSC STT Decoder",
                      "Failed to remove"
                      "extension == 0x%02x)",
                      i_table_id, i_extension);
        return;
    }

    dvbpsi_atsc_stt_decoder_t* p_stt_decoder = (dvbpsi_atsc_stt_decoder_t*)p_dec;
    if (p_stt_decoder->p_building_stt)
        dvbpsi_atsc_stt_delete(p_stt_decoder->p_building_stt);
    p_stt_decoder->p_building_stt = NULL;
    dvbpsi_decoder_delete(p_dec);
    p_dec = NULL;
}

/*****************************************************************************
 * dvbpsi_atsc_stt_init
 *****************************************************************************
 * Initialize a pre-allocated dvbpsi_atsc_stt_t structure.
 *****************************************************************************/
void dvbpsi_atsc_stt_init(dvbpsi_atsc_stt_t *p_stt, uint8_t i_table_id,
                         uint16_t i_extension, uint8_t i_version, bool b_current_next)
{
    assert(p_stt);

    p_stt->i_table_id = i_table_id;
    p_stt->i_extension = i_extension;

    p_stt->i_version = i_version;
    p_stt->b_current_next = b_current_next;

    p_stt->p_first_descriptor = NULL;
}

/*****************************************************************************
 * dvbpsi_atsc_stt_new
 *****************************************************************************
 * Allocate and initialize a dvbpsi_atsc_stt_t structure.
 *****************************************************************************/
dvbpsi_atsc_stt_t *dvbpsi_atsc_stt_new(uint8_t i_table_id, uint16_t i_extension,
                                      uint8_t i_version, bool b_current_next)
{
    dvbpsi_atsc_stt_t *p_stt;
    p_stt = (dvbpsi_atsc_stt_t*)malloc(sizeof(dvbpsi_atsc_stt_t));
    if (p_stt != NULL)
        dvbpsi_atsc_stt_init(p_stt, i_table_id, i_extension, i_version, b_current_next);
    return p_stt;
}

/*****************************************************************************
 * dvbpsi_atsc_stt_empty
 *****************************************************************************
 * Clean a dvbpsi_atsc_stt_t structure.
 *****************************************************************************/
void dvbpsi_atsc_stt_empty(dvbpsi_atsc_stt_t* p_stt)
{
  dvbpsi_DeleteDescriptors(p_stt->p_first_descriptor);
  p_stt->p_first_descriptor = NULL;
}

/*****************************************************************************
 * dvbpsi_atsc_stt_delete
 *****************************************************************************
 * Empty and Delere a dvbpsi_atsc_stt_t structure.
 *****************************************************************************/
void dvbpsi_atsc_stt_delete(dvbpsi_atsc_stt_t *p_stt)
{
    if (p_stt)
        dvbpsi_atsc_stt_empty(p_stt);
    free(p_stt);
    p_stt = NULL;
}

/*****************************************************************************
 * dvbpsi_atsc_STTAddDescriptor
 *****************************************************************************
 * Add a descriptor to the STT table.
 *****************************************************************************/
dvbpsi_descriptor_t *dvbpsi_atsc_STTAddDescriptor( dvbpsi_atsc_stt_t *p_stt,
                                               uint8_t i_tag, uint8_t i_length,
                                               uint8_t *p_data)
{
    dvbpsi_descriptor_t * p_descriptor = dvbpsi_NewDescriptor(i_tag, i_length, p_data);
    if (p_descriptor == NULL)
        return NULL;

    p_stt->p_first_descriptor = dvbpsi_AddDescriptor(p_stt->p_first_descriptor,
                                                     p_descriptor);
    assert(p_stt->p_first_descriptor);
    if (p_stt->p_first_descriptor == NULL)
        return NULL;

    return p_descriptor;
}

/*****************************************************************************
 * dvbpsi_ReInitSTT                                                          *
 *****************************************************************************/
static void dvbpsi_ReInitSTT(dvbpsi_atsc_stt_decoder_t *p_decoder, const bool b_force)
{
    assert(p_decoder);

    dvbpsi_decoder_reset(DVBPSI_DECODER(p_decoder), b_force);

    /* Force redecoding */
    if (b_force)
    {
        /* Free structures */
        if (p_decoder->p_building_stt)
            dvbpsi_atsc_stt_delete(p_decoder->p_building_stt);
    }
    p_decoder->p_building_stt = NULL;
}

static bool dvbpsi_CheckSTT(dvbpsi_t *p_dvbpsi, dvbpsi_atsc_stt_decoder_t *p_decoder,
                            dvbpsi_psi_section_t *p_section)
{
    bool b_reinit = false;

    assert(p_dvbpsi);
    assert(p_decoder);

    if (p_decoder->p_building_stt->i_version != p_section->i_version)
    {
        /* version_number */
        dvbpsi_error(p_dvbpsi, "ATSC STT decoder",
                     "'version_number' differs"
                     " whereas no discontinuity has occurred");
        b_reinit = true;
    }
    else if (p_decoder->i_last_section_number != p_section->i_last_number)
    {
        /* last_section_number */
        dvbpsi_error(p_dvbpsi, "ATSC STT decoder",
                     "'last_section_number' differs"
                     " whereas no discontinuity has occurred");
        b_reinit = true;
    }

    return b_reinit;
}

static bool dvbpsi_AddSectionSTT(dvbpsi_t *p_dvbpsi, dvbpsi_atsc_stt_decoder_t *p_decoder,
                                 dvbpsi_psi_section_t* p_section)
{
    assert(p_dvbpsi);
    assert(p_decoder);
    assert(p_section);

    /* Initialize the structures if it's the first section received */
    if (!p_decoder->p_building_stt)
    {
        p_decoder->p_building_stt = dvbpsi_atsc_stt_new(p_section->i_table_id, p_section->i_extension,
                                                       p_section->i_version, p_section->b_current_next);
        if (!p_decoder->p_building_stt)
            return false;

        p_decoder->i_last_section_number = p_section->i_last_number;
    }

    /* Add to linked list of sections */
    if (dvbpsi_decoder_psi_section_add(DVBPSI_DECODER(p_decoder), p_section))
        dvbpsi_debug(p_dvbpsi, "ATSC STT decoder", "overwrite section number %d",
                     p_section->i_number);

    return true;
}

/*****************************************************************************
 * dvbpsi_atsc_GatherSTTSections
 *****************************************************************************
 * Callback for the subtable demultiplexor.
 *****************************************************************************/
static void dvbpsi_atsc_GatherSTTSections(dvbpsi_t *p_dvbpsi, dvbpsi_psi_section_t *p_section)
{
    assert(p_dvbpsi);

    if (!dvbpsi_CheckPSISection(p_dvbpsi, p_section, 0xCD, "ATSC STT decoder"))
    {
        dvbpsi_DeletePSISections(p_section);
        return;
    }

    /* */
    dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, p_section->i_table_id, p_section->i_extension);
    if (!p_dec)
    {
        dvbpsi_DeletePSISections(p_section);
        return;
    }

    /* TS discontinuity check */
    dvbpsi_atsc_stt_decoder_t *p_stt_decoder = (dvbpsi_atsc_stt_decoder_t*)p_dec;
    if (p_stt_decoder->b_discontinuity)
    {
        dvbpsi_ReInitSTT(p_stt_decoder, true);
        p_stt_decoder->b_discontinuity = false;
    }
    else
    {
        /* Perform a few sanity checks */
        if (p_stt_decoder->p_building_stt)
        {
            if (dvbpsi_CheckSTT(p_dvbpsi, p_stt_decoder, p_section))
                dvbpsi_ReInitSTT(p_stt_decoder, true);
        }
        else
        {
            if (   (p_stt_decoder->b_current_valid)
                && (p_stt_decoder->current_stt.i_version == p_section->i_version)
                && (p_stt_decoder->current_stt.b_current_next ==
                                               p_section->b_current_next))
            {
                /* Don't decode since this version is already decoded */
                dvbpsi_debug(p_dvbpsi, "ATSC STT decoder",
                             "ignoring already decoded section %d",
                             p_section->i_number);
                dvbpsi_DeletePSISections(p_section);
                return;
            }
#if 0 /* FIXME: when to signal new table? */
            if ((p_stt_decoder->b_current_valid)
                && (p_stt_decoder->current_vct.i_version == p_section->i_version))
            {
                /* Signal a new VCT if the previous one wasn't active */
                if ((!p_stt_decoder->current_stt.b_current_next)
                    && (p_section->b_current_next))
                {
                    dvbpsi_atsc_stt_t * p_stt = (dvbpsi_atsc_stt_t*)malloc(sizeof(dvbpsi_atsc_stt_t));
                    if (p_stt)
                    {
                        p_stt_decoder->current_stt.b_current_next = 1;
                        *p_stt = p_stt_decoder->current_stt;
                        p_stt_decoder->pf_stt_callback(p_stt_decoder->p_priv, p_stt);
                    }
                }
                else
                    dvbpsi_error(p_dvbpsi, "ATSC STT decoder", "Could not signal new ATSC STT.");
            }
            dvbpsi_DeletePSISections(p_section);
            return;
#endif
        }
    }

    /* Add section to STT */
    if (!dvbpsi_AddSectionSTT(p_dvbpsi, p_stt_decoder, p_section))
    {
        dvbpsi_error(p_dvbpsi, "ATSC STT decoder", "failed decoding section %d",
                     p_section->i_number);
        dvbpsi_DeletePSISections(p_section);
        return;
    }

    /* Check if we have all the sections */
    if (dvbpsi_decoder_psi_sections_completed(DVBPSI_DECODER(p_stt_decoder)))
    {
        assert(p_stt_decoder->pf_stt_callback);

        /* Save the current information */
        p_stt_decoder->current_stt = *p_stt_decoder->p_building_stt;
        p_stt_decoder->b_current_valid = true;
        /* Decode the sections */
        dvbpsi_atsc_DecodeSTTSections(p_stt_decoder->p_building_stt,
                                      p_stt_decoder->p_sections);
        /* signal the new STT */
        p_stt_decoder->pf_stt_callback(p_stt_decoder->p_priv,
                                       p_stt_decoder->p_building_stt);
        /* Delete sections and Reinitialize the structures */
        dvbpsi_ReInitSTT(p_stt_decoder, false);
        assert(p_stt_decoder->p_sections == NULL);
    }
}

/*****************************************************************************
 * dvbpsi_DecodeSTTSection
 *****************************************************************************
 * STT decoder.
 *****************************************************************************/
static void dvbpsi_atsc_DecodeSTTSections(dvbpsi_atsc_stt_t* p_stt,
                              dvbpsi_psi_section_t* p_section)
{
    uint8_t *p_byte, *p_end;
    uint16_t i_length = 0;

    p_byte = p_section->p_payload_start + 1;
    p_stt->i_system_time = (((uint32_t)p_byte[0]) << 24) |
                           (((uint32_t)p_byte[1]) << 16) |
                           (((uint32_t)p_byte[2]) <<  8) |
                           ((uint32_t)p_byte[3]);
    p_stt->i_gps_utc_offset = p_byte[4];
    p_stt->i_daylight_savings = (((uint16_t)p_byte[5]) << 16) |
                                 ((uint16_t)p_byte[6]);
    p_byte += 7;
    /* Table descriptors */
    i_length = (p_section->i_length - 17);
    p_end = p_byte + i_length;

    while(p_byte + 2 <= p_end)
    {
        uint8_t i_tag = p_byte[0];
        uint8_t i_len = p_byte[1];
        if (i_len + 2 <= p_end - p_byte)
            dvbpsi_atsc_STTAddDescriptor(p_stt, i_tag, i_len, p_byte + 2);
        p_byte += 2 + i_len;
    }
}
