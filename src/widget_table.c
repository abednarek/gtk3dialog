/*
 * widget_table.c: GtkTreeView backend for the historical <table> widget.
 * Gtkdialog - A small utility for fast and easy GUI building.
 * Copyright (C) 2003-2007  László Pere <pipas@linux.pte.hu>
 * Copyright (C) 2011-2012  Thunor <thunorsif@hotmail.com>
 * Copyright (C) 2026       Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <string.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "widgets.h"
#include "signals.h"
#include "stringman.h"
#include "tag_attributes.h"

#define TABLE_COLUMN_COUNT "gtkdialog-table-column-count"
#define TABLE_FILTER_DATA "gtkdialog-table-filter-data"

typedef struct {
	gint column;
	gboolean sensitive;
} TableSortData;

typedef struct {
	GString *values;
	gint column;
	gboolean first;
} TableSelectionData;

typedef struct {
	GtkTreeModelFilter *model;
	gchar *entry_name;
	gchar *query;
	gint column_count;
	gboolean bound;
} TableFilterData;

static void widget_table_input_by_command(variable *var, const gchar *source,
	gboolean command);
static void widget_table_input_by_file(variable *var, const gchar *filename);
static void widget_table_input_by_items(variable *var);
static void widget_table_append_row(GtkWidget *widget, gchar **values);

static gint
widget_table_column_count(GtkWidget *widget)
{
	return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget),
		TABLE_COLUMN_COUNT));
}

static GtkListStore *
widget_table_store(GtkWidget *widget)
{
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
	if (GTK_IS_TREE_MODEL_SORT(model))
		model = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));
	if (GTK_IS_TREE_MODEL_FILTER(model))
		model = gtk_tree_model_filter_get_model(GTK_TREE_MODEL_FILTER(model));
	return GTK_LIST_STORE(model);
}

static void widget_table_filter_data_free(gpointer data)
{
	TableFilterData *filter = data;
	g_free(filter->entry_name);
	g_free(filter->query);
	g_free(filter);
}

static gboolean widget_table_visible(GtkTreeModel *model, GtkTreeIter *iter,
	gpointer user_data)
{
	TableFilterData *filter = user_data;
	gint column;
	if (filter->query == NULL || *filter->query == '\0')
		return TRUE;
	for (column = 0; column < filter->column_count; ++column) {
		gchar *text = NULL;
		gchar *folded;
		gboolean matched;
		gtk_tree_model_get(model, iter, column, &text, -1);
		folded = g_utf8_casefold(text != NULL ? text : "", -1);
		matched = strstr(folded, filter->query) != NULL;
		g_free(folded);
		g_free(text);
		if (matched)
			return TRUE;
	}
	return FALSE;
}

static void widget_table_filter_entry_changed(GtkEntry *entry,
	GtkWidget *widget)
{
	TableFilterData *filter = g_object_get_data(G_OBJECT(widget),
		TABLE_FILTER_DATA);
	g_free(filter->query);
	filter->query = g_utf8_casefold(gtk_entry_get_text(entry), -1);
	gtk_tree_model_filter_refilter(filter->model);
}

static void widget_table_filter_bind(GtkWidget *widget, gpointer unused)
{
	TableFilterData *filter = g_object_get_data(G_OBJECT(widget),
		TABLE_FILTER_DATA);
	variable *entry;
	(void)unused;
	if (filter->bound)
		return;
	entry = variables_get_by_name(filter->entry_name);
	if (entry == NULL || !GTK_IS_ENTRY(entry->Widget)) {
		if (gtk_widget_get_realized(widget))
			gtkdialog_warning("Cannot bind <table> filter to entry '%s'.",
				filter->entry_name);
		return;
	}
	filter->bound = TRUE;
	g_signal_connect_object(entry->Widget, "changed",
		G_CALLBACK(widget_table_filter_entry_changed), widget, 0);
	widget_table_filter_entry_changed(GTK_ENTRY(entry->Widget), widget);
}

static GtkTreePath *widget_table_store_path(GtkWidget *widget,
	GtkTreePath *view_path)
{
	TableFilterData *filter = g_object_get_data(G_OBJECT(widget),
		TABLE_FILTER_DATA);
	GtkTreeModel *view_model;
	GtkTreePath *filter_path;
	GtkTreePath *store_path;
	if (filter == NULL)
		return gtk_tree_path_copy(view_path);
	view_model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
	filter_path = gtk_tree_model_sort_convert_path_to_child_path(
		GTK_TREE_MODEL_SORT(view_model), view_path);
	if (filter_path == NULL)
		return NULL;
	store_path = gtk_tree_model_filter_convert_path_to_child_path(
		filter->model, filter_path);
	gtk_tree_path_free(filter_path);
	return store_path;
}

static gint
widget_table_natural_compare(GtkTreeModel *model, GtkTreeIter *left,
	GtkTreeIter *right, gpointer user_data)
{
	TableSortData *data = user_data;
	gchar *left_text;
	gchar *right_text;
	gint result;

	gtk_tree_model_get(model, left, data->column, &left_text, -1);
	gtk_tree_model_get(model, right, data->column, &right_text, -1);
	result = strnatcmp(left_text != NULL ? left_text : "",
		right_text != NULL ? right_text : "", data->sensitive);
	g_free(left_text);
	g_free(right_text);
	return result;
}

static void
widget_table_selection_changed(GtkTreeSelection *selection,
	gpointer user_data)
{
	GtkTreeView *view = gtk_tree_selection_get_tree_view(selection);

	on_any_widget_select_row_event(GTK_WIDGET(view), -1, -1, NULL,
		user_data);
}

void
widget_table_clear(variable *var)
{
	gtk_list_store_clear(widget_table_store(var->Widget));
}

GtkWidget *
widget_table_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GList *element;
	GType *types;
	GtkCellRenderer *renderer;
	GtkListStore *store;
	GtkSelectionMode selection_mode = GTK_SELECTION_SINGLE;
	GtkTreeSelection *selection;
	GtkTreeViewColumn *view_column;
	GtkWidget *widget;
	GtkSortType sort_type = GTK_SORT_ASCENDING;
	gchar *labels_text;
	gchar *value;
	gint column;
	gint column_count;
	gint sort_column = 0;
	gint sort_function = 0;
	gboolean auto_sort = FALSE;
	gboolean has_titles;
	list_t *labels;
	list_t *sliced;

	(void)Type;
	has_titles = attributeset_is_avail(Attr, ATTR_LABEL);
	labels_text = has_titles
		? g_strdup(attributeset_get_first(&element, Attr, ATTR_LABEL))
		: g_strdup("");
	labels = linecutter(labels_text, '|');
	column_count = MAX(1, labels->n_lines);
	types = g_new(GType, column_count);
	for (column = 0; column < column_count; ++column)
		types[column] = G_TYPE_STRING;
	store = gtk_list_store_newv(column_count, types);
	g_free(types);
	widget = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);
	g_object_set_data(G_OBJECT(widget), TABLE_COLUMN_COUNT,
		GINT_TO_POINTER(column_count));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(widget), has_titles);

	if (attr != NULL) {
		if ((value = get_tag_attribute(attr, "selection-mode")) != NULL) {
			/* GtkCList rejected GTK_SELECTION_NONE; keep that XML contract. */
			selection_mode = widget_parse_selection_mode(value, FALSE,
				"table selection-mode");
			kill_tag_attribute(attr, "selection-mode");
		}
		if ((value = get_tag_attribute(attr, "sort-function")) != NULL)
			sort_function = widget_parse_bounded_integer(value, 0, 2, 0,
				"table sort-function");
		if ((value = get_tag_attribute(attr, "sort-type")) != NULL)
			sort_type = widget_parse_sort_type(value, "table sort-type");
		if ((value = get_tag_attribute(attr, "sort-column")) != NULL)
			sort_column = widget_parse_column_index(value, column_count,
				"table sort-column");
		if ((value = get_tag_attribute(attr, "auto-sort")) != NULL)
			auto_sort = widget_attribute_is_true(value);
	}

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	gtk_tree_selection_set_mode(selection, selection_mode);
	for (column = 0; column < column_count; ++column) {
		renderer = gtk_cell_renderer_text_new();
		view_column = gtk_tree_view_column_new_with_attributes(
			column < labels->n_lines ? labels->line[column] : "",
			renderer, "text", column, NULL);
		gtk_tree_view_column_set_resizable(view_column, TRUE);
		gtk_tree_view_append_column(GTK_TREE_VIEW(widget), view_column);
		if (sort_function != 0) {
			TableSortData *sort_data = g_new(TableSortData, 1);
			sort_data->column = column;
			sort_data->sensitive = sort_function == 1;
			gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(store), column,
				widget_table_natural_compare, sort_data, g_free);
		}
	}

	if (attr != NULL &&
		(value = get_tag_attribute(attr, "column-header-active")) != NULL) {
		sliced = linecutter(g_strdup(value), '|');
		for (column = 0; column < sliced->n_lines &&
			column < column_count; ++column) {
			view_column = gtk_tree_view_get_column(GTK_TREE_VIEW(widget), column);
			if (widget_attribute_is_true(sliced->line[column]))
				gtk_tree_view_column_set_sort_column_id(view_column, column);
			else
				gtk_tree_view_column_set_clickable(view_column, FALSE);
		}
		list_t_free(sliced);
	}
	if (attr != NULL &&
		(value = get_tag_attribute(attr, "column-visible")) != NULL) {
		sliced = linecutter(g_strdup(value), '|');
		for (column = 0; column < sliced->n_lines &&
			column < column_count; ++column) {
			view_column = gtk_tree_view_get_column(GTK_TREE_VIEW(widget), column);
			gtk_tree_view_column_set_visible(view_column,
				widget_attribute_is_true(sliced->line[column]));
		}
		list_t_free(sliced);
	}
	if (auto_sort && sort_column >= 0)
		gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store),
			sort_column, sort_type);
	if (attr != NULL &&
		(value = get_tag_attribute(attr, "filter-entry")) != NULL) {
		if (*value != '\0') {
			TableFilterData *filter = g_new0(TableFilterData, 1);
			GtkTreeModel *filter_model;
			GtkTreeModel *sort_model;
			filter->entry_name = g_strdup(value);
			filter->column_count = column_count;
			filter_model = gtk_tree_model_filter_new(GTK_TREE_MODEL(store),
				NULL);
			filter->model = GTK_TREE_MODEL_FILTER(filter_model);
			gtk_tree_model_filter_set_visible_func(filter->model,
				widget_table_visible, filter, NULL);
			sort_model = gtk_tree_model_sort_new_with_model(filter_model);
			if (sort_function != 0) {
				for (column = 0; column < column_count; ++column) {
					TableSortData *sort_data = g_new(TableSortData, 1);
					sort_data->column = column;
					sort_data->sensitive = sort_function == 1;
					gtk_tree_sortable_set_sort_func(
						GTK_TREE_SORTABLE(sort_model), column,
						widget_table_natural_compare, sort_data, g_free);
				}
			}
			gtk_tree_view_set_model(GTK_TREE_VIEW(widget), sort_model);
			g_object_unref(sort_model);
			g_object_unref(filter_model);
			g_object_set_data_full(G_OBJECT(widget), TABLE_FILTER_DATA,
				filter, widget_table_filter_data_free);
			g_signal_connect(widget, "realize",
				G_CALLBACK(widget_table_filter_bind), NULL);
		} else {
			gtkdialog_warning("<table> filter-entry needs an entry name.");
		}
		kill_tag_attribute(attr, "filter-entry");
	}

	list_t_free(labels);
	return widget;
}

