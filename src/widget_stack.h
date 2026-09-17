/*
 * widget_stack.h: GTK3 GtkStack XML widget.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_STACK_H
#define WIDGET_STACK_H

#include "variables.h"

GtkWidget *widget_stack_create(AttributeSet *Attr, tag_attr *attr, gint Type);
gchar *widget_stack_envvar_construct(GtkWidget *widget);
void widget_stack_fileselect(variable *var, const char *name,
	const char *value);
void widget_stack_refresh(variable *var);
void widget_stack_save(variable *var);
void widget_stack_clear(variable *var);
void widget_stack_removeselected(variable *var);

#endif
