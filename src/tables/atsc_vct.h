/*
Copyright (C) 2006  Adam Charrett

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

vct.h

Decode PSIP Virtual Channel Table.

*/

/*!
 * \file atsc_vct.h
 * \author Adam Charrett
 * \brief Decode PSIP Virtual Channel Table. (ATSC VCT).
 */

#ifndef _ATSC_VCT_H
#define _ATSC_VCT_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * dvbpsi_atsc_vct_channel_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_atsc_vct_channel_s
 * \brief VCT channel structure.
 *
 * This structure is used to store a decoded VCT channel information.
 */
/*!
 * \typedef struct dvbpsi_atsc_vct_channel_s dvbpsi_atsc_vct_channel_t
 * \brief dvbpsi_atsc_vct_channel_t type definition.
 */
typedef struct dvbpsi_atsc_vct_channel_s
{
    uint8_t   i_short_name[14];/*!< Channel name (7*UTF16-BE)*/
    uint16_t  i_major_number;  /*!< Channel major number */
    uint16_t  i_minor_number;  /*!< Channel minor number */

    uint8_t   i_modulation;    /*!< Modulation mode. */
    uint32_t  i_carrier_freq;  /*!< Carrier center frequency. */
    uint16_t  i_channel_tsid;  /*!< Channel Transport stream id. */
    uint16_t  i_program_number;/*!< Channel MPEG program number. */
    uint8_t   i_etm_location;  /*!< Extended Text Message location. */
    bool      b_access_controlled; /*!< Whether the channel is scrambled. */
    bool      b_path_select;   /*!< Path selection, only used by CVCT. */
    bool      b_out_of_band;   /*!< Whether the channel is carried on the out-of-band
                                                      physical transmission channel, only used by CVCT. */
    bool      b_hidden;        /*!< Not accessible directly by the user. */
    bool      b_hide_guide;    /*!< Whether the channel should not be displayed in the guide. */

    uint8_t   i_service_type;  /*!< Channel type. */
    uint16_t  i_source_id;     /*!< Programming source associated with the channel.*/

    dvbpsi_descriptor_t *p_first_descriptor;  /*!< First descriptor. */

    struct dvbpsi_atsc_vct_channel_s *p_next; /*!< next element of the list */
} dvbpsi_atsc_vct_channel_t;

/*****************************************************************************
 * dvbpsi_atsc_vct_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_atsc_vct_s
 * \brief VCT structure.
 *
 * This structure is used to store a decoded VCT.
 */
/*!
 * \typedef struct dvbpsi_atsc_vct_s dvbpsi_atsc_vct_t
 * \brief dvbpsi_atsc_vct_t type definition.
 */
typedef struct dvbpsi_atsc_vct_s
{
    uint8_t  i_table_id;         /*!< table id */
    uint16_t i_extension;        /*!< subtable id */

    uint8_t  i_version;          /*!< version_number */
    bool     b_current_next;     /*!< current_next_indicator */
    uint8_t  i_protocol;         /*!< PSIP Protocol version */
    bool     b_cable_vct;        /*!< 1 if this is a cable VCT, 0 if it is a Terrestrial VCT. */

    dvbpsi_descriptor_t         *p_first_descriptor; /*!< First descriptor. */
    dvbpsi_atsc_vct_channel_t   *p_first_channel;    /*!< First channel information structure. */

} dvbpsi_atsc_vct_t;

/*****************************************************************************
 * dvbpsi_vct_callback
 *****************************************************************************/
/*!
 * \typedef void (* dvbpsi_atsc_vct_callback)(void* p_priv,
 *                                       dvbpsi_atsc_vct_t* p_new_vct)
 * \brief Callback type definition.
 */
typedef void (* dvbpsi_atsc_vct_callback)(void* p_priv, dvbpsi_atsc_vct_t* p_new_vct);

/*****************************************************************************
 * dvbpsi_atsc_vct_attach
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_atsc_vct_attach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id,
          uint16_t i_extension, dvbpsi_atsc_vct_callback pf_vct_callback,
                           void* p_priv)
 * \brief Creation and initialization of a VCT decoder.
 * \param p_dvbpsi dvbpsi handle to Subtable demultiplexor to which the decoder is attached.
 * \param i_table_id Table ID, 0xC8 or 0xC9.
 * \param i_extension Table ID extension, here TS ID.
 * \param pf_vct_callback function to call back on new VCT.
 * \param p_priv private data given in argument to the callback.
 * \return true if everything went ok, else false.
 */
