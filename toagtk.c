/* Miscellaneous GTK+/GimpUI stuff.
 * Mostly so we don't clutter up main.c
 */
#include "toagtk.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "toastring.h"

void
debug(const char * fmt, ...)
{
  char buf[2048] = "debug: NULL";
  va_list ap;
  va_start(ap, fmt);
  if (fmt) vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  gimp_message(buf);
}

/* ********************************************************************** */
/* ********************************************************************** */

/* color = 0xAARRGGBB */
void
toa_pixbuf_put_pixel(GdkPixbuf * pb, gint x, gint y, guint32 color)
{
  int stride = gdk_pixbuf_get_rowstride(pb);
  int channels = gdk_pixbuf_get_n_channels(pb);
  guchar * p = gdk_pixbuf_get_pixels(pb) + y * stride + x * channels;

  g_assert(gdk_pixbuf_get_colorspace(pb) == GDK_COLORSPACE_RGB);
  g_assert(gdk_pixbuf_get_bits_per_sample(pb) == 8);
  g_assert(channels == 3 || channels == 4);
  g_assert(x >= 0 && x < gdk_pixbuf_get_width(pb));
  g_assert(y >= 0 && y < gdk_pixbuf_get_height(pb));

  *p++ = (guchar)(color >> 16);
  *p++ = (guchar)(color >> 8);
  *p++ = (guchar)(color);
  if (channels == 4) *p = (guchar)(color >> 24);
}

GdkPixbuf *
toa_pixbuf_from_map(const double * map, gint maplen, gint size)
{
  GdkPixbuf * img = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, size, size);
  double dw = (double)(size - 1);
  double dh = (double)(size - 1);
  double mn = (double)(maplen - 1);
  gint i;
  gdk_pixbuf_fill(img, 0xFFFFFFFF); /* 0xRRGGBBAA !? */
  for (i = 0; i < maplen; ++i) {
    int x = (int)round((double)i * dw / mn);
    int y = (int)round((1.0 - map[i]) * dh);
    toa_pixbuf_put_pixel(img, x, y, 0x00000000);
  }
  return img;
}

/* ********************************************************************** */
/* ********************************************************************** */

void
toa_dialog_pack_secondary(GtkDialog * dialog, GtkWidget * child, gboolean expand, gboolean fill, guint pad)
{
  GtkWidget * acts = gtk_dialog_get_action_area(dialog);
  gtk_box_pack_start(GTK_BOX(acts), child, expand, fill, pad);
  gtk_button_box_set_child_secondary(GTK_BUTTON_BOX(acts), child, TRUE);
}

/* ********************************************************************** */
/* ********************************************************************** */

GtkWidget *
toa_scroll_text_view_new(GtkTextBuffer ** buf)
{
  GtkWidget * scroll = gtk_scrolled_window_new(NULL, NULL);
  GtkWidget * view = gtk_text_view_new();

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_ETCHED_IN);

  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 5);
  *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
  gtk_container_add(GTK_CONTAINER(scroll), view);
  gtk_widget_show(view);
  return scroll;
}

/* ********************************************************************** */
/* ********************************************************************** */

GtkWidget *
toa_scroll_label_new(const char * text, gint minW, gint minH)
{
  GtkWidget * scroll = gtk_scrolled_window_new(NULL, NULL);
  GtkWidget * label = gtk_label_new(text);

  gtk_widget_set_size_request(scroll, minW, minH);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  gtk_label_set_selectable(GTK_LABEL(label), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
  gtk_misc_set_padding(GTK_MISC(label), 6, 6);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll), label);
  gtk_viewport_set_shadow_type(GTK_VIEWPORT(gtk_bin_get_child(GTK_BIN(scroll))), GTK_SHADOW_NONE);
  gtk_widget_show(label);
  return scroll;
}

/* ********************************************************************** */
/* ********************************************************************** */

/* If icons are optional or the list may be empty, iconNone is required.
 */
