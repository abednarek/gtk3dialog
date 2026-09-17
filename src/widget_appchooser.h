/*
 * widget_appchooser.h: embedded GTK3 application chooser.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_APPCHOOSER_H
#define WIDGET_APPCHOOSER_H

GtkWidget *widget_appchooser_create(
	AttributeSet *Attr, tag_attr *attr, gint Type);
gchar *widget_appchooser_envvar_construct(GtkWidget *widget);
void widget_appchooser_fileselect(
	variable *var, const char *name, const char *value);
void widget_appchooser_refresh(variable *var);
void widget_appchooser_save(variable *var);
void widget_appchooser_clear(variable *var);

#endif
