%{
/*
 * gtkdialog_parser.y: A simple grammar for the XML-like language we use.
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

/*
**
** $Id: parser.y,v 1.5 2004/11/25 21:16:57 root Exp root $
** $Log: parser.y,v $
** Revision 1.5  2004/11/25 21:16:57  root
** *** empty log message ***
**
** Revision 1.4  2004/11/25 21:15:21  root
**   o No, the grammar still has problems.
**
** Revision 1.2  2004/11/25 19:53:03  pipas
**   o New object: tag attributes.
**
** Revision 1.1  2004/11/19 22:10:08  pipas
** Initial revision
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "gtkdialog.h"
#include "config.h"
#include "automaton.h"
#include "attributes.h"
#include "gtkdialog_parser.h"
#include "tag_attributes.h"

int linenumber = 1;
extern gchar *Token;
extern gboolean option_no_warning;
extern gboolean option_print_ir;

//
// Function declarations
//
int yywarning(char *c);
void yyerror_simple(char *c);

static inline void
start_up(void) 
{
	if (!option_print_ir) {
		run_program();
		return;
	} else {
		print_program();
		exit(EXIT_SUCCESS);
	}
}

static void
token_store_owned_argument(token command, gchar *argument)
{
	token_store_with_argument(command, argument);
	g_free(argument);
}

static void
token_store_owned_argument_attr(token command, gchar *argument,
	tag_attr *attributes)
{
	token_store_with_argument_attr(command, argument, attributes);
	g_free(argument);
}

static tag_attr *
new_tag_attributeset_owned(gchar *name, gchar *value)
{
	tag_attr *attributes = new_tag_attributeset(name, value);

	g_free(name);
	g_free(value);
	return attributes;
}

static tag_attr *
add_tag_attribute_owned(tag_attr *attributes, gchar *name, gchar *value)
{
	add_tag_attribute(attributes, name, value);
	g_free(name);
	g_free(value);
	return attributes;
}

%}

%union { 
  double     dval;
  char      *cval;
  GList     *lval;
  tag_attr *nvval;
  gint       ival;
};

%token         WINDOW PART_WINDOW EWINDOW
%token         VBOX PART_VBOX EVBOX
%token         HBOX PART_HBOX EHBOX
%token         NOTEBOOK ENOTEBOOK PART_NOTEBOOK
%token <cval>  FRAME
%token <cval>  TAG_ATTR_NAME
%type  <nvval> tagattr
%token         EFRAME
%token         ENTRY EENTRY PART_ENTRY
%token         SEARCHENTRY ESEARCHENTRY PART_SEARCHENTRY
%token         MENUBAR PART_MENUBAR EMENUBAR
%token         MENU PART_MENU EMENU
%token         POPUPMENU PART_POPUPMENU EPOPUPMENU
%token         MENUITEM PART_MENUITEM EMENUITEM
%token         MENUITEMSEPARATOR EMENUITEMSEPARATOR
%token         EDIT PART_EDIT EEDIT
%token         TREE PART_TREE ETREE
%token         CHOOSER PART_CHOOSER ECHOOSER
%token         LABEL ELABEL
%token         ITEM EITEM PART_ITEM
%token         BUTTON PART_BUTTON EBUTTON 
%token         BUTTONOK BUTTONCANCEL BUTTONHELP BUTTONYES BUTTONNO
%token         CHECKBOX ECHECKBOX PART_CHECKBOX
%token         RADIO ERADIO PART_RADIO
%token         PROGRESSBAR EPROGRESSBAR PART_PROGRESSBAR
%token         LIST PART_LIST ELIST
%token         TABLE ETABLE PART_TABLE
%token         COMBOBOX PART_COMBOBOX ECOMBOBOX
%token         GVIM EGVIM
%token         TEXT PART_TEXT ETEXT
%token         PIXMAP PART_PIXMAP EPIXMAP 
%token         DEFAULT EDEFAULT
%token         SENSITIVE ESENSITIVE
%token         VARIABLE PART_VARIABLE EVARIABLE
%token         WIDTH EWIDTH
%token         HEIGHT EHEIGHT
%token         INPUT INPUTFILE EINPUT PART_INPUT PART_INPUTFILE
%token         OUTPUT OUTPUTFILE EOUTPUT

%token         ACTION EACTION PART_ACTION 

%token         COMM ENDCOMM
%token         IF ENDIF 
%type  <ival>  then endif
%token         WHILE EWHILE
%type  <ival>  while do ewhile
%token         SHOW_WIDGETS
%token <cval>  EMB_VARIABLE EMB_NUMBER
%token         END_OF_FILE
%token <dval>  NUMBER 
%token <cval>  STRING

%left '='
%left '-' '+'        
%left '*' '/'

%nonassoc      UMINUS 

%define api.header.include {"gtkdialog_parser.h"}

	/**************************************************************
	 * Thunor: Newly supported widgets.
	 **************************************************************/
%token         HSEPARATOR PART_HSEPARATOR EHSEPARATOR
%token         VSEPARATOR PART_VSEPARATOR EVSEPARATOR
%token         COMBOBOXTEXT PART_COMBOBOXTEXT ECOMBOBOXTEXT
%token         COMBOBOXENTRY PART_COMBOBOXENTRY ECOMBOBOXENTRY
%token         HSCALE PART_HSCALE EHSCALE
%token         VSCALE PART_VSCALE EVSCALE
%token         SPINBUTTON PART_SPINBUTTON ESPINBUTTON
%token         TIMER PART_TIMER ETIMER
%token         TOGGLEBUTTON PART_TOGGLEBUTTON ETOGGLEBUTTON
%token         STATUSBAR PART_STATUSBAR ESTATUSBAR
%token         COLORBUTTON PART_COLORBUTTON ECOLORBUTTON
%token         FONTBUTTON PART_FONTBUTTON EFONTBUTTON
%token         TERMINAL PART_TERMINAL ETERMINAL
%token         EVENTBOX PART_EVENTBOX EEVENTBOX
%token         EXPANDER PART_EXPANDER EEXPANDER
%token         REVEALER PART_REVEALER EREVEALER
%token         SEARCHBAR PART_SEARCHBAR ESEARCHBAR
%token         ACTIONBAR PART_ACTIONBAR EACTIONBAR
%token         STACK PART_STACK ESTACK
%token         STACKSWITCHER PART_STACKSWITCHER ESTACKSWITCHER
%token         STACKSIDEBAR PART_STACKSIDEBAR ESTACKSIDEBAR
%token         LISTBOX PART_LISTBOX ELISTBOX
%token         FLOWBOX PART_FLOWBOX EFLOWBOX
%token         PLACESSIDEBAR PART_PLACESSIDEBAR EPLACESSIDEBAR
%token         HEADERBAR PART_HEADERBAR EHEADERBAR
%token         POPOVER PART_POPOVER EPOPOVER
%token         MENUBUTTON PART_MENUBUTTON EMENUBUTTON
%token         POPOVERMENU PART_POPOVERMENU EPOPOVERMENU
%token         MODELBUTTON PART_MODELBUTTON EMODELBUTTON
%token         OVERLAY PART_OVERLAY EOVERLAY
%token         SWITCH PART_SWITCH ESWITCH
%token         LEVELBAR PART_LEVELBAR ELEVELBAR
%token         HPANED PART_HPANED EHPANED
%token         VPANED PART_VPANED EVPANED
%token         ALIGNMENT PART_ALIGNMENT EALIGNMENT
%token         HBUTTONBOX PART_HBUTTONBOX EHBUTTONBOX
%token         VBUTTONBOX PART_VBUTTONBOX EVBUTTONBOX
%token         TOOLBAR PART_TOOLBAR ETOOLBAR
%token         TOOLBUTTON PART_TOOLBUTTON ETOOLBUTTON
%token         TOGGLETOOLBUTTON PART_TOGGLETOOLBUTTON ETOGGLETOOLBUTTON
%token         RADIOTOOLBUTTON PART_RADIOTOOLBUTTON ERADIOTOOLBUTTON
%token         MENUTOOLBUTTON PART_MENUTOOLBUTTON EMENUTOOLBUTTON
%token         SEPARATORTOOLITEM PART_SEPARATORTOOLITEM ESEPARATORTOOLITEM
%token         CALENDAR PART_CALENDAR ECALENDAR
%token         LINKBUTTON PART_LINKBUTTON ELINKBUTTON
%token         SPINNER PART_SPINNER ESPINNER
%token         GRID PART_GRID EGRID
%token         FILECHOOSERBUTTON PART_FILECHOOSERBUTTON EFILECHOOSERBUTTON
%token         INFOBAR PART_INFOBAR EINFOBAR
%token         ASSISTANT PART_ASSISTANT EASSISTANT
%token         ICONVIEW PART_ICONVIEW EICONVIEW
%token         SCALEBUTTON PART_SCALEBUTTON ESCALEBUTTON
%token         VOLUMEBUTTON PART_VOLUMEBUTTON EVOLUMEBUTTON
%token         ASPECTFRAME PART_ASPECTFRAME EASPECTFRAME
%token         MESSAGEDIALOG PART_MESSAGEDIALOG EMESSAGEDIALOG
%token         DIALOG PART_DIALOG EDIALOG
%token         ABOUTDIALOG PART_ABOUTDIALOG EABOUTDIALOG
%token         RECENTCHOOSER PART_RECENTCHOOSER ERECENTCHOOSER
%token         RECENTCHOOSERMENU PART_RECENTCHOOSERMENU ERECENTCHOOSERMENU
%token         STATUSICON PART_STATUSICON ESTATUSICON
%token         ACCELLABEL PART_ACCELLABEL EACCELLABEL
%token         ARROW PART_ARROW EARROW
%token         HSV PART_HSV EHSV
%token         PLUG PART_PLUG EPLUG
%token         OFFSCREENWINDOW PART_OFFSCREENWINDOW EOFFSCREENWINDOW
%token         FILECHOOSERDIALOG PART_FILECHOOSERDIALOG EFILECHOOSERDIALOG
%token         PAGESETUPDIALOG PART_PAGESETUPDIALOG EPAGESETUPDIALOG
%token         PRINTDIALOG PART_PRINTDIALOG EPRINTDIALOG
%token         HRULER PART_HRULER EHRULER
%token         VRULER PART_VRULER EVRULER
%token         TOOLITEM PART_TOOLITEM ETOOLITEM
%token         FIXED PART_FIXED EFIXED
%token         LAYOUT PART_LAYOUT ELAYOUT
%token         SCROLLEDWINDOW PART_SCROLLEDWINDOW ESCROLLEDWINDOW
%token         HANDLEBOX PART_HANDLEBOX EHANDLEBOX
%token         TOOLPALETTE PART_TOOLPALETTE ETOOLPALETTE
%token         TOOLITEMGROUP PART_TOOLITEMGROUP ETOOLITEMGROUP
%token         CELLVIEW PART_CELLVIEW ECELLVIEW
%token         DRAWINGAREA PART_DRAWINGAREA EDRAWINGAREA
%token         IMAGEVIEW PART_IMAGEVIEW EIMAGEVIEW
%token         PART_OUTPUTFILE
%token         SOCKET PART_SOCKET ESOCKET
%token         HSCROLLBAR PART_HSCROLLBAR EHSCROLLBAR
%token         VSCROLLBAR PART_VSCROLLBAR EVSCROLLBAR
%token         VIEWPORT PART_VIEWPORT EVIEWPORT
%token         COLORSELECTION PART_COLORSELECTION ECOLORSELECTION
%token         FONTSELECTION PART_FONTSELECTION EFONTSELECTION
%token         APPCHOOSERBUTTON PART_APPCHOOSERBUTTON EAPPCHOOSERBUTTON
%token         APPCHOOSER PART_APPCHOOSER EAPPCHOOSER
%token         APPCHOOSERDIALOG PART_APPCHOOSERDIALOG EAPPCHOOSERDIALOG
%token         COLORCHOOSERDIALOG PART_COLORCHOOSERDIALOG ECOLORCHOOSERDIALOG
%token         FONTCHOOSERDIALOG PART_FONTCHOOSERDIALOG EFONTCHOOSERDIALOG
%token         RECENTCHOOSERDIALOG PART_RECENTCHOOSERDIALOG ERECENTCHOOSERDIALOG
%token         SHORTCUTSWINDOW PART_SHORTCUTSWINDOW ESHORTCUTSWINDOW
%token         SHORTCUTSSECTION PART_SHORTCUTSSECTION ESHORTCUTSSECTION
%token         SHORTCUTSGROUP PART_SHORTCUTSGROUP ESHORTCUTSGROUP
%token         SHORTCUTSSHORTCUT PART_SHORTCUTSSHORTCUT ESHORTCUTSSHORTCUT

%% 
window
	: attr radio_group_scope wlist {
		token_store(RGROUP_POP);
    		token_store(PUSH | WIDGET_WINDOW); 
		start_up();
	}
	| WINDOW radio_group_scope wlist attr EWINDOW {
		token_store(RGROUP_POP);
    		token_store(PUSH | WIDGET_WINDOW); 
		start_up();
	}
	| PART_WINDOW tagattr '>' radio_group_scope wlist attr EWINDOW {
		token_store(RGROUP_POP);
    		token_store_attr(PUSH | WIDGET_WINDOW, $2); 
		start_up();
	}
	| ASSISTANT radio_group_scope wlist attr EASSISTANT {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_ASSISTANT);
		start_up();
	}
	| PART_ASSISTANT tagattr '>' radio_group_scope wlist attr EASSISTANT {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_ASSISTANT, $2);
		start_up();
	}
	| MESSAGEDIALOG attr EMESSAGEDIALOG {
		token_store(PUSH | WIDGET_MESSAGEDIALOG);
		start_up();
	}
	| PART_MESSAGEDIALOG tagattr '>' attr EMESSAGEDIALOG {
		token_store_attr(PUSH | WIDGET_MESSAGEDIALOG, $2);
		start_up();
	}
	| DIALOG radio_group_scope wlist attr EDIALOG {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_DIALOG);
		start_up();
	}
	| PART_DIALOG tagattr '>' radio_group_scope wlist attr EDIALOG {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_DIALOG, $2);
		start_up();
	}
	| ABOUTDIALOG attr EABOUTDIALOG {
		token_store(PUSH | WIDGET_ABOUTDIALOG);
		start_up();
	}
	| PART_ABOUTDIALOG tagattr '>' attr EABOUTDIALOG {
		token_store_attr(PUSH | WIDGET_ABOUTDIALOG, $2);
		start_up();
	}
	| POPUPMENU EPOPUPMENU {
		yyerror("The popupmenu widget requires at least one menuitem widget.");
	}
	| POPUPMENU radio_group_scope menuwlist attr EPOPUPMENU {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_POPUPMENU);
		start_up();
	}
	| PART_POPUPMENU tagattr '>' radio_group_scope menuwlist attr EPOPUPMENU {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_POPUPMENU, $2);
		start_up();
	}
	| PLUG radio_group_scope wlist attr EPLUG {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_PLUG);
		start_up();
	}
	| PART_PLUG tagattr '>' radio_group_scope wlist attr EPLUG {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_PLUG, $2);
		start_up();
	}
	| OFFSCREENWINDOW radio_group_scope wlist attr EOFFSCREENWINDOW {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_OFFSCREENWINDOW);
		start_up();
	}
	| PART_OFFSCREENWINDOW tagattr '>' radio_group_scope wlist attr
		EOFFSCREENWINDOW {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_OFFSCREENWINDOW, $2);
		start_up();
	}
	| FILECHOOSERDIALOG attr EFILECHOOSERDIALOG {
		token_store(PUSH | WIDGET_FILECHOOSERDIALOG);
		start_up();
	}
	| PART_FILECHOOSERDIALOG tagattr '>' attr EFILECHOOSERDIALOG {
		token_store_attr(PUSH | WIDGET_FILECHOOSERDIALOG, $2);
		start_up();
	}
	| APPCHOOSERDIALOG attr EAPPCHOOSERDIALOG {
		token_store(PUSH | WIDGET_APPCHOOSERDIALOG);
		start_up();
	}
	| PART_APPCHOOSERDIALOG tagattr '>' attr EAPPCHOOSERDIALOG {
		token_store_attr(PUSH | WIDGET_APPCHOOSERDIALOG, $2);
		start_up();
	}
	| COLORCHOOSERDIALOG attr ECOLORCHOOSERDIALOG {
		token_store(PUSH | WIDGET_COLORCHOOSERDIALOG);
		start_up();
	}
	| PART_COLORCHOOSERDIALOG tagattr '>' attr ECOLORCHOOSERDIALOG {
		token_store_attr(PUSH | WIDGET_COLORCHOOSERDIALOG, $2);
		start_up();
	}
	| FONTCHOOSERDIALOG attr EFONTCHOOSERDIALOG {
		token_store(PUSH | WIDGET_FONTCHOOSERDIALOG);
		start_up();
	}
	| PART_FONTCHOOSERDIALOG tagattr '>' attr EFONTCHOOSERDIALOG {
		token_store_attr(PUSH | WIDGET_FONTCHOOSERDIALOG, $2);
		start_up();
	}
	| RECENTCHOOSERDIALOG attr ERECENTCHOOSERDIALOG {
		token_store(PUSH | WIDGET_RECENTCHOOSERDIALOG);
		start_up();
	}
	| PART_RECENTCHOOSERDIALOG tagattr '>' attr ERECENTCHOOSERDIALOG {
		token_store_attr(PUSH | WIDGET_RECENTCHOOSERDIALOG, $2);
		start_up();
	}
	| SHORTCUTSWINDOW shortcutssections attr ESHORTCUTSWINDOW {
		token_store(PUSH | WIDGET_SHORTCUTSWINDOW);
		start_up();
	}
	| PART_SHORTCUTSWINDOW tagattr '>' shortcutssections attr ESHORTCUTSWINDOW {
		token_store_attr(PUSH | WIDGET_SHORTCUTSWINDOW, $2);
		start_up();
	}
	| PAGESETUPDIALOG attr EPAGESETUPDIALOG {
		token_store(PUSH | WIDGET_PAGESETUPDIALOG);
		start_up();
	}
	| PART_PAGESETUPDIALOG tagattr '>' attr EPAGESETUPDIALOG {
		token_store_attr(PUSH | WIDGET_PAGESETUPDIALOG, $2);
		start_up();
	}
	| PRINTDIALOG attr EPRINTDIALOG {
		token_store(PUSH | WIDGET_PRINTDIALOG);
		start_up();
	}
	| PART_PRINTDIALOG tagattr '>' attr EPRINTDIALOG {
		token_store_attr(PUSH | WIDGET_PRINTDIALOG, $2);
		start_up();
	}
  ;

