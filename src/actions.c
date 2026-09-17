/*
 * actions.c:
 * Gtkdialog - A small utility for fast and easy GUI building.
 * Copyright (C) 2003-2007  László Pere <pipas@linux.pte.hu>
 * Copyright (C) 2011-2012  Thunor <thunorsif@hotmail.com>
 * Copyright (C) 2026       Artur Bednarek <artur@unix.org.pl>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <gtk/gtk.h>

#ifndef G_OS_WIN32
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include "gtkdialog.h"
#include "widgets.h"
#include "actions.h"
#include "attributes.h"
#include "automaton.h"
#include "variables.h"
#include "tag_attributes.h"
#include "widget_terminal.h"

extern gchar *option_include_file;

/* Local function prototypes */
void action_closewindow(GtkWidget *widget, char *string);
gboolean action_launchwindow(GtkWidget *widget, char *string);
void action_exitprogram(GtkWidget *widget, char *string);
void action_refreshwidget(GtkWidget *widget, char *string);
void action_savewidget(GtkWidget *widget, char *string);
void action_fileselect(GtkWidget *widget, char *string);
void action_fileselection_destroy(GtkWidget *w, actioncommand *ac);
void action_clearwidget(GtkWidget *widget, char *string);
void action_removeselected(GtkWidget *widget, char *string);
void action_enable(GtkWidget *widget, char *string);
void action_disable(GtkWidget *widget, char *string);
void action_show(GtkWidget *widget, char *string);
void action_hide(GtkWidget *widget, char *string);
void action_activate(GtkWidget *widget, char *string);
void action_grabfocus(GtkWidget *widget, char *string);
void action_presentwindow(GtkWidget *widget, char *string);
void action_expandall(GtkWidget *widget, char *string);
void action_collapseall(GtkWidget *widget, char *string);
void action_edit_undo(GtkWidget *widget, char *string);
void action_edit_redo(GtkWidget *widget, char *string);
void action_imageview_zoomin(GtkWidget *widget, char *string);
void action_imageview_zoomout(GtkWidget *widget, char *string);
void action_imageview_zoomreset(GtkWidget *widget, char *string);
void action_imageview_fit(GtkWidget *widget, char *string);
void action_imageview_fitwidth(GtkWidget *widget, char *string);
void action_imageview_fitheight(GtkWidget *widget, char *string);
void action_reparentwidget(GtkWidget *widget, char *string);
void action_settablabel(GtkWidget *widget, char *string);
void action_reorderpage(GtkWidget *widget, char *string);
void action_appendtab(GtkWidget *widget, char *string);
gboolean action_removetab(GtkWidget *widget, char *string);
void action_selecttab(GtkWidget *widget, char *string);
void action_appendtabauto(GtkWidget *widget, char *string);
gboolean action_replacebox(GtkWidget *widget, char *string);
void action_hangupterminal(GtkWidget *widget, char *string);
void action_shellcommand(GtkWidget *widget, char *string);

/***********************************************************************
 * Action closewindow                                                  *
 ***********************************************************************/
/* Thunor: This function will close an existing window that contains a
 * variable that matches string although ideally the application developer
 * would have supplied a unique name in the window widget's variable
 * directive.
 */

void action_closewindow(GtkWidget *widget, char *string)
{
	GtkWidget        *window;
	variable         *existing;

	g_assert(GTK_IS_WIDGET(widget));

#ifdef DEBUG
	fprintf(stderr, "%s(): Entering.\n", __func__);
#endif

	/**
	 * Patriot Oct 2009: zigbert has been dreaming for this to be fixed.
	 * Issue: closewindow command closes THE window its called from.
	 * Fixed: Workaround applied to point to the correct window.
	 **/

	existing = variables_get_by_name(string);

#ifdef DEBUG
	fprintf(stderr, "%s(): string=%s existing=%p\n",
		__func__, string, existing);
	fflush(stderr);
#endif

	if (existing != NULL && existing->Widget != NULL) {

#ifdef DEBUG
		fprintf(stderr, "%s(): Name=%s Type=%i\n",
			__func__, existing->Name, existing->Type);
		fflush(stderr);
#endif

		window = gtk_widget_get_ancestor(existing->Widget, GTK_TYPE_WINDOW);
		/* A standalone menu is a GTK toplevel without being a GtkWindow. */
		if (window == NULL && GTK_IS_MENU(existing->Widget))
			window = existing->Widget;
		variables_drop_by_window_id(NULL, existing->window_id);
		if (window != NULL)
			gtk_widget_destroy(window);

#ifdef DEBUG
		fprintf(stderr, "%s(): variables_count_widgets()=%i\n", __func__,
			variables_count_widgets());
#endif

		/* If we are closing the last window then we can exit gtkdialog */
		if (variables_count_widgets() == 0) {

			printf("EXIT=\"closewindow\"\n");
#ifdef DEBUG
			fprintf(stderr, "%s(): Calling exit(EXIT_SUCCESS)\n", __func__);
#endif

			exit(EXIT_SUCCESS);

		}
	}

#ifdef DEBUG
	fprintf(stderr, "%s(): Exiting.\n", __func__);
#endif
}

