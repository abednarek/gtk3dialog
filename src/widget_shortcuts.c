/*
 * widget_shortcuts.c: Native GTK3 shortcuts window hierarchy.
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
#include "widget_shortcuts.h"

extern gboolean option_centering;
extern gboolean have_geometry_xy;
extern gboolean have_geometry_dxdy;
extern gint geometry_dx;
extern gint geometry_dy;
extern gint geometry_x;
extern gint geometry_y;

#define SHORTCUTS_PENDING_SECTION "gtkdialog-shortcuts-pending-section"
#define SHORTCUTS_DESTROYED "gtkdialog-shortcuts-destroyed"
#define SHORTCUTS_CURRENT_SECTION "gtkdialog-shortcuts-current-section"

static void widget_shortcuts_find_stack(GtkWidget *widget, gpointer data)
{
	GtkWidget **found = data;
	GList *children;
	GList *element;
	if (*found != NULL)
		return;
	if (GTK_IS_STACK(widget)) {
		children = gtk_container_get_children(GTK_CONTAINER(widget));
		for (element = children; element != NULL; element = element->next) {
			if (GTK_IS_SHORTCUTS_SECTION(element->data)) {
				*found = widget;
				break;
			}
		}
		g_list_free(children);
	}
	if (*found == NULL && GTK_IS_CONTAINER(widget))
		gtk_container_forall(GTK_CONTAINER(widget),
			widget_shortcuts_find_stack, data);
}

static GtkWidget *widget_shortcuts_section_stack(GtkWidget *window)
{
	GtkWidget *found = NULL;
	gtk_container_forall(GTK_CONTAINER(window),
		widget_shortcuts_find_stack, &found);
	return found;
}

static gchar *widget_shortcuts_visible_section(GtkWidget *window)
{
	GtkWidget *stack = widget_shortcuts_section_stack(window);
	GtkWidget *section = stack != NULL
		? gtk_stack_get_visible_child(GTK_STACK(stack)) : NULL;
	gchar *name = NULL;
	if (GTK_IS_SHORTCUTS_SECTION(section))
		g_object_get(section, "section-name", &name, NULL);
	return name;
}

static gboolean widget_shortcuts_has_section(GtkWidget *window,
	const gchar *name)
{
	GList *sections = gtk_container_get_children(GTK_CONTAINER(window));
	GList *element;
	gboolean found = FALSE;
	for (element = sections; element != NULL; element = element->next) {
		gchar *section_name = NULL;
		g_object_get(element->data, "section-name", &section_name, NULL);
		if (g_strcmp0(section_name, name) == 0)
			found = TRUE;
		g_free(section_name);
		if (found)
			break;
	}
	g_list_free(sections);
	return found;
}

static void widget_shortcuts_destroyed(GtkWidget *widget, gpointer unused)
{
	(void)unused;
	g_object_set_data(G_OBJECT(widget), SHORTCUTS_DESTROYED,
		GINT_TO_POINTER(TRUE));
}

static void widget_shortcuts_dispatch(GtkWidget *widget,
	AttributeSet *attributes, const gchar *signal)
{
	/* An action may destroy this window synchronously. Keep the instance
	 * alive through signal emission and skip GTK's default handler if
	 * closewindow: has already destroyed it. */
	g_object_ref(widget);
	widget_signal_executor(widget, attributes, signal);
	if (g_object_get_data(G_OBJECT(widget), SHORTCUTS_DESTROYED))
		g_signal_stop_emission_by_name(widget, signal);
	g_object_unref(widget);
}

static void widget_shortcuts_close(GtkWidget *widget, AttributeSet *attributes)
{
	widget_shortcuts_dispatch(widget, attributes, "close");
}

static void widget_shortcuts_search(GtkWidget *widget, AttributeSet *attributes)
{
	widget_shortcuts_dispatch(widget, attributes, "search");
}

static void widget_shortcuts_section_changed(GtkStack *stack,
	GParamSpec *property, AttributeSet *attributes)
{
	GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(stack));
	gchar *name;
	const gchar *previous;
	gboolean changed;
	gboolean had_previous;
	(void)property;
	if (!GTK_IS_SHORTCUTS_WINDOW(window) ||
		!gtk_widget_get_mapped(window) ||
		!GTK_IS_SHORTCUTS_SECTION(gtk_stack_get_visible_child(stack)))
		return;
	name = widget_shortcuts_visible_section(window);
	previous = g_object_get_data(G_OBJECT(window), SHORTCUTS_CURRENT_SECTION);
	had_previous = previous != NULL;
	changed = g_strcmp0(previous, name) != 0;
	g_object_set_data_full(G_OBJECT(window), SHORTCUTS_CURRENT_SECTION,
		name, g_free);
	if (changed && had_previous) {
		g_object_ref(stack);
		widget_signal_executor(window, attributes, "section-changed");
		g_object_unref(stack);
	}
}