radio_group_scope
  : { token_store(RGROUP_PUSH); }
  ;

// Containers.

wlist
  : widget
  | wlist widget       { 
		token_store(SUM);      
	}
  | imperative
  | wlist imperative
	| ALIGNMENT radio_group_scope wlist attr EALIGNMENT {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_ALIGNMENT);
	}
	| wlist ALIGNMENT radio_group_scope wlist attr EALIGNMENT {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_ALIGNMENT);
		token_store(SUM);
	}
	| PART_ALIGNMENT tagattr '>' radio_group_scope wlist attr EALIGNMENT {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_ALIGNMENT, $2);
	}
	| wlist PART_ALIGNMENT tagattr '>' radio_group_scope wlist attr EALIGNMENT {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_ALIGNMENT, $3);
		token_store(SUM);
	}
	| ASPECTFRAME radio_group_scope wlist attr EASPECTFRAME {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_ASPECTFRAME);
	}
	| wlist ASPECTFRAME radio_group_scope wlist attr EASPECTFRAME {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_ASPECTFRAME);
		token_store(SUM);
	}
	| PART_ASPECTFRAME tagattr '>' radio_group_scope wlist attr EASPECTFRAME {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_ASPECTFRAME, $2);
	}
	| wlist PART_ASPECTFRAME tagattr '>' radio_group_scope wlist attr EASPECTFRAME {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_ASPECTFRAME, $3);
		token_store(SUM);
	}
	| GRID radio_group_scope wlist attr EGRID {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_GRID);
	}
	| wlist GRID radio_group_scope wlist attr EGRID {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_GRID);
		token_store(SUM);
	}
	| PART_GRID tagattr '>' radio_group_scope wlist attr EGRID {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_GRID, $2);
	}
	| wlist PART_GRID tagattr '>' radio_group_scope wlist attr EGRID {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_GRID, $3);
		token_store(SUM);
	}
	| FIXED radio_group_scope wlist attr EFIXED {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_FIXED);
	}
	| wlist FIXED radio_group_scope wlist attr EFIXED {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_FIXED);
		token_store(SUM);
	}
	| PART_FIXED tagattr '>' radio_group_scope wlist attr EFIXED {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_FIXED, $2);
	}
	| wlist PART_FIXED tagattr '>' radio_group_scope wlist attr EFIXED {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_FIXED, $3);
		token_store(SUM);
	}
	| LAYOUT radio_group_scope wlist attr ELAYOUT {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_LAYOUT);
	}
	| wlist LAYOUT radio_group_scope wlist attr ELAYOUT {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_LAYOUT);
		token_store(SUM);
	}
	| PART_LAYOUT tagattr '>' radio_group_scope wlist attr ELAYOUT {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_LAYOUT, $2);
	}
	| wlist PART_LAYOUT tagattr '>' radio_group_scope wlist attr ELAYOUT {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_LAYOUT, $3);
		token_store(SUM);
	}
	| SCROLLEDWINDOW radio_group_scope wlist attr ESCROLLEDWINDOW {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_SCROLLEDWINDOW);
	}
	| wlist SCROLLEDWINDOW radio_group_scope wlist attr ESCROLLEDWINDOW {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_SCROLLEDWINDOW);
		token_store(SUM);
	}
	| PART_SCROLLEDWINDOW tagattr '>' radio_group_scope wlist attr ESCROLLEDWINDOW {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_SCROLLEDWINDOW, $2);
	}
	| wlist PART_SCROLLEDWINDOW tagattr '>' radio_group_scope wlist attr ESCROLLEDWINDOW {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_SCROLLEDWINDOW, $3);
		token_store(SUM);
	}
	| VIEWPORT radio_group_scope wlist attr EVIEWPORT {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_VIEWPORT);
	}
	| wlist VIEWPORT radio_group_scope wlist attr EVIEWPORT {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_VIEWPORT);
		token_store(SUM);
	}
	| PART_VIEWPORT tagattr '>' radio_group_scope wlist attr EVIEWPORT {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_VIEWPORT, $2);
	}
	| wlist PART_VIEWPORT tagattr '>' radio_group_scope wlist attr EVIEWPORT {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_VIEWPORT, $3);
		token_store(SUM);
	}
	| HANDLEBOX radio_group_scope wlist attr EHANDLEBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HANDLEBOX);
	}
	| wlist HANDLEBOX radio_group_scope wlist attr EHANDLEBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HANDLEBOX);
		token_store(SUM);
	}
	| PART_HANDLEBOX tagattr '>' radio_group_scope wlist attr EHANDLEBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HANDLEBOX, $2);
	}
	| wlist PART_HANDLEBOX tagattr '>' radio_group_scope wlist attr EHANDLEBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HANDLEBOX, $3);
		token_store(SUM);
	}
	| VBOX radio_group_scope wlist attr EVBOX   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_VBOX); 
	}
	| wlist VBOX radio_group_scope wlist attr EVBOX   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_VBOX); 
		token_store(SUM);      
	}
	| PART_VBOX tagattr '>' radio_group_scope wlist attr EVBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_VBOX, $2); 
	}
	| wlist PART_VBOX tagattr '>' radio_group_scope wlist attr EVBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_VBOX, $3); 
		token_store(SUM);      
	}
	| HBOX radio_group_scope wlist attr EHBOX   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HBOX); 
	}
	| wlist HBOX radio_group_scope wlist attr EHBOX   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HBOX); 
		token_store(SUM);      
	}
	| PART_HBOX tagattr '>' radio_group_scope wlist attr EHBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HBOX, $2); 
	}
	| wlist PART_HBOX tagattr '>' radio_group_scope wlist attr EHBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HBOX, $3); 
		token_store(SUM);      
	}
	| HBUTTONBOX radio_group_scope wlist attr EHBUTTONBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HBUTTONBOX);
	}
	| wlist HBUTTONBOX radio_group_scope wlist attr EHBUTTONBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HBUTTONBOX);
		token_store(SUM);
	}
	| PART_HBUTTONBOX tagattr '>' radio_group_scope wlist attr EHBUTTONBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HBUTTONBOX, $2);
	}
	| wlist PART_HBUTTONBOX tagattr '>' radio_group_scope wlist attr EHBUTTONBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HBUTTONBOX, $3);
		token_store(SUM);
	}
	| VBUTTONBOX radio_group_scope wlist attr EVBUTTONBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_VBUTTONBOX);
	}
	| wlist VBUTTONBOX radio_group_scope wlist attr EVBUTTONBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_VBUTTONBOX);
		token_store(SUM);
	}
	| PART_VBUTTONBOX tagattr '>' radio_group_scope wlist attr EVBUTTONBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_VBUTTONBOX, $2);
	}
	| wlist PART_VBUTTONBOX tagattr '>' radio_group_scope wlist attr EVBUTTONBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_VBUTTONBOX, $3);
		token_store(SUM);
	}
	| HPANED radio_group_scope wlist attr EHPANED {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HPANED);
	}
	| wlist HPANED radio_group_scope wlist attr EHPANED {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HPANED);
		token_store(SUM);
	}
	| PART_HPANED tagattr '>' radio_group_scope wlist attr EHPANED {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HPANED, $2);
	}
	| wlist PART_HPANED tagattr '>' radio_group_scope wlist attr EHPANED {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HPANED, $3);
		token_store(SUM);
	}
	| VPANED radio_group_scope wlist attr EVPANED {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_VPANED);
	}
	| wlist VPANED radio_group_scope wlist attr EVPANED {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_VPANED);
		token_store(SUM);
	}
	| PART_VPANED tagattr '>' radio_group_scope wlist attr EVPANED {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_VPANED, $2);
	}
	| wlist PART_VPANED tagattr '>' radio_group_scope wlist attr EVPANED {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_VPANED, $3);
		token_store(SUM);
	}
	| EVENTBOX radio_group_scope wlist attr EEVENTBOX   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_EVENTBOX); 
	}
	| wlist EVENTBOX radio_group_scope wlist attr EEVENTBOX   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_EVENTBOX); 
		token_store(SUM);      
	}
	| PART_EVENTBOX tagattr '>' radio_group_scope wlist attr EEVENTBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_EVENTBOX, $2); 
	}
	| wlist PART_EVENTBOX tagattr '>' radio_group_scope wlist attr EEVENTBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_EVENTBOX, $3); 
		token_store(SUM);      
	}
	| EXPANDER radio_group_scope wlist attr EEXPANDER   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_EXPANDER); 
	}
	| wlist EXPANDER radio_group_scope wlist attr EEXPANDER   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_EXPANDER); 
		token_store(SUM);      
	}
	| PART_EXPANDER tagattr '>' radio_group_scope wlist attr EEXPANDER {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_EXPANDER, $2); 
	}
	| wlist PART_EXPANDER tagattr '>' radio_group_scope wlist attr EEXPANDER {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_EXPANDER, $3); 
		token_store(SUM);      
	}
	| REVEALER radio_group_scope wlist attr EREVEALER {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_REVEALER);
	}
	| wlist REVEALER radio_group_scope wlist attr EREVEALER {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_REVEALER);
		token_store(SUM);
	}
	| PART_REVEALER tagattr '>' radio_group_scope wlist attr EREVEALER {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_REVEALER, $2);
	}
	| wlist PART_REVEALER tagattr '>' radio_group_scope wlist attr EREVEALER {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_REVEALER, $3);
		token_store(SUM);
	}
	| POPOVERMENU radio_group_scope wlist attr EPOPOVERMENU {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_POPOVERMENU);
	}
	| wlist POPOVERMENU radio_group_scope wlist attr EPOPOVERMENU {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_POPOVERMENU);
		token_store(SUM);
	}
	| PART_POPOVERMENU tagattr '>' radio_group_scope wlist attr EPOPOVERMENU {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_POPOVERMENU, $2);
	}
	| wlist PART_POPOVERMENU tagattr '>' radio_group_scope wlist attr EPOPOVERMENU {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_POPOVERMENU, $3);
		token_store(SUM);
	}
	| MODELBUTTON attr EMODELBUTTON {
		token_store(PUSH | WIDGET_MODELBUTTON);
	}
	| wlist MODELBUTTON attr EMODELBUTTON {
		token_store(PUSH | WIDGET_MODELBUTTON);
		token_store(SUM);
	}
	| PART_MODELBUTTON tagattr '>' attr EMODELBUTTON {
		token_store_attr(PUSH | WIDGET_MODELBUTTON, $2);
	}
	| wlist PART_MODELBUTTON tagattr '>' attr EMODELBUTTON {
		token_store_attr(PUSH | WIDGET_MODELBUTTON, $3);
		token_store(SUM);
	}
	| POPOVER radio_group_scope wlist attr EPOPOVER {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_POPOVER);
	}
	| wlist POPOVER radio_group_scope wlist attr EPOPOVER {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_POPOVER);
		token_store(SUM);
	}
	| PART_POPOVER tagattr '>' radio_group_scope wlist attr EPOPOVER {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_POPOVER, $2);
	}
	| wlist PART_POPOVER tagattr '>' radio_group_scope wlist attr EPOPOVER {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_POPOVER, $3);
		token_store(SUM);
	}
	| MENUBUTTON radio_group_scope wlist attr EMENUBUTTON {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_MENUBUTTON);
	}
	| wlist MENUBUTTON radio_group_scope wlist attr EMENUBUTTON {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_MENUBUTTON);
		token_store(SUM);
	}
	| PART_MENUBUTTON tagattr '>' radio_group_scope wlist attr EMENUBUTTON {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_MENUBUTTON, $2);
	}
	| wlist PART_MENUBUTTON tagattr '>' radio_group_scope wlist attr EMENUBUTTON {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_MENUBUTTON, $3);
		token_store(SUM);
	}
	| SEARCHBAR radio_group_scope wlist attr ESEARCHBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_SEARCHBAR);
	}
	| wlist SEARCHBAR radio_group_scope wlist attr ESEARCHBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_SEARCHBAR);
		token_store(SUM);
	}
	| PART_SEARCHBAR tagattr '>' radio_group_scope wlist attr ESEARCHBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_SEARCHBAR, $2);
	}
	| wlist PART_SEARCHBAR tagattr '>' radio_group_scope wlist attr ESEARCHBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_SEARCHBAR, $3);
		token_store(SUM);
	}
	| ACTIONBAR radio_group_scope wlist attr EACTIONBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_ACTIONBAR);
	}
	| wlist ACTIONBAR radio_group_scope wlist attr EACTIONBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_ACTIONBAR);
		token_store(SUM);
	}
	| PART_ACTIONBAR tagattr '>' radio_group_scope wlist attr EACTIONBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_ACTIONBAR, $2);
	}
	| wlist PART_ACTIONBAR tagattr '>' radio_group_scope wlist attr EACTIONBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_ACTIONBAR, $3);
		token_store(SUM);
	}
	| STACK radio_group_scope wlist attr ESTACK {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_STACK);
	}
	| wlist STACK radio_group_scope wlist attr ESTACK {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_STACK);
		token_store(SUM);
	}
	| PART_STACK tagattr '>' radio_group_scope wlist attr ESTACK {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_STACK, $2);
	}
	| wlist PART_STACK tagattr '>' radio_group_scope wlist attr ESTACK {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_STACK, $3);
		token_store(SUM);
	}
	| STACKSWITCHER attr ESTACKSWITCHER {
		token_store(PUSH | WIDGET_STACKSWITCHER);
	}
	| wlist STACKSWITCHER attr ESTACKSWITCHER {
		token_store(PUSH | WIDGET_STACKSWITCHER);
		token_store(SUM);
	}
	| PART_STACKSWITCHER tagattr '>' attr ESTACKSWITCHER {
		token_store_attr(PUSH | WIDGET_STACKSWITCHER, $2);
	}
	| wlist PART_STACKSWITCHER tagattr '>' attr ESTACKSWITCHER {
		token_store_attr(PUSH | WIDGET_STACKSWITCHER, $3);
		token_store(SUM);
	}
	| STACKSIDEBAR attr ESTACKSIDEBAR {
		token_store(PUSH | WIDGET_STACKSIDEBAR);
	}
	| wlist STACKSIDEBAR attr ESTACKSIDEBAR {
		token_store(PUSH | WIDGET_STACKSIDEBAR);
		token_store(SUM);
	}
	| PART_STACKSIDEBAR tagattr '>' attr ESTACKSIDEBAR {
		token_store_attr(PUSH | WIDGET_STACKSIDEBAR, $2);
	}
	| wlist PART_STACKSIDEBAR tagattr '>' attr ESTACKSIDEBAR {
		token_store_attr(PUSH | WIDGET_STACKSIDEBAR, $3);
		token_store(SUM);
	}
	| LISTBOX radio_group_scope wlist attr ELISTBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_LISTBOX);
	}
	| wlist LISTBOX radio_group_scope wlist attr ELISTBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_LISTBOX);
		token_store(SUM);
	}
	| PART_LISTBOX tagattr '>' radio_group_scope wlist attr ELISTBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_LISTBOX, $2);
	}
	| wlist PART_LISTBOX tagattr '>' radio_group_scope wlist attr ELISTBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_LISTBOX, $3);
		token_store(SUM);
	}
	| FLOWBOX radio_group_scope wlist attr EFLOWBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_FLOWBOX);
	}
	| wlist FLOWBOX radio_group_scope wlist attr EFLOWBOX {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_FLOWBOX);
		token_store(SUM);
	}
	| PART_FLOWBOX tagattr '>' radio_group_scope wlist attr EFLOWBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_FLOWBOX, $2);
	}
	| wlist PART_FLOWBOX tagattr '>' radio_group_scope wlist attr EFLOWBOX {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_FLOWBOX, $3);
		token_store(SUM);
	}
	| PLACESSIDEBAR attr EPLACESSIDEBAR {
		token_store(PUSH | WIDGET_PLACESSIDEBAR);
	}
	| wlist PLACESSIDEBAR attr EPLACESSIDEBAR {
		token_store(PUSH | WIDGET_PLACESSIDEBAR);
		token_store(SUM);
	}
	| PART_PLACESSIDEBAR tagattr '>' attr EPLACESSIDEBAR {
		token_store_attr(PUSH | WIDGET_PLACESSIDEBAR, $2);
	}
	| wlist PART_PLACESSIDEBAR tagattr '>' attr EPLACESSIDEBAR {
		token_store_attr(PUSH | WIDGET_PLACESSIDEBAR, $3);
		token_store(SUM);
	}
	| HEADERBAR radio_group_scope wlist attr EHEADERBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HEADERBAR);
	}
	| wlist HEADERBAR radio_group_scope wlist attr EHEADERBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_HEADERBAR);
		token_store(SUM);
	}
	| PART_HEADERBAR tagattr '>' radio_group_scope wlist attr EHEADERBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HEADERBAR, $2);
	}
	| wlist PART_HEADERBAR tagattr '>' radio_group_scope wlist attr EHEADERBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_HEADERBAR, $3);
		token_store(SUM);
	}
	| HEADERBAR attr EHEADERBAR {
		token_store(PUSH | WIDGET_HEADERBAR | HEADERBAR_EMPTY_CHILDREN);
	}
	| wlist HEADERBAR attr EHEADERBAR {
		token_store(PUSH | WIDGET_HEADERBAR | HEADERBAR_EMPTY_CHILDREN);
		token_store(SUM);
	}
	| PART_HEADERBAR tagattr '>' attr EHEADERBAR {
		token_store_attr(PUSH | WIDGET_HEADERBAR | HEADERBAR_EMPTY_CHILDREN, $2);
	}
	| wlist PART_HEADERBAR tagattr '>' attr EHEADERBAR {
		token_store_attr(PUSH | WIDGET_HEADERBAR | HEADERBAR_EMPTY_CHILDREN, $3);
		token_store(SUM);
	}
	| OVERLAY radio_group_scope wlist attr EOVERLAY {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_OVERLAY);
	}
	| wlist OVERLAY radio_group_scope wlist attr EOVERLAY {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_OVERLAY);
		token_store(SUM);
	}
	| PART_OVERLAY tagattr '>' radio_group_scope wlist attr EOVERLAY {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_OVERLAY, $2);
	}
	| wlist PART_OVERLAY tagattr '>' radio_group_scope wlist attr EOVERLAY {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_OVERLAY, $3);
		token_store(SUM);
	}
	| SWITCH attr ESWITCH {
		token_store(PUSH | WIDGET_SWITCH);
	}
	| wlist SWITCH attr ESWITCH {
		token_store(PUSH | WIDGET_SWITCH);
		token_store(SUM);
	}
	| PART_SWITCH tagattr '>' attr ESWITCH {
		token_store_attr(PUSH | WIDGET_SWITCH, $2);
	}
	| wlist PART_SWITCH tagattr '>' attr ESWITCH {
		token_store_attr(PUSH | WIDGET_SWITCH, $3);
		token_store(SUM);
	}
	| LEVELBAR attr ELEVELBAR {
		token_store(PUSH | WIDGET_LEVELBAR);
	}
	| wlist LEVELBAR attr ELEVELBAR {
		token_store(PUSH | WIDGET_LEVELBAR);
		token_store(SUM);
	}
	| PART_LEVELBAR tagattr '>' attr ELEVELBAR {
		token_store_attr(PUSH | WIDGET_LEVELBAR, $2);
	}
	| wlist PART_LEVELBAR tagattr '>' attr ELEVELBAR {
		token_store_attr(PUSH | WIDGET_LEVELBAR, $3);
		token_store(SUM);
	}
	| INFOBAR radio_group_scope wlist attr EINFOBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_INFOBAR);
	}
	| wlist INFOBAR radio_group_scope wlist attr EINFOBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_INFOBAR);
		token_store(SUM);
	}
	| PART_INFOBAR tagattr '>' radio_group_scope wlist attr EINFOBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_INFOBAR, $2);
	}
	| wlist PART_INFOBAR tagattr '>' radio_group_scope wlist attr EINFOBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_INFOBAR, $3);
		token_store(SUM);
	}
	| NOTEBOOK radio_group_scope wlist attr ENOTEBOOK   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_NOTEBOOK); 
	}
	| NOTEBOOK radio_group_scope attr ENOTEBOOK {
		token_store(RGROUP_POP);
		token_store(EMPTY_WIDGETS);
		token_store(PUSH | WIDGET_NOTEBOOK);
	}
	| wlist NOTEBOOK radio_group_scope wlist attr ENOTEBOOK   {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_NOTEBOOK); 
		token_store(SUM);      
	}
	| wlist NOTEBOOK radio_group_scope attr ENOTEBOOK {
		token_store(RGROUP_POP);
		token_store(EMPTY_WIDGETS);
		token_store(PUSH | WIDGET_NOTEBOOK);
		token_store(SUM);
	}
	| PART_NOTEBOOK tagattr '>' radio_group_scope wlist attr ENOTEBOOK {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_NOTEBOOK, $2);
	}
	| PART_NOTEBOOK tagattr '>' radio_group_scope attr ENOTEBOOK {
		token_store(RGROUP_POP);
		token_store(EMPTY_WIDGETS);
		token_store_attr(PUSH | WIDGET_NOTEBOOK, $2);
	}
	| wlist PART_NOTEBOOK tagattr '>' radio_group_scope wlist attr ENOTEBOOK {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_NOTEBOOK, $3);
		token_store(SUM);      
	}
	| wlist PART_NOTEBOOK tagattr '>' radio_group_scope attr ENOTEBOOK {
		token_store(RGROUP_POP);
		token_store(EMPTY_WIDGETS);
		token_store_attr(PUSH | WIDGET_NOTEBOOK, $3);
		token_store(SUM);
	}
	| FRAME radio_group_scope wlist attr EFRAME {
		token_store(RGROUP_POP);
		token_store_owned_argument(SET|ATTR_LABEL, $1);
		token_store(PUSH | WIDGET_FRAME); 
	}
	| wlist FRAME radio_group_scope wlist attr EFRAME {
		token_store(RGROUP_POP);
		token_store_owned_argument(SET|ATTR_LABEL, $2);
		token_store(PUSH | WIDGET_FRAME); 
		token_store(SUM);      
	}
  ;