GtkWidget *
toa_icon_combo_box_new(GdkPixbuf * iconNone, gint textChars)
{
  GtkListStore    * ls    = gtk_list_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
  GtkTreeModel    * model = GTK_TREE_MODEL(ls);
  GtkWidget       * w     = gtk_combo_box_new_with_model(model);
  GtkCellLayout   * cells = GTK_CELL_LAYOUT(w);
  GtkCellRenderer * icon  = gtk_cell_renderer_pixbuf_new();
  GtkCellRenderer * text  = gtk_cell_renderer_text_new();

  if (textChars >= 0) g_object_set(text, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
  g_object_set(text, "width-chars", textChars, NULL);
  gtk_cell_renderer_set_padding(text, 6, 0);

  gtk_cell_layout_pack_start(cells, icon, FALSE);
  gtk_cell_layout_pack_start(cells, text, TRUE);
  gtk_cell_layout_add_attribute(cells, icon, "pixbuf", 0);
  gtk_cell_layout_add_attribute(cells, text, "text", 1);
  g_object_set_data(G_OBJECT(w), "toa:icon-none", iconNone);
  return w;
}

int
toa_icon_combo_box_get_active(GtkWidget * w)
{
  return gtk_combo_box_get_active(GTK_COMBO_BOX(w));
}

int
toa_icon_combo_box_set_active(GtkWidget * w, int idx)
{
  /* never select an invalid row */
  GtkComboBox * box = GTK_COMBO_BOX(w);
  GtkTreeModel * model = gtk_combo_box_get_model(box);
  GtkListStore * ls = GTK_LIST_STORE(model);
  guint len = toa_list_store_get_row_count(ls);

  if (idx < 0) idx = 0;
  else if ((guint)idx >= len) idx = (gint)(len - 1);
  gtk_combo_box_set_active(box, idx);
  return idx;
}

void
toa_icon_combo_box_update(GtkWidget * w, GdkPixbuf ** icons, gchar ** texts)
{
  /* GTK+ crashes when we've selected a row that gets removed.
   * GTK+ also crashes when we try to select -1.
   *   (I'm guessing cell renderers can't handle 'no value/null'.)
   * So, we set_active(0), reuse rows, and never build an empty list.
   */
  GtkComboBox * box = GTK_COMBO_BOX(w);
  GtkTreeModel * model = gtk_combo_box_get_model(box);
  GtkListStore * ls = GTK_LIST_STORE(model);
  GdkPixbuf * iconNone = g_object_get_data(G_OBJECT(w), "toa:icon-none");
  int sel = gtk_combo_box_get_active(box);
  guint len = texts ? g_strv_length(texts) : 0;
  gboolean adjust = (sel >= 0 && (guint)sel >= len);
  GtkTreeIter iter;
  int i;

  if (adjust) gtk_combo_box_set_active(box, 0);
  toa_list_store_set_row_count(ls, len ? len : 1);

  gtk_tree_model_get_iter_first(model, &iter);
  if (!len) {
    gtk_list_store_set(ls, &iter, 0, iconNone, 1, "", -1);
    return;
  }
  for (i = 0; texts[i]; ++i) {
    GdkPixbuf * icon = icons[i] ? icons[i] : iconNone;
    gtk_list_store_set(ls, &iter, 0, icon, 1, texts[i], -1);
    gtk_tree_model_iter_next(model, &iter);
  }
  if (adjust || sel < 0)
    gtk_combo_box_set_active(box, (sel >= 0 && (guint)sel < len) ? sel : 0);
}

/* ********************************************************************** */
/* ********************************************************************** */

gchar *
toa_save_get_string(const gchar * name, const gchar * def)
{
  gchar * rv;
  gint len = gimp_get_data_size(name);
  if (len > 0) {
    rv = g_malloc((size_t)len);
    gimp_get_data(name, rv);
  } else {
    rv = def ? g_strdup(def) : NULL;
  }
  return rv;
}

void
toa_save_set_string(const gchar * name, const gchar * val)
{
  gimp_set_data(name, val, (guint)strlen(val) + 1);
}

/* ********************************************************************** */
/* ********************************************************************** */

gboolean
toa_tree_model_get_iter_nth(GtkTreeModel * tm, GtkTreeIter * iter, size_t n)
{
  if (gtk_tree_model_get_iter_first(tm, iter)) {
    do {
      if (!n--) return TRUE;
    } while (gtk_tree_model_iter_next(tm, iter));
  }
  return FALSE;
}

void
toa_list_store_set_row_count(GtkListStore * ls, size_t n)
{
  GtkTreeIter iter;
  guint ln = toa_list_store_get_row_count(ls);
  while (ln < n) { ln++; gtk_list_store_append(ls, &iter); }
  if (ln > n) {
    toa_tree_model_get_iter_nth(GTK_TREE_MODEL(ls), &iter, n);
    while (gtk_list_store_remove(ls, &iter)) /**/;
  }
}

guint
toa_list_store_get_row_count(GtkListStore * ls)
{
  GtkTreeModel * tm = GTK_TREE_MODEL(ls);
  GtkTreeIter iter;
  guint i = 0;
  if (gtk_tree_model_get_iter_first(tm, &iter)) {
    ++i;
    while (gtk_tree_model_iter_next(tm, &iter)) ++i;
  }
  return i;
}

/* ********************************************************************** */
/* ********************************************************************** */

void
toa_text_buffer_place_cursor_offset(GtkTextBuffer *buffer, int offset)
{
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_offset(buffer, &iter, offset);
  gtk_text_buffer_place_cursor(buffer, &iter);
}

void
toa_text_buffer_get_cursor_iter(GtkTextBuffer * buffer, GtkTextIter * pos)
{
  GtkTextMark * cursor = gtk_text_buffer_get_insert(buffer);
  gtk_text_buffer_get_iter_at_mark(buffer, pos, cursor);
}

void
toa_text_buffer_get_line(GtkTextIter * pos, GtkTextIter * start, GtkTextIter * end)
{
  *start = *pos;
  *end = *pos;
  gtk_text_iter_set_line_offset(start, 0);
  /* don't select two lines */
  if (!gtk_text_iter_ends_line(end)) gtk_text_iter_forward_to_line_end(end);
}
void
toa_text_buffer_get_current_line(GtkTextBuffer * buffer, GtkTextIter * start, GtkTextIter * end)
{
  toa_text_buffer_get_cursor_iter(buffer, start);
  toa_text_buffer_get_line(start, start, end);
}

gchar *
toa_text_buffer_get_line_text(GtkTextIter * pos)
{
  GtkTextBuffer * buffer = gtk_text_iter_get_buffer(pos);
  GtkTextIter start;
  GtkTextIter end;
  toa_text_buffer_get_line(pos, &start, &end);
  return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}
gchar *
toa_text_buffer_get_current_line_text(GtkTextBuffer * buffer)
{
  GtkTextIter start;
  toa_text_buffer_get_cursor_iter(buffer, &start);
  return toa_text_buffer_get_line_text(&start);
}
gchar *
toa_text_buffer_get_text(GtkTextBuffer * buffer)
{
  GtkTextIter start;
  GtkTextIter end;
  gtk_text_buffer_get_bounds(buffer, &start, &end);
  return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

void
toa_text_buffer_tag_line(GtkTextIter * pos, const char * name, gboolean add)
{
  GtkTextBuffer * buffer = gtk_text_iter_get_buffer(pos);
  GtkTextIter start;
  GtkTextIter end;
  toa_text_buffer_get_line(pos, &start, &end);
  if (!name)    gtk_text_buffer_remove_all_tags(buffer, &start, &end);
  else if (add) gtk_text_buffer_apply_tag_by_name(buffer, name, &start, &end);
  else          gtk_text_buffer_remove_tag_by_name(buffer, name, &start, &end);
}
void
toa_text_buffer_tag_current_line(GtkTextBuffer * buffer, const char * name, gboolean add)
{
  GtkTextIter start;
  toa_text_buffer_get_cursor_iter(buffer, &start);
  toa_text_buffer_tag_line(&start, name, add);
}

/* ********************************************************************** */
/* ********************************************************************** */

void
toa_text_buffer_for_each_line(GtkTextBuffer * buffer, ToaLineCallback cb, gpointer ctxt)
{
  GtkTextIter start;
  GtkTextIter end;
  gtk_text_buffer_get_start_iter(buffer, &start);
  do {
    end = start;
    if (!gtk_text_iter_ends_line(&end)) gtk_text_iter_forward_to_line_end(&end);
    cb(buffer, &start, &end, ctxt);
  } while (gtk_text_iter_forward_line(&start));
}

void
toa_text_buffer_for_lines(GtkTextBuffer * buffer, int startLine, int endLine, ToaLineCallback cb, gpointer ctxt)
{
  /* call cb with [start..end) */
  GtkTextIter start;
  GtkTextIter end;
  if (startLine < 0 || endLine < 0 || startLine == endLine) return;
  if (startLine > endLine) { int tmp = startLine; startLine = endLine; endLine = tmp; }
  gtk_text_buffer_get_iter_at_line(buffer, &start, startLine);
  do {
    end = start;
    if (!gtk_text_iter_ends_line(&end)) gtk_text_iter_forward_to_line_end(&end);
    cb(buffer, &start, &end, ctxt);
  } while (++startLine < endLine && gtk_text_iter_forward_line(&start));
}

/* ********************************************************************** */
/* ********************************************************************** */

void
toa_preview_draw_buffer(GimpPreview * preview, guchar * src, gint w, gint bpp)
{
  /* Bug in gimp_zoom_preview_draw_buffer.
   * The pixel type difference happens because
   * gimp_zoom_preview_get_source calls gimp_drawable_get_sub_thumbnail_data
   * which returns an "icon" buffer that is forced into RGB* or GRAY* but
   * gimp_zoom_preview_draw_buffer assumes the buffer's type matches
   * the priv->drawable's type.
   * So, GET-SOURCE returns RGB and DRAW-BUFFER assumes it's INDEXED.
   */
  GimpDrawable * zpd = gimp_zoom_preview_get_drawable(GIMP_ZOOM_PREVIEW(preview));
  GimpPreviewArea * area = GIMP_PREVIEW_AREA(gimp_preview_get_area(preview));
  GimpImageType type = gimp_drawable_type(zpd->drawable_id);
  gint pw = 0, ph = 0;
  gimp_preview_get_size(preview, &pw, &ph);
  if (type == GIMP_INDEXED_IMAGE && bpp == 3) type = GIMP_RGB_IMAGE;
  if (type == GIMP_INDEXEDA_IMAGE && bpp == 4) type = GIMP_RGBA_IMAGE;
  gimp_preview_area_draw(area, 0, 0, pw, ph, type, src, w * bpp);
}

/* ********************************************************************** */
/* ********************************************************************** */
