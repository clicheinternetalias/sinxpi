/* main.c
 * Map channel values to an expression:
 *   y=f(x) with x,y in [0,1].
 */
/* ********************************************************************** */
/* ********************************************************************** */

#define WINDOW_W   800
#define WINDOW_H   600
#define GRAPH_SIZE 160 /* size of editor graph in pixels */
#define DOC_SIZE   160 /* width of language documentation in pixels */
#define MENU_WIDTH  23 /* width of combo box menus in characters */
#define ICON_SIZE   16 /* size of combo box icons in pixels */

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "expr.h"
#include "toastring.h"
#include "toagtk.h"
#include "toaeditor.h"

#define UNUSED(x) ((x) = (x))

static GimpPDBStatusType g_status = GIMP_PDB_SUCCESS;

/* ********************************************************************** */
/* ********************************************************************** */

static const char * g_editor_docs_heads[] = {
  "Variables:\n",
  "\n\nConstants:\n",
  "\n\nFunctions:\n",
  "\n\nOperators:\n"
};
static const char * g_editor_docs_entry = "\n%s %s";
static const char * g_editor_docs_tail = "";

static char * g_editor_docs = NULL;

static struct {
  char * tok;
  char * note;
} g_editor_docs_toks[] = {
#undef COMMA
#define SYMBOL(ENUM,PREC,TOK,ARGC,DOC,EVAL)  { TOK , DOC },
#undef LIMIT
#include "expr-optab.inc"
  { NULL, NULL }
};

static void
g_editor_docs_init(void)
{
  GString * s = g_string_new(NULL);
  int i;
  int head = 0;

  for (i = 0; g_editor_docs_toks[i].tok; ++i) {
    char * tok = g_editor_docs_toks[i].tok;
    char * note = g_editor_docs_toks[i].note;
    if (head == 0 || (head == 1 && g_ascii_isupper(tok[0])) ||
                     (head == 2 && g_ascii_islower(tok[0])) ||
                     (head == 3 && g_ascii_ispunct(tok[0])))
      g_string_append(s, g_editor_docs_heads[head++]);
    if (note)
      g_string_append_printf(s, g_editor_docs_entry, tok, note);
  }
  g_string_append(s, g_editor_docs_tail);
  g_editor_docs = g_string_free(s, FALSE);
}

static void
g_editor_docs_destroy(void)
{
  if (g_editor_docs) g_free(g_editor_docs);
}

/* ********************************************************************** */
/* ********************************************************************** */

static char * g_exprs_defs = NULL;
static char * g_exprs_defs_array[] = { /* avoid 509 limit on literals */
#define EXPRLIT(ex)  ex ,
#include "expr-defs.inc"
#undef EXPRLIT
    NULL
};

static char ** g_exprs = NULL;

static void
exprs_set(const char * data)
{
  char ** d = toa_strparse(data);
  if (d) {
    if (g_exprs) g_strfreev(g_exprs);
    g_exprs = d;
    toa_save_set_string("plug-in-sinxpi-exprs", data);
  }
}

static void
exprs_init(void)
{
  char * data;
  g_exprs_defs = g_strjoinv("\n", g_exprs_defs_array);
  data = toa_save_get_string("plug-in-sinxpi-exprs", g_exprs_defs);
  g_exprs = toa_strparse(data);
  g_free(data);
}

static void
exprs_destroy(void)
{
  if (g_exprs) g_strfreev(g_exprs);
  if (g_exprs_defs) g_free(g_exprs_defs);
}

/* ********************************************************************** */
/* ********************************************************************** */

static struct exprmap_s {
  guchar r[256];
  guchar g[256];
  guchar b[256];
} g_map;

/* can't use indexes into g_exprs because the caller can non-interactively
 * specify any expression
 */
static struct expr_s {
  gchar * r;
  gchar * g;
  gchar * b;
} g_expr;

static void
expr_error_handle(const char * s, void * ctxt)
{
  if (ctxt) *(char**)ctxt = g_strdup(s);
  else g_message("%s", s);
}

static void
expr_init(void)
{
  g_expr.r = g_strdup(g_exprs[0]);
  g_expr.g = g_strdup(g_exprs[0]);
  g_expr.b = g_strdup(g_exprs[0]);
}

static void
expr_destroy(void)
{
  g_free(g_expr.r);
  g_free(g_expr.g);
  g_free(g_expr.b);
}