widget
  :  text
  | entry
  | searchentry
  | edit
  | tree
  | chooser
  | button
  | checkbox
  | radiobutton
  | progressbar
  | list
  | table
  | combobox
  | pixmap
  | gvim
  | socket
  | menubar
  | hseparator
  | vseparator
  | comboboxtext
  | comboboxentry
  | hscale
  | vscale
  | spinbutton
  | timer
  | togglebutton
  | statusbar
  | colorbutton
  | COLORSELECTION attr ECOLORSELECTION {
		token_store(PUSH | WIDGET_COLORSELECTION);
	}
  | PART_COLORSELECTION tagattr '>' attr ECOLORSELECTION {
		token_store_attr(PUSH | WIDGET_COLORSELECTION, $2);
	}
  | fontbutton
  | FONTSELECTION attr EFONTSELECTION {
		token_store(PUSH | WIDGET_FONTSELECTION);
	}
  | PART_FONTSELECTION tagattr '>' attr EFONTSELECTION {
		token_store_attr(PUSH | WIDGET_FONTSELECTION, $2);
	}
  | terminal
  | toolbar
  | toolpalette
  | calendar
  | linkbutton
  | spinner
  | filechooserbutton
  | appchooserbutton
  | appchooser
  | iconview
  | cellview
  | drawingarea
  | imageview
  | scalebutton
  | volumebutton
  | recentchooser
  | statusicon
  | accellabel
  | arrow
  | hsv
  | hruler
  | vruler
  | hscrollbar
  | vscrollbar
  ;

