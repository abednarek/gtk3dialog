/*
 * widget_flowbox.c: GTK3 GtkFlowBox XML widget.
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
#include "widget_flowbox.h"

static const gchar *widget_flowbox_child_name(GtkFlowBoxChild *child)
{
	return child == NULL ? NULL :
		g_object_get_data(G_OBJECT(child), "_flowbox_name");
}

static void widget_flowbox_set_value(GtkWidget *widget, const gchar *value)
{
	gchar *name = g_strdup(value);
	gint index;
	g_strstrip(name);
	for (index = 0; ; ++index) {
		GtkFlowBoxChild *child = gtk_flow_box_get_child_at_index(
			GTK_FLOW_BOX(widget), index);
		if (child == NULL)
			break;
		if (g_strcmp0(name, widget_flowbox_child_name(child)) == 0) {
			if (!gtk_widget_get_mapped(widget))
				g_object_set_data_full(G_OBJECT(widget),
					"_flowbox_pending_child", name, g_free);
			else {
				gtk_flow_box_select_child(GTK_FLOW_BOX(widget), child);
				g_free(name);
			}
			return;
		}
	}
	gtkdialog_warning("Unknown flowbox child '%s'.", name);
	g_free(name);
}

static gboolean widget_flowbox_apply_pending(gpointer data)
{
	GtkWidget *widget = data;
	const gchar *name = g_object_get_data(G_OBJECT(widget),
		"_flowbox_pending_child");
	if (name != NULL) {
		gchar *requested = g_strdup(name);
		g_object_set_data(G_OBJECT(widget), "_flowbox_pending_child", NULL);
		widget_flowbox_set_value(widget, requested);
		g_free(requested);
	}
	return G_SOURCE_REMOVE;
}

static void widget_flowbox_mapped(GtkWidget *widget, gpointer unused)
{
	(void)unused;
	g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, widget_flowbox_apply_pending,
		g_object_ref(widget), g_object_unref);
}

static void widget_flowbox_changed(GtkFlowBox *box, AttributeSet *Attr)
{
	if (g_object_get_data(G_OBJECT(box), "_flowbox_pending_child") != NULL)
		return;
	widget_signal_executor(GTK_WIDGET(box), Attr, "selection-changed");
}

static void widget_flowbox_activated(GtkFlowBox *box, GtkFlowBoxChild *child,
	AttributeSet *Attr)
{
	(void)child;
	widget_signal_executor(GTK_WIDGET(box), Attr, "child-activated");
}

static void widget_flowbox_input(variable *var, const gchar *source,
	gboolean command)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;
	if (input == NULL) {
		gtkdialog_warning("Cannot read flowbox input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_flowbox_set_value(var->Widget, line);
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_flowbox_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkWidget *widget = gtk_flow_box_new();
	stackelement children = pop();
	gchar **names = NULL;
	gchar *value;
	gint i;
	(void)Attr;
	(void)Type;
	if (attr != NULL && (value = get_tag_attribute(attr, "names")) != NULL) {
		names = g_strsplit(value, "|", -1);
		kill_tag_attribute(attr, "names");
	}
	if (children.nwidgets < 1)
		g_error("gtkdialog: <flowbox> requires at least one direct child widget.");
	if (names != NULL && g_strv_length(names) != (guint)children.nwidgets)
		g_error("gtkdialog: <flowbox> names must match its number of children.");
	for (i = 0; i < children.nwidgets; ++i) {
		GtkWidget *child = gtk_flow_box_child_new();
		gchar *generated = g_strdup_printf("%d", i);
		const gchar *name = names != NULL ? names[i] : generated;
		gint previous;
		if (!*name)
			g_error("gtkdialog: <flowbox> child names must be nonempty.");
		for (previous = 0; previous < i; ++previous)
			if (g_strcmp0(name, names != NULL ? names[previous] : NULL) == 0)
				g_error("gtkdialog: <flowbox> child names must be unique.");
		g_object_set_data_full(G_OBJECT(child), "_flowbox_name",
			g_strdup(name), g_free);
		gtk_container_add(GTK_CONTAINER(child), children.widgets[i]);
		gtk_container_add(GTK_CONTAINER(widget), child);
		gtk_widget_show(child);
		g_free(generated);
	}
	stackelement_clear(&children);
	g_strfreev(names);
	gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(widget),
		GTK_SELECTION_SINGLE);
	if (attr != NULL) {
		if ((value = get_tag_attribute(attr, "selection-mode")) != NULL) {
			if (g_ascii_strcasecmp(value, "none") == 0)
				gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(widget),
					GTK_SELECTION_NONE);
			else if (g_ascii_strcasecmp(value, "single") != 0)
				gtkdialog_warning("Unsupported flowbox selection-mode '%s'; using single.",
					value);
			kill_tag_attribute(attr, "selection-mode");
		}
		if ((value = get_tag_attribute(attr, "activate-on-single-click")) != NULL) {
			gboolean enabled;
			if (widget_parse_boolean(value, &enabled))
				gtk_flow_box_set_activate_on_single_click(GTK_FLOW_BOX(widget),
					enabled);
			else
				gtkdialog_warning("Invalid flowbox activate-on-single-click '%s'.",
					value);
			kill_tag_attribute(attr, "activate-on-single-click");
		}
		if ((value = get_tag_attribute(attr, "row-spacing")) != NULL) {
			gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(widget),
				widget_parse_bounded_integer(value, 0, G_MAXINT, 0,
					"flowbox row-spacing"));
			kill_tag_attribute(attr, "row-spacing");
		}
		if ((value = get_tag_attribute(attr, "column-spacing")) != NULL) {
			gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(widget),
				widget_parse_bounded_integer(value, 0, G_MAXINT, 0,
					"flowbox column-spacing"));
			kill_tag_attribute(attr, "column-spacing");
		}
		if ((value = get_tag_attribute(attr, "max-children-per-line")) != NULL) {
			gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(widget),
				widget_parse_bounded_integer(value, 1, G_MAXINT, 7,
					"flowbox max-children-per-line"));
			kill_tag_attribute(attr, "max-children-per-line");
		}
		if ((value = get_tag_attribute(attr, "homogeneous")) != NULL) {
			gboolean enabled;
			if (widget_parse_boolean(value, &enabled))
				gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(widget), enabled);
			else
				gtkdialog_warning("Invalid flowbox homogeneous '%s'.", value);
			kill_tag_attribute(attr, "homogeneous");
		}
		if ((value = get_tag_attribute(attr, "orientation")) != NULL) {
			if (g_ascii_strcasecmp(value, "vertical") == 0)
				gtk_orientable_set_orientation(GTK_ORIENTABLE(widget),
					GTK_ORIENTATION_VERTICAL);
			else if (g_ascii_strcasecmp(value, "horizontal") != 0)
				gtkdialog_warning("Invalid flowbox orientation '%s'.", value);
			kill_tag_attribute(attr, "orientation");
		}
	}
	g_signal_connect(widget, "map", G_CALLBACK(widget_flowbox_mapped), NULL);
	return widget;
}

gchar *widget_flowbox_envvar_construct(GtkWidget *widget)
{
	GList *selected = gtk_flow_box_get_selected_children(GTK_FLOW_BOX(widget));
	const gchar *name = selected == NULL ? NULL :
		widget_flowbox_child_name(GTK_FLOW_BOX_CHILD(selected->data));
	const gchar *pending = g_object_get_data(G_OBJECT(widget),
		"_flowbox_pending_child");
	gchar *value = g_strdup(pending != NULL ? pending :
		name != NULL ? name : "");
	g_list_free(selected);
	return value;
}

void widget_flowbox_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	widget_flowbox_set_value(var->Widget, value);
}

void widget_flowbox_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (!initialised && attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_flowbox_set_value(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_flowbox_input(var, input + 8, TRUE);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 && input[5]) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_flowbox_input(var, input + 5, FALSE);
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
				var->Attributes, ATTR_WIDTH), "flowbox width");
		if (attributeset_is_avail(var->Attributes, ATTR_HEIGHT))
			height = widget_parse_size_request(attributeset_get_first(&element,
				var->Attributes, ATTR_HEIGHT), "flowbox height");
		gtk_widget_set_size_request(var->Widget, width, height);
	}
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(var->Widget, "selected-children-changed",
		G_CALLBACK(widget_flowbox_changed), var->Attributes);
	g_signal_connect(var->Widget, "child-activated",
		G_CALLBACK(widget_flowbox_activated), var->Attributes);
}

void widget_flowbox_save(variable *var)
{
	GList *element;
	gchar *output = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	FILE *file;
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("<flowbox> has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write flowbox output '%s'.", output + 5);
		return;
	}
	{
		gchar *name = widget_flowbox_envvar_construct(var->Widget);
		fputs(name, file);
		g_free(name);
	}
	widget_close_output(file, output + 5);
}

void widget_flowbox_clear(variable *var)
{
	g_object_set_data(G_OBJECT(var->Widget), "_flowbox_pending_child", NULL);
	gtk_flow_box_unselect_all(GTK_FLOW_BOX(var->Widget));
}

void widget_flowbox_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by <flowbox>.");
}