static void
expr_set(int which, gchar * ex)
{
  gchar ** p;
  switch (which) {
    case 'r': case 0: p = &(g_expr.r); break;
    case 'g': case 1: p = &(g_expr.g); break;
    case 'b': case 2: p = &(g_expr.b); break;
    default: return;
  }
  if (*p) g_free(*p);
  *p = g_strdup(ex);
}

#define expr_mapfloat(MAP,SRC,ERR) expr_map0(MAP, TRUE, SRC, ERR)
#define expr_mapbyte(MAP,SRC,ERR)  expr_map0(MAP, FALSE, SRC, ERR)
static gboolean
expr_map0(void * map, gboolean isFloat, const char * src, char ** err)
{
  double * mapf = isFloat ? map : NULL;
  guchar * mapb = isFloat ? NULL : map;
  int i;
  EXPR * ex;
  expr_set_error_handler(&expr_error_handle, (void*)err);
  ex = expr_new(src);
  for (i = 0; i <= 255; ++i) {
    double rv = ((double)i) / 255.0;
    if (ex) expr_eval(ex, rv, &rv);
    rv = isnan(rv) ? 0.0 : (rv < 0.0) ? 0.0 : (rv > 1.0) ? 1.0 : rv;
    if (mapf) mapf[i] = rv;
    else      mapb[i] = (guchar)(rv * 255.0);
  }
  expr_delete(ex);
  return ex != NULL;
}

static gboolean
expr_buildmap(void)
{
  gboolean r = expr_mapbyte(g_map.r, g_expr.r, NULL);
  gboolean g = expr_mapbyte(g_map.g, g_expr.g, NULL);
  gboolean b = expr_mapbyte(g_map.b, g_expr.b, NULL);
  return r && g && b;
}

static GdkPixbuf *
expr_pixbuf(const char * src, gint size, char ** err)
{
  double map[256];
  expr_mapfloat(map, src, err);
  return toa_pixbuf_from_map(map, 256, size);
}

/* ********************************************************************** */
/* ********************************************************************** */

/* Perform the pixel munging on the drawable.
 * This is where Photo Finish captures the Magics.
 */

static void
filter_preview_channels(GimpPreview * preview)
{
  gint x, y, w = 0, h = 0, bpp = 0;
  guchar * src = gimp_zoom_preview_get_source(GIMP_ZOOM_PREVIEW(preview), &w, &h, &bpp);
  guchar * p = src;

  for (y = 0; y < h; ++y) {
    for (x = 0; x < w; ++x) {
      if (bpp >= 3) {
        p[0] = g_map.r[p[0]];
        p[1] = g_map.g[p[1]];
        p[2] = g_map.b[p[2]];
      } else {
        p[0] = g_map.r[p[0]];
      }
      p += bpp;
    }
  }
  toa_preview_draw_buffer(preview, src, w, bpp);
  g_free(src);
}

#if 0
/* because of a bug in gimp_preview_draw_buffer, this function is unused,
 * but it works (except for the part where gimp_preview_draw also has the bug)
 */
static void
filter_preview_indexed(GimpPreview * preview)
{
  GimpPreviewArea * area = GIMP_PREVIEW_AREA(gimp_preview_get_area(preview));
  GimpDrawable * zpd = gimp_zoom_preview_get_drawable(GIMP_ZOOM_PREVIEW(preview));
  gint32 img_id = gimp_drawable_get_image(zpd->drawable_id);
  gint maplen = 0;
  guchar * map = gimp_image_get_colormap(img_id, &maplen);
  gint i = 0;

  while (i < maplen * 3) {
    map[i] = g_map.r[map[i]]; ++i;
    map[i] = g_map.g[map[i]]; ++i;
    map[i] = g_map.b[map[i]]; ++i;
  }
  gimp_preview_area_set_colormap(area, map, maplen);
  gimp_preview_draw(preview);
  /* invalidate goes into a repeating event loop; use draw instead */
  /*gimp_preview_invalidate(preview);*/
}
#endif

