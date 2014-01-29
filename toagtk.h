#ifndef _TOAGTK_H_
#define _TOAGTK_H_ 1

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

extern void debug(const char * fmt, ...);

extern gchar * toa_save_get_string(const gchar * name, const gchar * def);
extern void    toa_save_set_string(const gchar * name, const gchar * val);

extern void        toa_pixbuf_put_pixel(GdkPixbuf * pb, gint x, gint y, guint32 color);
extern GdkPixbuf * toa_pixbuf_from_map(const double * map, gint maplen, gint size);

extern void toa_dialog_pack_secondary(GtkDialog * dialog, GtkWidget * child, gboolean expand, gboolean fill, guint pad);

extern GtkWidget * toa_scroll_label_new(const char * text, gint minW, gint minH);
extern GtkWidget * toa_scroll_text_view_new(GtkTextBuffer ** buf);

extern GtkWidget * toa_icon_combo_box_new(GdkPixbuf * iconNone, gint textChars);
extern int         toa_icon_combo_box_get_active(GtkWidget * w);
extern int         toa_icon_combo_box_set_active(GtkWidget * w, int idx);
extern void        toa_icon_combo_box_update(GtkWidget * w, GdkPixbuf ** icons, gchar ** texts);

extern gboolean toa_tree_model_get_iter_nth(GtkTreeModel * tm, GtkTreeIter * iter, size_t n);
extern void     toa_list_store_set_row_count(GtkListStore * ls, size_t n);
extern guint    toa_list_store_get_row_count(GtkListStore * ls);

extern void    toa_text_buffer_place_cursor_offset(GtkTextBuffer *buffer, int offset);
extern void    toa_text_buffer_get_cursor_iter(GtkTextBuffer * buffer, GtkTextIter * pos);
extern void    toa_text_buffer_get_line(GtkTextIter * pos, GtkTextIter * start, GtkTextIter * end);
extern void    toa_text_buffer_get_current_line(GtkTextBuffer * buffer, GtkTextIter * start, GtkTextIter * end);
extern gchar * toa_text_buffer_get_line_text(GtkTextIter * pos);
extern gchar * toa_text_buffer_get_current_line_text(GtkTextBuffer * buffer);
extern gchar * toa_text_buffer_get_text(GtkTextBuffer * buffer);
extern void    toa_text_buffer_tag_line(GtkTextIter * pos, const char * name, gboolean add);
extern void    toa_text_buffer_tag_current_line(GtkTextBuffer * buffer, const char * name, gboolean add);

typedef void (*ToaLineCallback)(GtkTextBuffer*, GtkTextIter*, GtkTextIter*, gpointer);
extern void toa_text_buffer_for_each_line(GtkTextBuffer * buffer, ToaLineCallback cb, gpointer ctxt);
extern void toa_text_buffer_for_lines(GtkTextBuffer * buffer, int startLine, int endLine, ToaLineCallback cb, gpointer ctxt);

extern void toa_preview_draw_buffer(GimpPreview * preview, guchar * src, gint w, gint bpp);

#endif /* _TOAGTK_H_ */