static void
widget_table_collect_selection(GtkTreeModel *model, GtkTreePath *path,
	GtkTreeIter *iter, gpointer user_data)
{
	TableSelectionData *data = user_data;
	gchar *value;

	(void)path;
	gtk_tree_model_get(model, iter, data->column, &value, -1);
	if (!data->first)
		g_string_append_c(data->values, '\n');
	g_string_append(data->values, value != NULL ? value : "");
	data->first = FALSE;
	g_free(value);
}

gchar *
widget_table_envvar_construct(GtkWidget *widget)
{
	GtkTreeSelection *selection;
	TableSelectionData data;
	variable *var = find_variable_by_widget(widget);
	gchar *value;
	gint column = 0;

	if (var != NULL && var->widget_tag_attr != NULL &&
		(value = get_tag_attribute(var->widget_tag_attr,
			"exported-column")) != NULL)
		column = widget_parse_column_index(value,
			widget_table_column_count(widget), "table exported-column");

	data.values = g_string_new(NULL);
	data.column = column;
	data.first = TRUE;
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	gtk_tree_selection_selected_foreach(selection,
		widget_table_collect_selection, &data);
	return g_string_free(data.values, FALSE);
}

void
widget_table_fileselect(variable *var, const char *name, const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	fprintf(stderr, "%s(): Fileselect not implemented for this widget.\n",
		__func__);
}