entry
  : ENTRY attr EENTRY {
                          token_store(PUSH | WIDGET_ENTRY); 
			 }
  | PART_ENTRY tagattr '>' attr EENTRY {
                token_store_attr(PUSH | WIDGET_ENTRY, $2);
	}
  | ENTRY attr ENTRY {
                  yyerror("</entry> expected instead of <entry>.");} 
  ;

searchentry
  : SEARCHENTRY attr ESEARCHENTRY {
		token_store(PUSH | WIDGET_SEARCHENTRY);
	}
  | PART_SEARCHENTRY tagattr '>' attr ESEARCHENTRY {
		token_store_attr(PUSH | WIDGET_SEARCHENTRY, $2);
	}
  | SEARCHENTRY attr SEARCHENTRY {
		yyerror("</searchentry> expected instead of <searchentry>.");
	}
  ;

edit
  : EDIT attr EEDIT  {
    		token_store(PUSH | WIDGET_EDIT); 
	}
  | PART_EDIT tagattr '>' attr EEDIT {
    		token_store_attr(PUSH | WIDGET_EDIT, $2); 
    	}
  | EDIT attr EDIT   {
    		yyerror("</edit> expected instead of <edit>.");
	}
  ;

tree
  : TREE attr ETREE  {
		token_store(PUSH | WIDGET_TREE); 
	}
  | PART_TREE tagattr '>' attr ETREE {
    		token_store_attr(PUSH | WIDGET_TREE, $2); 
	}
  | TREE attr TREE {
   		yyerror("</tree> expected instead of <tree>.");
	}
  ;

