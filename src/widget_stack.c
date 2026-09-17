/*
 * widget_stack.c: GTK3 GtkStack XML widget.
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
#include <string.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "tag_attributes.h"
#include "widgets.h"
#include "widget_stack.h"

static void widget_stack_set_value(GtkWidget *widget, const gchar *value)
{
	gchar *name = g_strdup(value);
	g_strstrip(name);
	if (gtk_stack_get_child_by_name(GTK_STACK(widget), name) == NULL) {
		gtkdialog_warning("Unknown stack page '%s'.", name);
		g_free(name);
		return;
	}
	/* A stack page must be visible before GTK accepts it as the current page.
	 * Keep startup input until the containing window has been shown. */
	if (!gtk_widget_get_mapped(widget))
		g_object_set_data_full(G_OBJECT(widget), "_stack_pending_page",
			name, g_free);
	else {
		gtk_stack_set_visible_child_name(GTK_STACK(widget), name);
		g_free(name);
	}
}

static void widget_stack_mapped(GtkWidget *widget, gpointer unused)
{
	const gchar *name = g_object_get_data(G_OBJECT(widget),
		"_stack_pending_page");
	(void)unused;
	if (name != NULL) {
		gchar *requested = g_strdup(name);
		g_object_set_data(G_OBJECT(widget), "_stack_pending_page", NULL);
		gtk_stack_set_visible_child_name(GTK_STACK(widget), requested);
		g_free(requested);
	}
}

static void widget_stack_changed(GtkStack *stack, GParamSpec *property,
	AttributeSet *Attr)
{
	(void)property;
	if (!gtk_widget_get_mapped(GTK_WIDGET(stack)))
		return;
	widget_signal_executor(GTK_WIDGET(stack), Attr, "page-changed");
}

static void widget_stack_input(variable *var, const gchar *source,
	gboolean command)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;
	if (input == NULL) {
		gtkdialog_warning("Cannot read stack input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_stack_set_value(var->Widget, line);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_stack_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkWidget *widget = gtk_stack_new();
	stackelement pages;
	gchar **names = NULL;
	gchar **titles = NULL;
	gchar *value;
	gint i;
	(void)Attr;
	(void)Type;
	if (attr != NULL) {
		if ((value = get_tag_attribute(attr, "names")) != NULL) {
			names = g_strsplit(value, "|", -1);
			kill_tag_attribute(attr, "names");
		}
		if ((value = get_tag_attribute(attr, "titles")) != NULL) {
			titles = g_strsplit(value, "|", -1);
			kill_tag_attribute(attr, "titles");
		}
	}
	pages = pop();
	if (pages.nwidgets < 1)
		g_error("gtkdialog: <stack> requires at least one direct child widget.");
	if (names != NULL && g_strv_length(names) != (guint)pages.nwidgets)
		g_error("gtkdialog: <stack> names must match its number of pages.");
	if (titles != NULL && g_strv_length(titles) != (guint)pages.nwidgets)
		g_error("gtkdialog: <stack> titles must match its number of pages.");
	for (i = 0; i < pages.nwidgets; ++i) {
		gchar *generated = g_strdup_printf("%d", i);
		const gchar *name = names != NULL ? names[i] : generated;
		const gchar *title = titles != NULL ? titles[i] : name;
		if (!*name || gtk_stack_get_child_by_name(GTK_STACK(widget), name))
			g_error("gtkdialog: <stack> page names must be unique and nonempty.");
		gtk_stack_add_titled(GTK_STACK(widget), pages.widgets[i], name, title);
		g_free(generated);
	}
	stackelement_clear(&pages);
	g_strfreev(names);
	g_strfreev(titles);
	if (attr != NULL) {
		if ((value = get_tag_attribute(attr, "transition-type")) != NULL) {
			GEnumClass *enum_class = g_type_class_ref(GTK_TYPE_STACK_TRANSITION_TYPE);
			GEnumValue *enum_value = g_enum_get_value_by_nick(enum_class, value);
			if (enum_value != NULL)
				gtk_stack_set_transition_type(GTK_STACK(widget), enum_value->value);
			else
				gtkdialog_warning("Invalid stack transition type '%s'.", value);
			g_type_class_unref(enum_class);
			kill_tag_attribute(attr, "transition-type");
		}
		if ((value = get_tag_attribute(attr, "transition-duration")) != NULL) {
			gtk_stack_set_transition_duration(GTK_STACK(widget),
				widget_parse_bounded_integer(value, 0, G_MAXINT, 200,
					"stack transition-duration"));
			kill_tag_attribute(attr, "transition-duration");
		}
		if ((value = get_tag_attribute(attr, "visible-child")) != NULL) {
			widget_stack_set_value(widget, value);
			kill_tag_attribute(attr, "visible-child");
		}
	}
	g_signal_connect(widget, "map", G_CALLBACK(widget_stack_mapped), NULL);
	return widget;
}

gchar *widget_stack_envvar_construct(GtkWidget *widget)
{
	const gchar *name = gtk_stack_get_visible_child_name(GTK_STACK(widget));
	if (name == NULL)
		name = g_object_get_data(G_OBJECT(widget), "_stack_pending_page");
	return g_strdup(name != NULL ? name : "");
}

void widget_stack_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_stack_set_value(var->Widget, value);
}

void widget_stack_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (!initialised && attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_stack_set_value(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_stack_input(var, input + 8, TRUE);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 && input[5]) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_stack_input(var, input + 5, FALSE);
		}
		input = attributeset_get_next(&element, var->Attributes, ATTR_INPUT);
	}
	if (initialised)
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_WIDTH) ||
		attributeset_is_avail(var->Attributes, ATTR_HEIGHT)) {
		gint width = -1;
		gint height = -1;
		if (attributeset_is_avail(var->Attributes, ATTR_WIDTH))
			width = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_WIDTH), "stack width");
		if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
			height = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_HEIGHT), "stack height");
		gtk_widget_set_size_request(var->Widget, width, height);
	}
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(var->Widget, "notify::visible-child",
		G_CALLBACK(widget_stack_changed), var->Attributes);
}

void widget_stack_save(variable *var)
{
	GList *element;
	gchar *output = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	FILE *file;
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("<stack> has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write stack output '%s'.", output + 5);
		return;
	}
	{
		gchar *name = widget_stack_envvar_construct(var->Widget);
		fputs(name, file);
		g_free(name);
	}
	widget_close_output(file, output + 5);
}

void widget_stack_clear(variable *var)
{
	GList *children = gtk_container_get_children(GTK_CONTAINER(var->Widget));
	if (children != NULL) {
		gchar *first = NULL;
		gtk_container_child_get(GTK_CONTAINER(var->Widget), children->data,
			"name", &first, NULL);
		widget_stack_set_value(var->Widget, first);
		g_free(first);
	}
	g_list_free(children);
}

void widget_stack_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <stack>.");
}
