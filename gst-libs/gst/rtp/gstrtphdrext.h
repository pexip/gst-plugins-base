/* GStreamer
 * Copyright (C) <2012> Wim Taymans <wim.taymans@gmail.com>
 * Copyright (C) <2020> Matthew Waters <matthew@centricular.com>
 *
 * gstrtphdrext.h: RTP header extensions
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GST_RTPHDREXT_H__
#define __GST_RTPHDREXT_H__

#include <gst/gst.h>
#include <gst/rtp/gstrtpbuffer.h>

G_BEGIN_DECLS

#define GST_RTP_HDREXT_BASE "urn:ietf:params:rtp-hdrext:"

/* RFC 6051 */
#define GST_RTP_HDREXT_NTP_64 "ntp-64"

#define GST_RTP_HDREXT_NTP_64_SIZE 8

GST_RTP_API
gboolean       gst_rtp_hdrext_set_ntp_64  (gpointer data, guint size, guint64 ntptime);

GST_RTP_API
gboolean       gst_rtp_hdrext_get_ntp_64  (gpointer data, guint size, guint64 *ntptime);

#define GST_RTP_HDREXT_NTP_56 "ntp-56"

#define GST_RTP_HDREXT_NTP_56_SIZE 7

GST_RTP_API
gboolean       gst_rtp_hdrext_set_ntp_56  (gpointer data, guint size, guint64 ntptime);

GST_RTP_API
gboolean       gst_rtp_hdrext_get_ntp_56  (gpointer data, guint size, guint64 *ntptime);

/**
 * GST_RTP_HDREXT_ELEMENT_CLASS:
 *
 * Constant string used in element classification to signal that this element
 * is a RTP header extension.
 */
#define GST_RTP_HDREXT_ELEMENT_CLASS "Network/Extension/RTPHeader"

GST_RTP_API
GType gst_rtp_header_extension_get_type (void);
#define GST_TYPE_RTP_HEADER_EXTENSION (gst_rtp_header_extension_get_type())
#define GST_RTP_HEADER_EXTENSION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_RTP_HEADER_EXTENSION,GstRTPHeaderExtension))
#define GST_RTP_HEADER_EXTENSION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_RTP_HEADER_EXTENSION,GstRTPHeaderExtensionClass))
#define GST_RTP_HEADER_EXTENSION_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS ((obj),GST_TYPE_RTP_HEADER_EXTENSION,GstRTPHeaderExtensionClass))
#define GST_IS_RTP_HEADER_EXTENSION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_RTP_HEADER_EXTENSION))
#define GST_IS_RTP_HEADER_EXTENSION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_RTP_HEADER_EXTENSION))
#define GST_RTP_HEADER_EXTENSION_CAST(obj) ((GstRTPHeaderExtension *)(obj))

typedef struct _GstRTPHeaderExtension      GstRTPHeaderExtension;
typedef struct _GstRTPHeaderExtensionClass GstRTPHeaderExtensionClass;
typedef struct _GstRTPHeaderExtensionPrivate GstRTPHeaderExtensionPrivate;

/**
 * GstRTPHeaderExtensionFlags:
 * @GST_RTP_HEADER_EXTENSION_ONE_BYTE: The one byte rtp extension header.
 *              1-16 data bytes per extension with a maximum of
 *              14 extension ids in total.
 * @GST_RTP_HEADER_EXTENSION_TWO_BYTE: The two byte rtp extension header.
 *              256 data bytes per extension with a maximum of 255 (or 256
 *              including appbits) extensions in total.
 *
 * Flags that apply to a RTP Audio/Video header extension.
 */
typedef enum /*< underscore_name=gst_rtp_header_extension_flags >*/
{
  GST_RTP_HEADER_EXTENSION_ONE_BYTE = (1 << 0),
  GST_RTP_HEADER_EXTENSION_TWO_BYTE = (1 << 1),
} GstRTPHeaderExtensionFlags;

/**
 * GstRTPHeaderExtension:
 * @parent: the parent #GObject
 * @ext_id: the configured extension id
 *
 * Instance struct for a RTP Audio/Video header extension.
 */
struct _GstRTPHeaderExtension
{
  GstElement parent;

  guint ext_id;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};

/**
 * GstRTPHeaderExtensionClass:
 * @parent_class: the parent class
 *
 * Base class for RTP  Header extensions.
 */
struct _GstRTPHeaderExtensionClass
{
  GstElementClass parent_class;

  /*< public >*/
  const gchar *         (*get_uri)                  (GstRTPHeaderExtension * ext);

  GstRTPHeaderExtensionFlags (*get_supported_flags) (GstRTPHeaderExtension * ext);

  gsize                 (*get_max_size)             (GstRTPHeaderExtension * ext,
                                                     const GstBuffer * input_meta);

  gsize                 (*write)                    (GstRTPHeaderExtension * ext,
                                                     const GstBuffer * input_meta,
                                                     GstRTPHeaderExtensionFlags write_flags,
                                                     GstBuffer * output,
                                                     guint8 * data,
                                                     gsize size);

  gboolean              (*read)                     (GstRTPHeaderExtension * ext,
                                                     GstRTPHeaderExtensionFlags read_flags,
                                                     const guint8 * data,
                                                     gsize size,
                                                     GstBuffer * buffer);

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GstRTPHeaderExtension, gst_object_unref)

GST_RTP_API
const gchar *       gst_rtp_header_extension_get_uri            (GstRTPHeaderExtension * ext);
GST_RTP_API
gsize               gst_rtp_header_extension_get_max_size       (GstRTPHeaderExtension * ext,
                                                                 const GstBuffer * input_meta);
GST_RTP_API
GstRTPHeaderExtensionFlags gst_rtp_header_extension_get_supported_flags (GstRTPHeaderExtension * ext);
GST_RTP_API
guint               gst_rtp_header_extension_get_id             (GstRTPHeaderExtension * ext);
GST_RTP_API
void                gst_rtp_header_extension_set_id             (GstRTPHeaderExtension * ext,
                                                                 guint ext_id);
GST_RTP_API
gsize               gst_rtp_header_extension_write              (GstRTPHeaderExtension * ext,
                                                                 const GstBuffer * input_meta,
                                                                 GstRTPHeaderExtensionFlags write_flags,
                                                                 GstBuffer * output,
                                                                 guint8 * data,
                                                                 gsize size);
GST_RTP_API
gboolean            gst_rtp_header_extension_read               (GstRTPHeaderExtension * ext,
                                                                 GstRTPHeaderExtensionFlags read_flags,
                                                                 const guint8 * data,
                                                                 gsize size,
                                                                 GstBuffer * buffer);
GST_RTP_API
GList *             gst_rtp_get_header_extension_list           (void);
GST_RTP_API
GstRTPHeaderExtension * gst_rtp_header_extension_create_from_uri (const gchar * uri);

G_END_DECLS

#endif /* __GST_RTPHDREXT_H__ */