chooser
  : CHOOSER attr ECHOOSER  {
		token_store(PUSH | WIDGET_CHOOSER); 
	}
  | PART_CHOOSER tagattr '>' attr ECHOOSER {
		token_store_attr(PUSH | WIDGET_CHOOSER, $2); 
	}
  | CHOOSER attr CHOOSER {
		yyerror("</chooser> expected instead of <chooser>.");
	}
  ;

text
  : TEXT attr ETEXT {
		token_store(PUSH | WIDGET_TEXT); 
	} 
  | PART_TEXT tagattr '>' attr ETEXT {
                token_store_attr(PUSH | WIDGET_TEXT, $2);
	}
  | TEXT attr TEXT  {yyerror("</text> expected instead of <text>.");}
  ;

button
  : BUTTON attr EBUTTON       {token_store(PUSH | WIDGET_BUTTON);  }
  | PART_BUTTON tagattr '>' attr EBUTTON {
                token_store_attr(PUSH | WIDGET_BUTTON, $2);
	}
  | BUTTONOK attr EBUTTON     {token_store(PUSH | WIDGET_OKBUTTON);}
  | BUTTONCANCEL attr EBUTTON {token_store(PUSH | WIDGET_CANCELBUTTON);}
  | BUTTONHELP attr EBUTTON   {token_store(PUSH | WIDGET_HELPBUTTON);}
  | BUTTONNO attr EBUTTON     {token_store(PUSH | WIDGET_NOBUTTON);}
  | BUTTONYES attr EBUTTON    {token_store(PUSH | WIDGET_YESBUTTON);}
  ;

checkbox
  : CHECKBOX attr ECHECKBOX {
		token_store(PUSH | WIDGET_CHECKBOX);
	}
  | PART_CHECKBOX tagattr '>' attr ECHECKBOX {
                token_store_attr(PUSH | WIDGET_CHECKBOX, $2);
	}
  | CHECKBOX attr CHECKBOX  {
		yyerror("</checkbox> expected instead of <checkbox>.");
	}
  ;

radiobutton
  : RADIO attr ERADIO    {
	   	token_store(PUSH | WIDGET_RADIOBUTTON);
           }
  | PART_RADIO tagattr '>' attr ERADIO {
                token_store_attr(PUSH | WIDGET_RADIOBUTTON, $2);
	   }
  | RADIO attr RADIO  {
		yyerror("</radiobutton> expected instead of <radiobutton>.");
           }
  ;

progressbar
  : PROGRESSBAR attr EPROGRESSBAR {
	   	token_store(PUSH | WIDGET_PROGRESSBAR);
           }
  | PART_PROGRESSBAR tagattr '>' attr EPROGRESSBAR {
                token_store_attr(PUSH | WIDGET_PROGRESSBAR, $2);
	   }
  | PROGRESSBAR attr PROGRESSBAR  {
		yyerror("</progressbar> expected instead of <progressbar>.");
           }
  ;

list
  : LIST attr ELIST {
		token_store(PUSH | WIDGET_LIST); 
	}
  | PART_LIST tagattr '>' attr ELIST {
		token_store_attr(PUSH | WIDGET_LIST, $2); 
    	}
  | LIST attr LIST   {
    		yyerror("</list> expected instead of <list>.");
	}
  ;

table
  : TABLE attr ETABLE {
		token_store(PUSH | WIDGET_TABLE); 
	}
  | PART_TABLE tagattr '>' attr ETABLE {
		token_store_attr(PUSH | WIDGET_TABLE, $2); 
    	}
  | TABLE attr TABLE   {
    		yyerror("</table> expected instead of <table>.");
	}
  ;

