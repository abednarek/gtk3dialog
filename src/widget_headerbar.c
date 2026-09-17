/*
 * widget_headerbar.c: GTK3 GtkHeaderBar XML container.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <gtk/gtk.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_headerbar.h"

static tag_attr *widget_headerbar_child_attributes(GtkWidget *widget)
{
	variable *var;
	while (widget != NULL) {
		var = find_variable_by_widget(widget);
		if (var != NULL)
			return var->widget_tag_attr;
		if (!GTK_IS_BIN(widget))
			break;
		widget = gtk_bin_get_child(GTK_BIN(widget));
	}
	return NULL;
}

GtkWidget *widget_headerbar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	stackelement children = { 0 };
	GtkWidget *widget = gtk_header_bar_new();
	gchar *value;
	gboolean has_center = FALSE;
	gint i;
	(void)Attr;
	if (!(Type & HEADERBAR_EMPTY_CHILDREN))
		children = pop();
	if (attr != NULL) {
		if ((value = get_tag_attribute(attr, "title")) != NULL) {
			gtk_header_bar_set_title(GTK_HEADER_BAR(widget), value);
			kill_tag_attribute(attr, "title");
		}
		if ((value = get_tag_attribute(attr, "subtitle")) != NULL) {
			gtk_header_bar_set_subtitle(GTK_HEADER_BAR(widget), value);
			kill_tag_attribute(attr, "subtitle");
		}
		if ((value = get_tag_attribute(attr, "show-close-button")) != NULL) {
			gboolean enabled;
			if (widget_parse_boolean(value, &enabled))
				gtk_header_bar_set_show_close_button(
					GTK_HEADER_BAR(widget), enabled);
			else
				gtkdialog_warning("Invalid headerbar show-close-button '%s'.",
					value);
			kill_tag_attribute(attr, "show-close-button");
		}
		if ((value = get_tag_attribute(attr, "has-subtitle")) != NULL) {
			gboolean enabled;
			if (widget_parse_boolean(value, &enabled))
				gtk_header_bar_set_has_subtitle(GTK_HEADER_BAR(widget), enabled);
			else
				gtkdialog_warning("Invalid headerbar has-subtitle '%s'.", value);
			kill_tag_attribute(attr, "has-subtitle");
		}
	}
	for (i = 0; i < children.nwidgets; ++i) {
		GtkWidget *child = children.widgets[i];
		tag_attr *child_attr = widget_headerbar_child_attributes(child);
		gchar *pack = child_attr == NULL ? NULL :
			get_tag_attribute(child_attr, "headerbar-pack");
		if (pack == NULL || g_ascii_strcasecmp(pack, "start") == 0)
			gtk_header_bar_pack_start(GTK_HEADER_BAR(widget), child);
		else if (g_ascii_strcasecmp(pack, "end") == 0)
			gtk_header_bar_pack_end(GTK_HEADER_BAR(widget), child);
		else if (g_ascii_strcasecmp(pack, "center") == 0) {
			if (has_center) {
				stackelement_clear(&children);
				g_error("gtkdialog: <headerbar> accepts one center child.");
			}
			gtk_header_bar_set_custom_title(GTK_HEADER_BAR(widget), child);
			has_center = TRUE;
		} else {
			gtkdialog_warning("Invalid headerbar-pack value '%s'; using start.",
				pack);
			gtk_header_bar_pack_start(GTK_HEADER_BAR(widget), child);
		}
		if (pack != NULL)
			kill_tag_attribute(child_attr, "headerbar-pack");
	}
	stackelement_clear(&children);
	return widget;
}

gchar *widget_headerbar_envvar_construct(GtkWidget *widget)
{
	(void)widget;
	return g_strdup("");
}

void widget_headerbar_refresh(variable *var)
{
	GList *element;
	gint width = -1;
	gint height = -1;
	if (GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised")))
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
		width = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_WIDTH), "headerbar width");
	if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
		height = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_HEIGHT), "headerbar height");
	if (width != -1 || height != -1)
		gtk_widget_set_size_request(var->Widget, width, height);
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT) ||
		attributeset_is_avail(var->Attributes, ATTR_INPUT) ||
		attributeset_is_avail(var->Attributes, ATTR_OUTPUT))
		gtkdialog_warning("<headerbar> has no independent value; use its children.");
}

void widget_headerbar_save(variable *var)
{
	(void)var;
	gtkdialog_warning("<save> is not supported by <headerbar>.");
}

void widget_headerbar_clear(variable *var)
{
	(void)var;
	gtkdialog_warning("<clear> is not supported by <headerbar>.");
}

void widget_headerbar_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <headerbar>.");
}
