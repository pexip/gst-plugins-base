/* GStreamer
 * Copyright (C) <2017> Havard Graff <havard@pexip.com>
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

#ifndef __GST_RTP_AUDIO_LEVEL_META_H__
#define __GST_RTP_AUDIO_LEVEL_META_H__

#include <gst/gst.h>
#include <gst/rtp/gstrtpbuffer.h>

G_BEGIN_DECLS

#define GST_RTP_AUDIO_LEVEL_META_API_TYPE  (gst_rtp_audio_level_meta_api_get_type())
#define GST_RTP_AUDIO_LEVEL_META_INFO  (gst_rtp_audio_level_meta_get_info())
typedef struct _GstRTPAudioLevelMeta GstRTPAudioLevelMeta;

/**
 * GstRTPAudioLevelMeta:
 * @meta: parent #GstMeta
 * @level: the -dBov from 0-127 (127 is silence).
 * @voice_activity: whether the buffer contains voice activity
 *
 * Meta containing Audio Level Indication: https://tools.ietf.org/html/rfc6464
 *
 * Since: 1.12
 */
struct _GstRTPAudioLevelMeta
{
  GstMeta meta;

  guint8 level;
  gboolean voice_activity;
};

GST_RTP_API
GType                  gst_rtp_audio_level_meta_api_get_type                (void);

GST_RTP_API
GstRTPAudioLevelMeta * gst_buffer_add_rtp_audio_level_meta                  (GstBuffer * buffer,
                                                                             guint8 level,
                                                                             gboolean voice_activity);
GST_RTP_API
GstRTPAudioLevelMeta * gst_buffer_get_rtp_audio_level_meta                  (GstBuffer * buffer);

GST_RTP_API
GstRTPAudioLevelMeta * gst_buffer_extract_rtp_audio_level_meta_one_byte_ext (GstBuffer *buffer,
                                                                             GstRTPBuffer *rtp,
                                                                             guint8 id);
GST_RTP_API
gboolean               gst_rtp_audio_level_meta_add_one_byte_ext            (GstRTPAudioLevelMeta *meta,
                                                                             GstRTPBuffer *rtp, guint8 id);

GST_RTP_API
gboolean               gst_rtp_audio_level_meta_add_two_byte_ext            (GstRTPAudioLevelMeta *meta,
                                                                             GstRTPBuffer *rtp, guint8 id);
GST_RTP_API
const GstMetaInfo *    gst_rtp_audio_level_meta_get_info                    (void);

G_END_DECLS

#endif /* __GST_RTP_AUDIO_LEVEL_META_H__ */
