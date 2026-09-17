/*
 * widget_alignment.c:
 * Gtkdialog - A small utility for fast and easy GUI building.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
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
 */

#include <gtk/gtk.h>
#include "config.h"
#include "gtkdialog.h"
#include "attributes.h"
#include "automaton.h"
#include "widgets.h"
#include "widget_alignment.h"
#include "tag_attributes.h"

void widget_alignment_clear(variable *var)
{
	(void)var;
	fprintf(stderr, "%s(): Clear not implemented for this widget.\n",
		__func__);
}

static GtkWidget *widget_alignment_wrap(GtkWidget *child,
	GtkOrientation orientation, GtkAlign align, gboolean fill)
{
	GtkWidget *box;
	GtkWidget *spacer;

	box = gtk_box_new(orientation, 0);
	gtk_widget_show(box);
	if (fill) {
		gtk_box_pack_start(GTK_BOX(box), child, TRUE, TRUE, 0);
		return box;
	}
	if (align == GTK_ALIGN_CENTER || align == GTK_ALIGN_END) {
		spacer = gtk_box_new(orientation, 0);
		gtk_widget_show(spacer);
		gtk_box_pack_start(GTK_BOX(box), spacer, TRUE, TRUE, 0);
	}
	gtk_box_pack_start(GTK_BOX(box), child, FALSE, FALSE, 0);
	if (align == GTK_ALIGN_CENTER || align == GTK_ALIGN_START) {
		spacer = gtk_box_new(orientation, 0);
		gtk_widget_show(spacer);
		gtk_box_pack_start(GTK_BOX(box), spacer, TRUE, TRUE, 0);
	}
	return box;
}

GtkWidget *widget_alignment_create(
	AttributeSet *Attr, tag_attr *attr, gint Type)
{
	GtkAlign halign;
	GtkAlign valign;
	GtkWidget *horizontal;
	GtkWidget *widget;
	stackelement child;
	gchar *value;
	gdouble xalign = 0.5;
	gdouble yalign = 0.5;
	gdouble xscale = 1.0;
	gdouble yscale = 1.0;
	gint padding;

	(void)Attr;
	(void)Type;
	child = pop();
	if (child.nwidgets != 1) {
		stackelement_clear(&child);
		g_error("gtkdialog: <alignment> requires exactly one direct "
			"child widget.");
	}

	if (attr != NULL) {
		if ((value = get_tag_attribute(attr, "xalign")) != NULL)
			widget_parse_finite_double(value, &xalign, "alignment xalign");
		if ((value = get_tag_attribute(attr, "yalign")) != NULL)
			widget_parse_finite_double(value, &yalign, "alignment yalign");
		if ((value = get_tag_attribute(attr, "xscale")) != NULL)
			widget_parse_finite_double(value, &xscale, "alignment xscale");
		if ((value = get_tag_attribute(attr, "yscale")) != NULL)
			widget_parse_finite_double(value, &yscale, "alignment yscale");
	}
	xalign = CLAMP(xalign, 0.0, 1.0);
	yalign = CLAMP(yalign, 0.0, 1.0);
	xscale = CLAMP(xscale, 0.0, 1.0);
	yscale = CLAMP(yscale, 0.0, 1.0);
	halign = xscale > 0.0 ? GTK_ALIGN_FILL
		: xalign < 0.34 ? GTK_ALIGN_START
		: xalign > 0.66 ? GTK_ALIGN_END : GTK_ALIGN_CENTER;
	valign = yscale > 0.0 ? GTK_ALIGN_FILL
		: yalign < 0.34 ? GTK_ALIGN_START
		: yalign > 0.66 ? GTK_ALIGN_END : GTK_ALIGN_CENTER;
	horizontal = widget_alignment_wrap(child.widgets[0],
		GTK_ORIENTATION_HORIZONTAL, halign, xscale > 0.0);
	widget = widget_alignment_wrap(horizontal, GTK_ORIENTATION_VERTICAL,
		valign, yscale > 0.0);
	if (attr != NULL) {
#define APPLY_ALIGNMENT_MARGIN(name, setter) \
		if ((value = get_tag_attribute(attr, name)) != NULL) { \
			padding = widget_parse_nonnegative_integer(value, 0, name); \
			setter(child.widgets[0], padding); \
		}
		APPLY_ALIGNMENT_MARGIN("top-padding", gtk_widget_set_margin_top)
		APPLY_ALIGNMENT_MARGIN("bottom-padding", gtk_widget_set_margin_bottom)
		APPLY_ALIGNMENT_MARGIN("left-padding", gtk_widget_set_margin_start)
		APPLY_ALIGNMENT_MARGIN("right-padding", gtk_widget_set_margin_end)
#undef APPLY_ALIGNMENT_MARGIN
		kill_tag_attribute(attr, "xalign");
		kill_tag_attribute(attr, "yalign");
		kill_tag_attribute(attr, "xscale");
		kill_tag_attribute(attr, "yscale");
		kill_tag_attribute(attr, "top-padding");
		kill_tag_attribute(attr, "bottom-padding");
		kill_tag_attribute(attr, "left-padding");
		kill_tag_attribute(attr, "right-padding");
	}
	stackelement_clear(&child);
	return widget;
}

gchar *widget_alignment_envvar_construct(GtkWidget *widget)
{
	(void)widget;
	return g_strdup("");
}

void widget_alignment_fileselect(
	variable *var, const char *name, const char *value)
{
	(void)var;
	(void)name;
	(void)value;
	fprintf(stderr, "%s(): Fileselect not implemented for this widget.\n",
		__func__);
}

void widget_alignment_refresh(variable *var)
{
	gboolean initialised;

	initialised = GPOINTER_TO_INT(g_object_get_data(
		G_OBJECT(var->Widget), "_initialised"));
	if (initialised)
		return;

	if (attributeset_is_avail(var->Attributes, ATTR_LABEL))
		fprintf(stderr,
			"%s(): <label> not implemented for this widget.\n", __func__);
	if (attributeset_is_avail(var->Attributes, ATTR_ITEM))
		fprintf(stderr,
			"%s(): <item> not implemented for this widget.\n", __func__);
	if (attributeset_is_avail(var->Attributes, ATTR_DEFAULT))
		fprintf(stderr,
			"%s(): <default> not implemented for this widget.\n", __func__);
	if (attributeset_is_avail(var->Attributes, ATTR_INPUT))
		fprintf(stderr,
			"%s(): <input> not implemented for this widget.\n", __func__);
	if (attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "false") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "disabled") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "no") ||
		attributeset_cmp_left(var->Attributes, ATTR_SENSITIVE, "0"))
		gtk_widget_set_sensitive(var->Widget, FALSE);
}

void widget_alignment_removeselected(variable *var)
{
	(void)var;
	fprintf(stderr,
		"%s(): Removeselected not implemented for this widget.\n", __func__);
}

void widget_alignment_save(variable *var)
{
	(void)var;
	fprintf(stderr, "%s(): Save not implemented for this widget.\n",
		__func__);
}
