/* GStreamer
 * Copyright (C) <2015> Stian Selnes <stian@pexip.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_RTP_META_H__
#define __GST_RTP_META_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_RTP_SOURCE_META_API_TYPE  (gst_rtp_source_meta_api_get_type())
#define GST_RTP_SOURCE_META_INFO  (gst_rtp_source_meta_get_info())
typedef struct _GstRTPSourceMeta GstRTPSourceMeta;

/**
 * GstRTPSourceMeta:
 * @meta: parent #GstMeta
 * @ssrc: (allow-none): pointer to the SSRC
 * @csrc: (allow-none): pointer to the CSRCs
 * @csrc_count: number of elements in @csrc
 *
 * Meta describing the source(s) of the buffer.
 *
 * Since: 1.8
 */
struct _GstRTPSourceMeta
{
  GstMeta meta;

  guint32 *ssrc;
  guint32 *csrc;
  guint csrc_count;
};

GstRTPSourceMeta *  gst_buffer_add_rtp_source_meta (GstBuffer * buf, guint32 * ssrc, guint32 * csrc, guint csrc_count);
GstRTPSourceMeta *  gst_buffer_get_rtp_source_meta (GstBuffer * buf);
guint               gst_rtp_source_meta_get_source_count (const GstRTPSourceMeta * meta);
GType               gst_rtp_source_meta_api_get_type (void);
const GstMetaInfo * gst_rtp_source_meta_get_info (void);
G_END_DECLS

#endif