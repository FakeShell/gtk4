#include <gtk/gtk.h>

#include <math.h>
#include <stdlib.h>

static void
hsv_to_rgb (GdkRGBA *rgba,
            gdouble  h,
            gdouble  s,
            gdouble  v)
{
  gdouble hue, saturation, value;
  gdouble f, p, q, t;

  rgba->alpha = 1.0;

  if ( s == 0.0)
    {
      rgba->red = v;
      rgba->green = v;
      rgba->blue = v; /* heh */
    }
  else
    {
      hue = h * 6.0;
      saturation = s;
      value = v;

      if (hue == 6.0)
        hue = 0.0;

      f = hue - (int) hue;
      p = value * (1.0 - saturation);
      q = value * (1.0 - saturation * f);
      t = value * (1.0 - saturation * (1.0 - f));

      switch ((int) hue)
        {
        case 0:
          rgba->red = value;
          rgba->green = t;
          rgba->blue = p;
          break;

        case 1:
          rgba->red = q;
          rgba->green = value;
          rgba->blue = p;
          break;

        case 2:
          rgba->red = p;
          rgba->green = value;
          rgba->blue = t;
          break;

        case 3:
          rgba->red = p;
          rgba->green = q;
          rgba->blue = value;
          break;

        case 4:
          rgba->red = t;
          rgba->green = p;
          rgba->blue = value;
          break;

        case 5:
          rgba->red = value;
          rgba->green = p;
          rgba->blue = q;
          break;

        default:
          g_assert_not_reached ();
        }
    }
}

