/* GStreamer
 *
 * Copyright (C) 2020 Philippe Normand <philn@igalia.com>
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

#ifndef __GTK_GST_PLAYER_WRAPPED_VIDEO_RENDERER_H__
#define __GTK_GST_PLAYER_WRAPPED_VIDEO_RENDERER_H__

#include <gst/player/gstplayer-types.h>
#include <gst/player/gstplayer-video-renderer.h>

G_BEGIN_DECLS

typedef struct _GtkGstPlayerWrappedVideoRenderer
    GtkGstPlayerWrappedVideoRenderer;
typedef struct _GtkGstPlayerWrappedVideoRendererClass
    GtkGstPlayerWrappedVideoRendererClass;

#define GST_TYPE_PLAYER_WRAPPED_VIDEO_RENDERER             (gtk_gst_player_wrapped_video_renderer_get_type ())
#define GST_IS_PLAYER_WRAPPED_VIDEO_RENDERER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_PLAYER_WRAPPED_VIDEO_RENDERER))
#define GST_IS_PLAYER_WRAPPED_VIDEO_RENDERER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_PLAYER_WRAPPED_VIDEO_RENDERER))
#define GTK_GST_PLAYER_WRAPPED_VIDEO_RENDERER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_PLAYER_WRAPPED_VIDEO_RENDERER, GtkGstPlayerWrappedVideoRendererClass))
#define GTK_GST_PLAYER_WRAPPED_VIDEO_RENDERER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_PLAYER_WRAPPED_VIDEO_RENDERER, GtkGstPlayerWrappedVideoRenderer))
#define GTK_GST_PLAYER_WRAPPED_VIDEO_RENDERER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_PLAYER_WRAPPED_VIDEO_RENDERER, GtkGstPlayerWrappedVideoRendererClass))
#define GTK_GST_PLAYER_WRAPPED_VIDEO_RENDERER_CAST(obj)        ((GtkGstPlayerWrappedVideoRenderer*)(obj))

GType gtk_gst_player_wrapped_video_renderer_get_type (void);

GtkGstPlayerVideoRenderer * gtk_gst_player_wrapped_video_renderer_new (GtkGstPlayerVideoRenderer * renderer, GtkGstPlayer * player);


G_END_DECLS

#endif /* __GTK_GST_PLAYER_WRAPPED_VIDEO_RENDERER_H__ */