void
widget_table_refresh(variable *var)
{
	GList *element;
	GtkTreePath *path;
	GtkTreeSelection *selection;
	gchar *act;
	gchar *value;
	gboolean freeze = FALSE;
	gint initialised = FALSE;
	gint selected_row;

	if (g_object_get_data(G_OBJECT(var->Widget), "_initialised") != NULL)
		initialised = GPOINTER_TO_INT(g_object_get_data(
			G_OBJECT(var->Widget), "_initialised"));
	if (var->widget_tag_attr != NULL &&
		(value = get_tag_attribute(var->widget_tag_attr,
			"freeze-thaw")) != NULL && widget_attribute_is_true(value)) {
		freeze = TRUE;
		g_object_freeze_notify(G_OBJECT(widget_table_store(var->Widget)));
	}

	act = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (act != NULL) {
		if (input_is_shell_command(act))
			widget_table_input_by_command(var, act + 8, TRUE);
		if (strncasecmp(act, "file:", 5) == 0 && strlen(act) > 5) {
			if (!initialised)
				widget_file_monitor_try_create(var, act + 5);
			widget_table_input_by_file(var, act + 5);
		}
		act = attributeset_get_next(&element, var->Attributes, ATTR_INPUT);
	}
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		widget_table_input_by_items(var);

	if (!initialised) {
		if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
			fprintf(stderr, "%s(): <default> not implemented for this widget.\n",
				__func__);
		if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
			attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "disabled") ||
			attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
			attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
			gtk_widget_set_sensitive(var->Widget, FALSE);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(var->Widget));
		g_signal_connect(selection, "changed",
			G_CALLBACK(widget_table_selection_changed), var->Attributes);
	}

	if (freeze)
		g_object_thaw_notify(G_OBJECT(widget_table_store(var->Widget)));
	if (!initialised && g_object_get_data(G_OBJECT(var->Widget),
		TABLE_FILTER_DATA) != NULL)
		widget_table_filter_bind(var->Widget, NULL);
	if (var->widget_tag_attr != NULL &&
		(value = get_tag_attribute(var->widget_tag_attr, "selected-row")) != NULL) {
		selected_row = widget_parse_nonnegative_integer(value, -1,
			"table selected-row");
		if (selected_row >= 0) {
			path = gtk_tree_path_new_from_indices(selected_row, -1);
			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(var->Widget));
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_path_free(path);
		}
	}
}