combobox
  : COMBOBOX attr ECOMBOBOX                  {
    		token_store(PUSH | WIDGET_COMBOBOX);
	}
  | PART_COMBOBOX tagattr '>' attr ECOMBOBOX {
    		token_store_attr(PUSH | WIDGET_COMBOBOX, $2);
	}                                 
  ;

gvim
  : GVIM attr EGVIM             {token_store(PUSH | WIDGET_GVIM);}
  ;

pixmap
  : PIXMAP attr EPIXMAP       {token_store(PUSH | WIDGET_PIXMAP);}
  | PART_PIXMAP tagattr '>' attr EPIXMAP {
    		token_store_attr(PUSH | WIDGET_PIXMAP, $2);
	}
  ;

calendar
  : CALENDAR attr ECALENDAR {
		token_store(PUSH | WIDGET_CALENDAR);
	}
  | PART_CALENDAR tagattr '>' attr ECALENDAR {
		token_store_attr(PUSH | WIDGET_CALENDAR, $2);
	}
  ;

	/**************************************************************
	 * Thunor: Newly supported widgets.
	 * Don't forget to add them to the widget list above and
	 * to create a token for them towards the top of this file.
	 * The WIDGET_*s are defined in automaton.h.
	 **************************************************************/

toolbar
  : TOOLBAR ETOOLBAR {
		yyerror("The toolbar widget requires at least one tool item.");
	}
  | TOOLBAR radio_group_scope toolbarwlist attr ETOOLBAR {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_TOOLBAR);
	}
  | PART_TOOLBAR tagattr '>' radio_group_scope toolbarwlist attr ETOOLBAR {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_TOOLBAR, $2);
	}
  ;

toolpalette
  : TOOLPALETTE ETOOLPALETTE {
		yyerror("The toolpalette widget requires at least one toolitemgroup.");
	}
  | TOOLPALETTE radio_group_scope toolpalettewlist attr ETOOLPALETTE {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_TOOLPALETTE);
	}
  | PART_TOOLPALETTE tagattr '>' radio_group_scope toolpalettewlist attr ETOOLPALETTE {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_TOOLPALETTE, $2);
	}
  ;

toolpalettewlist
  : toolitemgroup
  | toolpalettewlist toolitemgroup {
		token_store(SUM);
	}
  ;

shortcutssections
  : shortcutssection
  | shortcutssections shortcutssection { token_store(SUM); }
  ;

shortcutssection
  : SHORTCUTSSECTION shortcutsgroups attr ESHORTCUTSSECTION {
		token_store(PUSH | WIDGET_SHORTCUTSSECTION);
	}
  | PART_SHORTCUTSSECTION tagattr '>' shortcutsgroups attr ESHORTCUTSSECTION {
		token_store_attr(PUSH | WIDGET_SHORTCUTSSECTION, $2);
	}
  ;

shortcutsgroups
  : shortcutsgroup
  | shortcutsgroups shortcutsgroup { token_store(SUM); }
  ;

shortcutsgroup
  : SHORTCUTSGROUP shortcutsshortcuts attr ESHORTCUTSGROUP {
		token_store(PUSH | WIDGET_SHORTCUTSGROUP);
	}
  | PART_SHORTCUTSGROUP tagattr '>' shortcutsshortcuts attr ESHORTCUTSGROUP {
		token_store_attr(PUSH | WIDGET_SHORTCUTSGROUP, $2);
	}
  ;

shortcutsshortcuts
  : shortcutsshortcut
  | shortcutsshortcuts shortcutsshortcut { token_store(SUM); }
  ;

shortcutsshortcut
  : SHORTCUTSSHORTCUT attr ESHORTCUTSSHORTCUT {
		token_store(PUSH | WIDGET_SHORTCUTSSHORTCUT);
	}
  | PART_SHORTCUTSSHORTCUT tagattr '>' attr ESHORTCUTSSHORTCUT {
		token_store_attr(PUSH | WIDGET_SHORTCUTSSHORTCUT, $2);
	}
  ;

toolitemgroup
  : TOOLITEMGROUP ETOOLITEMGROUP {
		yyerror("The toolitemgroup widget requires at least one tool item.");
	}
  | TOOLITEMGROUP radio_group_scope toolbarwlist attr ETOOLITEMGROUP {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_TOOLITEMGROUP);
	}
  | PART_TOOLITEMGROUP tagattr '>' radio_group_scope toolbarwlist attr ETOOLITEMGROUP {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_TOOLITEMGROUP, $2);
	}
  ;

toolbarwlist
  : toolbutton
  | toolitem
  | toggletoolbutton
  | radiotoolbutton
  | menutoolbutton
  | separatortoolitem
  | toolbarwlist toolbutton {
		token_store(SUM);
	}
  | toolbarwlist toolitem {
		token_store(SUM);
	}
  | toolbarwlist toggletoolbutton {
		token_store(SUM);
	}
  | toolbarwlist radiotoolbutton {
		token_store(SUM);
	}
  | toolbarwlist menutoolbutton {
		token_store(SUM);
	}
  | toolbarwlist separatortoolitem {
		token_store(SUM);
	}
  ;

toolitem
  : TOOLITEM ETOOLITEM {
		yyerror("The toolitem widget requires exactly one child widget.");
	}
  | TOOLITEM radio_group_scope wlist attr ETOOLITEM {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_TOOLITEM);
	}
  | PART_TOOLITEM tagattr '>' radio_group_scope wlist attr ETOOLITEM {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_TOOLITEM, $2);
	}
  ;

toolbutton
  : TOOLBUTTON attr ETOOLBUTTON {
		token_store(PUSH | WIDGET_TOOLBUTTON);
	}
  | PART_TOOLBUTTON tagattr '>' attr ETOOLBUTTON {
		token_store_attr(PUSH | WIDGET_TOOLBUTTON, $2);
	}
  ;

toggletoolbutton
  : TOGGLETOOLBUTTON attr ETOGGLETOOLBUTTON {
		token_store(PUSH | WIDGET_TOGGLETOOLBUTTON);
	}
  | PART_TOGGLETOOLBUTTON tagattr '>' attr ETOGGLETOOLBUTTON {
		token_store_attr(PUSH | WIDGET_TOGGLETOOLBUTTON, $2);
	}
  ;

radiotoolbutton
  : RADIOTOOLBUTTON attr ERADIOTOOLBUTTON {
		token_store(PUSH | WIDGET_RADIOTOOLBUTTON);
	}
  | PART_RADIOTOOLBUTTON tagattr '>' attr ERADIOTOOLBUTTON {
		token_store_attr(PUSH | WIDGET_RADIOTOOLBUTTON, $2);
	}
  ;

menutoolbutton
  : MENUTOOLBUTTON EMENUTOOLBUTTON {
		yyerror("The menutoolbutton widget requires at least one menu item.");
	}
  | MENUTOOLBUTTON radio_group_scope menuwlist attr EMENUTOOLBUTTON {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_MENUTOOLBUTTON);
	}
  | PART_MENUTOOLBUTTON tagattr '>' radio_group_scope menuwlist attr EMENUTOOLBUTTON {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_MENUTOOLBUTTON, $2);
	}
  ;

separatortoolitem
  : SEPARATORTOOLITEM attr ESEPARATORTOOLITEM {
		token_store(PUSH | WIDGET_SEPARATORTOOLITEM);
	}
  | PART_SEPARATORTOOLITEM tagattr '>' attr ESEPARATORTOOLITEM {
		token_store_attr(PUSH | WIDGET_SEPARATORTOOLITEM, $2);
	}
  ;

menubar
  : MENUBAR EMENUBAR {
		yyerror("The menubar widget requires at least one menu widget.");
	}
  | MENUBAR input attr EMENUBAR {
		token_store(EMPTY_WIDGETS);
		token_store(PUSH | WIDGET_MENUBAR);
	}
  | PART_MENUBAR tagattr '>' input attr EMENUBAR {
		token_store(EMPTY_WIDGETS);
		token_store_attr(PUSH | WIDGET_MENUBAR, $2);
	}
  | MENUBAR menu attr EMENUBAR {
		token_store(PUSH | WIDGET_MENUBAR);
	}
  | menu MENUBAR menu attr EMENUBAR {
		token_store(PUSH | WIDGET_MENUBAR);
		token_store(SUM);
	}
  | PART_MENUBAR tagattr '>' menu attr EMENUBAR {
		token_store_attr(PUSH | WIDGET_MENUBAR, $2);
	}
  | menu PART_MENUBAR tagattr '>' menu attr EMENUBAR {
		token_store_attr(PUSH | WIDGET_MENUBAR, $3);
		token_store(SUM);
	}
  ;

menuwlist
  : menu
  | menuitem
  | menuitemseparator
  | recentchoosermenu
  ;

menu
  : MENU EMENU {
		yyerror("The menu widget requires at least one menuitem widget.");
	}
	| MENU radio_group_scope input attr EMENU {
		token_store(RGROUP_POP);
		token_store(EMPTY_WIDGETS);
		token_store(PUSH | WIDGET_MENU);
	}
	| PART_MENU tagattr '>' radio_group_scope input attr EMENU {
		token_store(RGROUP_POP);
		token_store(EMPTY_WIDGETS);
		token_store_attr(PUSH | WIDGET_MENU, $2);
	}
	| MENU radio_group_scope menuwlist attr EMENU {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_MENU);
	}
	| menuwlist MENU radio_group_scope menuwlist attr EMENU {
		token_store(RGROUP_POP);
		token_store(PUSH | WIDGET_MENU);
		token_store(SUM);
	}
	| PART_MENU tagattr '>' radio_group_scope menuwlist attr EMENU {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_MENU, $2);
	}
	| menuwlist PART_MENU tagattr '>' radio_group_scope menuwlist attr EMENU {
		token_store(RGROUP_POP);
		token_store_attr(PUSH | WIDGET_MENU, $3);
		token_store(SUM);
	}
  ;