static void
filter_indexed(GimpDrawable * drawable, gint x, gint y, gint w, gint h,
               gboolean hasDisplay)
{
  gint32 img_id = gimp_drawable_get_image(drawable->drawable_id);
  gint maplen = 0;
  guchar * map = gimp_image_get_colormap(img_id, &maplen);
  gint i = 0;

  while (i < maplen * 3) {
    map[i] = g_map.r[map[i]]; ++i;
    map[i] = g_map.g[map[i]]; ++i;
    map[i] = g_map.b[map[i]]; ++i;
  }
  /* the undo wrapper forces the history to name it "SinXPI" instead of "Set Colormap". */
  gimp_image_undo_group_start(img_id);
  gimp_image_set_colormap(img_id, map, maplen);
  gimp_drawable_update(drawable->drawable_id, x, y, w, h);
  gimp_image_undo_group_end(img_id);

  if (hasDisplay) {
    gimp_displays_flush();
  }
}

static void
filter_channels(GimpDrawable * drawable, gint x, gint y, gint w, gint h,
                gboolean hasDisplay)
{
  GimpPixelRgn srcRgn;
  GimpPixelRgn dstRgn;
  gpointer pr;
  gint ix, iy;
  gdouble progress = 0;
  gdouble maxProgress = (gdouble)w * (gdouble)h;
  gboolean rgb = gimp_drawable_is_rgb(drawable->drawable_id);
  gboolean alpha = gimp_drawable_has_alpha(drawable->drawable_id);

  if (hasDisplay) {
    gimp_progress_init("SinXPI Processing...");
  }
  gimp_pixel_rgn_init(&srcRgn, drawable, x, y, w, h, FALSE, FALSE);
  gimp_pixel_rgn_init(&dstRgn, drawable, x, y, w, h, TRUE,  TRUE);

  for (pr = gimp_pixel_rgns_register(2, &srcRgn, &dstRgn);
       pr;
       pr = gimp_pixel_rgns_process(pr)) { /* for each region in the image */
    guchar * srcRow = srcRgn.data;
    guchar * dstRow = dstRgn.data;
    for (iy = 0; iy < srcRgn.h; ++iy) { /* for each row in the region */
      guchar * s = srcRow;
      guchar * d = dstRow;
      for (ix = 0; ix < srcRgn.w; ++ix) { /* for each pixel in the row */
        if (rgb) {
          d[0] = g_map.r[s[0]];
          d[1] = g_map.g[s[1]];
          d[2] = g_map.b[s[2]];
          if (alpha) d[3] = s[3];
        } else {
          d[0] = g_map.r[s[0]];
          if (alpha) d[1] = s[1];
        }
        s += srcRgn.bpp; /* next pixel */
        d += dstRgn.bpp;
      }
      srcRow += srcRgn.rowstride; /* next row */
      dstRow += dstRgn.rowstride;
    }
    if (hasDisplay) {
      progress += (gdouble)srcRgn.w * (gdouble)srcRgn.h;
      gimp_progress_update(progress / maxProgress);
    }
  }
  gimp_drawable_flush(drawable);
  gimp_drawable_merge_shadow(drawable->drawable_id, TRUE);
  gimp_drawable_update(drawable->drawable_id, x, y, w, h);
  if (hasDisplay) {
    gimp_progress_end();
    gimp_displays_flush();
  }
}

static void
filterImage(GimpDrawable * drawable, gboolean hasDisplay)
{
  gint x = 0, y = 0, w = 0, h = 0;
  if (!expr_buildmap()) { /* don't mod image on expr error */
    g_status = GIMP_PDB_EXECUTION_ERROR;
    return;
  }
  if (gimp_drawable_mask_intersect(drawable->drawable_id, &x, &y, &w, &h)) {
    if (gimp_drawable_is_indexed(drawable->drawable_id)) {
      filter_indexed(drawable, x, y, w, h, hasDisplay);
    } else {
      filter_channels(drawable, x, y, w, h, hasDisplay);
    }
  }
}

static void
preview_cb(GimpPreview * preview, GimpDrawable * drawable)
{
  UNUSED(drawable);
  expr_buildmap(); /* don't fail this; always update the preview */
  filter_preview_channels(preview); /* no indexed variant of preview */
}

/* ********************************************************************** */
/* ********************************************************************** */

static void
update_menus(GtkWidget ** menus)
{
  guint len = g_strv_length(g_exprs);
  GdkPixbuf ** icons = g_new(GdkPixbuf*, len);
  int i;
  for (i = 0; g_exprs[i]; ++i)
    icons[i] = expr_pixbuf(g_exprs[i], ICON_SIZE, NULL);
  toa_icon_combo_box_update(menus[0], icons, g_exprs);
  toa_icon_combo_box_update(menus[1], icons, g_exprs);
  toa_icon_combo_box_update(menus[2], icons, g_exprs);
  g_free(icons);
}