void
widget_table_removeselected(variable *var)
{
	GList *paths;
	GList *item;
	GList *references = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(var->Widget));
	model = GTK_TREE_MODEL(widget_table_store(var->Widget));
	paths = gtk_tree_selection_get_selected_rows(selection, NULL);
	for (item = paths; item != NULL; item = item->next) {
		GtkTreePath *store_path = widget_table_store_path(var->Widget,
			item->data);
		if (store_path != NULL) {
			references = g_list_prepend(references,
				gtk_tree_row_reference_new(model, store_path));
			gtk_tree_path_free(store_path);
		}
		gtk_tree_path_free(item->data);
	}
	g_list_free(paths);
	for (item = references; item != NULL; item = item->next) {
		GtkTreePath *store_path = gtk_tree_row_reference_get_path(item->data);
		if (store_path != NULL &&
			gtk_tree_model_get_iter(model, &iter, store_path))
			gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
		if (store_path != NULL)
			gtk_tree_path_free(store_path);
		gtk_tree_row_reference_free(item->data);
	}
	g_list_free(references);
}

void
widget_table_save(variable *var)
{
	FILE *outfile;
	GList *element;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *act;
	gchar *filename = NULL;
	gchar *text;
	gint column;
	gint column_count;
	gboolean valid;
	gboolean first_row = TRUE;

	act = attributeset_get_first(&element, var->Attributes, ATTR_OUTPUT);
	while (act != NULL) {
		if (strncasecmp(act, "file:", 5) == 0 && strlen(act) > 5) {
			filename = act + 5;
			break;
		}
		act = attributeset_get_next(&element, var->Attributes, ATTR_OUTPUT);
	}
	if (filename == NULL) {
		fprintf(stderr, "%s(): No <output file> directive found.\n", __func__);
		return;
	}
	outfile = fopen(filename, "w");
	if (outfile == NULL) {
		fprintf(stderr, "%s(): Couldn't open '%s' for writing.\n",
			__func__, filename);
		return;
	}

	model = GTK_TREE_MODEL(widget_table_store(var->Widget));
	column_count = widget_table_column_count(var->Widget);
	valid = gtk_tree_model_get_iter_first(model, &iter);
	while (valid) {
		if (!first_row)
			fputc('\n', outfile);
		for (column = 0; column < column_count; ++column) {
			gtk_tree_model_get(model, &iter, column, &text, -1);
			if (column != 0)
				fputc('|', outfile);
			fputs(text != NULL ? text : "", outfile);
			g_free(text);
		}
		first_row = FALSE;
		valid = gtk_tree_model_iter_next(model, &iter);
	}
	widget_close_output(outfile, filename);
}