menuitem
  : MENUITEM attr EMENUITEM {
		token_store(PUSH | WIDGET_MENUITEM);
	}
  | menuwlist MENUITEM attr EMENUITEM {
		token_store(PUSH | WIDGET_MENUITEM);
		token_store(SUM);
	}
  | PART_MENUITEM tagattr '>' attr EMENUITEM {
		token_store_attr(PUSH | WIDGET_MENUITEM, $2);
	}
  | menuwlist PART_MENUITEM tagattr '>' attr EMENUITEM {
		token_store_attr(PUSH | WIDGET_MENUITEM, $3);
		token_store(SUM);
	}
  ;

menuitemseparator
  : MENUITEMSEPARATOR EMENUITEMSEPARATOR {
		token_store(PUSH | WIDGET_MENUITEMSEPARATOR);
	}
  | menuwlist MENUITEMSEPARATOR EMENUITEMSEPARATOR {
		token_store(PUSH | WIDGET_MENUITEMSEPARATOR);
		token_store(SUM);
	}
  ;

recentchoosermenu
  : RECENTCHOOSERMENU attr ERECENTCHOOSERMENU {
		token_store(PUSH | WIDGET_RECENTCHOOSERMENU);
	}
  | menuwlist RECENTCHOOSERMENU attr ERECENTCHOOSERMENU {
		token_store(PUSH | WIDGET_RECENTCHOOSERMENU);
		token_store(SUM);
	}
  | PART_RECENTCHOOSERMENU tagattr '>' attr ERECENTCHOOSERMENU {
		token_store_attr(PUSH | WIDGET_RECENTCHOOSERMENU, $2);
	}
  | menuwlist PART_RECENTCHOOSERMENU tagattr '>' attr ERECENTCHOOSERMENU {
		token_store_attr(PUSH | WIDGET_RECENTCHOOSERMENU, $3);
		token_store(SUM);
	}
  ;

hseparator
  : HSEPARATOR EHSEPARATOR {
		token_store(PUSH | WIDGET_HSEPARATOR);
	}
  | PART_HSEPARATOR tagattr '>' EHSEPARATOR {
		token_store_attr(PUSH | WIDGET_HSEPARATOR, $2);
	}
  ;

vseparator
  : VSEPARATOR EVSEPARATOR {
		token_store(PUSH | WIDGET_VSEPARATOR);
	}
  | PART_VSEPARATOR tagattr '>' EVSEPARATOR {
		token_store_attr(PUSH | WIDGET_VSEPARATOR, $2);
	}
  ;

comboboxtext
  : COMBOBOXTEXT attr ECOMBOBOXTEXT {
		token_store(PUSH | WIDGET_COMBOBOXTEXT);
	}
  | PART_COMBOBOXTEXT tagattr '>' attr ECOMBOBOXTEXT {
		token_store_attr(PUSH | WIDGET_COMBOBOXTEXT, $2);
	}
  ;

comboboxentry
  : COMBOBOXENTRY attr ECOMBOBOXENTRY {
		token_store(PUSH | WIDGET_COMBOBOXENTRY);
	}
  | PART_COMBOBOXENTRY tagattr '>' attr ECOMBOBOXENTRY {
		token_store_attr(PUSH | WIDGET_COMBOBOXENTRY, $2);
	}
  ;

hscale
  : HSCALE attr EHSCALE {
		token_store(PUSH | WIDGET_HSCALE);
	}
  | PART_HSCALE tagattr '>' attr EHSCALE {
		token_store_attr(PUSH | WIDGET_HSCALE, $2);
	}
  ;

vscale
  : VSCALE attr EVSCALE {
		token_store(PUSH | WIDGET_VSCALE);
	}
  | PART_VSCALE tagattr '>' attr EVSCALE {
		token_store_attr(PUSH | WIDGET_VSCALE, $2);
	}
  ;

spinbutton
  : SPINBUTTON attr ESPINBUTTON {
		token_store(PUSH | WIDGET_SPINBUTTON);
	}
  | PART_SPINBUTTON tagattr '>' attr ESPINBUTTON {
		token_store_attr(PUSH | WIDGET_SPINBUTTON, $2);
	}
  ;

timer
  : TIMER attr ETIMER {
		token_store(PUSH | WIDGET_TIMER);
	}
  | PART_TIMER tagattr '>' attr ETIMER {
		token_store_attr(PUSH | WIDGET_TIMER, $2);
	}
  ;

togglebutton
  : TOGGLEBUTTON attr ETOGGLEBUTTON {
		token_store(PUSH | WIDGET_TOGGLEBUTTON);
	}
  | PART_TOGGLEBUTTON tagattr '>' attr ETOGGLEBUTTON {
		token_store_attr(PUSH | WIDGET_TOGGLEBUTTON, $2);
	}
  ;

statusbar
  : STATUSBAR attr ESTATUSBAR {
		token_store(PUSH | WIDGET_STATUSBAR);
	}
  | PART_STATUSBAR tagattr '>' attr ESTATUSBAR {
		token_store_attr(PUSH | WIDGET_STATUSBAR, $2);
	}
  ;

statusicon
  : STATUSICON attr ESTATUSICON {
		token_store(PUSH | WIDGET_STATUSICON);
	}
  | PART_STATUSICON tagattr '>' attr ESTATUSICON {
		token_store_attr(PUSH | WIDGET_STATUSICON, $2);
	}
  ;

accellabel
  : ACCELLABEL attr EACCELLABEL {
		token_store(PUSH | WIDGET_ACCELLABEL);
	}
  | PART_ACCELLABEL tagattr '>' attr EACCELLABEL {
		token_store_attr(PUSH | WIDGET_ACCELLABEL, $2);
	}
  ;

arrow
  : ARROW attr EARROW {
		token_store(PUSH | WIDGET_ARROW);
	}
  | PART_ARROW tagattr '>' attr EARROW {
		token_store_attr(PUSH | WIDGET_ARROW, $2);
	}
  ;

hsv
  : HSV attr EHSV {
		token_store(PUSH | WIDGET_HSV);
	}
  | PART_HSV tagattr '>' attr EHSV {
		token_store_attr(PUSH | WIDGET_HSV, $2);
	}
  ;

hruler
  : HRULER attr EHRULER {
		token_store(PUSH | WIDGET_HRULER);
	}
  | PART_HRULER tagattr '>' attr EHRULER {
		token_store_attr(PUSH | WIDGET_HRULER, $2);
	}
  ;

vruler
  : VRULER attr EVRULER {
		token_store(PUSH | WIDGET_VRULER);
	}
  | PART_VRULER tagattr '>' attr EVRULER {
		token_store_attr(PUSH | WIDGET_VRULER, $2);
	}
  ;

colorbutton
  : COLORBUTTON attr ECOLORBUTTON {
		token_store(PUSH | WIDGET_COLORBUTTON);
	}
  | PART_COLORBUTTON tagattr '>' attr ECOLORBUTTON {
		token_store_attr(PUSH | WIDGET_COLORBUTTON, $2);
	}
  ;

fontbutton
  : FONTBUTTON attr EFONTBUTTON {
		token_store(PUSH | WIDGET_FONTBUTTON);
	}
  | PART_FONTBUTTON tagattr '>' attr EFONTBUTTON {
		token_store_attr(PUSH | WIDGET_FONTBUTTON, $2);
	}
  ;

terminal
  : TERMINAL attr ETERMINAL {
		token_store(PUSH | WIDGET_TERMINAL);
	}
  | PART_TERMINAL tagattr '>' attr ETERMINAL {
		token_store_attr(PUSH | WIDGET_TERMINAL, $2);
	}
  ;

attr
  :
  | attr defaultvalue
  | attr sensitive
  | attr label
  | attr width
  | attr height
  | attr input
  | attr output
  | attr variable
  | attr action
  | attr item
  ;

label
  :    LABEL STRING ELABEL          {
		token_store_owned_argument(SET | ATTR_LABEL, $2);     }
  ;

sensitive
  : SENSITIVE STRING ESENSITIVE       {
     token_store_owned_argument(SET | ATTR_SENSITIVE, $2);  }
  ; 

defaultvalue
  : DEFAULT STRING EDEFAULT  {
     token_store_owned_argument(SET | ATTR_DEFAULT, $2);   }
  ;

width
  : WIDTH STRING EWIDTH             {
     token_store_owned_argument(SET | ATTR_WIDTH, $2);    }
  ;

height
  : HEIGHT STRING EHEIGHT           {
     token_store_owned_argument(SET | ATTR_HEIGHT, $2);   }
  ;

input
  : INPUT STRING EINPUT    {
		token_store_owned_argument(SET|ATTR_INPUT|SUB_ATTR_SHELL, $2);
	}
  | PART_INPUT tagattr '>' STRING EINPUT {
		token_store_owned_argument_attr(SET|ATTR_INPUT, $4, $2);
	}
  | INPUTFILE STRING EINPUT  {
		token_store_owned_argument(SET|ATTR_INPUT|SUB_ATTR_FILE, $2);
	}
  | PART_INPUTFILE tagattr '>' STRING EINPUT {
		token_store_owned_argument_attr(SET|ATTR_INPUT|SUB_ATTR_FILE, $4, $2);
	}
  | PART_INPUTFILE tagattr '>' EINPUT {
		token_store_with_argument_attr(SET|ATTR_INPUT|SUB_ATTR_FILE, "", $2); 
	}
  ;

