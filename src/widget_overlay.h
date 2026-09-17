/*
 * widget_overlay.h: GTK3 GtkOverlay XML widget.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_OVERLAY_H
#define WIDGET_OVERLAY_H

#include "variables.h"

GtkWidget *widget_overlay_create(AttributeSet *Attr, tag_attr *attr,
	gint Type);
gchar *widget_overlay_envvar_construct(GtkWidget *widget);
void widget_overlay_fileselect(variable *var, const char *name,
	const char *value);
void widget_overlay_refresh(variable *var);
void widget_overlay_clear(variable *var);
void widget_overlay_save(variable *var);
void widget_overlay_removeselected(variable *var);

#endif
