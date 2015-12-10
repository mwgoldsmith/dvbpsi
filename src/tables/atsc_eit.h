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

eit.h

*/

/*!
 * \file atsc_eit.h
 * \author Adam Charrett
 * \brief Decode PSIP Event Information Table (ATSC EIT).
 */

#ifndef _ATSC_EIT_H
#define _ATSC_EIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * dvbpsi_atsc_eit_event_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_atsc_eit_event_s
 * \brief ATSC EIT Event structure.
 *
 * This structure is used to store decoded event information.
 */
/*!
 * \typedef struct dvbpsi_atsc_eit_event_s dvbpsi_atsc_eit_event_t
 * \brief dvbpsi_atsc_eit_event_t type definition.
 */
typedef struct dvbpsi_atsc_eit_event_s
{
    uint16_t   i_event_id;      /*!< Event ID */
    uint32_t   i_start_time;    /*!< Start time in GPS seconds */
    uint8_t    i_etm_location;  /*!< Extended Text Message location. */
    uint32_t   i_length_seconds;/*!< Length of program in seconds. */
    uint8_t    i_title_length;  /*!< Length of the title in bytes */
    uint8_t    i_title[256];    /*!< Title in multiple string structure format. */

    dvbpsi_descriptor_t *p_first_descriptor; /*!< First descriptor structure. */

    struct dvbpsi_atsc_eit_event_s   *p_next;/*!< Next event information structure. */

} dvbpsi_atsc_eit_event_t;

/*****************************************************************************
 * dvbpsi_atsc_eit_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_atsc_eit_s
 * \brief ATSC EIT structure.
 *
 * This structure is used to store a decoded EIT.
 */
/*!
 * \typedef struct dvbpsi_atsc_eit_s dvbpsi_atsc_eit_t
 * \brief dvbpsi_atsc_eit_t type definition.
 */
typedef struct dvbpsi_atsc_eit_s
{
    uint8_t                 i_table_id;         /*!< table id */
    uint16_t                i_extension;        /*!< subtable id */

    uint8_t                 i_version;          /*!< version_number */
    bool                    b_current_next;     /*!< current_next_indicator */
    uint16_t                i_source_id;        /*!< Source id used to match against channels */
    uint8_t                 i_protocol;         /*!< PSIP Protocol version */

    dvbpsi_atsc_eit_event_t *p_first_event;     /*!< First event information structure. */

    dvbpsi_descriptor_t     *p_first_descriptor;/*!< First descriptor structure. */
} dvbpsi_atsc_eit_t;

/*****************************************************************************
 * dvbpsi_eit_callback
 *****************************************************************************/
/*!
 * \typedef void (* dvbpsi_atsc_eit_callback)(void* p_cb_data,
                                         dvbpsi_atsc_eit_t* p_new_eit)
 * \brief Callback type definition.
 */
typedef void (* dvbpsi_atsc_eit_callback)(void* p_cb_data, dvbpsi_atsc_eit_t* p_new_eit);

/*****************************************************************************
 * dvbpsi_atsc_eit_attach
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_atsc_eit_attach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id,
          uint16_t i_extension, dvbpsi_atsc_eit_callback pf_callback, void* p_cb_data)
 *
 * \brief Creation and initialization of a EIT decoder.
 * \param p_dvbpsi dvbpsi handle to Subtable demultiplexor to which the decoder is attached
 * \param i_table_id Table ID, 0xCB.
 * \param i_extension Table ID extension, here TS ID.
 * \param pf_callback function to call back on new EIT.
 * \param p_cb_data private data given in argument to the callback.
 * \return true if everything went ok, false otherwise
 */
bool dvbpsi_atsc_eit_attach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,
                           dvbpsi_atsc_eit_callback pf_callback, void* p_cb_data);

/*!
 * \brief dvbpsi_atsc_AttachEIT is deprecated use @see dvbpsi_atsc_eit_attach() instead.
 * \param p_dvbpsi dvbpsi handle to Subtable demultiplexor to which the decoder is attached
 * \param i_table_id Table ID, 0xCB.
 * \param i_extension Table ID extension, here TS ID.
 * \param pf_callback function to call back on new EIT.
 * \param p_cb_data private data given in argument to the callback.
 * \return true if everything went ok, false otherwise
 */
__attribute__((deprecated,unused))
inline bool dvbpsi_atsc_AttachEIT(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,
                                  dvbpsi_atsc_eit_callback pf_callback, void* p_cb_data)
{
    return dvbpsi_atsc_eit_attach(p_dvbpsi, i_table_id, i_extension,
                                  pf_callback, p_cb_data);
}

/*****************************************************************************
 * dvbpsi_atsc_eit_detach
 *****************************************************************************/
/*!
 * \fn void dvbpsi_atsc_eit_detach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id,
          uint16_t i_extension)
 * \brief Destroy a EIT decoder.
 * \param p_dvbpsi dvbpsi handle to Subtable demultiplexor to which the decoder is attached.
 * \param i_table_id Table ID, 0xCB.
 * \param i_extension Table ID extension, here TS ID.
 * \return nothing.
 */
void dvbpsi_atsc_eit_detach(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension);

/*!
 * \brief dvbpsi_atsc_DetachEIT is deprecated use @see dvbpsi_atsc_eit_detach() instead.
 * \param p_dvbpsi dvbpsi handle to Subtable demultiplexor to which the decoder is attached.
 * \param i_table_id Table ID, 0xCB.
 * \param i_extension Table ID extension, here TS ID.
 * \return nothing.
 */
__attribute__((deprecated,unused))
inline void dvbpsi_atsc_DetachEIT(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension)
{
    dvbpsi_atsc_eit_detach(p_dvbpsi, i_table_id, i_extension);
}

