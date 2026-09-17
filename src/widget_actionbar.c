/*
 * widget_actionbar.c: GTK3 GtkActionBar XML container.
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
#include "widget_actionbar.h"

static tag_attr *widget_actionbar_child_attributes(GtkWidget *widget)
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

GtkWidget *widget_actionbar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	stackelement children = pop();
	GtkWidget *widget = gtk_action_bar_new();
	gboolean has_center = FALSE;
	gint i;
	(void)Attr;
	(void)attr;
	(void)Type;
	for (i = 0; i < children.nwidgets; ++i) {
		GtkWidget *child = children.widgets[i];
		tag_attr *child_attr = widget_actionbar_child_attributes(child);
		gchar *pack = child_attr == NULL ? NULL :
			get_tag_attribute(child_attr, "actionbar-pack");
		if (pack == NULL || g_ascii_strcasecmp(pack, "start") == 0)
			gtk_action_bar_pack_start(GTK_ACTION_BAR(widget), child);
		else if (g_ascii_strcasecmp(pack, "end") == 0)
			gtk_action_bar_pack_end(GTK_ACTION_BAR(widget), child);
		else if (g_ascii_strcasecmp(pack, "center") == 0) {
			if (has_center) {
				stackelement_clear(&children);
				g_error("gtkdialog: <actionbar> accepts one center child.");
			}
			gtk_action_bar_set_center_widget(GTK_ACTION_BAR(widget), child);
			has_center = TRUE;
		} else {
			gtkdialog_warning("Invalid actionbar-pack value '%s'; using start.",
				pack);
			gtk_action_bar_pack_start(GTK_ACTION_BAR(widget), child);
		}
		if (pack != NULL)
			kill_tag_attribute(child_attr, "actionbar-pack");
	}
	stackelement_clear(&children);
	return widget;
}

gchar *widget_actionbar_envvar_construct(GtkWidget *widget)
{
	(void)widget;
	return g_strdup("");
}

void widget_actionbar_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	gtkdialog_warning("<fileselect> is not supported by <actionbar>.");
}

void widget_actionbar_refresh(variable *var)
{
	GList *element;
	gint width = -1;
	gint height = -1;
	if (GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised")))
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
		width = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_WIDTH), "actionbar width");
	if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
		height = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_HEIGHT), "actionbar height");
	if (width != -1 || height != -1)
		gtk_widget_set_size_request(var->Widget, width, height);
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT) ||
		attributeset_is_avail(var->Attributes, ATTR_INPUT) ||
		attributeset_is_avail(var->Attributes, ATTR_OUTPUT))
		gtkdialog_warning("<actionbar> has no independent value; use its children.");
}

void widget_actionbar_save(variable *var)
{
	(void)var;
	gtkdialog_warning("<save> is not supported by <actionbar>.");
}

void widget_actionbar_clear(variable *var)
{
	(void)var;
	gtkdialog_warning("<clear> is not supported by <actionbar>.");
}

void widget_actionbar_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <actionbar>.");
}