/***********************************************************************
 * Action launchwindow                                                 *
 ***********************************************************************/
/* Thunor: This function will launch a new window, the source of which
 * should be contained within an existing exported shell variable.
 * 
 * The parameter to this function is the shell variable name which MUST
 * match any widget's variable within the program, although it would be
 * better practice to use a dedicated window widget's variable for this.
 * 
 * Currently the variable name matching requirement remains unenforced
 * and will require to remain so as there will be broken applications
 * out there using fixes and workarounds. The best that can be hoped for
 * is a warning message once the new window has been loaded. 
 */

gboolean action_launchwindow(GtkWidget *widget, char *string)
{
#ifdef DEBUG
	extern gchar     *program_src;
	extern gint       charsreaded;
#endif
	extern int        instruction_counter;
	GtkWidget        *window;
	variable         *existing;
	gint              widget_type;

#ifdef DEBUG
	fprintf(stderr, "%s(): string=%s\n", __func__, string);
#endif

	/* Retain the historical restriction against launching from a timer. */
	widget_type = variables_get_widget_type(widget);
	if (widget_type == 0) {
		gtkdialog_warning("%s(): Can't launch %s because the originating widget has "
			"no active variable mapping.", __func__, string);
		return FALSE;
	}

	if (widget_type != WIDGET_TIMER) {

		/* Thunor: Added to check that a program isn't already being parsed
		 * as run_program() is not reentrant and it'll error with a parser
		 * message that actually relates to the newly launched program
		 * (see Issue16 on the gtkdialog website) */
		if (instruction_counter == 0) {

			/* Check if a variable already exists with the same name as that
			 * passed as the parameter to launch to establish if the window
			 * has already been launched (it's not foolproof but it works) */
			existing = variables_get_by_name(string);

			if (existing != NULL && existing->Widget != NULL) {

				/* The window has already been launched so just give it the focus */
#ifdef DEBUG
				fprintf(stderr, "%s(): existing=%p Name=%s\n", __func__,
					existing, existing->Name);
#endif

				/**
				* Patriot Oct 2009: Fixing the issue above [in action_closewindow]
				* also requires a minor adjustment to this section.
				**/
				window = gtk_widget_get_ancestor(existing->Widget, GTK_TYPE_WINDOW);
				if (existing->Type != WIDGET_OFFSCREENWINDOW && window != NULL)
					gtk_window_present(GTK_WINDOW(window));
				else if (existing->Type != WIDGET_OFFSCREENWINDOW &&
					GTK_IS_MENU(existing->Widget))
					gtk_menu_popup(GTK_MENU(existing->Widget), NULL, NULL,
						NULL, NULL, 0, gtk_get_current_event_time());
				else if (existing->Type != WIDGET_OFFSCREENWINDOW)
					gtkdialog_warning("%s(): %s is not a presentable window "
						"or popup menu.", __func__, string);

			} else {

				/* Get the program source from the envvar and initialise
				 * everything necessary ready for a new parse */
				get_program_from_variable(string);

#ifdef DEBUG
				fprintf(stderr, "%s():\ncharsreaded=%i\nprogram_src=%s\n",
					__func__, charsreaded, program_src);
#endif

				/* Export all variables */
				variables_export_all();

#ifdef DEBUG
				fprintf(stderr, "%s():\ncharsreaded=%i\nprogram_src=%s\n",
					__func__, charsreaded, program_src);
#endif

				/* Parse and build the new window in the existing GTK loop. */
				gtkdialog_parse();
				return TRUE;

			}

		} else {
			fprintf(stderr, "%s(): It's not possible to launch %s at this time: \
try launching after the originating window has fully loaded.\n",
				__func__, string);
		}

	} else {
		fprintf(stderr, "%s(): It's not possible to launch %s from within a timer: \
try launching from a checkbox which is activated by the timer.\n",
			__func__, string);
	}

	return FALSE;
}

/***********************************************************************
 * Action exit                                                         *
 ***********************************************************************/

void action_exitprogram(GtkWidget *widget, char *string)
{
	gchar *escaped;

	print_variables(NULL);

	if (string[0] == '=')
		/* Thunor: An interesting undocumented feature temp temp */
		printf("EXIT%s", string);
	else {
		escaped = variables_shell_escape(string);
		printf("EXIT=\"%s\"\n", escaped);
		g_free(escaped);
	}

#ifdef DEBUG
	fprintf(stderr, "%s(): Calling exit(EXIT_SUCCESS)\n", __func__);
#endif

	exit(EXIT_SUCCESS);
}