static void widget_shortcuts_set_section(GtkWidget *widget, const gchar *name)
{
	if (name == NULL || *name == '\0')
		return;
	if (!widget_shortcuts_has_section(widget, name)) {
		gtkdialog_warning("Unknown shortcuts section '%s'.", name);
		return;
	}
	/* GtkStack cannot make an invisible section current. GTKDialog builds
	 * the hierarchy before show_all(), so apply startup selection on map. */
	if (!gtk_widget_get_mapped(widget))
		g_object_set_data_full(G_OBJECT(widget), SHORTCUTS_PENDING_SECTION,
			g_strdup(name), g_free);
	else
		g_object_set(widget, "section-name", name, NULL);
}

static void widget_shortcuts_mapped(GtkWidget *widget, gpointer unused)
{
	const gchar *pending = g_object_get_data(G_OBJECT(widget),
		SHORTCUTS_PENDING_SECTION);
	gchar *name;
	(void)unused;
	if (pending == NULL)
		return;
	name = g_strdup(pending);
	g_object_set_data(G_OBJECT(widget), SHORTCUTS_PENDING_SECTION, NULL);
	g_object_set(widget, "section-name", name, NULL);
	g_free(name);
}

static void widget_shortcuts_input(variable *var, const gchar *source,
	gboolean command)
{
	FILE *input = command ? widget_opencommand(source) : fopen(source, "r");
	gchar *line;
	if (input == NULL) {
		gtkdialog_warning("Cannot read shortcuts window input '%s'.", source);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_shortcuts_set_section(var->Widget, g_strstrip(line));
		g_free(line);
	}
	if (command)
		widget_closecommand(input, source);
	else
		fclose(input);
}

GtkWidget *widget_shortcuts_create(AttributeSet *attributes,
	tag_attr *tag_attributes, gint type)
{
	GtkWidget *widget;
	stackelement children = { 0 };
	gint i;

	switch (type) {
	case WIDGET_SHORTCUTSWINDOW:
		widget = g_object_new(GTK_TYPE_SHORTCUTS_WINDOW, NULL);
		g_signal_connect(widget, "destroy",
			G_CALLBACK(widget_shortcuts_destroyed), NULL);
		g_signal_connect(widget, "map",
			G_CALLBACK(widget_shortcuts_mapped), NULL);
		if (have_geometry_dxdy)
			gtk_widget_set_size_request(widget, geometry_dx, geometry_dy);
		if (have_geometry_xy)
			gtk_window_move(GTK_WINDOW(widget), geometry_x, geometry_y);
		if (option_centering)
			gtk_window_set_position(GTK_WINDOW(widget),
				GTK_WIN_POS_CENTER_ALWAYS);
		break;
	case WIDGET_SHORTCUTSSECTION:
		widget = g_object_new(GTK_TYPE_SHORTCUTS_SECTION, NULL);
		break;
	case WIDGET_SHORTCUTSGROUP:
		widget = g_object_new(GTK_TYPE_SHORTCUTS_GROUP, NULL);
		break;
	case WIDGET_SHORTCUTSSHORTCUT:
		widget = g_object_new(GTK_TYPE_SHORTCUTS_SHORTCUT, NULL);
		widget_set_tag_attributes(widget, tag_attributes);
		return widget;
	default:
		g_error("gtkdialog: invalid shortcuts widget type.");
		return NULL;
	}

	/* GTK indexes these children when they are added. Set their identifying
	 * properties before insertion, rather than waiting for realization. */
	if (type != WIDGET_SHORTCUTSWINDOW)
		widget_set_tag_attributes(widget, tag_attributes);
	children = pop();
	for (i = 0; i < children.nwidgets; ++i) {
		if (type == WIDGET_SHORTCUTSWINDOW) {
			gchar *name = NULL;
			g_object_get(children.widgets[i], "section-name", &name,
				NULL);
			if (name == NULL || *name == '\0')
				g_error("gtkdialog: <shortcutssection> requires a nonempty "
					"section-name.");
			if (widget_shortcuts_has_section(widget, name))
				g_error("gtkdialog: duplicate shortcuts section '%s'.", name);
			g_free(name);
		}
		gtk_container_add(GTK_CONTAINER(widget), children.widgets[i]);
	}
	if (type == WIDGET_SHORTCUTSWINDOW && children.nwidgets > 0) {
		gchar *first_name = NULL;
		const gchar *requested = tag_attributes != NULL
			? get_tag_attribute(tag_attributes, "section-name") : NULL;
		g_object_get(children.widgets[0], "section-name", &first_name,
			NULL);
		widget_shortcuts_set_section(widget, first_name);
		if (requested != NULL) {
			widget_shortcuts_set_section(widget, requested);
			kill_tag_attribute(tag_attributes, "section-name");
		}
		g_free(first_name);
	}
	stackelement_clear(&children);
	(void)attributes;
	return widget;
}