GskRenderNode *
rounded_borders (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *, n);
  GskRenderNode *container;
  GskRoundedRect outline;
  float widths[4];
  GdkRGBA colors[4];
  guint i;

  for (i = 0; i < n; i++)
    {
      outline.bounds.size.width = g_random_int_range (20, 1000);
      outline.bounds.origin.x = g_random_int_range (0, 1000 - outline.bounds.size.width);
      outline.bounds.size.height = g_random_int_range (20, 1000);
      outline.bounds.origin.y = g_random_int_range (0, 1000 - outline.bounds.size.height);
      outline.corner[0].width = outline.corner[0].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[1].width = outline.corner[1].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[2].width = outline.corner[2].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[3].width = outline.corner[3].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      widths[0] = widths[1] = widths[2] = widths[3] = g_random_int_range (0, 5);
      hsv_to_rgb (&colors[0], g_random_double (), 1.0, 1.0);
      colors[3] = colors[2] = colors[1] = colors[0];
      nodes[i] = gsk_border_node_new (&outline, widths, colors);
    }

  container = gsk_container_node_new (nodes, n);

  for (i = 0; i < n; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

GskRenderNode *
rounded_backgrounds (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *, n);
  GskRenderNode *container, *texture;
  GskRoundedRect outline;
  GdkRGBA color;
  guint i;

  for (i = 0; i < n; i++)
    {
      outline.bounds.size.width = g_random_int_range (20, 100);
      outline.bounds.origin.x = g_random_int_range (0, 1000 - outline.bounds.size.width);
      outline.bounds.size.height = g_random_int_range (20, 100);
      outline.bounds.origin.y = g_random_int_range (0, 1000 - outline.bounds.size.height);
      outline.corner[0].width = outline.corner[0].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[1].width = outline.corner[1].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[2].width = outline.corner[2].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[3].width = outline.corner[3].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      hsv_to_rgb (&color, g_random_double (), g_random_double_range (0.15, 0.4), g_random_double_range (0.6, 0.85));
      color.alpha = g_random_double_range (0.5, 0.75);
      texture = gsk_color_node_new (&color, &outline.bounds);
      nodes[i] = gsk_rounded_clip_node_new (texture, &outline);
    }

  container = gsk_container_node_new (nodes, n);

  for (i = 0; i < n; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

GskRenderNode *
colors (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *, 10 * n);
  GskRenderNode *container;
  graphene_rect_t bounds;
  GdkRGBA color;
  guint i;

  for (i = 0; i < 10 * n; i++)
    {
      bounds.size.width = g_random_int_range (20, 100);
      bounds.origin.x = g_random_int_range (0, 1000 - bounds.size.width);
      bounds.size.height = g_random_int_range (20, 100);
      bounds.origin.y = g_random_int_range (0, 1000 - bounds.size.height);
      hsv_to_rgb (&color, g_random_double (), g_random_double_range (0.15, 0.4), g_random_double_range (0.6, 0.85));
      color.alpha = g_random_double_range (0.5, 0.75);
      nodes[i] = gsk_color_node_new (&color, &bounds);
    }

  container = gsk_container_node_new (nodes, 10 * n);

  for (i = 0; i < 10 * n; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

GskRenderNode *
clipped_colors (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *,n);
  GskRenderNode *container;
  graphene_rect_t bounds;
  GdkRGBA color;
  guint i;

  for (i = 0; i < n; i++)
    {
      bounds.size.width = g_random_int_range (20, 100);
      bounds.origin.x = g_random_int_range (0, 1000 - bounds.size.width);
      bounds.size.height = g_random_int_range (20, 100);
      bounds.origin.y = g_random_int_range (0, 1000 - bounds.size.height);
      hsv_to_rgb (&color, g_random_double (), g_random_double_range (0.15, 0.4), g_random_double_range (0.6, 0.85));
      color.alpha = g_random_double_range (0.5, 0.75);
      nodes[i] = gsk_color_node_new (&color, &bounds);
    }

  container = gsk_container_node_new (nodes, n);

  for (i = 0; i < n; i++)
    gsk_render_node_unref (nodes[i]);

#define GRID_SIZE 4
  for (i = 0; i < GRID_SIZE * GRID_SIZE; i++)
    {
      guint x = i % GRID_SIZE;
      guint y = i / GRID_SIZE;

      if ((x + y) % 2)
        continue;

      nodes[i / 2] = gsk_clip_node_new (container,
                                        &GRAPHENE_RECT_INIT(
                                            x * 1000 / GRID_SIZE, y * 1000 / GRID_SIZE,
                                            1000 / GRID_SIZE, 1000 / GRID_SIZE
                                        ));
    }

  gsk_render_node_unref (container);

  container = gsk_container_node_new (nodes, GRID_SIZE * GRID_SIZE / 2);

  for (i = 0; i < GRID_SIZE * GRID_SIZE / 2; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

static int
compare_color_stops (gconstpointer a,
                     gconstpointer b,
                     gpointer user_data)
{
  const GskColorStop *stopa = a;
  const GskColorStop *stopb = b;

  if (stopa->offset < stopb->offset)
    return -1;
  else if (stopa->offset > stopb->offset)
    return 1;
  else
    return 0;
}

GskRenderNode *
linear_gradient (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *, n);
  GskRenderNode *container;
  graphene_rect_t bounds;
  GskColorStop stops[5];
  graphene_point_t start, end;
  guint i, j, n_stops;

  for (i = 0; i < n; i++)
    {
      bounds.size.width = g_random_int_range (20, 100);
      bounds.origin.x = g_random_int_range (0, 1000 - bounds.size.width);
      bounds.size.height = g_random_int_range (20, 100);
      bounds.origin.y = g_random_int_range (0, 1000 - bounds.size.height);
      do {
        start.x = g_random_double_range (- bounds.size.width / 4, bounds.size.width / 4);
        if (start.x >= 0)
          start.x += bounds.origin.x;
        else
          start.x += bounds.origin.x + bounds.size.width;
        start.y = g_random_double_range (- bounds.size.height / 4, bounds.size.height / 4);
        if (start.y >= 0)
          start.y += bounds.origin.y;
        else
          start.y += bounds.origin.y + bounds.size.height;
        end.x = g_random_double_range (- bounds.size.width / 4, bounds.size.width / 4);
        if (end.x >= 0)
          end.x += bounds.origin.x;
        else
          end.x += bounds.origin.x + bounds.size.width;
        end.y = g_random_double_range (- bounds.size.height / 4, bounds.size.height / 4);
        if (end.y >= 0)
          end.y += bounds.origin.y;
        else
          end.y += bounds.origin.y + bounds.size.height;
      } while (graphene_point_equal (&start, &end));
      n_stops = g_random_int_range (2, 5);
      for (j = 0; j < n_stops; j++)
        {
          if (j == 0)
            stops[j].offset = 0;
          else if (j == n_stops - 1)
            stops[j].offset = 1;
          else
            stops[j].offset = g_random_double_range (0, 1);
          hsv_to_rgb (&stops[j].color, g_random_double (), g_random_double_range (0.15, 0.4), g_random_double_range (0.6, 0.85));
          stops[j].color.alpha = g_random_double_range (0, 1);
        }
      g_qsort_with_data (stops, n_stops, sizeof (stops[0]), compare_color_stops, 0);
      if (g_random_boolean ())
        nodes[i] = gsk_linear_gradient_node_new (&bounds, &start, &end, stops, n_stops);
      else
        nodes[i] = gsk_repeating_linear_gradient_node_new (&bounds, &start, &end, stops, n_stops);
    }

  container = gsk_container_node_new (nodes, n);

  for (i = 0; i < n; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

GskRenderNode *
borders (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *, n);
  GskRenderNode *container;
  GskRoundedRect outline;
  GdkRGBA colors[4];
  float widths[4];
  guint i, j;

  for (i = 0; i < n; i++)
    {
      outline.bounds.size.width = g_random_int_range (20, 100);
      outline.bounds.origin.x = g_random_int_range (0, 1000 - outline.bounds.size.width);
      outline.bounds.size.height = g_random_int_range (20, 100);
      outline.bounds.origin.y = g_random_int_range (0, 1000 - outline.bounds.size.height);
      outline.corner[1].width = outline.corner[1].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[2].width = outline.corner[2].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[3].width = outline.corner[3].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      for (j = 0; j < 4; j++)
        {
          outline.corner[0].width = 10 - (int) sqrt (g_random_int_range (0, 100));
          outline.corner[0].height = 10 - (int) sqrt (g_random_int_range (0, 100));
          hsv_to_rgb (&colors[j], g_random_double (), 1.0, 0.5); //g_random_double_range (0.15, 0.4), g_random_double_range (0.6, 0.85));
          colors[j].alpha = 1.0; //g_random_double_range (0.8, 1.0);
          widths[j] = g_random_int_range (1, 6);
        }
      nodes[i] = gsk_border_node_new (&outline, widths, colors);
    }

  container = gsk_container_node_new (nodes, n);

  for (i = 0; i < n; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

const char *example =
"'Twas brillig, and the slithy toves\n"
"Did gyre and gimble in the wabe;\n"
"All mimsy were the borogoves,\n"
"And the mome raths outgrabe.\n"
"\n"
"'Beware the Jabberwock, my son!\n"
"The jaws that bite, the claws that catch!\n"
"Beware the Jubjub bird, and shun\n"
"The frumious Bandersnatch!'";

GskRenderNode *
text (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *, n);
  GskRenderNode *container;
  PangoFontDescription *desc;
  PangoContext *context;
  PangoLayout *layout;
  int i;

  context = gdk_pango_context_get ();

  desc = pango_font_description_new ();
  pango_font_description_set_family (desc, "Cantarell");
  layout = pango_layout_new (context);
  pango_layout_set_text (layout, example, -1);

  for (i = 0; i < n; i++)
    {
      PangoFont *font;
      PangoLayoutIter *iter;
      PangoGlyphString *glyphs;
      PangoLayoutRun *run;
      GdkRGBA color;
      int x, y;

      if (g_random_boolean ())
        pango_font_description_set_style (desc, PANGO_STYLE_ITALIC);
      else
        pango_font_description_set_style (desc, PANGO_STYLE_NORMAL);

      pango_font_description_set_weight (desc, 100 * g_random_int_range (1, 10));
      pango_font_description_set_size (desc, PANGO_SCALE * g_random_int_range (8,32));

      font = pango_context_load_font (context, desc);

      pango_layout_set_font_description (layout, desc);
      iter = pango_layout_get_iter (layout);
      do
        {
          if (g_random_boolean ())
            pango_layout_iter_next_run (iter);
          if (g_random_boolean ())
            pango_layout_iter_next_run (iter);
          run = pango_layout_iter_get_run (iter);
        }
      while (run == NULL);

      glyphs = run->glyphs;

      x = g_random_int_range (0, 1000);
      y = g_random_int_range (0, 1000);

      hsv_to_rgb (&color, g_random_double (), g_random_double_range (0.15, 0.4), g_random_double_range (0.6, 0.85));

      nodes[i] = gsk_text_node_new (font, glyphs, &color, x, y);

      pango_layout_iter_free (iter);
      g_object_unref (font);
    }

  pango_font_description_free (desc);

  container = gsk_container_node_new (nodes, n);

  for (i = 0; i < n; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

GskRenderNode *
cairo_node (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *, n);
  GskRenderNode *container;
  cairo_surface_t *surface;
  graphene_rect_t bounds;
  gint offset = 0, height, width, stride;
  guchar *buf;
  guint i;

  for (i = 0; i < n; i++)
    {
      bounds.size.width = g_random_int_range (20, 100);
      bounds.origin.x = g_random_int_range (0, 1000 - bounds.size.width);
      bounds.size.height = g_random_int_range (20, 100);
      bounds.origin.y = g_random_int_range (0, 1000 - bounds.size.height);

      height = g_random_int_range (1, 100);
      width = g_random_int_range (1, 100);
      stride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32, width + offset);

      buf = g_malloc (stride * height);

      if (i % 3 == 0)
        surface = NULL;
      else
        surface = cairo_image_surface_create_for_data (buf, CAIRO_FORMAT_ARGB32, width, height, stride);

      nodes[i] = gsk_cairo_node_new_for_surface (&bounds, surface);

      /* Offset in stride helps to test when image is not 'tightly packed' condition */
      offset = offset ? 0 : 3;

      cairo_surface_destroy (surface);
      g_free (buf);
    }

  container = gsk_container_node_new (nodes, n);

  for (i = 0; i < n; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

GskRenderNode *
box_shadows (guint n)
{
  GskRenderNode **nodes = g_newa (GskRenderNode *, n);
  GskRenderNode *container;
  int i, j;
  GskRoundedRect outline;
  GdkRGBA color;
  float dx, dy;
  float spread;
  float blur;

  for (i = 0; i < n; i++)
    {
      outline.bounds.size.width = g_random_int_range (20, 100);
      outline.bounds.origin.x = g_random_int_range (0, 1000 - outline.bounds.size.width);
      outline.bounds.size.height = g_random_int_range (20, 100);
      outline.bounds.origin.y = g_random_int_range (0, 1000 - outline.bounds.size.height);
      outline.corner[1].width = outline.corner[1].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[2].width = outline.corner[2].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      outline.corner[3].width = outline.corner[3].height = 10 - (int) sqrt (g_random_int_range (0, 100));
      for (j = 0; j < 4; j++)
        {
          outline.corner[0].width = 10 - (int) sqrt (g_random_int_range (0, 100));
          outline.corner[0].height = 10 - (int) sqrt (g_random_int_range (0, 100));
        }

      hsv_to_rgb (&color, g_random_double (), g_random_double_range (0.15, 0.4), g_random_double_range (0.6, 0.85));

      dx = g_random_double_range (0.0, 5.0);
      dy = g_random_double_range (0.0, 5.0);
      spread = g_random_double_range (0.0, 10.0);
      blur = g_random_double_range (0.0, 10.0);

      if (g_random_boolean ())
        nodes[i] = gsk_inset_shadow_node_new (&outline, &color, dx, dy, spread, blur);
      else
        nodes[i] = gsk_outset_shadow_node_new (&outline, &color, dx, dy, spread, blur);
    }

  container = gsk_container_node_new (nodes, n);

  for (i = 0; i < n; i++)
    gsk_render_node_unref (nodes[i]);

  return container;
}

int
main (int argc, char **argv)
{
  static const struct {
    const char *name;
    GskRenderNode * (* func) (guint n);
  } functions[] = {
    { "cairo.node", cairo_node },
    { "colors.node", colors },
    { "clipped-colors.node", clipped_colors },
    { "rounded-borders.node", rounded_borders },
    { "rounded-backgrounds.node", rounded_backgrounds },
    { "linear-gradient.node", linear_gradient },
    { "borders.node", borders },
    { "text.node", text },
    { "box-shadows.node", box_shadows },
  };
  GError *error = NULL;
  GskRenderNode *node;
  GPatternSpec *matcher;
  char *pattern;
  guint i, n;

  gtk_init ();

  n = 100000;
  pattern = "*";

  if (argc > 1)
    {
      if (argc > 2)
        pattern = argv[2];
      n = atoi (argv[1]);
    }

  matcher = g_pattern_spec_new (pattern);

  for (i = 0; i < G_N_ELEMENTS (functions); i++)
    {
      if (!g_pattern_match_string (matcher, functions[i].name))
        continue;

      node = functions[i].func (n);
      if (!gsk_render_node_write_to_file (node, functions[i].name, &error))
        {
          g_print ("Error writing \"%s\": %s\n", functions[i].name, error->message);
          g_clear_error (&error);
          return 1;
        }
      gsk_render_node_unref (node);
      g_print ("Created test file \"%s\".\n", functions[i].name);
    }

  g_pattern_spec_free (matcher);

  return 0;
}
