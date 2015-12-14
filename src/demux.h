/*****************************************************************************
 * demux.h
 *
 * Copyright (C) 2001-2011 VideoLAN
 * $Id$
 *
 * Authors: Johan Bilien <jobi@via.ecp.fr>
 *          Jean-Paul Saman <jpsaman@videolan.org>
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
 * \file <demux.h>
 * \author Johan Bilien <jobi@via.ecp.fr>
 * \brief Subtable demutiplexor use @see chain.h instead
 *
 * Subtable demultiplexor structure
 * @note deprecated
 */

#ifndef _DVBPSI_DEMUX_H_
#define _DVBPSI_DEMUX_H_

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * dvbpsi_demux_new_cb_t
 *****************************************************************************/
/*!
 * \typedef void(* dvbpsi_demux_new_cb_t) (dvbpsi_t *p_dvbpsi,
                                           uint8_t  i_table_id,
                                           uint16_t i_extension,
                                           void *   p_cb_data);
 * \brief Callback used in case of a new subtable detected.
 */
typedef void (*dvbpsi_demux_new_cb_t) (dvbpsi_t *p_dvbpsi,  /*!< pointer to dvbpsi handle */
                                       uint8_t  i_table_id, /*!< table id to attach */
                                       uint16_t i_extension,/*!< table extention to attach */
                                       void *   p_cb_data); /*!< pointer to callback data */

/*!
 * \typedef void(*dvbpsi_demux_gather_cb_t)(dvbpsi_t *p_dvbpsi,
                                            void *p_cb_data,
                                            dvbpsi_psi_section_t *p_section);
 * \brief Callback used for gathering psi sections on behalf of subtable decoders.
 */
typedef void (*dvbpsi_demux_gather_cb_t) (dvbpsi_t *p_dvbpsi, /*!< pointer to dvbpsi handle */
                                          dvbpsi_decoder_t *p_decoder, /*!< pointer to decoder */
                                          dvbpsi_psi_section_t *p_section); /*!< pointer to psi section */

/*!
 * \typedef void (*dvbpsi_demux_detach_cb_t) (dvbpsi_t *p_dvbpsi,
                                              uint8_t i_table_id,
                                              uint16_t i_extension);
 * \brief Callback used for detaching subtable decoder from demuxer
 */
typedef void (*dvbpsi_demux_detach_cb_t) (dvbpsi_t *p_dvbpsi,    /*!< pointer to dvbpsi handle */
                                          uint8_t i_table_id,    /*!< table id to detach */
                                          uint16_t i_extension); /*!< table extention to detach */

/*****************************************************************************
 * dvbpsi_demux_subdec_t
 *****************************************************************************/
/*!
 * \typedef struct dvbpsi_demux_subdec_s dvbpsi_demux_subdec_t
 * \brief dvbpsi_demux_subdec_t is deprecated @see dvbpsi_decoder_t instead.
 */
typedef dvbpsi_decoder_t dvbpsi_demux_subdec_t;

/*****************************************************************************
 * dvbpsi_demux_s
 *****************************************************************************/
/*!
 * \typedef struct dvbpsi_demux_s dvbpsi_demux_t
 * \brief dvbpsi_demux_t type definition is deprecated @see dvbpsi_t instead.
 */
typedef dvbpsi_t dvbpsi_demux_t;


/*****************************************************************************
 * dvbpsi_AttachDemux
 *****************************************************************************/
/*!
 * \fn __attribute__((deprecated,unused)) bool dvbpsi_AttachDemux(dvbpsi_t *p_dvbpsi,
 *                               dvbpsi_demux_new_cb_t pf_new_cb, void * p_new_cb_data)
 * \brief dvbpsi_AttachDemux is deprecated use @see dvbpsi_demux_chain_new() instead.
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \param pf_new_cb A callcack called when a new type of subtable is found.
 * \param p_new_cb_data Data given to the previous callback.
 * \return true on success, false on failure
 */
__attribute__((deprecated,unused))
bool dvbpsi_AttachDemux(dvbpsi_t *            p_dvbpsi,
                        dvbpsi_demux_new_cb_t pf_new_cb,
                        void *                p_new_cb_data);

/*****************************************************************************
 * dvbpsi_DetachDemux
 *****************************************************************************/
/*!
 * \fn __attribute__((deprecated,unused)) void dvbpsi_DetachDemux(dvbpsi_t *p_dvbpsi)
 * \brief dvbpsi_DetachDemux is deprecated use @see dvbpsi_demux_chain_delete() instead
 * \param p_dvbpsi The handle of the demux to be destroyed.
 * \return nothing
 */
__attribute__((deprecated,unused))
void dvbpsi_DetachDemux(dvbpsi_t *p_dvbpsi);

/*****************************************************************************
 * dvbpsi_demuxGetSubDec
 *****************************************************************************/