static void
widget_table_input_by_command(variable *var, const gchar *source,
	gboolean command)
{
	FILE *infile;
	gchar *line;
	list_t *sliced;

	infile = command ? widget_opencommand(source) : fopen(source, "r");
	if (infile == NULL) {
		fprintf(stderr, "%s(): Couldn't open '%s' for reading.\n",
			__func__, source);
		return;
	}
	while ((line = widget_read_line(infile)) != NULL) {
		sliced = linecutter(g_strdup(line), '|');
		widget_table_append_row(var->Widget, sliced->line);
		list_t_free(sliced);
		g_free(line);
	}
	if (command)
		widget_closecommand(infile, source);
	else
		fclose(infile);
}

static void
widget_table_input_by_file(variable *var, const gchar *filename)
{
	widget_table_input_by_command(var, filename, FALSE);
}

static void
widget_table_append_row(GtkWidget *widget, gchar **values)
{
	GtkListStore *store;
	GtkTreeIter iter;
	gint column;
	gint column_count;
	guint value_count;

	store = widget_table_store(widget);
	column_count = widget_table_column_count(widget);
	value_count = g_strv_length(values);
	gtk_list_store_append(store, &iter);
	for (column = 0; column < column_count; ++column)
		gtk_list_store_set(store, &iter, column,
			column < (gint)value_count ? values[column] : "", -1);
}

static void
widget_table_input_by_items(variable *var)
{
	GList *element;
	gchar *text;
	list_t *sliced;

	text = attributeset_get_first(&element, var->Attributes, ATTR_ITEM);
	while (text != NULL) {
		sliced = linecutter(g_strdup(text), '|');
		widget_table_append_row(var->Widget, sliced->line);
		list_t_free(sliced);
		text = attributeset_get_next(&element, var->Attributes, ATTR_ITEM);
	}
}