output
  : OUTPUT STRING EOUTPUT {
		fprintf(stderr, "<output>: Not implemented.\n");
		g_free($2);
	}
  | OUTPUTFILE STRING EOUTPUT {
		token_store_owned_argument(SET|ATTR_OUTPUT|SUB_ATTR_FILE, $2);
	}
  | PART_OUTPUTFILE tagattr '>' STRING EOUTPUT {
		token_store_owned_argument_attr(SET|ATTR_OUTPUT|SUB_ATTR_FILE, $4, $2);
	}
  ;

variable
  : VARIABLE STRING EVARIABLE {
		token_store_owned_argument(SET | ATTR_VARIABLE, $2);
	}
  | PART_VARIABLE tagattr '>' STRING EVARIABLE {
		token_store_owned_argument_attr(SET | ATTR_VARIABLE, $4, $2);
	}
  ; 

action
  : ACTION STRING EACTION  {
		token_store_owned_argument(SET|ATTR_ACTION, $2);
	}
  | PART_ACTION tagattr '>' STRING EACTION {
		token_store_owned_argument_attr(SET | ATTR_ACTION, $4, $2);
	}
  ;

item
  : ITEM STRING EITEM {
		token_store_owned_argument(SET | ATTR_ITEM, $2);
	}
  | ITEM EITEM {
		token_store_with_argument( SET | ATTR_ITEM, "");
    	}
  | PART_ITEM tagattr '>' STRING EITEM {
		token_store_owned_argument_attr(SET | ATTR_ITEM, $4, $2);
                    }
  ;

tagattr
  : TAG_ATTR_NAME '=' STRING {
		$$ = new_tag_attributeset_owned($1, $3);
	}
  | tagattr TAG_ATTR_NAME '=' STRING {
		$$ = add_tag_attribute_owned($1, $2, $4);
	}
  ;

imperative
  : COMM assignment '>' ENDCOMM 
  | SHOW_WIDGETS { 
		token_store(SHOW);     
	}
  | if expression '>' then wlist endif {
  		instruction_set_jump($4, $6 + 1);
	}
  | while expression '>' do wlist ewhile {
		instruction_set_jump($4, $6 + 1);
		instruction_set_jump($6, $1 + 1);
	}
  ;

assignment
  : EMB_VARIABLE ':' '=' expression {
		token_store_owned_argument(IMASSG | VARIABLE_NAME, $1);
	}
  ;

expression
  : EMB_VARIABLE {
		token_store_owned_argument(IMPUSH | VARIABLE_NAME, $1);
	}
  | EMB_NUMBER {
		token_store_owned_argument(IMPUSH | CONST_NUMBER, $1);
  	}
  | expression '+' expression {
  		token_store(IMPUSH | OP_ADD);
	}
  | expression '-' expression {
  		token_store(IMPUSH | OP_SUBST);
	}
  | expression '*' expression {
  		token_store(IMPUSH | OP_MULT);
	}
  | expression '/' expression {
  		token_store(IMPUSH | OP_DIV);
	}
  | expression '=' expression {
  		token_store(IMPUSH | REL_EQ);
	}
  | expression '!' '=' expression {
  		token_store(IMPUSH | REL_NE);
	}
  ;


if: IF 
  ;

then
  :     { 
		token_store(IFNGOTO); 
		$$ = instruction_get_pc();
	}
  ;

endif
  : ENDIF  {
		token_store(IMMERGE);
		$$ = instruction_get_pc();
	}
  ;

while
  : WHILE { $$ = instruction_get_pc(); }
  ;

ewhile
  : EWHILE {
		token_store(IMMERGE);
		token_store(GOTO); 
		$$ = instruction_get_pc();
	}
  ;

do
  : { 
		token_store(IFNGOTO); 
		$$ = instruction_get_pc();
    }
  ;

linkbutton
  : LINKBUTTON attr ELINKBUTTON {
		token_store(PUSH | WIDGET_LINKBUTTON);
	}
  | PART_LINKBUTTON tagattr '>' attr ELINKBUTTON {
		token_store_attr(PUSH | WIDGET_LINKBUTTON, $2);
	}
  ;

spinner
  : SPINNER attr ESPINNER {
		token_store(PUSH | WIDGET_SPINNER);
	}
  | PART_SPINNER tagattr '>' attr ESPINNER {
		token_store_attr(PUSH | WIDGET_SPINNER, $2);
	}
  ;

filechooserbutton
  : FILECHOOSERBUTTON attr EFILECHOOSERBUTTON {
		token_store(PUSH | WIDGET_FILECHOOSERBUTTON);
	}
  | PART_FILECHOOSERBUTTON tagattr '>' attr EFILECHOOSERBUTTON {
		token_store_attr(PUSH | WIDGET_FILECHOOSERBUTTON, $2);
	}
  ;

appchooserbutton
  : APPCHOOSERBUTTON attr EAPPCHOOSERBUTTON {
		token_store(PUSH | WIDGET_APPCHOOSERBUTTON);
	}
  | PART_APPCHOOSERBUTTON tagattr '>' attr EAPPCHOOSERBUTTON {
		token_store_attr(PUSH | WIDGET_APPCHOOSERBUTTON, $2);
	}
  ;

appchooser
  : APPCHOOSER attr EAPPCHOOSER {
		token_store(PUSH | WIDGET_APPCHOOSER);
	}
  | PART_APPCHOOSER tagattr '>' attr EAPPCHOOSER {
		token_store_attr(PUSH | WIDGET_APPCHOOSER, $2);
	}
  ;

iconview
  : ICONVIEW attr EICONVIEW {
		token_store(PUSH | WIDGET_ICONVIEW);
	}
  | PART_ICONVIEW tagattr '>' attr EICONVIEW {
		token_store_attr(PUSH | WIDGET_ICONVIEW, $2);
	}
  ;

cellview
  : CELLVIEW attr ECELLVIEW {
		token_store(PUSH | WIDGET_CELLVIEW);
	}
  | PART_CELLVIEW tagattr '>' attr ECELLVIEW {
		token_store_attr(PUSH | WIDGET_CELLVIEW, $2);
	}
  ;

drawingarea
  : DRAWINGAREA attr EDRAWINGAREA {
		token_store(PUSH | WIDGET_DRAWINGAREA);
	}
  | PART_DRAWINGAREA tagattr '>' attr EDRAWINGAREA {
		token_store_attr(PUSH | WIDGET_DRAWINGAREA, $2);
	}
  ;

imageview
  : IMAGEVIEW attr EIMAGEVIEW {
		token_store(PUSH | WIDGET_IMAGEVIEW);
	}
  | PART_IMAGEVIEW tagattr '>' attr EIMAGEVIEW {
		token_store_attr(PUSH | WIDGET_IMAGEVIEW, $2);
	}
  ;

scalebutton
  : SCALEBUTTON attr ESCALEBUTTON {
		token_store(PUSH | WIDGET_SCALEBUTTON);
	}
  | PART_SCALEBUTTON tagattr '>' attr ESCALEBUTTON {
		token_store_attr(PUSH | WIDGET_SCALEBUTTON, $2);
	}
  ;

volumebutton
  : VOLUMEBUTTON attr EVOLUMEBUTTON {
		token_store(PUSH | WIDGET_VOLUMEBUTTON);
	}
  | PART_VOLUMEBUTTON tagattr '>' attr EVOLUMEBUTTON {
		token_store_attr(PUSH | WIDGET_VOLUMEBUTTON, $2);
	}
  ;

recentchooser
  : RECENTCHOOSER attr ERECENTCHOOSER {
		token_store(PUSH | WIDGET_RECENTCHOOSER);
	}
  | PART_RECENTCHOOSER tagattr '>' attr ERECENTCHOOSER {
		token_store_attr(PUSH | WIDGET_RECENTCHOOSER, $2);
	}
  ;

socket
  : SOCKET attr ESOCKET {
		token_store(PUSH | WIDGET_SOCKET);
	}
  | PART_SOCKET tagattr '>' attr ESOCKET {
		token_store_attr(PUSH | WIDGET_SOCKET, $2);
	}
  ;

hscrollbar
  : HSCROLLBAR attr EHSCROLLBAR {
		token_store(PUSH | WIDGET_HSCROLLBAR);
	}
  | PART_HSCROLLBAR tagattr '>' attr EHSCROLLBAR {
		token_store_attr(PUSH | WIDGET_HSCROLLBAR, $2);
	}
  ;

vscrollbar
  : VSCROLLBAR attr EVSCROLLBAR {
		token_store(PUSH | WIDGET_VSCROLLBAR);
	}
  | PART_VSCROLLBAR tagattr '>' attr EVSCROLLBAR {
		token_store_attr(PUSH | WIDGET_VSCROLLBAR, $2);
	}
  ;

%%

extern gboolean option_print_ir;

int gtkdialog_wrap(void)
{
	#ifdef DEBUG
	g_message("%s(): Start", __func__);
	#endif
	return 1;
}

int gtkdialog_error(char *c)
{
	g_error("%s: Error in line %d, near token '%s': %s\n", 
		PACKAGE, linenumber, Token, c);
}

void yyerror_simple(char *c)
{
	g_error("%s: Error: %s", PACKAGE, c);
}

int yywarning(char *c){
	#ifdef DEBUG
		gtkdialog_warning("Warning: %s.", c);
	#endif
	if (!option_no_warning)
		gtkdialog_warning("%s: Warning: %s.", PACKAGE, c);
	return option_no_warning;
}
