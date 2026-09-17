/*
 * widget_popover.c: GTK3 popover and menu button XML widgets.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <gtk/gtk.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "signals.h"
#include "widget_popover.h"

static void widget_menubutton_set_value(GtkWidget *widget, const gchar *value)
{
	gboolean active;
	if (!widget_parse_boolean(value, &active)) {
		gtkdialog_warning("Invalid menubutton Boolean value '%s'.", value);
		return;
	}
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), active);
}

static void widget_popover_closed(GtkPopover *popover, AttributeSet *Attr)
{
	widget_signal_executor(GTK_WIDGET(popover), Attr, "closed");
}

static void widget_menubutton_toggled(GtkToggleButton *button,
	AttributeSet *Attr)
{
	widget_signal_executor(GTK_WIDGET(button), Attr, "toggled");
}

static tag_attr *widget_popovermenu_child_attributes(GtkWidget *widget)
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

GtkWidget *widget_popovermenu_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	stackelement children = pop();
	GtkWidget *widget;
	GHashTable *names;
	gchar *value;
	gint i;
	(void)Attr;
	(void)Type;
	if (children.nwidgets < 1) {
		stackelement_clear(&children);
		g_error("gtkdialog: <popovermenu> requires at least one content page.");
	}
	widget = gtk_popover_menu_new();
	names = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	for (i = 0; i < children.nwidgets; ++i) {
		GtkWidget *child = children.widgets[i];
		tag_attr *child_attr = widget_popovermenu_child_attributes(child);
		gchar *submenu = child_attr == NULL ? NULL :
			get_tag_attribute(child_attr, "submenu");
		if (i > 0 && (submenu == NULL || !*submenu)) {
			stackelement_clear(&children);
			g_error("gtkdialog: each additional <popovermenu> page needs a submenu name.");
		}
		if (i == 0 && submenu != NULL &&
			g_strcmp0(submenu, "main") != 0) {
			stackelement_clear(&children);
			g_error("gtkdialog: the first <popovermenu> page must be main.");
		}
		if (g_hash_table_contains(names, submenu == NULL ? "main" : submenu)) {
			stackelement_clear(&children);
			g_error("gtkdialog: duplicate <popovermenu> submenu name '%s'.",
				submenu == NULL ? "main" : submenu);
		}
		g_hash_table_add(names, g_strdup(submenu == NULL ? "main" : submenu));
		gtk_container_add(GTK_CONTAINER(widget), child);
		if (submenu != NULL) {
			gtk_container_child_set(GTK_CONTAINER(widget), child,
				"submenu", submenu, NULL);
			kill_tag_attribute(child_attr, "submenu");
		}
	}
	g_hash_table_unref(names);
	stackelement_clear(&children);
	if (attr != NULL &&
		(value = get_tag_attribute(attr, "position")) != NULL) {
		GtkPositionType position = GTK_POS_BOTTOM;
		if (g_ascii_strcasecmp(value, "top") == 0)
			position = GTK_POS_TOP;
		else if (g_ascii_strcasecmp(value, "left") == 0)
			position = GTK_POS_LEFT;
		else if (g_ascii_strcasecmp(value, "right") == 0)
			position = GTK_POS_RIGHT;
		else if (g_ascii_strcasecmp(value, "bottom") != 0)
			gtkdialog_warning("Invalid popovermenu position '%s'; using bottom.",
				value);
		gtk_popover_set_position(GTK_POPOVER(widget), position);
		kill_tag_attribute(attr, "position");
	}
	return widget;
}

GtkWidget *widget_modelbutton_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	GtkWidget *widget = gtk_model_button_new();
	GList *element;
	gchar *value;
	(void)Type;
	value = attributeset_is_avail(Attr, ATTR_LABEL) ?
		attributeset_get_first(&element, Attr, ATTR_LABEL) : "Item";
	g_object_set(widget, "text", value, NULL);
	if (attr != NULL) {
		if ((value = get_tag_attribute(attr, "menu-name")) != NULL) {
			g_object_set(widget, "menu-name", value, NULL);
			kill_tag_attribute(attr, "menu-name");
		}
		if ((value = get_tag_attribute(attr, "inverted")) != NULL) {
			gboolean enabled;
			if (widget_parse_boolean(value, &enabled))
				g_object_set(widget, "inverted", enabled, NULL);
			else
				gtkdialog_warning("Invalid modelbutton inverted value '%s'.", value);
			kill_tag_attribute(attr, "inverted");
		}
		if ((value = get_tag_attribute(attr, "centered")) != NULL) {
			gboolean enabled;
			if (widget_parse_boolean(value, &enabled))
				g_object_set(widget, "centered", enabled, NULL);
			else
				gtkdialog_warning("Invalid modelbutton centered value '%s'.", value);
			kill_tag_attribute(attr, "centered");
		}
	}
	return widget;
}

GtkWidget *widget_popover_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	stackelement child = pop();
	GtkWidget *widget;
	gchar *value;
	(void)Attr;
	(void)Type;
	if (child.nwidgets != 1) {
		stackelement_clear(&child);
		g_error("gtkdialog: <popover> requires exactly one direct content widget.");
	}
	widget = gtk_popover_new(NULL);
	gtk_container_add(GTK_CONTAINER(widget), child.widgets[0]);
	stackelement_clear(&child);
	if (attr != NULL &&
		(value = get_tag_attribute(attr, "position")) != NULL) {
		GtkPositionType position = GTK_POS_BOTTOM;
		if (g_ascii_strcasecmp(value, "top") == 0)
			position = GTK_POS_TOP;
		else if (g_ascii_strcasecmp(value, "left") == 0)
			position = GTK_POS_LEFT;
		else if (g_ascii_strcasecmp(value, "right") == 0)
			position = GTK_POS_RIGHT;
		else if (g_ascii_strcasecmp(value, "bottom") != 0)
			gtkdialog_warning("Invalid popover position '%s'; using bottom.",
				value);
		gtk_popover_set_position(GTK_POPOVER(widget), position);
		kill_tag_attribute(attr, "position");
	}
	return widget;
}

GtkWidget *widget_menubutton_create(AttributeSet *Attr, tag_attr *attr,
	gint Type)
{
	stackelement child = pop();
	GtkWidget *widget;
	GList *element;
	gchar *value;
	(void)Type;
	if (child.nwidgets != 1 || !GTK_IS_POPOVER(child.widgets[0])) {
		stackelement_clear(&child);
		g_error("gtkdialog: <menubutton> requires one direct <popover> or <popovermenu> child.");
	}
	widget = gtk_menu_button_new();
	if (attributeset_is_avail(Attr, ATTR_LABEL))
		gtk_button_set_label(GTK_BUTTON(widget),
			attributeset_get_first(&element, Attr, ATTR_LABEL));
	else
		gtk_button_set_label(GTK_BUTTON(widget), "Options");
	gtk_menu_button_set_popover(GTK_MENU_BUTTON(widget), child.widgets[0]);
	stackelement_clear(&child);
	if (attr != NULL && (value = get_tag_attribute(attr, "active")) != NULL) {
		widget_menubutton_set_value(widget, value);
		kill_tag_attribute(attr, "active");
	}
	return widget;
}

gchar *widget_popover_envvar_construct(GtkWidget *widget)
{
	(void)widget;
	return g_strdup("");
}

gchar *widget_menubutton_envvar_construct(GtkWidget *widget)
{
	return g_strdup(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
		? "true" : "false");
}

gchar *widget_modelbutton_envvar_construct(GtkWidget *widget)
{
	(void)widget;
	return g_strdup("");
}

void widget_modelbutton_refresh(variable *var)
{
	GList *element;
	gint width = -1;
	gint height = -1;
	if (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(var->Widget),
		"_initialised")))
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
		width = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_WIDTH), "modelbutton width");
	if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
		height = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_HEIGHT), "modelbutton height");
	if (width != -1 || height != -1)
		gtk_widget_set_size_request(var->Widget, width, height);
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT) ||
		attributeset_is_avail(var->Attributes, ATTR_INPUT) ||
		attributeset_is_avail(var->Attributes, ATTR_OUTPUT))
		gtkdialog_warning("<modelbutton> has no independent value.");
	g_signal_connect(var->Widget, "clicked",
		G_CALLBACK(button_clicked_attr), var->Attributes);
}

void widget_modelbutton_save(variable *var)
{
	(void)var;
	gtkdialog_warning("<save> is not supported by <modelbutton>.");
}

void widget_modelbutton_clear(variable *var)
{
	(void)var;
	gtkdialog_warning("<clear> is not supported by <modelbutton>.");
}

void widget_modelbutton_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <modelbutton>.");
}

void widget_popover_refresh(variable *var)
{
	GList *element;
	gint width = -1;
	gint height = -1;
	if (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(var->Widget),
		"_initialised")))
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
		width = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_WIDTH), "popover width");
	if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
		height = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_HEIGHT), "popover height");
	if (width != -1 || height != -1)
		gtk_widget_set_size_request(var->Widget, width, height);
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT) ||
		attributeset_is_avail(var->Attributes, ATTR_INPUT) ||
		attributeset_is_avail(var->Attributes, ATTR_OUTPUT))
		gtkdialog_warning("<popover> has no independent value; use its children.");
	g_signal_connect(var->Widget, "closed",
		G_CALLBACK(widget_popover_closed), var->Attributes);
}

void widget_menubutton_refresh(variable *var)
{
	GList *element;
	gint width = -1;
	gint height = -1;
	if (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(var->Widget),
		"_initialised")))
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_menubutton_set_value(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));
	if (attributeset_is_avail(var->Attributes, ATTR_INPUT))
		gtkdialog_warning("<input> is not supported by <menubutton>.");
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
		width = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_WIDTH), "menubutton width");
	if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
		height = widget_parse_size_request(attributeset_get_first(&element,
			var->Attributes, ATTR_HEIGHT), "menubutton height");
	if (width != -1 || height != -1)
		gtk_widget_set_size_request(var->Widget, width, height);
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(var->Widget, "toggled",
		G_CALLBACK(widget_menubutton_toggled), var->Attributes);
}

void widget_popover_save(variable *var)
{
	(void)var;
	gtkdialog_warning("<save> is not supported by <popover>.");
}

void widget_menubutton_save(variable *var)
{
	GList *element;
	gchar *output = attributeset_get_first(&element, var->Attributes,
		ATTR_OUTPUT);
	FILE *file;
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("<menubutton> has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write menubutton output '%s'.", output + 5);
		return;
	}
	fputs(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(var->Widget))
		? "true" : "false", file);
	widget_close_output(file, output + 5);
}

void widget_popover_clear(variable *var)
{
	(void)var;
	gtkdialog_warning("<clear> is not supported by <popover>.");
}

void widget_menubutton_clear(variable *var)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(var->Widget), FALSE);
}

void widget_popover_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <popover>.");
}

void widget_menubutton_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <menubutton>.");
}

void widget_menubutton_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_menubutton_set_value(var->Widget, value);
}