/*!
 * \fn __attribute__((deprecated,unused)) dvbpsi_demux_subdec_t *dvbpsi_demuxGetSubDec(dvbpsi_demux_t *, uint8_t, uint16_t)
 * \brief dvbpsi_demux_GetSubDec is deprecated use @see dvbpsi_decoder_chain_get() instead.
 * \param p_demux Pointer to the demux structure.
 * \param i_table_id Table ID of the wanted subtable.
 * \param i_extension Table ID extension of the wanted subtable.
 * \return a pointer to the found subdecoder, or NULL.
 *
 */
__attribute__((deprecated,unused))
dvbpsi_demux_subdec_t * dvbpsi_demuxGetSubDec(dvbpsi_demux_t * p_demux,
                                              uint8_t          i_table_id,
                                              uint16_t         i_extension);

/*****************************************************************************
 * dvbpsi_Demux
 *****************************************************************************/
/*!
 * \fn __attribute__((deprecated)) void dvbpsi_Demux(dvbpsi_t *p_dvbpsi,
                         dvbpsi_psi_section_t * p_section)
 * \brief dvbpsi_Demux is deprecated @see dvbpsi_decoder_chain_demux() instead.
 * \param p_dvbpsi PSI decoder handle.
 * \param p_section PSI section.
 */
__attribute__((deprecated,unused))
void dvbpsi_Demux(dvbpsi_t *p_dvbpsi, dvbpsi_psi_section_t *p_section);

/*****************************************************************************
 * dvbpsi_NewDemuxSubDecoder
 *****************************************************************************/
/*!
 * \fn __attribute__((deprecated)) dvbpsi_demux_subdec_t *dvbpsi_NewDemuxSubDecoder(const uint8_t i_table_id,
                                                 const uint16_t i_extension,
                                                 dvbpsi_demux_detach_cb_t pf_detach,
                                                 dvbpsi_demux_gather_cb_t pf_gather,
                                                 dvbpsi_decoder_t *p_decoder)
 * \brief dvbpsi_NewDemuxSubDecoder is deprecated use @see dvbpsi_decoder_chain_add() instead.
 * \param i_table_id table id to create subtable decoder for
 * \param i_extension table extension to create subtable decoder for
 * \param pf_detach pointer to detach function for subtable decoder.
 * \param pf_gather pointer to gather function for subtable decoder.
 * \param p_decoder pointer to private decoder.
 * \return pointer to demux subtable decoder.
 */
__attribute__((deprecated,unused))
dvbpsi_demux_subdec_t *dvbpsi_NewDemuxSubDecoder(const uint8_t i_table_id,
                                                 const uint16_t i_extension,
                                                 dvbpsi_demux_detach_cb_t pf_detach,
                                                 dvbpsi_demux_gather_cb_t pf_gather,
                                                 dvbpsi_decoder_t *p_decoder);

/*****************************************************************************
 * dvbpsi_DeleteDemuxSubDecoder
 *****************************************************************************/
/*!
 * \fn __attribute__((deprecated)) void dvbpsi_DeleteDemuxSubDecoder(dvbpsi_demux_subdec_t *p_subdec)
 * \brief dvbpsi_DeleteDemuxSubDecoder is deprecated use @see dvbpsi_decoder_chain_remove() instead.
 * \param p_subdec pointer to demux subtable decoder.
 * \return nothing.
 */
__attribute__((deprecated,unused))
void dvbpsi_DeleteDemuxSubDecoder(dvbpsi_demux_subdec_t *p_subdec);

/*****************************************************************************
 * dvbpsi_AttachDemuxSubDecoder
 *****************************************************************************/
/*!
 * \fn __attribute__((deprecated)) void dvbpsi_AttachDemuxSubDecoder(dvbpsi_demux_t *p_demux, dvbpsi_demux_subdec_t *p_subdec)
 * \brief dvbpsi_AttachDemuxSubDecoder is deprecated use dvbpsi_<table>_detach() instead.
 * \param p_demux pointer to dvbpsi_demux_t
 * \param p_subdec pointer to dvbpsi_demux_subdec_t
 * \return nothing
 */
__attribute__((deprecated,unused))
void dvbpsi_AttachDemuxSubDecoder(dvbpsi_demux_t *p_demux, dvbpsi_demux_subdec_t *p_subdec);

/*****************************************************************************
 * dvbpsi_DetachDemuxSubDecoder
 *****************************************************************************/
/*!
 * \fn __attribute__((deprecated)) void dvbpsi_DetachDemuxSubDecoder(dvbpsi_demux_t *p_demux, dvbpsi_demux_subdec_t *p_subdec)
 * \brief dvbpsi_DetachDemuxSubDecoder is deprecated use dvbpsi_<table>_detach() instead.
 * \param p_demux pointer to dvbpsi_demux_t
 * \param p_subdec pointer to dvbpsi_demux_subdec_t
 * \return nothing
 */
__attribute__((deprecated,unused))
void dvbpsi_DetachDemuxSubDecoder(dvbpsi_demux_t *p_demux, dvbpsi_demux_subdec_t *p_subdec);

#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of demux.h"
#endif