bool dvbpsi_atsc_vct_attach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id,
          uint16_t i_extension, dvbpsi_atsc_vct_callback pf_vct_callback,
                           void* p_priv);

/*!
 * \brief dvbpsi_atsc_AttachVCT is deprecated use @see dvbpsi_atsc_vct_attach() instead.
 * \param p_dvbpsi dvbpsi handle to Subtable demultiplexor to which the decoder is attached.
 * \param i_table_id Table ID, 0xC8 or 0xC9.
 * \param i_extension Table ID extension, here TS ID.
 * \param pf_vct_callback function to call back on new VCT.
 * \param p_priv private data given in argument to the callback.
 * \return true if everything went ok, else false.
 */
__attribute__((deprecated,unused))
inline bool dvbpsi_atsc_AttachVCT(dvbpsi_t *p_dvbpsi, uint8_t i_table_id,
            uint16_t i_extension, dvbpsi_atsc_vct_callback pf_vct_callback,
            void* p_priv)
{
    return dvbpsi_atsc_vct_attach(p_dvbpsi, i_table_id, i_extension,
                                 pf_vct_callback, p_priv);
}

/*****************************************************************************
 * dvbpsi_atsc_vct_detach
 *****************************************************************************/
/*!
 * \fn void dvbpsi_atsc_vct_detach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id,
 *                                uint16_t i_extension)
 *
 * \brief Destroy a VCT decoder.
 * \param p_dvbpsi dvbpsi handle to Subtable demultiplexor to which the decoder is attached.
 * \param i_table_id Table ID, 0xC8 or 0xC9.
 * \param i_extension Table ID extension, here TS ID.
 * \return nothing.
 */
void dvbpsi_atsc_vct_detach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension);

/*!
 * \brief dvbpsi_atsc_DetachVCT is deprecated use @see dvbpsi_atsc_vct_detach() instead.
 * \param p_dvbpsi dvbpsi handle to Subtable demultiplexor to which the decoder is attached.
 * \param i_table_id Table ID, 0xC8 or 0xC9.
 * \param i_extension Table ID extension, here TS ID.
 * \return nothing.
 */
__attribute__((deprecated,unused))
inline void dvbpsi_atsc_DetachVCT(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension)
{
    dvbpsi_atsc_vct_detach(p_dvbpsi, i_table_id, i_extension);
}

/*****************************************************************************
 * dvbpsi_atsc_vct_init
 *****************************************************************************/
/*!
 * \fn void dvbpsi_atsc_vct_init(dvbpsi_atsc_vct_t* p_vct, uint8_t i_table_id,
                         uint16_t i_extension, uint8_t i_protocol, bool b_cable_vct,
                         uint8_t i_version, bool b_current_next)
 * \brief Initialize a user-allocated dvbpsi_atsc_vct_t structure.
 * \param p_vct pointer to the VCT structure
 * \param i_table_id Table ID, 0xC8 or 0xC9.
 * \param i_extension Table ID extension, here TS ID.
 * \param i_protocol PSIP Protocol version.
 * \param b_cable_vct Whether this is CVCT or a TVCT.
 * \param i_version VCT version
 * \param b_current_next current next indicator
 * \return nothing.
 */
void dvbpsi_atsc_vct_init(dvbpsi_atsc_vct_t* p_vct, uint8_t i_table_id,
                         uint16_t i_extension, uint8_t i_protocol, bool b_cable_vct,
                         uint8_t i_version, bool b_current_next);

/*!
 * \brief dvbpsi_atsc_InitVCT is deprecated use @see dvbpsi_atsc_vct_init() instead.
 * \param i_table_id Table ID, 0xC8 or 0xC9.
 * \param i_extension Table ID extension, here TS ID.
 * \param i_protocol PSIP Protocol version.
 * \param b_cable_vct Whether this is CVCT or a TVCT.
 * \param i_version VCT version
 * \param b_current_next current next indicator
 * \return nothing.
 */