/* ********************************************************************** */
/* ********************************************************************** */

static GtkTextBuffer * ed_buffer = NULL;
static GtkImage      * ed_graph  = NULL;
static GtkLabel      * ed_error  = NULL;

static void
update_display_for_expr(GtkTextIter * line)
{
  gchar * expr = line ? toa_text_buffer_get_line_text(line)
                      : toa_text_buffer_get_current_line_text(ed_buffer);
  char * err = NULL;
  /* TODO: stop building an image when we know we wont need it */
  GdkPixbuf * img = expr_pixbuf(expr, GRAPH_SIZE, &err);
  if (line) { /* not the current line; settle for marking the buffer */
    toa_text_buffer_tag_line(line, "expr-error", err != NULL);
    g_object_unref(G_OBJECT(img));
  } else { /* using current line; update everything */
    toa_text_buffer_tag_current_line(ed_buffer, "expr-error", err != NULL);
    gtk_image_set_from_pixbuf(ed_graph, img);
    gtk_label_set_text(ed_error, err ? err : "");
  }
  if (err) g_free(err);
  g_free(expr);
}

static void
editor_restore_cb(GtkWidget * button, gpointer data)
{
  gtk_text_buffer_set_text(ed_buffer, g_exprs_defs, -1);
  toa_text_buffer_place_cursor_offset(ed_buffer, 0);
  UNUSED(button);
  UNUSED(data);
}

static void
editor_cursor_cb(GObject * obj, GParamSpec * pspec, gpointer data)
{
  update_display_for_expr(NULL);
  UNUSED(obj);
  UNUSED(pspec);
  UNUSED(data);
}

static void
editor_tag_line_cb(GtkTextBuffer * buf, GtkTextIter * start, GtkTextIter * end, gpointer data)
{
  update_display_for_expr(start);
  UNUSED(buf);
  UNUSED(end);
  UNUSED(data);
}

static int change_start = -1;
static int change_end = -1;

static void
editor_insert_cb(GtkTextBuffer * buf, GtkTextIter * start, gchar * text, gint textlen, gpointer data)
{
  /* We need to wait until the text is inserted before updating,
   * but the changed event doesn't know the modified range.
   */
  change_start = gtk_text_iter_get_line(start);
  change_end = change_start + (int)toa_strcnt(text, '\n');
  UNUSED(buf);
  UNUSED(textlen);
  UNUSED(data);
}
static void
editor_changed_cb(GtkTextBuffer * buf, gpointer data)
{
  /* for_lines is [start..end) which is okay;
   * the cursor event will handle the line when start == end
   */
  toa_text_buffer_for_lines(ed_buffer, change_start, change_end, &editor_tag_line_cb, NULL);
  change_start = change_end = -1;
  UNUSED(buf);
  UNUSED(data);
}

/* <dialog button="restore">
 *   <hbox>
 *     <textview/>
 *     <vbox>
 *       <pix graph/>
 *       <label docs/>
 *     </vbox>
 *   </hbox>
 *   <label error/>
 * </dialog>
 */