/***********************************************************************
 * Action refresh                                                      *
 ***********************************************************************/

void action_refreshwidget(GtkWidget *widget, char *string)
{
	variables_refresh(string);
}

static void action_reparentwidget_mark_destroyed(GtkWidget *widget,
	gboolean *destroyed)
{
	(void)widget;
	*destroyed = TRUE;
}

/* Move an existing direct child without rebuilding its variable or actions. */
void action_reparentwidget(GtkWidget *widget, char *string)
{
	gchar **parts = g_strsplit(string, ":", 3);
	variable *child_var;
	variable *destination_var;
	GtkWidget *child, *destination, *parent;
	GtkWidget *label;
	gchar *label_text = NULL;
	gboolean expand = TRUE, fill = TRUE;
	gboolean child_destroyed = FALSE, destination_destroyed = FALSE;
	guint padding = 0;
	GtkPackType pack_type = GTK_PACK_START;
	gulong child_destroy_handler, destination_destroy_handler;

	(void)widget;
	if (parts[0] == NULL || *parts[0] == '\0' ||
		parts[1] == NULL || *parts[1] == '\0') {
		gtkdialog_warning("reparentwidget requires CHILD:DESTINATION[:TAB_LABEL].");
		goto done;
	}
	child_var = variables_get_by_name(parts[0]);
	destination_var = variables_get_by_name(parts[1]);
	if (child_var == NULL || destination_var == NULL ||
		child_var->Widget == NULL || destination_var->Widget == NULL) {
		gtkdialog_warning("reparentwidget requires two live widget variables.");
		goto done;
	}
	child = child_var->Widget;
	destination = destination_var->Widget;
	parent = gtk_widget_get_parent(child);
	if (child == destination ||
		child_var->window_id != destination_var->window_id ||
		parent == NULL ||
		(!GTK_IS_BOX(parent) && !GTK_IS_NOTEBOOK(parent)) ||
		(!GTK_IS_BOX(destination) && !GTK_IS_NOTEBOOK(destination)) ||
		gtk_widget_is_ancestor(destination, child)) {
		gtkdialog_warning("reparentwidget requires a direct box/notebook child "
			"and a box/notebook in the same window.");
		goto done;
	}
	if (parent == destination)
		goto done;
	if (GTK_IS_BOX(parent))
		gtk_box_query_child_packing(GTK_BOX(parent), child, &expand, &fill,
			&padding, &pack_type);
	if (GTK_IS_NOTEBOOK(destination))
		label_text = g_strdup(parts[2] != NULL ? parts[2] : child_var->Name);

	/* Removal may emit callbacks that close either widget's window. */
	g_object_ref(child);
	g_object_ref(destination);
	child_destroy_handler = g_signal_connect(child, "destroy",
		G_CALLBACK(action_reparentwidget_mark_destroyed), &child_destroyed);
	destination_destroy_handler = g_signal_connect(destination, "destroy",
		G_CALLBACK(action_reparentwidget_mark_destroyed),
		&destination_destroyed);
	gtk_container_remove(GTK_CONTAINER(parent), child);
	if (!child_destroyed && !destination_destroyed) {
		if (GTK_IS_NOTEBOOK(destination)) {
			label = program_notebook_tab_label(child, label_text,
				g_object_get_data(G_OBJECT(child),
					"gtkdialog-tab-instance"),
				g_object_get_data(G_OBJECT(destination),
					"gtkdialog-tab-close-buttons"),
				g_object_get_data(G_OBJECT(destination),
					"gtkdialog-tab-close-action"));
			gtk_notebook_append_page(GTK_NOTEBOOK(destination), child, label);
		} else if (pack_type == GTK_PACK_END) {
			gtk_box_pack_end(GTK_BOX(destination), child, expand, fill,
				padding);
		} else {
			gtk_box_pack_start(GTK_BOX(destination), child, expand, fill,
				padding);
		}
	}
	if (g_signal_handler_is_connected(child, child_destroy_handler))
		g_signal_handler_disconnect(child, child_destroy_handler);
	if (g_signal_handler_is_connected(destination, destination_destroy_handler))
		g_signal_handler_disconnect(destination, destination_destroy_handler);
	g_object_unref(destination);
	g_object_unref(child);
	g_free(label_text);

done:
	g_strfreev(parts);
}