/*****************************************************************************
 * dvbpsi_atsc_eit_init
 *****************************************************************************/
/*!
 * \fn void dvbpsi_atsc_eit_init(dvbpsi_atsc_eit_t* p_eit, uint8_t i_table_id, uint16_t i_extension,
                                uint8_t i_version, uint8_t i_protocol,
                                uint16_t i_source_id, bool b_current_next);
 * \brief Initialize a user-allocated dvbpsi_atsc_eit_t structure.
 * \param p_eit pointer to the EIT structure
 * \param i_table_id Table ID, 0xCB.
 * \param i_extension Table ID extension, here TS ID.
 * \param i_version EIT version
 * \param i_protocol PSIP Protocol version.
 * \param i_source_id Source id.
 * \param b_current_next current next indicator
 * \return nothing.
 */
void dvbpsi_atsc_eit_init(dvbpsi_atsc_eit_t* p_eit, uint8_t i_table_id, uint16_t i_extension,
                         uint8_t i_version, uint8_t i_protocol, uint16_t i_source_id, bool b_current_next);

/*!
 * \brief dvbpsi_atsc_InitEIT is deprecated use @see dvbpsi_atsc_eit_init() instead.
 * \param p_eit pointer to the EIT structure
 * \param i_table_id Table ID, 0xCB.
 * \param i_extension Table ID extension, here TS ID.
 * \param i_version EIT version
 * \param i_protocol PSIP Protocol version.
 * \param i_source_id Source id.
 * \param b_current_next current next indicator
 * \return nothing.
 */
__attribute__((deprecated,unused))
inline void dvbpsi_atsc_InitEIT(dvbpsi_atsc_eit_t* p_eit, uint8_t i_table_id, uint16_t i_extension,
                                uint8_t i_version, uint8_t i_protocol, uint16_t i_source_id, bool b_current_next)
{
    dvbpsi_atsc_eit_init(p_eit, i_table_id, i_extension, i_version,
                         i_protocol, i_source_id, b_current_next);
}

/*****************************************************************************
 * dvbpsi_atsc_eit_new
 *****************************************************************************/
/*!
 * \fn dvbpsi_atsc_eit_t *dvbpsi_atsc_eit_new(uint8_t i_table_id, uint16_t i_extension,
                                             uint8_t i_version, uint8_t i_protocol,
                                             uint16_t i_source_id, bool b_current_next)
 * \brief Allocate and initialize a new dvbpsi_eit_t structure. Use ObjectRefDec to delete it.
 * \param i_table_id Table ID, 0xCB.
 * \param i_extension Table ID extension, here TS ID.
 * \param i_version EIT version
 * \param i_protocol PSIP Protocol version.
 * \param i_source_id Source id.
 * \param b_current_next current next indicator
 * \return p_eit pointer to the EIT structure or NULL on error
 */
dvbpsi_atsc_eit_t *dvbpsi_atsc_eit_new(uint8_t i_table_id, uint16_t i_extension,
                                      uint8_t i_version, uint8_t i_protocol,
                                      uint16_t i_source_id, bool b_current_next);

/*!
 * \brief dvbpsi_atsc_NewEIT is deprecated use @see dvbpsi_atsc_eit_new() instead.
 * \param i_table_id Table ID, 0xCB.
 * \param i_extension Table ID extension, here TS ID.
 * \param i_version EIT version
 * \param i_protocol PSIP Protocol version.
 * \param i_source_id Source id.
 * \param b_current_next current next indicator
 * \return p_eit pointer to the EIT structure or NULL on error
 */
__attribute__((deprecated,unused))
inline dvbpsi_atsc_eit_t *dvbpsi_atsc_NewEIT(uint8_t i_table_id, uint16_t i_extension,
                                             uint8_t i_version, uint8_t i_protocol,
                                             uint16_t i_source_id, bool b_current_next)
{
    return dvbpsi_atsc_eit_new(i_table_id, i_extension,
                               i_version, i_protocol,
                               i_source_id, b_current_next);
}

/*****************************************************************************
 * dvbpsi_atsc_eit_empty
 *****************************************************************************/
/*!
 * \fn void dvbpsi_atsc_eit_empty(dvbpsi_atsc_eit_t* p_eit)
 * \brief Clean a dvbpsi_eit_t structure.
 * \param p_eit pointer to the EIT structure
 * \return nothing.
 */
void dvbpsi_atsc_eit_empty(dvbpsi_atsc_eit_t *p_eit);

/*!
 * \brief dvbpsi_atsc_EmptyEIT is deprecated use @see dvbpsi_atsc_eit_empty() instead.
 * \param p_eit pointer to the EIT structure
 */
__attribute__((deprecated,unused))
inline void dvbpsi_atsc_EmptyEIT(dvbpsi_atsc_eit_t *p_eit)
{
    dvbpsi_atsc_eit_empty(p_eit);
}

/*****************************************************************************
 * dvbpsi_atsc_eit_delete
 *****************************************************************************/
/*!
 * \fn void dvbpsi_atsc_eit_delete(dvbpsi_atsc_eit_t *p_eit)
 * \brief Clean and free a dvbpsi_eit_t structure.
 * \param p_eit pointer to the EIT structure
 * \return nothing.
 */
void dvbpsi_atsc_eit_delete(dvbpsi_atsc_eit_t *p_eit);

/*!
 * \brief dvbpsi_atsc_DeleteEIT is deprecated use @see dvbpsi_atsc_eit_delete() instead.
 * \param p_eit pointer to the EIT structure
 */
__attribute__((deprecated,unused))
inline void dvbpsi_atsc_DeleteEIT(dvbpsi_atsc_eit_t *p_eit)
{
    dvbpsi_atsc_eit_delete(p_eit);
}

#ifdef __cplusplus
};
#endif

#endif