gchar *widget_shortcuts_envvar_construct(GtkWidget *widget, gint type)
{
	gchar *value = NULL;
	const gchar *property = type == WIDGET_SHORTCUTSWINDOW
		? "section-name" : "title";
	if (type == WIDGET_SHORTCUTSWINDOW ||
		type == WIDGET_SHORTCUTSSECTION ||
		type == WIDGET_SHORTCUTSGROUP ||
		type == WIDGET_SHORTCUTSSHORTCUT)
		g_object_get(widget, property, &value, NULL);
	if (type == WIDGET_SHORTCUTSWINDOW &&
		g_object_get_data(G_OBJECT(widget), SHORTCUTS_PENDING_SECTION) != NULL) {
		g_free(value);
		value = g_strdup(g_object_get_data(G_OBJECT(widget),
			SHORTCUTS_PENDING_SECTION));
	}
	if (type == WIDGET_SHORTCUTSWINDOW) {
		gchar *visible = widget_shortcuts_visible_section(widget);
		if (visible != NULL) {
			g_free(value);
			value = visible;
		} else if (g_object_get_data(G_OBJECT(widget),
			SHORTCUTS_CURRENT_SECTION) != NULL) {
			g_free(value);
			value = g_strdup(g_object_get_data(G_OBJECT(widget),
				SHORTCUTS_CURRENT_SECTION));
		}
	}
	if (value == NULL)
		return g_strdup("");
	return value;
}

void widget_shortcuts_fileselect(variable *var, const char *name,
	const char *value)
{
	(void)name;
	if (var->Type == WIDGET_SHORTCUTSWINDOW)
		widget_shortcuts_set_section(var->Widget, value);
	else
		gtkdialog_warning("Only <shortcutswindow> accepts section selection.");
}

void widget_shortcuts_refresh(variable *var)
{
	GList *element;
	gchar *input;
	GtkWidget *section_stack;
	gboolean initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (var->Type != WIDGET_SHORTCUTSWINDOW)
		return;
	if (!initialised && attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_shortcuts_set_section(var->Widget,
			attributeset_get_first(&element, var->Attributes, ATTR_DEFAULT));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_shortcuts_input(var, input + 8, TRUE);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 && input[5]) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_shortcuts_input(var, input + 5, FALSE);
		}
		input = attributeset_get_next(&element, var->Attributes, ATTR_INPUT);
	}
	if (initialised)
		return;
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
	g_signal_connect(var->Widget, "close",
		G_CALLBACK(widget_shortcuts_close), var->Attributes);
	g_signal_connect(var->Widget, "search",
		G_CALLBACK(widget_shortcuts_search), var->Attributes);
	section_stack = widget_shortcuts_section_stack(var->Widget);
	if (section_stack != NULL)
		g_signal_connect(section_stack, "notify::visible-child",
			G_CALLBACK(widget_shortcuts_section_changed),
			var->Attributes);
	g_signal_connect(var->Widget, "delete-event",
		G_CALLBACK(window_delete_event_handler), NULL);
}

void widget_shortcuts_save(variable *var)
{
	GList *element;
	gchar *output = attributeset_get_first(&element, var->Attributes,
		ATTR_OUTPUT);
	gchar *value;
	FILE *file;
	while (output != NULL &&
		(g_ascii_strncasecmp(output, "file:", 5) != 0 || !output[5]))
		output = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	if (output == NULL) {
		gtkdialog_warning("Shortcuts widget has no <output file> directive.");
		return;
	}
	file = fopen(output + 5, "w");
	if (file == NULL) {
		gtkdialog_warning("Cannot write shortcuts output '%s'.", output + 5);
		return;
	}
	value = widget_shortcuts_envvar_construct(var->Widget, var->Type);
	fputs(value, file);
	g_free(value);
	widget_close_output(file, output + 5);
}

void widget_shortcuts_clear(variable *var)
{
	(void)var;
	gtkdialog_warning("<clear> is not supported by shortcuts widgets.");
}

void widget_shortcuts_removeselected(variable *var)
{
	(void)var;
	gtkdialog_warning("<removeselected> is not supported by shortcuts widgets.");
}
