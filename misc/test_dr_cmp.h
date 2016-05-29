static int my_memcmp(const void *s1, const void *s2, size_t n,
    int (*compar)(const void *, const void *))
{
    for(size_t i = 0 ; i < n ; ++i)
    {
        int cmp = compar(s1, s2);
        if(cmp != 0) return cmp;
    }
    return 0;
}

static int compare_dvbpsi_service_list_service_s(const void *s1, const void *s2)
{
    const struct dvbpsi_service_list_service_s *a = s1, *b = s2;
    if(a->i_service_id < b->i_service_id ||
      a->i_service_type < b->i_service_type) return -1;
    else if(a->i_service_id > b->i_service_id ||
      a->i_service_type > b->i_service_type) return 1;
    else return 0;
}

static int compare_dvbpsi_nvod_ref_t(const void *s1, const void *s2)
{
    const dvbpsi_nvod_ref_t *a = s1, *b = s2;
    if(a->i_transport_stream_id < b->i_transport_stream_id ||
        a->i_original_network_id < b->i_original_network_id ||
        a->i_service_id < b->i_service_id) return -1;
    else if(a->i_transport_stream_id > b->i_transport_stream_id ||
        a->i_original_network_id > b->i_original_network_id ||
        a->i_service_id > b->i_service_id) return 1;
    else return 0;
}

static int compare_dvbpsi_ca_system_t(const void *s1, const void *s2)
{
    const dvbpsi_ca_system_t *a = s1, *b = s2;
    if(a->i_ca_system_id < b->i_ca_system_id) return -1;
    else if(a->i_ca_system_id > b->i_ca_system_id) return 1;
    else return 0;
}

static int compare_dvbpsi_content_t(const void *s1, const void *s2)
{
    const dvbpsi_content_t *a = s1, *b = s2;
    if(a->i_type < b->i_type || a->i_user_byte < b->i_user_byte) return -1;
    else if(a->i_type > b->i_type || a->i_user_byte > b->i_user_byte) return 1;
    else return 0;
}

static int compare_dvbpsi_parental_rating_t(const void *s1, const void *s2)
{
    const dvbpsi_parental_rating_t *a = s1, *b = s2;
    uint32_t a_cc = (a->i_country_code & 0xffffff),
        b_cc = (b->i_country_code & 0xffffff);
    if(a_cc < b_cc || a->i_rating < b->i_rating) return -1;
    else if(a_cc > b_cc || a->i_rating > b->i_rating) return 1;
    else return 0;
}

static int compare_dvbpsi_teletextpage_t(const void *s1, const void *s2)
{
    const dvbpsi_teletextpage_t *a = s1, *b = s2;
    int iso_cmp = memcmp(a->i_iso6392_language_code, b->i_iso6392_language_code,
        sizeof(a->i_iso6392_language_code));
    uint8_t a_tt = (a->i_teletext_type & 0x1f),
        a_tmn = (a->i_teletext_magazine_number & 0x07),
        b_tt = (b->i_teletext_type & 0x1f),
        b_tmn = (b->i_teletext_magazine_number & 0x07);
    if(iso_cmp < 0 || a_tt < b_tt || a_tmn < b_tmn ||
        a->i_teletext_page_number < b->i_teletext_page_number) return -1;
    else if(iso_cmp > 0 || a_tt > b_tt || a_tmn > b_tmn ||
        a->i_teletext_page_number > b->i_teletext_page_number) return 1;
    else return 0;
}

static int compare_dvbpsi_local_time_offset_t(const void *s1, const void *s2)
{
    const dvbpsi_local_time_offset_t *a = s1, *b = s2;
    int iso_cmp = memcmp(a->i_country_code, b->i_country_code,
        sizeof(a->i_country_code));
    uint8_t a_cr_id = (a->i_country_region_id & 0x3f),
        a_ltop = (a->i_local_time_offset_polarity & 1),
        b_cr_id = (b->i_country_region_id & 0x3f),
        b_ltop = (b->i_local_time_offset_polarity & 1);
    uint64_t a_toc = (a->i_time_of_change & 0xffffffffff),
        b_toc = (b->i_time_of_change & 0xffffffffff);

    if(iso_cmp < 0 || a_cr_id < b_cr_id || a_ltop < b_ltop ||
        a->i_local_time_offset < b->i_local_time_offset ||
        a_toc < b_toc || a->i_next_time_offset < b->i_next_time_offset)
        return -1;
    else if(iso_cmp > 0 || a_cr_id > b_cr_id || a_ltop > b_ltop ||
        a->i_local_time_offset > b->i_local_time_offset ||
        a_toc > b_toc || a->i_next_time_offset > b->i_next_time_offset)
        return 1;
    else return 0;
}

static int compare_dvbpsi_subtitle_t(const void *s1, const void *s2)
{
    const dvbpsi_subtitle_t *a = s1, *b = s2;
    int iso_cmp = memcmp(a->i_iso6392_language_code, b->i_iso6392_language_code,
        sizeof(a->i_iso6392_language_code));

    if(iso_cmp < 0 || a->i_subtitling_type < b->i_subtitling_type ||
        a->i_composition_page_id < b->i_composition_page_id ||
        a->i_ancillary_page_id < b->i_ancillary_page_id) return -1;
    else if(iso_cmp > 0 || a->i_subtitling_type > b->i_subtitling_type ||
        a->i_composition_page_id > b->i_composition_page_id ||
        a->i_ancillary_page_id > b->i_ancillary_page_id) return 1;
    else return 0;
}

static int compare_dvbpsi_fmc_t(const void *s1, const void *s2)
{
    const dvbpsi_fmc_t *a = s1, *b = s2;
    if(a->i_es_id < b->i_es_id ||
        a->i_flex_mux_channel < b->i_flex_mux_channel) return -1;
    else if(a->i_es_id > b->i_es_id ||
        a->i_flex_mux_channel > b->i_flex_mux_channel) return 1;
    else return 0;
}
