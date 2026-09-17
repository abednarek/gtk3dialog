/*
 * widget_overlay.c: GTK3 GtkOverlay XML widget.
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
#include "widget_overlay.h"

static tag_attr *widget_overlay_child_attributes(GtkWidget *widget)
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

static gboolean widget_overlay_take_pass_through(tag_attr *attr,
	gboolean fallback)
{
	gchar *value;
	gboolean result;
	if (attr == NULL ||
		(value = get_tag_attribute(attr, "pass-through")) == NULL)
		return fallback;
	if (!widget_parse_boolean(value, &result)) {
		gtkdialog_warning("Invalid overlay pass-through value '%s'.", value);
		result = fallback;
	}
	kill_tag_attribute(attr, "pass-through");
	return result;
}

static gboolean widget_overlay_child_pass_through(GtkWidget *child,
	gboolean fallback)
{
	tag_attr *attr = widget_overlay_child_attributes(child);
	const gchar *value;
	gboolean result;
	if (attr != NULL && get_tag_attribute(attr, "pass-through") != NULL)
		return widget_overlay_take_pass_through(attr, fallback);
	/* Unnamed children still carry unknown tag attributes as object data. */
	while (child != NULL) {
		value = g_object_get_data(G_OBJECT(child), "pass-through");
		if (value != NULL) {
			if (widget_parse_boolean(value, &result))
				return result;
			gtkdialog_warning("Invalid overlay pass-through value '%s'.", value);
			return fallback;
		}
		if (!GTK_IS_BIN(child))
			break;
		child = gtk_bin_get_child(GTK_BIN(child));
	}
	return fallback;
}

GtkWidget *widget_overlay_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	GtkWidget *widget;
	stackelement children;
	gboolean default_pass_through;
	gint i;
	(void)Attr;
	(void)Type;
	default_pass_through = widget_overlay_take_pass_through(attr, FALSE);
	children = pop();
	if (children.nwidgets < 1) {
		stackelement_clear(&children);
		g_error("gtkdialog: <overlay> requires a direct main child widget.");
	}
	widget = gtk_overlay_new();
	gtk_container_add(GTK_CONTAINER(widget), children.widgets[0]);
	for (i = 1; i < children.nwidgets; ++i) {
		gboolean pass_through = widget_overlay_child_pass_through(
			children.widgets[i], default_pass_through);
		gtk_overlay_add_overlay(GTK_OVERLAY(widget), children.widgets[i]);
		gtk_overlay_set_overlay_pass_through(GTK_OVERLAY(widget),
			children.widgets[i], pass_through);
	}
	stackelement_clear(&children);
	return widget;
}

gchar *widget_overlay_envvar_construct(GtkWidget *widget)
{
	(void)widget;
	return g_strdup("");
}

void widget_overlay_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	gtkdialog_warning("<fileselect> is not supported by <overlay>.");
}

void widget_overlay_refresh(variable *var)
{
	GList *element;
	gint width = -1;
	gint height = -1;
	if (GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised")))
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
		width = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_WIDTH), "overlay width");
	if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
		height = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_HEIGHT), "overlay height");
	if (width != -1 || height != -1)
		gtk_widget_set_size_request(var->Widget, width, height);
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT) ||
		attributeset_is_avail(var->Attributes, ATTR_INPUT) ||
		attributeset_is_avail(var->Attributes, ATTR_OUTPUT))
		gtkdialog_warning("<overlay> has no independent value; use its children.");
}

void widget_overlay_clear(variable *var)
{
	(void)var;
	gtkdialog_warning("<clear> is not supported by <overlay>.");
}

void widget_overlay_save(variable *var)
{
	(void)var;
	gtkdialog_warning("<save> is not supported by <overlay>.");
}

void widget_overlay_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <overlay>.");
}
