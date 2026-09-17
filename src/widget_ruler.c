/*
 * widget_ruler.c: GTK3 ruler compatibility widgets.
 * Gtkdialog - A small utility for fast and easy GUI building.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <pango/pangocairo.h>

#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "signals.h"
#include "widgets.h"
#include "widget_ruler.h"

typedef enum {
	GTKDIALOG_RULER_PIXELS,
	GTKDIALOG_RULER_INCHES,
	GTKDIALOG_RULER_CENTIMETERS
} GtkdialogRulerMetric;

typedef struct {
	GtkDrawingArea parent;
	GtkOrientation orientation;
	GtkdialogRulerMetric metric;
	gdouble lower;
	gdouble upper;
	gdouble position;
	gdouble max_size;
} GtkdialogRuler;

typedef GtkDrawingAreaClass GtkdialogRulerClass;

enum {
	PROP_0,
	PROP_LOWER,
	PROP_UPPER,
	PROP_POSITION,
	PROP_MAX_SIZE,
	PROP_METRIC,
	N_PROPERTIES
};

static GParamSpec *ruler_properties[N_PROPERTIES];

G_DEFINE_TYPE(GtkdialogRuler, gtkdialog_ruler, GTK_TYPE_DRAWING_AREA)

static GType
gtkdialog_ruler_metric_get_type(void)
{
	static gsize type_id;

	if (g_once_init_enter(&type_id)) {
		static const GEnumValue values[] = {
			{ GTKDIALOG_RULER_PIXELS, "GTK_PIXELS", "pixels" },
			{ GTKDIALOG_RULER_INCHES, "GTK_INCHES", "inches" },
			{ GTKDIALOG_RULER_CENTIMETERS,
				"GTK_CENTIMETERS", "centimeters" },
			{ 0, NULL, NULL }
		};
		GType registered = g_enum_register_static(
			"GtkdialogRulerMetric", values);
		g_once_init_leave(&type_id, registered);
	}
	return type_id;
}

static gdouble
gtkdialog_ruler_axis_position(GtkdialogRuler *ruler, gdouble coordinate,
	gdouble length)
{
	gdouble fraction;

	if (length <= 8.0)
		return ruler->lower;
	fraction = (coordinate - 4.0) / (length - 8.0);
	if (fraction < 0.0)
		fraction = 0.0;
	else if (fraction > 1.0)
		fraction = 1.0;
	return ruler->lower + fraction * (ruler->upper - ruler->lower);
}

static gboolean
gtkdialog_ruler_motion_notify(GtkWidget *widget, GdkEventMotion *event)
{
	GtkAllocation allocation;
	GtkdialogRuler *ruler = (GtkdialogRuler *)widget;
	gdouble coordinate;
	gdouble length;

	gtk_widget_get_allocation(widget, &allocation);
	if (ruler->orientation == GTK_ORIENTATION_HORIZONTAL) {
		coordinate = event->x;
		length = allocation.width;
	} else {
		coordinate = event->y;
		length = allocation.height;
	}
	g_object_set(ruler, "position",
		gtkdialog_ruler_axis_position(ruler, coordinate, length), NULL);
	return TRUE;
}

static void
gtkdialog_ruler_draw_label(GtkWidget *widget, cairo_t *cr,
	gdouble coordinate, gdouble value, gboolean horizontal)
{
	PangoLayout *layout;
	PangoFontDescription *font;
	gchar text[G_ASCII_DTOSTR_BUF_SIZE];
	gint width;

	g_ascii_formatd(text, sizeof(text), "%.4g", value);
	layout = gtk_widget_create_pango_layout(widget, text);
	font = pango_font_description_new();
	pango_font_description_set_size(font, 7 * PANGO_SCALE);
	pango_layout_set_font_description(layout, font);
	pango_layout_get_pixel_size(layout, &width, NULL);
	pango_font_description_free(font);

	cairo_save(cr);
	if (horizontal)
		cairo_move_to(cr, coordinate - width / 2.0, 2.0);
	else {
		cairo_translate(cr, 2.0, coordinate + width / 2.0);
		cairo_rotate(cr, -G_PI_2);
		cairo_move_to(cr, 0.0, 0.0);
	}
	pango_cairo_show_layout(cr, layout);
	cairo_restore(cr);
	g_object_unref(layout);
}

static gboolean
gtkdialog_ruler_draw(GtkWidget *widget, cairo_t *cr)
{
	GtkAllocation allocation;
	GtkStyleContext *context;
	GtkdialogRuler *ruler = (GtkdialogRuler *)widget;
	GdkRGBA color;
	gboolean horizontal;
	gdouble axis_length;
	gdouble baseline;
	gdouble coordinate;
	gdouble fraction;
	gdouble range;
	gint divisions;
	gint index;
	gint major_count;
	gint minor_count;

	gtk_widget_get_allocation(widget, &allocation);
	context = gtk_widget_get_style_context(widget);
	gtk_render_background(context, cr, 0, 0,
		allocation.width, allocation.height);
	gtk_render_frame(context, cr, 0, 0,
		allocation.width, allocation.height);
	gtk_style_context_get_color(context, GTK_STATE_FLAG_NORMAL, &color);
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_set_line_width(cr, 1.0);

	horizontal = ruler->orientation == GTK_ORIENTATION_HORIZONTAL;
	axis_length = horizontal ? allocation.width : allocation.height;
	baseline = horizontal ? allocation.height - 2.5 : allocation.width - 2.5;
	if (horizontal) {
		cairo_move_to(cr, 3.5, baseline);
		cairo_line_to(cr, allocation.width - 3.5, baseline);
	} else {
		cairo_move_to(cr, baseline, 3.5);
		cairo_line_to(cr, baseline, allocation.height - 3.5);
	}
	cairo_stroke(cr);

	major_count = (gint)((axis_length - 8.0) / 70.0);
	if (major_count < 1)
		major_count = 1;
	divisions = ruler->metric == GTKDIALOG_RULER_INCHES ? 8 : 10;
	minor_count = major_count * divisions;
	range = ruler->upper - ruler->lower;
	for (index = 0; index <= minor_count; ++index) {
		gdouble tick = index % divisions == 0 ? 11.0 :
			(index % (divisions / 2) == 0 ? 7.0 : 4.0);

		fraction = (gdouble)index / minor_count;
		coordinate = 4.0 + fraction * (axis_length - 8.0);
		if (horizontal) {
			cairo_move_to(cr, coordinate + 0.5, baseline);
			cairo_line_to(cr, coordinate + 0.5, baseline - tick);
		} else {
			cairo_move_to(cr, baseline, coordinate + 0.5);
			cairo_line_to(cr, baseline - tick, coordinate + 0.5);
		}
		cairo_stroke(cr);
		if (index % divisions == 0)
			gtkdialog_ruler_draw_label(widget, cr, coordinate,
				ruler->lower + fraction * range, horizontal);
	}

	if (range != 0.0)
		fraction = (ruler->position - ruler->lower) / range;
	else
		fraction = 0.0;
	if (fraction < 0.0)
		fraction = 0.0;
	else if (fraction > 1.0)
		fraction = 1.0;
	coordinate = 4.0 + fraction * (axis_length - 8.0);
	if (horizontal) {
		cairo_move_to(cr, coordinate, baseline - 1.0);
		cairo_line_to(cr, coordinate - 5.0, baseline - 8.0);
		cairo_line_to(cr, coordinate + 5.0, baseline - 8.0);
	} else {
		cairo_move_to(cr, baseline - 1.0, coordinate);
		cairo_line_to(cr, baseline - 8.0, coordinate - 5.0);
		cairo_line_to(cr, baseline - 8.0, coordinate + 5.0);
	}
	cairo_close_path(cr);
	cairo_fill(cr);
	return FALSE;
}

static void
gtkdialog_ruler_get_property(GObject *object, guint property_id,
	GValue *value, GParamSpec *pspec)
{
	GtkdialogRuler *ruler = (GtkdialogRuler *)object;

	switch (property_id) {
		case PROP_LOWER: g_value_set_double(value, ruler->lower); break;
		case PROP_UPPER: g_value_set_double(value, ruler->upper); break;
		case PROP_POSITION: g_value_set_double(value, ruler->position); break;
		case PROP_MAX_SIZE: g_value_set_double(value, ruler->max_size); break;
		case PROP_METRIC: g_value_set_enum(value, ruler->metric); break;
		default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
	}
}

static void
gtkdialog_ruler_set_property(GObject *object, guint property_id,
	const GValue *value, GParamSpec *pspec)
{
	GtkdialogRuler *ruler = (GtkdialogRuler *)object;

	switch (property_id) {
		case PROP_LOWER: ruler->lower = g_value_get_double(value); break;
		case PROP_UPPER: ruler->upper = g_value_get_double(value); break;
		case PROP_POSITION: ruler->position = g_value_get_double(value); break;
		case PROP_MAX_SIZE: ruler->max_size = g_value_get_double(value); break;
		case PROP_METRIC: ruler->metric = g_value_get_enum(value); break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			return;
	}
	gtk_widget_queue_draw(GTK_WIDGET(object));
}

static void
gtkdialog_ruler_get_preferred_width(GtkWidget *widget,
	gint *minimum, gint *natural)
{
	GtkdialogRuler *ruler = (GtkdialogRuler *)widget;

	if (ruler->orientation == GTK_ORIENTATION_HORIZONTAL) {
		*minimum = 80;
		*natural = 200;
	} else
		*minimum = *natural = 30;
}

static void
gtkdialog_ruler_get_preferred_height(GtkWidget *widget,
	gint *minimum, gint *natural)
{
	GtkdialogRuler *ruler = (GtkdialogRuler *)widget;

	if (ruler->orientation == GTK_ORIENTATION_HORIZONTAL)
		*minimum = *natural = 30;
	else {
		*minimum = 80;
		*natural = 200;
	}
}

static void
gtkdialog_ruler_class_init(GtkdialogRulerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	object_class->get_property = gtkdialog_ruler_get_property;
	object_class->set_property = gtkdialog_ruler_set_property;
	widget_class->draw = gtkdialog_ruler_draw;
	widget_class->motion_notify_event = gtkdialog_ruler_motion_notify;

	ruler_properties[PROP_LOWER] = g_param_spec_double("lower", "Lower",
		"Lower end of the ruler range", -G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
		G_PARAM_READWRITE);
	ruler_properties[PROP_UPPER] = g_param_spec_double("upper", "Upper",
		"Upper end of the ruler range", -G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
		G_PARAM_READWRITE);
	ruler_properties[PROP_POSITION] = g_param_spec_double("position",
		"Position", "Current marker position", -G_MAXDOUBLE, G_MAXDOUBLE,
		0.0, G_PARAM_READWRITE);
	ruler_properties[PROP_MAX_SIZE] = g_param_spec_double("max-size",
		"Maximum size", "Largest anticipated ruler value", -G_MAXDOUBLE,
		G_MAXDOUBLE, 0.0, G_PARAM_READWRITE);
	ruler_properties[PROP_METRIC] = g_param_spec_enum("metric", "Metric",
		"Measurement unit", gtkdialog_ruler_metric_get_type(),
		GTKDIALOG_RULER_PIXELS, G_PARAM_READWRITE);
	g_object_class_install_properties(object_class, N_PROPERTIES,
		ruler_properties);
}

static void
gtkdialog_ruler_init(GtkdialogRuler *ruler)
{
	ruler->orientation = GTK_ORIENTATION_HORIZONTAL;
	ruler->metric = GTKDIALOG_RULER_PIXELS;
	gtk_widget_add_events(GTK_WIDGET(ruler), GDK_POINTER_MOTION_MASK);
}

static void
widget_ruler_set_position(GtkWidget *widget, const gchar *text,
	const gchar *source)
{
	gdouble position;

	if (widget_parse_finite_double(text, &position, source))
		g_object_set(G_OBJECT(widget), "position", position, NULL);
}

static gchar *
widget_ruler_first_output_filename(AttributeSet *attributes)
{
	GList *element;
	gchar *directive;

	directive = attributeset_get_first(&element, attributes, ATTR_OUTPUT);
	while (directive != NULL) {
		if (g_ascii_strncasecmp(directive, "file:", 5) == 0 &&
			strlen(directive) > 5)
			return directive + 5;
		directive = attributeset_get_next(&element, attributes, ATTR_OUTPUT);
	}
	return NULL;
}

static void
widget_ruler_input_by_command(variable *var, const gchar *command)
{
	FILE *input = widget_opencommand(command);
	gchar *line;

	if (input == NULL) {
		fprintf(stderr, "%s(): Couldn't open '%s' for reading.\n",
			__func__, command);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_ruler_set_position(var->Widget, line, "ruler command input");
		g_free(line);
	}
	widget_closecommand(input, command);
}

static void
widget_ruler_input_by_file(variable *var, const gchar *filename)
{
	FILE *input = fopen(filename, "r");
	gchar *line;

	if (input == NULL) {
		fprintf(stderr, "%s(): Couldn't open '%s' for reading.\n",
			__func__, filename);
		return;
	}
	line = widget_read_line(input);
	if (line != NULL) {
		widget_ruler_set_position(var->Widget, line, "ruler file input");
		g_free(line);
	}
	fclose(input);
}

void
widget_ruler_clear(variable *var)
{
	gdouble lower;

	g_object_get(G_OBJECT(var->Widget), "lower", &lower, NULL);
	g_object_set(G_OBJECT(var->Widget), "position", lower, NULL);
}

GtkWidget *
widget_ruler_create(AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkdialogRuler *ruler;

	(void)Attr;
	(void)attr;
	ruler = g_object_new(gtkdialog_ruler_get_type(), NULL);
	ruler->orientation = Type == WIDGET_HRULER ?
		GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL;
	return GTK_WIDGET(ruler);
}

gchar *
widget_ruler_envvar_construct(GtkWidget *widget)
{
	gchar value[G_ASCII_DTOSTR_BUF_SIZE];
	gdouble position;

	g_object_get(G_OBJECT(widget), "position", &position, NULL);
	g_ascii_dtostr(value, sizeof(value), position);
	return g_strdup(value);
}

void
widget_ruler_fileselect(variable *var, const char *name, const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	fprintf(stderr, "%s(): Fileselect not implemented for this widget.\n",
		__func__);
}

void
widget_ruler_refresh(variable *var)
{
	GList *element;
	gchar *input;
	gboolean initialised;

	initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	input = attributeset_get_first(&element, var->Attributes, ATTR_INPUT);
	while (input != NULL) {
		if (input_is_shell_command(input))
			widget_ruler_input_by_command(var, input + 8);
		else if (g_ascii_strncasecmp(input, "file:", 5) == 0 &&
			strlen(input) > 5) {
			if (!initialised)
				widget_file_monitor_try_create(var, input + 5);
			widget_ruler_input_by_file(var, input + 5);
		}
		input = attributeset_get_next(&element,
			var->Attributes, ATTR_INPUT);
	}

	if (initialised)
		return;
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		widget_ruler_set_position(var->Widget,
			attributeset_get_first(&element,
				var->Attributes, ATTR_DEFAULT), "ruler default");
	if (attributeset_is_avail(var->Attributes, ATTR_LABEL))
		fprintf(stderr, "%s(): <label> not implemented for this widget.\n",
			__func__);
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		fprintf(stderr, "%s(): <item> not implemented for this widget.\n",
			__func__);
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "disabled") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
}

void
widget_ruler_removeselected(variable *var)
{
	(void)var;
	fprintf(stderr, "%s(): Removeselected not implemented for this widget.\n",
		__func__);
}

void
widget_ruler_save(variable *var)
{
	FILE *output;
	gchar *filename;
	gchar *value;

	filename = widget_ruler_first_output_filename(var->Attributes);
	if (filename == NULL) {
		fprintf(stderr, "%s(): No <output file> directive found.\n", __func__);
		return;
	}
	output = fopen(filename, "w");
	if (output == NULL) {
		fprintf(stderr, "%s(): Couldn't open '%s' for writing.\n",
			__func__, filename);
		return;
	}
	value = widget_ruler_envvar_construct(var->Widget);
	fputs(value, output);
	g_free(value);
	widget_close_output(output, filename);
}
