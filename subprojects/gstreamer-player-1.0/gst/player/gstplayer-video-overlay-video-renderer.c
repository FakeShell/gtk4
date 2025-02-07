/* GStreamer
 *
 * Copyright (C) 2014-2015 Sebastian Dröge <sebastian@centricular.com>
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

/**
 * SECTION:gstplayer-videooverlayvideorenderer
 * @title: GtkGstPlayerVideoOverlayVideoRenderer
 * @short_description: Player Video Overlay Video Renderer
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstplayer-video-overlay-video-renderer.h"
#include "gstplayer.h"

#include <gst/video/video.h>

struct _GtkGstPlayerVideoOverlayVideoRenderer
{
  GObject parent;

  GstVideoOverlay *video_overlay;
  gpointer window_handle;
  gint x, y, width, height;

  GstElement *video_sink;       /* configured video sink, or NULL      */
};

struct _GtkGstPlayerVideoOverlayVideoRendererClass
{
  GObjectClass parent_class;
};

static void
    gtk_gst_player_video_overlay_video_renderer_interface_init
    (GtkGstPlayerVideoRendererInterface * iface);

enum
{
  VIDEO_OVERLAY_VIDEO_RENDERER_PROP_0,
  VIDEO_OVERLAY_VIDEO_RENDERER_PROP_WINDOW_HANDLE,
  VIDEO_OVERLAY_VIDEO_RENDERER_PROP_VIDEO_SINK,
  VIDEO_OVERLAY_VIDEO_RENDERER_PROP_LAST
};

G_DEFINE_TYPE_WITH_CODE (GtkGstPlayerVideoOverlayVideoRenderer,
    gtk_gst_player_video_overlay_video_renderer, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (GST_TYPE_PLAYER_VIDEO_RENDERER,
        gtk_gst_player_video_overlay_video_renderer_interface_init));

static GParamSpec
    * video_overlay_video_renderer_param_specs
    [VIDEO_OVERLAY_VIDEO_RENDERER_PROP_LAST] = { NULL, };

