/*
 * Shared GtkAppChooser value and file output.
 * Copyright (C) 2026 Artur Bednarek <artur@unix.org.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef WIDGET_APPCHOOSER_COMMON_H
#define WIDGET_APPCHOOSER_COMMON_H

gchar *widget_appchooser_selected_id(GtkWidget *widget);
void widget_appchooser_save_id(variable *var);

#endif
