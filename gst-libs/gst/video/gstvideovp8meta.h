/* GStreamer
 * Copyright (C) <2017> Pexip AS
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
 *
 * Author: John-Mark Bell <jmb@pexip.com>
 */

#ifndef __GST_VIDEO_VP8_META_H__
#define __GST_VIDEO_VP8_META_H__

#include <gst/gst.h>
#include <gst/video/video.h>

G_BEGIN_DECLS

#define GST_VIDEO_VP8_META_API_TYPE (gst_video_vp8_meta_api_get_type())
#define GST_VIDEO_VP8_META_INFO (gst_video_vp8_meta_get_info())
typedef struct _GstVideoVP8Meta GstVideoVP8Meta;

/**
 * GstVideoVP8Meta:
 * @meta: parent #GstMeta
 * @use_temporal_scaling: whether temporal scaling is in use
 * @layer_sync: whether the buffer contains (part of) a frame that depends
 *              only on the base layer
 * @temporal_layer_id: temporal layer id of the frame (fragment) in this buffer
 * @tl0picidx: temporal layer zero index
 *
 * Meta containing additional data of use when (de)payloading:
 * https://tools.ietf.org/html/rfc7741
 *
 * Since 1.12
 */
struct _GstVideoVP8Meta
{
  GstMeta meta;

  /* Temporal scaling */
  gboolean use_temporal_scaling;
  gboolean layer_sync;
  guint8 temporal_layer_id;
  guint8 tl0picidx;
};

GST_EXPORT
GType gst_video_vp8_meta_api_get_type (void);

GST_EXPORT
const GstMetaInfo * gst_video_vp8_meta_get_info (void);

GST_EXPORT
GstVideoVP8Meta * gst_buffer_add_video_vp8_meta (GstBuffer * buffer);

GST_EXPORT
GstVideoVP8Meta * gst_buffer_add_video_vp8_meta_full (GstBuffer * buffer,
                                                      gboolean use_temporal_scaling,
                                                      gboolean layer_sync,
                                                      guint8 temporal_layer_id,
                                                      guint8 tl0picidx);

GST_EXPORT
GstVideoVP8Meta * gst_buffer_get_video_vp8_meta (GstBuffer * buffer);

G_END_DECLS

#endif /* __GST_VIDEO_VP8_META_H__ */
