/*
 * widget_appchooserbutton.h: GTK3 application chooser button.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_APPCHOOSERBUTTON_H
#define WIDGET_APPCHOOSERBUTTON_H

GtkWidget *widget_appchooserbutton_create(
	AttributeSet *Attr, tag_attr *attr, gint Type);
gchar *widget_appchooserbutton_envvar_construct(GtkWidget *widget);
void widget_appchooserbutton_fileselect(
	variable *var, const char *name, const char *value);
void widget_appchooserbutton_refresh(variable *var);
void widget_appchooserbutton_save(variable *var);
void widget_appchooserbutton_clear(variable *var);

#endif