/* Rename a live notebook page without replacing its content widget. */
void action_settablabel(GtkWidget *widget, char *string)
{
	gchar **parts = g_strsplit(string, ":", 2);
	variable *page_var;
	GtkWidget *page, *parent;

	(void)widget;
	if (parts[0] == NULL || *parts[0] == '\0' || parts[1] == NULL) {
		gtkdialog_warning("settablabel requires PAGE:LABEL.");
		goto done;
	}
	page_var = variables_get_by_name(parts[0]);
	if (page_var == NULL || page_var->Widget == NULL) {
		gtkdialog_warning("settablabel requires a live page variable.");
		goto done;
	}
	page = page_var->Widget;
	parent = gtk_widget_get_parent(page);
	if (!GTK_IS_NOTEBOOK(parent)) {
		gtkdialog_warning("settablabel requires a direct notebook page.");
		goto done;
	}
	program_notebook_set_tab_label(parent, page, parts[1]);

done:
	g_strfreev(parts);
}

/* Reorder an existing page while keeping its content and selection. */
void action_reorderpage(GtkWidget *widget, char *string)
{
	gchar **parts = g_strsplit(string, ":", 2);
	variable *page_var;
	GtkWidget *page, *parent;
	gint current, last, position;

	(void)widget;
	if (parts[0] == NULL || *parts[0] == '\0' ||
		parts[1] == NULL || *parts[1] == '\0') {
		gtkdialog_warning("reorderpage requires PAGE:POSITION.");
		goto done;
	}
	page_var = variables_get_by_name(parts[0]);
	if (page_var == NULL || page_var->Widget == NULL) {
		gtkdialog_warning("reorderpage requires a live page variable.");
		goto done;
	}
	page = page_var->Widget;
	parent = gtk_widget_get_parent(page);
	if (!GTK_IS_NOTEBOOK(parent)) {
		gtkdialog_warning("reorderpage requires a direct notebook page.");
		goto done;
	}
	current = gtk_notebook_page_num(GTK_NOTEBOOK(parent), page);
	last = gtk_notebook_get_n_pages(GTK_NOTEBOOK(parent)) - 1;
	position = widget_parse_bounded_integer(parts[1], 0, last, current,
		"notebook reorder position");
	if (position != current)
		gtk_notebook_reorder_child(GTK_NOTEBOOK(parent), page, position);

done:
	g_strfreev(parts);
}

void action_appendtab(GtkWidget *widget, char *string)
{
	extern int instruction_counter;
	gchar **parts = g_strsplit(string, ":", 4);
	variable *destination;
	const gchar *cursor;
	gchar *previous_program_name;

	(void)widget;
	if (parts[0] == NULL || *parts[0] == '\0' ||
		parts[1] == NULL || *parts[1] == '\0' ||
		parts[2] == NULL || *parts[2] == '\0' ||
		parts[3] == NULL) {
		gtkdialog_warning("appendtab requires TEMPLATE:NOTEBOOK:INSTANCE:LABEL.");
		goto done;
	}
	if (!(g_ascii_isalpha(parts[2][0]) || parts[2][0] == '_')) {
		gtkdialog_warning("appendtab instance must be an identifier.");
		goto done;
	}
	for (cursor = parts[2] + 1; *cursor != '\0'; ++cursor) {
		if (!(g_ascii_isalnum(*cursor) || *cursor == '_')) {
			gtkdialog_warning("appendtab instance must be an identifier.");
			goto done;
		}
	}
	destination = variables_get_by_name(parts[1]);
	if (destination == NULL || destination->Widget == NULL ||
		!GTK_IS_NOTEBOOK(destination->Widget)) {
		gtkdialog_warning("appendtab requires a live notebook variable.");
		goto done;
	}
	if (instruction_counter != 0) {
		gtkdialog_warning("appendtab cannot build a template while another "
			"program is being built.");
		goto done;
	}
	variables_export_all();
	if (!program_set_tab_target(destination->Widget,
			destination->window_id, parts[2], parts[3])) {
		gtkdialog_warning("appendtab cannot build a template now or the "
			"instance is already live.");
		goto done;
	}
	previous_program_name = g_strdup(get_program_name());
	if (!get_program_from_template_variable(parts[0], parts[2])) {
		program_clear_tab_target();
		g_free(previous_program_name);
		goto done;
	}
	gtkdialog_parse();
	set_program_name(previous_program_name);
	g_free(previous_program_name);
	program_clear_tab_target();

done:
	g_strfreev(parts);
}

