/*
 * widget_stacksidebar.c: GTK3 GtkStackSidebar XML widget.
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
#include "variables.h"
#include "widgets.h"
#include "widget_stacksidebar.h"

static void widget_stacksidebar_bind(GtkWidget *widget)
{
	const gchar *target_name = g_object_get_data(G_OBJECT(widget),
		"_stacksidebar_target");
	variable *target;
	if (gtk_stack_sidebar_get_stack(GTK_STACK_SIDEBAR(widget)) != NULL)
		return;
	if (target_name == NULL || !*target_name)
		return;
	target = variables_get_by_name(target_name);
	if (target == NULL || target->Widget == NULL ||
		!GTK_IS_STACK(target->Widget)) {
		gtkdialog_warning("Cannot bind <stacksidebar> to stack '%s'.",
			target_name);
		return;
	}
	gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(widget),
		GTK_STACK(target->Widget));
}

static void widget_stacksidebar_realized(GtkWidget *widget, gpointer unused)
{
	(void)unused;
	widget_stacksidebar_bind(widget);
}

GtkWidget *widget_stacksidebar_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	GtkWidget *widget;
	gchar *target;
	(void)Attr;
	(void)Type;
	if (attr == NULL ||
		(target = get_tag_attribute(attr, "stack")) == NULL || !*target)
		g_error("gtkdialog: <stacksidebar> requires a stack attribute.");
	widget = gtk_stack_sidebar_new();
	g_object_set_data_full(G_OBJECT(widget), "_stacksidebar_target",
		g_strdup(target), g_free);
	kill_tag_attribute(attr, "stack");
	g_signal_connect(widget, "realize",
		G_CALLBACK(widget_stacksidebar_realized), NULL);
	return widget;
}

gchar *widget_stacksidebar_envvar_construct(GtkWidget *widget)
{
	(void)widget;
	return g_strdup("");
}

void widget_stacksidebar_refresh(variable *var)
{
	GList *element;
	gint width = -1;
	gint height = -1;
	if (gtk_widget_get_realized(var->Widget))
		widget_stacksidebar_bind(var->Widget);
	if (GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised")))
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
		width = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_WIDTH), "stack sidebar width");
	if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
		height = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_HEIGHT), "stack sidebar height");
	if (width != -1 || height != -1)
		gtk_widget_set_size_request(var->Widget, width, height);
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT) ||
		attributeset_is_avail(var->Attributes, ATTR_INPUT) ||
		attributeset_is_avail(var->Attributes, ATTR_OUTPUT))
		gtkdialog_warning("<stacksidebar> has no independent value; use its <stack>.");
}