static void
edit_cb(GtkWidget * widget, GtkWidget ** menus)
{
  GtkWidget * parent = menus[4];
  GtkWidget * dialog;
  GtkWidget * content;
  GtkWidget * btnRestore;
  GtkWidget * hbox;
  GtkWidget * scroll;
  GtkWidget * vboxSide;
  GtkWidget * docs;
  gchar * text;
  gboolean run;

  UNUSED(widget);

  dialog = gimp_dialog_new(
    "SinXPI Expression Editor", "sinxpi-editor", NULL, 0,
    NULL, "plug-in-sinxpi",
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    GTK_STOCK_OK,     GTK_RESPONSE_OK,
    NULL);
  gtk_window_set_default_size(GTK_WINDOW(dialog), WINDOW_W, WINDOW_H);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));

  content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  /* hbox */
  hbox = gtk_hbox_new(FALSE, 3);
  gtk_box_pack_start(GTK_BOX(content), hbox, TRUE, TRUE, 0);
  gtk_widget_show(hbox);

  /* text view */
  scroll = toa_editor_new(&ed_buffer);
  gtk_text_buffer_create_tag(ed_buffer, "expr-error", "foreground", "#f00", NULL);
  gtk_box_pack_start(GTK_BOX(hbox), scroll, TRUE, TRUE, 0);
  gtk_widget_show(scroll);
  g_signal_connect(ed_buffer, "notify::cursor-position", G_CALLBACK(editor_cursor_cb), NULL);
  g_signal_connect(ed_buffer, "insert-text", G_CALLBACK(editor_insert_cb), NULL);
  g_signal_connect(ed_buffer, "changed", G_CALLBACK(editor_changed_cb), NULL);

  /* sidebar */
  vboxSide = gtk_vbox_new(FALSE, 3);
  gtk_box_pack_end(GTK_BOX(hbox), vboxSide, FALSE, TRUE, 0);
  gtk_widget_show(vboxSide);

  /* preview graph */
  ed_graph = GTK_IMAGE(gtk_image_new());
  gtk_box_pack_start(GTK_BOX(vboxSide), GTK_WIDGET(ed_graph), FALSE, FALSE, 0);
  gtk_widget_show(GTK_WIDGET(ed_graph));

  /* documentation */
  docs = toa_scroll_label_new(g_editor_docs, DOC_SIZE, -1);
  gtk_box_pack_start(GTK_BOX(vboxSide), docs, TRUE, TRUE, 0);
  gtk_widget_show(docs);

  /* error message */
  ed_error = GTK_LABEL(gtk_label_new(""));
  gtk_label_set_selectable(ed_error, TRUE);
  gtk_misc_set_alignment(GTK_MISC(ed_error), 0.0, -1);
  gtk_misc_set_padding(GTK_MISC(ed_error), 6, 6);
  gtk_box_pack_end(GTK_BOX(content), GTK_WIDGET(ed_error), FALSE, TRUE, 0);
  gtk_widget_show(GTK_WIDGET(ed_error));

  /* restore */
  btnRestore = gtk_button_new_with_mnemonic("_Defaults");
  toa_dialog_pack_secondary(GTK_DIALOG(dialog), btnRestore, FALSE, TRUE, 0);
  gtk_widget_show(btnRestore);
  g_signal_connect(btnRestore, "clicked", G_CALLBACK(editor_restore_cb), NULL);

  /* load the expressions */
  text = toa_save_get_string("plug-in-sinxpi-exprs", g_exprs_defs);
  gtk_text_buffer_set_text(ed_buffer, text, -1);
  toa_text_buffer_place_cursor_offset(ed_buffer, 0);
  g_free(text);

  gtk_widget_show(dialog);
  run = (gimp_dialog_run(GIMP_DIALOG(dialog)) == GTK_RESPONSE_OK);
  if (run) {
    text = toa_text_buffer_get_text(ed_buffer);
    exprs_set(text);
    g_free(text);
    update_menus(menus);
  }
  gtk_widget_destroy(dialog);
}

/* ********************************************************************** */
/* ********************************************************************** */

static gboolean g_locked = TRUE;
static gboolean g_lock_updating = FALSE;

static void
lockrgb_cb(GtkWidget * widget, GtkWidget ** menus)
{
  g_locked = (GTK_TOGGLE_BUTTON(widget)->active != FALSE);
  if (g_locked) {
    gint idx = toa_icon_combo_box_get_active(menus[0]);
    g_lock_updating = TRUE;
    toa_icon_combo_box_set_active(menus[1], idx);
    toa_icon_combo_box_set_active(menus[2], idx);
    g_lock_updating = FALSE;
  }
}

static void
menu_cb(GtkWidget * widget, GtkWidget ** menus)
{
  char * expr = g_exprs[toa_icon_combo_box_get_active(widget)];
  int i;
  int us = 0;
  for (i = 0; i < 3; ++i) {
    if (menus[i] == widget) { us = i; break; }
  }
  expr_set(us, expr ? expr : "x");
  if (g_locked && !g_lock_updating) {
    gint idx = toa_icon_combo_box_get_active(widget);
    g_lock_updating = TRUE;
    for (i = 0; i < 3; ++i) {
      if (i != us) toa_icon_combo_box_set_active(menus[i], idx);
    }
    g_lock_updating = FALSE;
  }
  gimp_preview_invalidate(GIMP_PREVIEW(menus[3]));
}