gboolean action_replacebox(GtkWidget *widget, char *string)
{
	extern int instruction_counter;
	gchar **parts = g_strsplit(string, ":", 3);
	variable *destination;
	const gchar *cursor;
	gchar *previous_program_name;
	gboolean source_will_be_removed = FALSE;
	gboolean attached = FALSE;

	if (parts[0] == NULL || *parts[0] == '\0' ||
		parts[1] == NULL || *parts[1] == '\0' ||
		parts[2] == NULL || *parts[2] == '\0') {
		gtkdialog_warning("replacebox requires TEMPLATE:BOX:INSTANCE.");
		goto done;
	}
	if (!(g_ascii_isalpha(parts[2][0]) || parts[2][0] == '_')) {
		gtkdialog_warning("replacebox instance must be an identifier.");
		goto done;
	}
	for (cursor = parts[2] + 1; *cursor != '\0'; ++cursor) {
		if (!(g_ascii_isalnum(*cursor) || *cursor == '_')) {
			gtkdialog_warning("replacebox instance must be an identifier.");
			goto done;
		}
	}
	destination = variables_get_by_name(parts[1]);
	if (destination == NULL || destination->Widget == NULL ||
		(destination->Type != WIDGET_HBOX &&
		 destination->Type != WIDGET_VBOX)) {
		gtkdialog_warning("replacebox requires a live box variable.");
		goto done;
	}
	if (GTK_IS_WIDGET(widget))
		source_will_be_removed = gtk_widget_is_ancestor(widget,
			destination->Widget);
	if (instruction_counter != 0) {
		gtkdialog_warning("replacebox cannot build a template while another "
			"program is being built.");
		goto done;
	}
	variables_export_all();
	if (!program_set_box_target(destination->Widget,
			destination->window_id, parts[2])) {
		gtkdialog_warning("replacebox cannot build a template now or the "
			"instance is already live.");
		goto done;
	}
	previous_program_name = g_strdup(get_program_name());
	if (!get_program_from_template_variable(parts[0], parts[2])) {
		program_clear_tab_target();
		g_free(previous_program_name);
		goto done;
	}
	gtkdialog_parse();
	attached = program_fragment_was_attached();
	set_program_name(previous_program_name);
	g_free(previous_program_name);
	program_clear_tab_target();

done:
	g_strfreev(parts);
	return source_will_be_removed && attached;
}

void action_hangupterminal(GtkWidget *widget, char *string)
{
	variable *target = variables_get_by_name(string);

	(void)widget;
	if (target == NULL || target->Type != WIDGET_TERMINAL ||
		target->Widget == NULL) {
		gtkdialog_warning("hangupterminal requires a live terminal variable: %s",
			string);
		return;
	}
	(void)widget_terminal_hangup(target->Widget);
}

gboolean action_removetab(GtkWidget *widget, char *string)
{
	gboolean source_removed;

	if (!program_remove_tab_instance(string, widget, &source_removed)) {
		gtkdialog_warning("removetab requires a live template instance "
			"in a notebook.");
		return FALSE;
	}
	return source_removed;
}

void action_selecttab(GtkWidget *widget, char *string)
{
	(void)widget;
	if (!program_select_tab_instance(string))
		gtkdialog_warning("selecttab requires a live template instance "
			"in a notebook.");
}

void action_appendtabauto(GtkWidget *widget, char *string)
{
	static guint64 next_instance = 0;
	gchar **parts = g_strsplit(string, ":", 3);
	gchar *instance = NULL;
	gchar *number;
	gchar **label_parts;
	gchar *label;
	gchar *command;

	if (parts[0] == NULL || *parts[0] == '\0' ||
		parts[1] == NULL || *parts[1] == '\0' || parts[2] == NULL) {
		gtkdialog_warning("appendtabauto requires TEMPLATE:NOTEBOOK:LABEL.");
		goto done;
	}
	do {
		g_clear_pointer(&instance, g_free);
		if (next_instance == G_MAXUINT64) {
			gtkdialog_warning("appendtabauto instance counter exhausted.");
			goto done;
		}
		instance = g_strdup_printf("GTKDIALOG_TAB_%" G_GUINT64_FORMAT,
			++next_instance);
	} while (program_tab_instance_is_live(instance));
	number = g_strdup_printf("%" G_GUINT64_FORMAT, next_instance);
	label_parts = g_strsplit(parts[2], "@INDEX@", -1);
	label = g_strjoinv(number, label_parts);
	command = g_strdup_printf("%s:%s:%s:%s", parts[0], parts[1],
		instance, label);
	action_appendtab(widget, command);
	if (program_tab_instance_is_live(instance))
		program_select_tab_instance(instance);
	g_free(command);
	g_free(label);
	g_strfreev(label_parts);
	g_free(number);

done:
	g_free(instance);
	g_strfreev(parts);
}

/***********************************************************************
 * Action save                                                         *
 ***********************************************************************/

void action_savewidget(GtkWidget *widget, char *string)
{
	variables_save(string);
}

/***********************************************************************
 * Action fileselect                                                   *
 ***********************************************************************/
/* This function will create and open a fileselection dialog and
 * connect it to the action_fileselection_made function to take
 * the selected file name */

