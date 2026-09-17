/*
 * widget_appchooser.c: embedded GTK3 application chooser.
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
#include "signals.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_appchooser.h"
#include "widget_appchooser_common.h"

static void widget_appchooser_selected(GtkWidget *widget, GAppInfo *app,
	AttributeSet *Attr)
{
	(void)app;
	widget_signal_executor(widget, Attr, "application-selected");
}

static void widget_appchooser_activated(GtkWidget *widget, GAppInfo *app,
	AttributeSet *Attr)
{
	(void)app;
	widget_signal_executor(widget, Attr, "application-activated");
}

GtkWidget *widget_appchooser_create(
	AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkWidget *widget;
	gchar *content_type = NULL;

	(void)Attr;
	(void)Type;
	if (attr != NULL)
		content_type = get_tag_attribute(attr, "content-type");
	if (content_type == NULL || content_type[0] == '\0') {
		gtkdialog_warning("App chooser needs a content-type; using text/plain.");
		content_type = "text/plain";
	}
	widget = gtk_app_chooser_widget_new(content_type);
	if (attr != NULL)
		kill_tag_attribute(attr, "content-type");
	return widget;
}

gchar *widget_appchooser_envvar_construct(GtkWidget *widget)
{
	return widget_appchooser_selected_id(widget);
}

void widget_appchooser_fileselect(
	variable *var, const char *name, const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	gtkdialog_warning("App chooser cannot select an application by ID.");
}

void widget_appchooser_refresh(variable *var)
{
	GList *element;
	gboolean initialised;

	initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (initialised) {
		gtk_app_chooser_refresh(GTK_APP_CHOOSER(var->Widget));
		return;
	}
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT) ||
		attributeset_is_avail(var->Attributes, ATTR_INPUT))
		gtkdialog_warning("App chooser does not support default or input selection by ID.");
	if (attributeset_is_avail(var->Attributes, ATTR_LABEL))
		gtkdialog_warning("App chooser does not support <label>.");
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		gtkdialog_warning("App chooser does not support <item>.");
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH) ||
		attributeset_is_avail(var->Attributes, ATTR_HEIGHT)) {
		gint width = -1;
		gint height = -1;
		if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
			width = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_WIDTH), "app chooser width");
		if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
			height = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_HEIGHT), "app chooser height");
		gtk_widget_set_size_request(var->Widget, width, height);
	}
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "disabled") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(G_OBJECT(var->Widget), "application-selected",
		G_CALLBACK(widget_appchooser_selected), var->Attributes);
	g_signal_connect(G_OBJECT(var->Widget), "application-activated",
		G_CALLBACK(widget_appchooser_activated), var->Attributes);
}

void widget_appchooser_save(variable *var)
{
	widget_appchooser_save_id(var);
}

void widget_appchooser_clear(variable *var)
{
	(void)var;
	gtkdialog_warning("App chooser does not support clear or removeselected.");
}