/* <dialog>
 *   <hbox>
 *     <preview/>
 *     <vbox>
 *       <align valign=middle>
 *         <table>
 *           <tr><label R/><menu red expr/></tr>
 *           <tr><label G/><menu green expr/></tr>
 *           <tr><label B/><menu blue expr/></tr>
 *           <tr><checkbox rgb locked/></tr>
 *         </table>
 *       </align>
 *       <button manage expressions.../>
 *     </vbox>
 *   </hbox>
 * </dialog>
 */
static gboolean
doDialog(GimpDrawable * drawable)
{
  GtkWidget * menus[5];
  GtkWidget * dialog;
  GtkWidget * content;
  GtkWidget * hbox;
  GtkWidget * preview;
  GtkWidget * vboxSide;
  GtkWidget * align;
  GtkWidget * table;
  GtkWidget * labels[3];
  GtkWidget * checkLocked;
  GtkWidget * btnEdit;
  gboolean run;
  gboolean gray = gimp_drawable_is_gray(drawable->drawable_id);
  guint i;

  gimp_ui_init("sinxpi", FALSE);

  /* dialog */
  dialog = menus[4] = gimp_dialog_new(
    "SinXPI", "sinxpi", NULL, 0,
    NULL, "plug-in-sinxpi",
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    GTK_STOCK_OK,     GTK_RESPONSE_OK,
    NULL);
  gtk_window_set_default_size(GTK_WINDOW(dialog), WINDOW_W, WINDOW_H);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gimp_window_set_transient(GTK_WINDOW(dialog));

  content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  /* hbox */
  hbox = gtk_hbox_new(FALSE, 6);
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 6);
  gtk_box_pack_start(GTK_BOX(content), hbox, TRUE, TRUE, 0);
  gtk_widget_show(hbox);

  /* preview */
  preview = gimp_zoom_preview_new(drawable);
  gimp_preview_set_bounds(GIMP_PREVIEW(preview), 0, 0, 1024, 1024);
  gtk_box_pack_start(GTK_BOX(hbox), preview, TRUE, TRUE, 0);
  gtk_widget_show(preview);
  g_signal_connect(preview, "invalidated", G_CALLBACK(preview_cb), drawable);
  preview_cb(GIMP_PREVIEW(preview), drawable);

  /* sidebar */
  vboxSide = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), vboxSide, FALSE, FALSE, 0);
  gtk_widget_show(vboxSide);

  /* align */
  align = gtk_alignment_new(0.0, 0.5, 1.0, 0.0);
  gtk_box_pack_start(GTK_BOX(vboxSide), align, TRUE, TRUE, 0);
  gtk_widget_show(align);

  /* menus and labels */
  {
    char * names[3] = { "_R", "_G", "_B" };
    for (i = 0; i < 3; ++i) {
      menus[i] = toa_icon_combo_box_new(NULL, MENU_WIDTH);
      g_signal_connect(menus[i], "changed", G_CALLBACK(menu_cb), menus);
      labels[i] = gtk_label_new_with_mnemonic(names[i]);
      gtk_label_set_mnemonic_widget(GTK_LABEL(labels[i]), menus[i]);
    }
  }
  menus[3] = preview;
  update_menus(menus);
  menu_cb(menus[0], menus);
  menu_cb(menus[1], menus);
  menu_cb(menus[2], menus);

  /* table: menus, labels, checkbox */
  table = gtk_table_new(4, 2, FALSE);
  gtk_table_set_col_spacings(GTK_TABLE(table), 6);
  for (i = 0; i < 3; ++i) {
    gtk_table_attach(GTK_TABLE(table), labels[i], 0, 1, i, i + 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(table), menus[i],  1, 2, i, i + 1, GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_widget_show(labels[i]);
    gtk_widget_show(menus[i]);
  }
  checkLocked = gtk_check_button_new_with_mnemonic("_Lock RGB Expressions");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkLocked), gray || g_locked);
  gtk_widget_set_sensitive(checkLocked, !gray);
  gtk_table_attach(GTK_TABLE(table), checkLocked,  0, 2, 3, 4, GTK_EXPAND, GTK_SHRINK, 0, 0);
  gtk_widget_show(checkLocked);
  g_signal_connect(checkLocked, "toggled", G_CALLBACK(lockrgb_cb), menus);

  gtk_container_add(GTK_CONTAINER(align), table);
  gtk_widget_show(table);

  /* button edit */
  btnEdit = gtk_button_new_with_mnemonic("_Manage Expressions...");
  gtk_box_pack_end(GTK_BOX(vboxSide), btnEdit, FALSE, FALSE, 0);
  gtk_widget_show(btnEdit);
  g_signal_connect(btnEdit, "clicked", G_CALLBACK(edit_cb), menus);

  gtk_widget_show(dialog);
  run = (gimp_dialog_run(GIMP_DIALOG(dialog)) == GTK_RESPONSE_OK);
  gtk_widget_destroy(dialog);
  return run;
}