void action_fileselect(GtkWidget *widget, char *string)
{
	list_t                 *mime_types = NULL;
	list_t                 *patterns = NULL;
	variable               *var = NULL;
	GList                  *element;
	GtkFileChooserAction    action = GTK_FILE_CHOOSER_ACTION_OPEN;
	GtkFileFilter          *filter;
	GtkWidget              *chooser;
	gchar                  *filename = NULL;
	gchar                  *title = "Gtkdialog";
	gchar                  *value;
	gint                    count;
	gint                    response;

#ifdef DEBUG
	fprintf(stderr, "%s(): string='%s'\n", __func__, string);
#endif

	var = variables_get_by_name(string);
	if (var != NULL && var->Widget != NULL && var->Attributes != NULL) {

		/* Set the title of the chooser dialog to the label directive
		 * of the target widget if available (this was the original
		 * behaviour that although used in the 16.00-fileselect example
		 * it is undocumented so possibly few people have realised this) */
		if (attributeset_is_avail(var->Attributes, ATTR_LABEL)) {
			title = attributeset_get_first(&element, var->Attributes, ATTR_LABEL);
#ifdef DEBUG
			fprintf(stderr, "%s(): title='%s' from label directive\n",
				__func__, title);
#endif
		}

		if (var->widget_tag_attr) {
			/* Set title if present */
			if ((value = get_tag_attribute(var->widget_tag_attr, "fs-title"))) {
				title = value;
#ifdef DEBUG
				fprintf(stderr, "%s(): title='%s' from tag attribute\n",
					__func__, title);
#endif
			}
			/* Set file chooser action if present */
			if ((value = get_tag_attribute(var->widget_tag_attr, "fs-action")) ||
				(value = get_tag_attribute(var->widget_tag_attr, "accept"))) {	/* Deprecated */				
				if ((strcasecmp(value, "file") == 0) ||
					(strcasecmp(value, "filename") == 0)) {			/* Deprecated */
					action = GTK_FILE_CHOOSER_ACTION_OPEN;
				} else if ((strcasecmp(value, "newfile") == 0) ||
					(strcasecmp(value, "savefilename") == 0)) {		/* Deprecated */
					action = GTK_FILE_CHOOSER_ACTION_SAVE;
				} else if ((strcasecmp(value, "folder") == 0) ||
					(strcasecmp(value, "directory") == 0)) {		/* Deprecated */
					action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
				} else if ((strcasecmp(value, "newfolder") == 0) ||
					(strcasecmp(value, "newdirectory") == 0)) {		/* Deprecated */
					action = GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER;
				}
#ifdef DEBUG
				fprintf(stderr, "%s(): action=%i\n", __func__, action);
#endif
			}
		}

		/* Create the file chooser dialog */
		chooser = gtk_file_chooser_dialog_new(title, NULL, action,
			GTK_STOCK_OK, GTK_RESPONSE_OK,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

		/* Set current folder if present */
		if (var->widget_tag_attr &&
			((value = get_tag_attribute(var->widget_tag_attr, "fs-folder")))) {
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(chooser), value);
#ifdef DEBUG
			fprintf(stderr, "%s(): current folder='%s'\n", __func__, value);
#endif
		}

		/* Add file filters */
		if (var->widget_tag_attr) {
			/* Filters of type pattern */
			if ((value = get_tag_attribute(var->widget_tag_attr, "fs-filters"))) {
				patterns = linecutter(g_strdup(value), '|');
				for (count = 0; count < patterns->n_lines; count++) {
					filter = gtk_file_filter_new();
					gtk_file_filter_set_name(filter, patterns->line[count]);
					gtk_file_filter_add_pattern(filter, patterns->line[count]);
					gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
#ifdef DEBUG
					fprintf(stderr, "%s(): patterns->line[count]='%s'\n",
						__func__, patterns->line[count]);
#endif
				}
			}
			/* Filters of type mime */
			if ((value = get_tag_attribute(var->widget_tag_attr, "fs-filters-mime"))) {
				mime_types = linecutter(g_strdup(value), '|');
				for (count = 0; count < mime_types->n_lines; count++) {
					filter = gtk_file_filter_new();
					gtk_file_filter_set_name(filter, mime_types->line[count]);
					gtk_file_filter_add_mime_type(filter, mime_types->line[count]);
					gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
#ifdef DEBUG
					fprintf(stderr, "%s(): mime_types->line[count]='%s'\n",
						__func__, mime_types->line[count]);
#endif
				}
			}
		}

		/* Add an "All files" "*" filter which will become the
		 * default if no user filters were added above */
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, "All files");
		gtk_file_filter_add_pattern(filter, "*");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

		/* Run the file chooser dialog and get the response.
		 * Note that accept="newdirectory" (fs-action="newfolder")
		 * currently crashes the file chooser dialog if nothing is
		 * selected or created which is mentioned in the example */
		response = gtk_dialog_run(GTK_DIALOG(chooser));

#ifdef DEBUG
		fprintf(stderr, "%s(): response=%i\n", __func__, response);
#endif

		/* If the user pressed the OK button we set the target widget */
		switch (response) {
			case GTK_RESPONSE_CANCEL:
				/* Nothing to do */
				break;
			case GTK_RESPONSE_OK:
				filename = gtk_file_chooser_get_filename(
					GTK_FILE_CHOOSER(chooser));
#ifdef DEBUG
				fprintf(stderr, "%s(): filename=%s\n", __func__,
					filename ? filename : "(none)");
#endif
				if (filename) {
					variables_set_value(string, filename);
					g_free(filename);
				}
				break;
		}

		/* Free linecutter memory */
		if (patterns) list_t_free(patterns);
		if (mime_types) list_t_free(mime_types);

		/* Destroy the file chooser dialog */
		gtk_widget_destroy(chooser);
	}

}