static void
gtk_gst_player_video_overlay_video_renderer_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec)
{
  GtkGstPlayerVideoOverlayVideoRenderer *self =
      GTK_GST_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (object);

  switch (prop_id) {
    case VIDEO_OVERLAY_VIDEO_RENDERER_PROP_WINDOW_HANDLE:
      self->window_handle = g_value_get_pointer (value);
      if (self->video_overlay)
        gst_video_overlay_set_window_handle (self->video_overlay,
            (guintptr) self->window_handle);
      break;
    case VIDEO_OVERLAY_VIDEO_RENDERER_PROP_VIDEO_SINK:
      self->video_sink = gst_object_ref_sink (g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_gst_player_video_overlay_video_renderer_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkGstPlayerVideoOverlayVideoRenderer *self =
      GTK_GST_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (object);

  switch (prop_id) {
    case VIDEO_OVERLAY_VIDEO_RENDERER_PROP_WINDOW_HANDLE:
      g_value_set_pointer (value, self->window_handle);
      break;
    case VIDEO_OVERLAY_VIDEO_RENDERER_PROP_VIDEO_SINK:
      g_value_set_object (value, self->video_sink);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_gst_player_video_overlay_video_renderer_finalize (GObject * object)
{
  GtkGstPlayerVideoOverlayVideoRenderer *self =
      GTK_GST_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (object);

  if (self->video_overlay)
    gst_object_unref (self->video_overlay);

  if (self->video_sink)
    gst_object_unref (self->video_sink);

  G_OBJECT_CLASS
      (gtk_gst_player_video_overlay_video_renderer_parent_class)->finalize (object);
}

static void
    gtk_gst_player_video_overlay_video_renderer_class_init
    (GtkGstPlayerVideoOverlayVideoRendererClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property =
      gtk_gst_player_video_overlay_video_renderer_set_property;
  gobject_class->get_property =
      gtk_gst_player_video_overlay_video_renderer_get_property;
  gobject_class->finalize = gtk_gst_player_video_overlay_video_renderer_finalize;

  video_overlay_video_renderer_param_specs
      [VIDEO_OVERLAY_VIDEO_RENDERER_PROP_WINDOW_HANDLE] =
      g_param_spec_pointer ("window-handle", "Window Handle",
      "Window handle to embed the video into",
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

  video_overlay_video_renderer_param_specs
      [VIDEO_OVERLAY_VIDEO_RENDERER_PROP_VIDEO_SINK] =
      g_param_spec_object ("video-sink", "Video Sink",
      "the video output element to use (NULL = default sink)",
      GST_TYPE_ELEMENT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class,
      VIDEO_OVERLAY_VIDEO_RENDERER_PROP_LAST,
      video_overlay_video_renderer_param_specs);
}

static void
    gtk_gst_player_video_overlay_video_renderer_init
    (GtkGstPlayerVideoOverlayVideoRenderer * self)
{
  self->x = self->y = self->width = self->height = -1;
  self->video_sink = NULL;
}

static GstElement *gtk_gst_player_video_overlay_video_renderer_create_video_sink
    (GtkGstPlayerVideoRenderer * iface, GtkGstPlayer * player)
{
  GstElement *video_overlay;
  GtkGstPlayerVideoOverlayVideoRenderer *self =
      GTK_GST_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (iface);

  if (self->video_overlay)
    gst_object_unref (self->video_overlay);

  video_overlay = gtk_gst_player_get_pipeline (player);
  g_return_val_if_fail (GST_IS_VIDEO_OVERLAY (video_overlay), NULL);

  self->video_overlay = GST_VIDEO_OVERLAY (video_overlay);

  gst_video_overlay_set_window_handle (self->video_overlay,
      (guintptr) self->window_handle);
  if (self->width != -1 || self->height != -1)
    gst_video_overlay_set_render_rectangle (self->video_overlay, self->x,
        self->y, self->width, self->height);

  return self->video_sink;
}

static void
    gtk_gst_player_video_overlay_video_renderer_interface_init
    (GtkGstPlayerVideoRendererInterface * iface)
{
  iface->create_video_sink =
      gtk_gst_player_video_overlay_video_renderer_create_video_sink;
}

/**
 * gtk_gst_player_video_overlay_video_renderer_new:
 * @window_handle: (allow-none): Window handle to use or %NULL
 *
 * Returns: (transfer full):
 */
GtkGstPlayerVideoRenderer *
gtk_gst_player_video_overlay_video_renderer_new (gpointer window_handle)
{
  return g_object_new (GST_TYPE_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER,
      "window-handle", window_handle, NULL);
}

/**
 * gtk_gst_player_video_overlay_video_renderer_new_with_sink:
 * @window_handle: (allow-none): Window handle to use or %NULL
 * @video_sink: (transfer floating): the custom video_sink element to be set for the video renderer
 *
 * Returns: (transfer full):
 *
 * Since: 1.12
 */
GtkGstPlayerVideoRenderer *
gtk_gst_player_video_overlay_video_renderer_new_with_sink (gpointer window_handle,
    GstElement * video_sink)
{
  return g_object_new (GST_TYPE_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER,
      "window-handle", window_handle, "video-sink", video_sink, NULL);
}

/**
 * gtk_gst_player_video_overlay_video_renderer_set_window_handle:
 * @self: #GtkGstPlayerVideoRenderer instance
 * @window_handle: handle referencing to the platform specific window
 *
 * Sets the platform specific window handle into which the video
 * should be rendered
 **/
void gtk_gst_player_video_overlay_video_renderer_set_window_handle
    (GtkGstPlayerVideoOverlayVideoRenderer * self, gpointer window_handle)
{
  g_return_if_fail (GST_IS_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (self));

  g_object_set (self, "window-handle", window_handle, NULL);
}

/**
 * gtk_gst_player_video_overlay_video_renderer_get_window_handle:
 * @self: #GtkGstPlayerVideoRenderer instance
 *
 * Returns: (transfer none): The currently set, platform specific window
 * handle
 */
gpointer
    gtk_gst_player_video_overlay_video_renderer_get_window_handle
    (GtkGstPlayerVideoOverlayVideoRenderer * self) {
  gpointer window_handle;

  g_return_val_if_fail (GST_IS_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (self),
      NULL);

  g_object_get (self, "window-handle", &window_handle, NULL);

  return window_handle;
}

/**
 * gtk_gst_player_video_overlay_video_renderer_expose:
 * @self: a #GtkGstPlayerVideoOverlayVideoRenderer instance.
 *
 * Tell an overlay that it has been exposed. This will redraw the current frame
 * in the drawable even if the pipeline is PAUSED.
 */
void gtk_gst_player_video_overlay_video_renderer_expose
    (GtkGstPlayerVideoOverlayVideoRenderer * self)
{
  g_return_if_fail (GST_IS_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (self));

  if (self->video_overlay)
    gst_video_overlay_expose (self->video_overlay);
}

/**
 * gtk_gst_player_video_overlay_video_renderer_set_render_rectangle:
 * @self: a #GtkGstPlayerVideoOverlayVideoRenderer instance
 * @x: the horizontal offset of the render area inside the window
 * @y: the vertical offset of the render area inside the window
 * @width: the width of the render area inside the window
 * @height: the height of the render area inside the window
 *
 * Configure a subregion as a video target within the window set by
 * gtk_gst_player_video_overlay_video_renderer_set_window_handle(). If this is not
 * used or not supported the video will fill the area of the window set as the
 * overlay to 100%. By specifying the rectangle, the video can be overlaid to
 * a specific region of that window only. After setting the new rectangle one
 * should call gtk_gst_player_video_overlay_video_renderer_expose() to force a
 * redraw. To unset the region pass -1 for the @width and @height parameters.
 *
 * This method is needed for non fullscreen video overlay in UI toolkits that
 * do not support subwindows.
 *
 */
void gtk_gst_player_video_overlay_video_renderer_set_render_rectangle
    (GtkGstPlayerVideoOverlayVideoRenderer * self, gint x, gint y, gint width,
    gint height)
{
  g_return_if_fail (GST_IS_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (self));

  self->x = x;
  self->y = y;
  self->width = width;
  self->height = height;

  if (self->video_overlay)
    gst_video_overlay_set_render_rectangle (self->video_overlay,
        x, y, width, height);
}

/**
 * gtk_gst_player_video_overlay_video_renderer_get_render_rectangle:
 * @self: a #GtkGstPlayerVideoOverlayVideoRenderer instance
 * @x: (out) (allow-none): the horizontal offset of the render area inside the window
 * @y: (out) (allow-none): the vertical offset of the render area inside the window
 * @width: (out) (allow-none): the width of the render area inside the window
 * @height: (out) (allow-none): the height of the render area inside the window
 *
 * Return the currently configured render rectangle. See gtk_gst_player_video_overlay_video_renderer_set_render_rectangle()
 * for details.
 *
 */
void gtk_gst_player_video_overlay_video_renderer_get_render_rectangle
    (GtkGstPlayerVideoOverlayVideoRenderer * self, gint * x, gint * y,
    gint * width, gint * height)
{
  g_return_if_fail (GST_IS_PLAYER_VIDEO_OVERLAY_VIDEO_RENDERER (self));

  if (x)
    *x = self->x;
  if (y)
    *y = self->y;
  if (width)
    *width = self->width;
  if (height)
    *height = self->height;
}
