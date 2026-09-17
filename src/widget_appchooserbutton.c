/*
 * widget_appchooserbutton.c: GTK3 application chooser button.
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
#include "widget_appchooserbutton.h"
#include "widget_appchooser_common.h"

static void widget_appchooserbutton_changed(GtkWidget *widget,
	AttributeSet *Attr)
{
	widget_signal_executor(widget, Attr, "changed");
}

GtkWidget *widget_appchooserbutton_create(
	AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkWidget *widget;
	gchar *content_type = NULL;

	(void)Attr;
	(void)Type;
	if (attr != NULL)
		content_type = get_tag_attribute(attr, "content-type");
	if (content_type == NULL || content_type[0] == '\0') {
		gtkdialog_warning("App chooser button needs a content-type; using text/plain.");
		content_type = "text/plain";
	}
	widget = gtk_app_chooser_button_new(content_type);
	if (attr != NULL)
		kill_tag_attribute(attr, "content-type");
	return widget;
}

gchar *widget_appchooserbutton_envvar_construct(GtkWidget *widget)
{
	return widget_appchooser_selected_id(widget);
}

void widget_appchooserbutton_fileselect(
	variable *var, const char *name, const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	gtkdialog_warning("App chooser button cannot select an application by ID.");
}

void widget_appchooserbutton_refresh(variable *var)
{
	gboolean initialised;

	initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (initialised) {
		gtk_app_chooser_refresh(GTK_APP_CHOOSER(var->Widget));
		return;
	}
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT) ||
		attributeset_is_avail(var->Attributes, ATTR_INPUT))
		gtkdialog_warning("App chooser button does not support default or input selection by ID.");
	if (attributeset_is_avail(var->Attributes, ATTR_LABEL))
		gtkdialog_warning("App chooser button does not support <label>.");
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		gtkdialog_warning("App chooser button does not support <item>.");
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "disabled") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(G_OBJECT(var->Widget), "changed",
		G_CALLBACK(widget_appchooserbutton_changed), var->Attributes);
}

void widget_appchooserbutton_save(variable *var)
{
	widget_appchooser_save_id(var);
}

void widget_appchooserbutton_clear(variable *var)
{
	(void)var;
	gtkdialog_warning("App chooser button does not support clear or removeselected.");
}