/***********************************************************************
 *                                                                     *
 ***********************************************************************/
/* This function is called when the user presses the Cancel button in
 * a fileselection dialog */

void action_fileselection_destroy(GtkWidget *w, actioncommand *ac)
{
	/*
	 ** We destroy the fileselection dialog and free the actioncommand
	 ** memory.
	 */
	gtk_widget_destroy(ac->source_widget);
	g_free(ac->destination_name);
	g_free(ac);
}

/***********************************************************************
 * Action clear                                                        *
 ***********************************************************************/
/* This action removes all elements from a widget */

void action_clearwidget(GtkWidget *widget, char *string)
{
	variables_clear(string);
}

/***********************************************************************
 * Action removeselected                                               *
 ***********************************************************************/
/* This action removes a selected element from a widget */

void action_removeselected(GtkWidget *widget, char *string)
{
	remove_selected_variable(string);
}

/***********************************************************************
 * Tree expansion                                                      *
 ***********************************************************************/

void action_expandall(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_expand_all(string);
}

void action_collapseall(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_collapse_all(string);
}

/***********************************************************************
 * Action enable                                                       *
 ***********************************************************************/

void action_enable(GtkWidget *widget, char *string)
{
	variables_enable(string);
}

/***********************************************************************
 * Action disable                                                      *
 ***********************************************************************/

void action_disable(GtkWidget *widget, char *string)
{
	variables_disable(string);
}

/***********************************************************************
 * Action show                                                         *
 ***********************************************************************/

void action_show(GtkWidget *widget, char *string)
{
	variables_show(string);
}

/***********************************************************************
 * Action hide                                                         *
 ***********************************************************************/

void action_hide(GtkWidget *widget, char *string)
{
	variables_hide(string);
}

/***********************************************************************
 * Action activate                                                     *
 ***********************************************************************/

void action_activate(GtkWidget *widget, char *string)
{
	variables_activate(string);
}

/***********************************************************************
 * Action grabfocus                                                    *
 ***********************************************************************/

void action_grabfocus(GtkWidget *widget, char *string)
{
	variables_grabfocus(string);
}

/***********************************************************************
 * Action presentwindow                                                *
 ***********************************************************************/

void action_presentwindow(GtkWidget *widget, char *string)
{
	variables_presentwindow(string);
}

/***********************************************************************
 * Action command                                                      *
 ***********************************************************************/
/* This fuction will export variables and run a shell command */

void action_shellcommand(GtkWidget *widget, char *string)
{
	char *command;
	char *quoted_include;
	int result;

	variables_export_all();

	if (option_include_file == NULL) {

		result = system(string);
		if (result == -1)
			gtkdialog_warning("Failed to execute shell command '%s': %s",
				string, g_strerror(errno));
		else
			widget_report_shell_status(string, result);

	} else {

		/* Debian 01_bashism patch: use dot rather than source.
		command = g_strdup_printf("source %s; %s", */
		quoted_include = g_shell_quote(option_include_file);
		command = g_strdup_printf(". %s; %s", quoted_include, string);
		result = system(command);
		if (result == -1)
			gtkdialog_warning("Failed to execute shell command '%s': %s",
				command, g_strerror(errno));
		else
			widget_report_shell_status(command, result);
		g_free(quoted_include);
		g_free(command);

	}

}

/***********************************************************************
 * Source editing history                                               *
 ***********************************************************************/

void action_edit_undo(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_edit_undo(string);
}

void action_edit_redo(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_edit_redo(string);
}

/***********************************************************************
 * Image-view scaling                                                  *
 ***********************************************************************/

void action_imageview_zoomin(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_imageview_zoom_in(string);
}

void action_imageview_zoomout(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_imageview_zoom_out(string);
}

void action_imageview_zoomreset(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_imageview_zoom_reset(string);
}

void action_imageview_fit(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_imageview_fit(string);
}

void action_imageview_fitwidth(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_imageview_fit_width(string);
}

void action_imageview_fitheight(GtkWidget *widget, char *string)
{
	(void)widget;
	variables_imageview_fit_height(string);
}

/***********************************************************************
 * Execute Action                                                      *
 ***********************************************************************/
/* On exit: returns 0 if action function is unknown
 *                  1 if action function is valid
 *                  2 if this and any calling action sequence must stop */

