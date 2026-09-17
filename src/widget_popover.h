/*
 * widget_popover.h: GTK3 popover and menu button XML widgets.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_POPOVER_H
#define WIDGET_POPOVER_H

#include "variables.h"

GtkWidget *widget_popover_create(AttributeSet *Attr, tag_attr *attr, gint Type);
GtkWidget *widget_menubutton_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
GtkWidget *widget_popovermenu_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
GtkWidget *widget_modelbutton_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_popover_envvar_construct(GtkWidget *widget);
gchar *widget_menubutton_envvar_construct(GtkWidget *widget);
gchar *widget_modelbutton_envvar_construct(GtkWidget *widget);
void widget_popover_refresh(variable *var);
void widget_menubutton_refresh(variable *var);
void widget_modelbutton_refresh(variable *var);
void widget_popover_save(variable *var);
void widget_menubutton_save(variable *var);
void widget_modelbutton_save(variable *var);
void widget_popover_clear(variable *var);
void widget_menubutton_clear(variable *var);
void widget_modelbutton_clear(variable *var);
void widget_popover_removeselected(variable *var);
void widget_menubutton_removeselected(variable *var);
void widget_modelbutton_removeselected(variable *var);
void widget_menubutton_fileselect(variable *var, const char *name,
	const char *value);

#endif