static void
run(const gchar * name,
    gint nparams, const GimpParam * param,
    gint * nreturn_vals, GimpParam ** return_vals)
{
  GimpDrawable * drawable = gimp_drawable_get(param[2].data.d_drawable);

  /*if (!freopen("/home/username/work/sinxpi-gimp/run-errs.txt", "w", stderr)) debug("no freopen");*/

  exprs_init();
  expr_init();
  g_editor_docs_init();

  UNUSED(name);
  UNUSED(nparams);

  switch (param[0].data.d_int32) { /* run mode */
    case GIMP_RUN_INTERACTIVE:
      if (doDialog(drawable)) {
        toa_save_set_string("plug-in-sinxpi-expr-r", g_expr.r);
        toa_save_set_string("plug-in-sinxpi-expr-g", g_expr.g);
        toa_save_set_string("plug-in-sinxpi-expr-b", g_expr.b);
        filterImage(drawable, TRUE);
      }
      break;
    case GIMP_RUN_WITH_LAST_VALS: {
        gchar * r = toa_save_get_string("plug-in-sinxpi-expr-r", "x");
        gchar * g = toa_save_get_string("plug-in-sinxpi-expr-g", "x");
        gchar * b = toa_save_get_string("plug-in-sinxpi-expr-b", "x");
        expr_set('r', r);
        expr_set('g', g);
        expr_set('b', b);
        g_free(r);
        g_free(g);
        g_free(b);
        filterImage(drawable, TRUE);
      }
      break;
    case GIMP_RUN_NONINTERACTIVE: {
        gchar * r = param[3].data.d_string;
        gchar * g = param[4].data.d_string;
        gchar * b = param[5].data.d_string;
        expr_set('r', r && *r ? r : "x");
        expr_set('g', g && *g ? g : "x");
        expr_set('b', b && *b ? b : "x");
        filterImage(drawable, FALSE);
      }
      break;
  }

  g_editor_docs_destroy();
  expr_destroy();
  exprs_destroy();
  gimp_drawable_detach(drawable);

  /* Return value */
  {
    static GimpParam values[1];
    *nreturn_vals = 1;
    *return_vals = values;
    values[0].type = GIMP_PDB_STATUS;
    values[0].data.d_status = g_status;
  }
}

/* ********************************************************************** */
/* ********************************************************************** */

static void
query(void)
{
  static GimpParamDef args[] = {
    { GIMP_PDB_INT32,    "run-mode", "Run mode" },
    { GIMP_PDB_IMAGE,    "image",    "Input image (unused)" },
    { GIMP_PDB_DRAWABLE, "drawable", "Input drawable" },
    { GIMP_PDB_STRING,   "exprR",    "Red Channel Expression" },
    { GIMP_PDB_STRING,   "exprG",    "Green Channel Expression" },
    { GIMP_PDB_STRING,   "exprB",    "Blue Channel Expression" }
  };
  gimp_install_procedure(
    "plug-in-sinxpi", /* name */
    "Map RGB values to an expression.", /* blurb */
    "Map RGB values to an expression: y=f(x) with x,y in [0..1].", /* help */
    "the other anonymous", /* author */
    "Public Domain", /* copyright */
    "Chaos 3180", /* date */
    "Sin_XPI...", /* menu label */
    "RGB*,GRAY*,INDEXED*", /* image types */
    GIMP_PLUGIN, /* proc type */
    G_N_ELEMENTS(args), /* parameter count */
    0, /* return value count */
    args, /* parameter defs */
    NULL); /* return value defs */

  gimp_plugin_menu_register("plug-in-sinxpi", "<Image>/Colors");
}

GimpPlugInInfo PLUG_IN_INFO = {
  NULL,  /* on GIMP startup */
  NULL,  /* on GIMP shutdown */
  query, /* register GIMP UI hooks (only when plugin modified) */
  run    /* dialog and exec an expression */
};

MAIN()

/* ********************************************************************** */
/* ********************************************************************** */