int execute_action(GtkWidget *widget, const char *command, const char *type)
{
	static guint64    launch_serial = 0;
	gchar            *command_prefix;
	gchar            *command_string;
	gint              retval = 1;
	guint64           initial_launch_serial = launch_serial;
	CommandType       t;

	/* Thunor: I've re-engineered my signal blocking mechanism.
	 * All signal callbacks end up here, so if the global flag is
	 * true then the action functions simply don't get executed */
	if (!function_signals_block) {

		if (type == NULL || g_utf8_strlen(type, -1) == 0) {
			command_get_prefix(command, &command_prefix, &command_string);
		} else {
			command_prefix = g_strdup(type);
			command_string = g_strdup(command);
		}

#ifdef DEBUG
		fprintf(stderr, "%s(): command: '%s' type: '%s'.\n", __func__,
			command_string, command_prefix);
#endif

		t = command_prefix_get_type(command_prefix);
		switch (t) {
			case CommandShellCommand:
				action_shellcommand(widget, command_string);
				break;

			case CommandExit:
				action_exitprogram(widget, command_string);
				break;

			case CommandCloseWindow:
				action_closewindow(widget, command_string);
				break;

			case CommandLaunch:
				/* A newly built window historically kept this action callback
				 * inside a nested GTK loop, so later synchronous actions did
				 * not run. Record it so nested action callbacks propagate the
				 * stop back through their callers. */
				if (action_launchwindow(widget, command_string))
					++launch_serial;
				break;

			case CommandEnable:
				action_enable(widget, command_string);
				break;

			case CommandDisable:
				action_disable(widget, command_string);
				break;

			case CommandShow:
				action_show(widget, command_string);
				break;

			case CommandHide:
				action_hide(widget, command_string);
				break;

			case CommandActivate:
				action_activate(widget, command_string);
				break;

			case CommandGrabFocus:
				action_grabfocus(widget, command_string);
				break;

			case CommandPresentWindow:
				action_presentwindow(widget, command_string);
				break;

			case CommandRefresh:
				variables_export_all();
				action_refreshwidget(widget, command_string);
				break;

			case CommandSave:
				action_savewidget(widget, command_string);
				break;

			case CommandFileSelect:
				action_fileselect(widget, command_string);
				break;

			case CommandClear:
				action_clearwidget(widget, command_string);
				break;

			case CommandRemoveSelected:
				action_removeselected(widget, command_string);
				break;

			case CommandExpandAll:
				action_expandall(widget, command_string);
				break;

			case CommandCollapseAll:
				action_collapseall(widget, command_string);
				break;

			case CommandUndo:
				action_edit_undo(widget, command_string);
				break;

			case CommandRedo:
				action_edit_redo(widget, command_string);
				break;

			case CommandZoomIn:
				action_imageview_zoomin(widget, command_string);
				break;

			case CommandZoomOut:
				action_imageview_zoomout(widget, command_string);
				break;

			case CommandZoomReset:
				action_imageview_zoomreset(widget, command_string);
				break;

			case CommandFit:
				action_imageview_fit(widget, command_string);
				break;

			case CommandFitWidth:
				action_imageview_fitwidth(widget, command_string);
				break;

			case CommandFitHeight:
				action_imageview_fitheight(widget, command_string);
				break;

			case CommandReparentWidget:
				action_reparentwidget(widget, command_string);
				break;

			case CommandSetTabLabel:
				action_settablabel(widget, command_string);
				break;

			case CommandReorderPage:
				action_reorderpage(widget, command_string);
				break;

			case CommandAppendTab:
				action_appendtab(widget, command_string);
				break;

			case CommandRemoveTab:
				if (action_removetab(widget, command_string))
					retval = 2;
				break;

			case CommandSelectTab:
				action_selecttab(widget, command_string);
				break;

			case CommandAppendTabAuto:
				action_appendtabauto(widget, command_string);
				break;

			case CommandReplaceBox:
				if (action_replacebox(widget, command_string))
					retval = 2;
				break;

			case CommandHangupTerminal:
				action_hangupterminal(widget, command_string);
				break;

			case CommandBreak:
				retval = 2;
				break;

			case CommandLoadStyles:
				load_styles_file(command_string);
				retval = 0;
				break;

			/* Reserved legacy functions. They have always been no-ops in the
			 * compiled implementation, so retain that observable behavior. */
			case CommandInsert:
				retval = 0;
				break;

			case CommandAppend:
				retval = 0;
				break;

			default:
				gtkdialog_warning("%s(): Unknown action function '%s'", __func__,
					command_prefix);
				retval = 0;
				break;
		}
			
		g_free(command_prefix);
		g_free(command_string);
	}
	if (launch_serial != initial_launch_serial)
		retval = 2;

	return retval;
}