__attribute__((deprecated,unused))
inline void dvbpsi_atsc_InitVCT(dvbpsi_atsc_vct_t* p_vct, uint8_t i_table_id,
                                uint16_t i_extension, uint8_t i_protocol, bool b_cable_vct,
                                uint8_t i_version, bool b_current_next)
{
    dvbpsi_atsc_vct_init(p_vct, i_table_id, i_extension, i_protocol, b_cable_vct,
                        i_version, b_current_next);
}

/*****************************************************************************
 * dvbpsi_atsc_vct_new
 *****************************************************************************/
/*!
 * \fn dvbpsi_atsc_vct_t *dvbpsi_atsc_vct_new(uint8_t i_table_id, uint16_t i_extension,
 *                   uint8_t i_protocol, bool b_cable_vct, uint8_t i_version, bool b_current_next);
 *
 * \brief Allocate and initialize a new dvbpsi_vct_t structure.
 * \param i_table_id Table ID, 0xC8 or 0xC9.
 * \param i_extension Table ID extension, here TS ID.
 * \param i_protocol PSIP Protocol version.
 * \param b_cable_vct Whether this is CVCT or a TVCT.
 * \param i_version VCT version
 * \param b_current_next current next indicator
 * \return p_vct pointer to the VCT structure
 */
dvbpsi_atsc_vct_t *dvbpsi_atsc_vct_new(uint8_t i_table_id, uint16_t i_extension,
                    uint8_t i_protocol, bool b_cable_vct, uint8_t i_version,
                    bool b_current_next);

/*!
 * \brief dvbpsi_atsc_NewVCT is deprecated use @see dvbpsi_atsc_vct_new() instead.
 * \param i_table_id Table ID, 0xC8 or 0xC9.
 * \param i_extension Table ID extension, here TS ID.
 * \param i_protocol PSIP Protocol version.
 * \param b_cable_vct Whether this is CVCT or a TVCT.
 * \param i_version VCT version
 * \param b_current_next current next indicator
 * \return p_vct pointer to the VCT structure
 */
__attribute__((deprecated,unused))
inline dvbpsi_atsc_vct_t *dvbpsi_atsc_NewVCT(uint8_t i_table_id, uint16_t i_extension,
        uint8_t i_protocol, bool b_cable_vct, uint8_t i_version, bool b_current_next)
{
    return dvbpsi_atsc_vct_new(i_table_id, i_extension, i_protocol,
                               b_cable_vct, i_version, b_current_next);
}

/*****************************************************************************
 * dvbpsi_atsc_vct_empty
 *****************************************************************************/
/*!
 * \fn void dvbpsi_atsc_vct_empty(dvbpsi_atsc_vct_t* p_vct)
 * \brief Clean a dvbpsi_vct_t structure.
 * \param p_vct pointer to the VCT structure
 * \return nothing.
 */
void dvbpsi_atsc_vct_empty(dvbpsi_atsc_vct_t *p_vct);

/*!
 * \brief dvbpsi_atsc_EmptyVCT is deprecated use @see dvbpsi_atsc_vct_empty() instead.
 * \brief Clean a dvbpsi_vct_t structure.
 * \param p_vct pointer to the VCT structure
 * \return nothing.
 */
__attribute__((deprecated,unused))
inline void dvbpsi_atsc_EmptyVCT(dvbpsi_atsc_vct_t *p_vct)
{
    dvbpsi_atsc_vct_empty(p_vct);
}

/*****************************************************************************
 * dvbpsi_atsc_vct_delete
 *****************************************************************************/
/*!
 * \fn void dvbpsi_atsc_vct_delete(dvbpsi_atsc_vct_t *p_vct)
 * \brief Clean and free a dvbpsi_vct_t structure.
 * \param p_vct pointer to the VCT structure
 * \return nothing.
 */
void dvbpsi_atsc_vct_delete(dvbpsi_atsc_vct_t *p_vct);

/*!
 * \brief dvbpsi_atsc_DeleteVCT is deprecated use @see dvbpsi_atsc_vct_delete() instead.
 * \brief Clean a dvbpsi_vct_t structure.
 * \param p_vct pointer to the VCT structure
 * \return nothing.
 */
__attribute__((deprecated,unused))
inline void dvbpsi_atsc_DeleteVCT(dvbpsi_atsc_vct_t *p_vct)
{
    dvbpsi_atsc_vct_delete(p_vct);
}
#ifdef __cplusplus
};
#endif

#endif
