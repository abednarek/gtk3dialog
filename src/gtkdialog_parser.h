/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_GTKDIALOG_GTKDIALOG_PARSER_H_INCLUDED
# define YY_GTKDIALOG_GTKDIALOG_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gtkdialog_debug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    WINDOW = 258,                  /* WINDOW  */
    PART_WINDOW = 259,             /* PART_WINDOW  */
    EWINDOW = 260,                 /* EWINDOW  */
    VBOX = 261,                    /* VBOX  */
    PART_VBOX = 262,               /* PART_VBOX  */
    EVBOX = 263,                   /* EVBOX  */
    HBOX = 264,                    /* HBOX  */
    PART_HBOX = 265,               /* PART_HBOX  */
    EHBOX = 266,                   /* EHBOX  */
    NOTEBOOK = 267,                /* NOTEBOOK  */
    ENOTEBOOK = 268,               /* ENOTEBOOK  */
    PART_NOTEBOOK = 269,           /* PART_NOTEBOOK  */
    FRAME = 270,                   /* FRAME  */
    TAG_ATTR_NAME = 271,           /* TAG_ATTR_NAME  */
    EFRAME = 272,                  /* EFRAME  */
    ENTRY = 273,                   /* ENTRY  */
    EENTRY = 274,                  /* EENTRY  */
    PART_ENTRY = 275,              /* PART_ENTRY  */
    SEARCHENTRY = 276,             /* SEARCHENTRY  */
    ESEARCHENTRY = 277,            /* ESEARCHENTRY  */
    PART_SEARCHENTRY = 278,        /* PART_SEARCHENTRY  */
    MENUBAR = 279,                 /* MENUBAR  */
    PART_MENUBAR = 280,            /* PART_MENUBAR  */
    EMENUBAR = 281,                /* EMENUBAR  */
    MENU = 282,                    /* MENU  */
    PART_MENU = 283,               /* PART_MENU  */
    EMENU = 284,                   /* EMENU  */
    POPUPMENU = 285,               /* POPUPMENU  */
    PART_POPUPMENU = 286,          /* PART_POPUPMENU  */
    EPOPUPMENU = 287,              /* EPOPUPMENU  */
    MENUITEM = 288,                /* MENUITEM  */
    PART_MENUITEM = 289,           /* PART_MENUITEM  */
    EMENUITEM = 290,               /* EMENUITEM  */
    MENUITEMSEPARATOR = 291,       /* MENUITEMSEPARATOR  */
    EMENUITEMSEPARATOR = 292,      /* EMENUITEMSEPARATOR  */
    EDIT = 293,                    /* EDIT  */
    PART_EDIT = 294,               /* PART_EDIT  */
    EEDIT = 295,                   /* EEDIT  */
    TREE = 296,                    /* TREE  */
    PART_TREE = 297,               /* PART_TREE  */
    ETREE = 298,                   /* ETREE  */
    CHOOSER = 299,                 /* CHOOSER  */
    PART_CHOOSER = 300,            /* PART_CHOOSER  */
    ECHOOSER = 301,                /* ECHOOSER  */
    LABEL = 302,                   /* LABEL  */
    ELABEL = 303,                  /* ELABEL  */
    ITEM = 304,                    /* ITEM  */
    EITEM = 305,                   /* EITEM  */
    PART_ITEM = 306,               /* PART_ITEM  */
    BUTTON = 307,                  /* BUTTON  */
    PART_BUTTON = 308,             /* PART_BUTTON  */
    EBUTTON = 309,                 /* EBUTTON  */
    BUTTONOK = 310,                /* BUTTONOK  */
    BUTTONCANCEL = 311,            /* BUTTONCANCEL  */
    BUTTONHELP = 312,              /* BUTTONHELP  */
    BUTTONYES = 313,               /* BUTTONYES  */
    BUTTONNO = 314,                /* BUTTONNO  */
    CHECKBOX = 315,                /* CHECKBOX  */
    ECHECKBOX = 316,               /* ECHECKBOX  */
    PART_CHECKBOX = 317,           /* PART_CHECKBOX  */
    RADIO = 318,                   /* RADIO  */
    ERADIO = 319,                  /* ERADIO  */
    PART_RADIO = 320,              /* PART_RADIO  */
    PROGRESSBAR = 321,             /* PROGRESSBAR  */
    EPROGRESSBAR = 322,            /* EPROGRESSBAR  */
    PART_PROGRESSBAR = 323,        /* PART_PROGRESSBAR  */
    LIST = 324,                    /* LIST  */
    PART_LIST = 325,               /* PART_LIST  */
    ELIST = 326,                   /* ELIST  */
    TABLE = 327,                   /* TABLE  */
    ETABLE = 328,                  /* ETABLE  */
    PART_TABLE = 329,              /* PART_TABLE  */
    COMBOBOX = 330,                /* COMBOBOX  */
    PART_COMBOBOX = 331,           /* PART_COMBOBOX  */
    ECOMBOBOX = 332,               /* ECOMBOBOX  */
    GVIM = 333,                    /* GVIM  */
    EGVIM = 334,                   /* EGVIM  */
    TEXT = 335,                    /* TEXT  */
    PART_TEXT = 336,               /* PART_TEXT  */
    ETEXT = 337,                   /* ETEXT  */
    PIXMAP = 338,                  /* PIXMAP  */
    PART_PIXMAP = 339,             /* PART_PIXMAP  */
    EPIXMAP = 340,                 /* EPIXMAP  */
    DEFAULT = 341,                 /* DEFAULT  */
    EDEFAULT = 342,                /* EDEFAULT  */
    SENSITIVE = 343,               /* SENSITIVE  */
    ESENSITIVE = 344,              /* ESENSITIVE  */
    VARIABLE = 345,                /* VARIABLE  */
    PART_VARIABLE = 346,           /* PART_VARIABLE  */
    EVARIABLE = 347,               /* EVARIABLE  */
    WIDTH = 348,                   /* WIDTH  */
    EWIDTH = 349,                  /* EWIDTH  */
    HEIGHT = 350,                  /* HEIGHT  */
    EHEIGHT = 351,                 /* EHEIGHT  */
    INPUT = 352,                   /* INPUT  */
    INPUTFILE = 353,               /* INPUTFILE  */
    EINPUT = 354,                  /* EINPUT  */
    PART_INPUT = 355,              /* PART_INPUT  */
    PART_INPUTFILE = 356,          /* PART_INPUTFILE  */
    OUTPUT = 357,                  /* OUTPUT  */
    OUTPUTFILE = 358,              /* OUTPUTFILE  */
    EOUTPUT = 359,                 /* EOUTPUT  */
    ACTION = 360,                  /* ACTION  */
    EACTION = 361,                 /* EACTION  */
    PART_ACTION = 362,             /* PART_ACTION  */
    COMM = 363,                    /* COMM  */
    ENDCOMM = 364,                 /* ENDCOMM  */
    IF = 365,                      /* IF  */
    ENDIF = 366,                   /* ENDIF  */
    WHILE = 367,                   /* WHILE  */
    EWHILE = 368,                  /* EWHILE  */
    SHOW_WIDGETS = 369,            /* SHOW_WIDGETS  */
    EMB_VARIABLE = 370,            /* EMB_VARIABLE  */
    EMB_NUMBER = 371,              /* EMB_NUMBER  */
    END_OF_FILE = 372,             /* END_OF_FILE  */
    NUMBER = 373,                  /* NUMBER  */
    STRING = 374,                  /* STRING  */
    UMINUS = 375,                  /* UMINUS  */
    HSEPARATOR = 376,              /* HSEPARATOR  */
    PART_HSEPARATOR = 377,         /* PART_HSEPARATOR  */
    EHSEPARATOR = 378,             /* EHSEPARATOR  */
    VSEPARATOR = 379,              /* VSEPARATOR  */
    PART_VSEPARATOR = 380,         /* PART_VSEPARATOR  */
    EVSEPARATOR = 381,             /* EVSEPARATOR  */
    COMBOBOXTEXT = 382,            /* COMBOBOXTEXT  */
    PART_COMBOBOXTEXT = 383,       /* PART_COMBOBOXTEXT  */
    ECOMBOBOXTEXT = 384,           /* ECOMBOBOXTEXT  */
    COMBOBOXENTRY = 385,           /* COMBOBOXENTRY  */
    PART_COMBOBOXENTRY = 386,      /* PART_COMBOBOXENTRY  */
    ECOMBOBOXENTRY = 387,          /* ECOMBOBOXENTRY  */
    HSCALE = 388,                  /* HSCALE  */
    PART_HSCALE = 389,             /* PART_HSCALE  */
    EHSCALE = 390,                 /* EHSCALE  */
    VSCALE = 391,                  /* VSCALE  */
    PART_VSCALE = 392,             /* PART_VSCALE  */
    EVSCALE = 393,                 /* EVSCALE  */
    SPINBUTTON = 394,              /* SPINBUTTON  */
    PART_SPINBUTTON = 395,         /* PART_SPINBUTTON  */
    ESPINBUTTON = 396,             /* ESPINBUTTON  */
    TIMER = 397,                   /* TIMER  */
    PART_TIMER = 398,              /* PART_TIMER  */
    ETIMER = 399,                  /* ETIMER  */
    TOGGLEBUTTON = 400,            /* TOGGLEBUTTON  */
    PART_TOGGLEBUTTON = 401,       /* PART_TOGGLEBUTTON  */
    ETOGGLEBUTTON = 402,           /* ETOGGLEBUTTON  */
    STATUSBAR = 403,               /* STATUSBAR  */
    PART_STATUSBAR = 404,          /* PART_STATUSBAR  */
    ESTATUSBAR = 405,              /* ESTATUSBAR  */
    COLORBUTTON = 406,             /* COLORBUTTON  */
    PART_COLORBUTTON = 407,        /* PART_COLORBUTTON  */
    ECOLORBUTTON = 408,            /* ECOLORBUTTON  */
    FONTBUTTON = 409,              /* FONTBUTTON  */
    PART_FONTBUTTON = 410,         /* PART_FONTBUTTON  */
    EFONTBUTTON = 411,             /* EFONTBUTTON  */
    TERMINAL = 412,                /* TERMINAL  */
    PART_TERMINAL = 413,           /* PART_TERMINAL  */
    ETERMINAL = 414,               /* ETERMINAL  */
    EVENTBOX = 415,                /* EVENTBOX  */
    PART_EVENTBOX = 416,           /* PART_EVENTBOX  */
    EEVENTBOX = 417,               /* EEVENTBOX  */
    EXPANDER = 418,                /* EXPANDER  */
    PART_EXPANDER = 419,           /* PART_EXPANDER  */
    EEXPANDER = 420,               /* EEXPANDER  */
    REVEALER = 421,                /* REVEALER  */
    PART_REVEALER = 422,           /* PART_REVEALER  */
    EREVEALER = 423,               /* EREVEALER  */
    SEARCHBAR = 424,               /* SEARCHBAR  */
    PART_SEARCHBAR = 425,          /* PART_SEARCHBAR  */
    ESEARCHBAR = 426,              /* ESEARCHBAR  */
    ACTIONBAR = 427,               /* ACTIONBAR  */
    PART_ACTIONBAR = 428,          /* PART_ACTIONBAR  */
    EACTIONBAR = 429,              /* EACTIONBAR  */
    STACK = 430,                   /* STACK  */
    PART_STACK = 431,              /* PART_STACK  */
    ESTACK = 432,                  /* ESTACK  */
    STACKSWITCHER = 433,           /* STACKSWITCHER  */
    PART_STACKSWITCHER = 434,      /* PART_STACKSWITCHER  */
    ESTACKSWITCHER = 435,          /* ESTACKSWITCHER  */
    STACKSIDEBAR = 436,            /* STACKSIDEBAR  */
    PART_STACKSIDEBAR = 437,       /* PART_STACKSIDEBAR  */
    ESTACKSIDEBAR = 438,           /* ESTACKSIDEBAR  */
    LISTBOX = 439,                 /* LISTBOX  */
    PART_LISTBOX = 440,            /* PART_LISTBOX  */
    ELISTBOX = 441,                /* ELISTBOX  */
    FLOWBOX = 442,                 /* FLOWBOX  */
    PART_FLOWBOX = 443,            /* PART_FLOWBOX  */
    EFLOWBOX = 444,                /* EFLOWBOX  */
    PLACESSIDEBAR = 445,           /* PLACESSIDEBAR  */
    PART_PLACESSIDEBAR = 446,      /* PART_PLACESSIDEBAR  */
    EPLACESSIDEBAR = 447,          /* EPLACESSIDEBAR  */
    HEADERBAR = 448,               /* HEADERBAR  */
    PART_HEADERBAR = 449,          /* PART_HEADERBAR  */
    EHEADERBAR = 450,              /* EHEADERBAR  */
    POPOVER = 451,                 /* POPOVER  */
    PART_POPOVER = 452,            /* PART_POPOVER  */
    EPOPOVER = 453,                /* EPOPOVER  */
    MENUBUTTON = 454,              /* MENUBUTTON  */
    PART_MENUBUTTON = 455,         /* PART_MENUBUTTON  */
    EMENUBUTTON = 456,             /* EMENUBUTTON  */
    POPOVERMENU = 457,             /* POPOVERMENU  */
    PART_POPOVERMENU = 458,        /* PART_POPOVERMENU  */
    EPOPOVERMENU = 459,            /* EPOPOVERMENU  */
    MODELBUTTON = 460,             /* MODELBUTTON  */
    PART_MODELBUTTON = 461,        /* PART_MODELBUTTON  */
    EMODELBUTTON = 462,            /* EMODELBUTTON  */
    OVERLAY = 463,                 /* OVERLAY  */
    PART_OVERLAY = 464,            /* PART_OVERLAY  */
    EOVERLAY = 465,                /* EOVERLAY  */
    SWITCH = 466,                  /* SWITCH  */
    PART_SWITCH = 467,             /* PART_SWITCH  */
    ESWITCH = 468,                 /* ESWITCH  */
    LEVELBAR = 469,                /* LEVELBAR  */
    PART_LEVELBAR = 470,           /* PART_LEVELBAR  */
    ELEVELBAR = 471,               /* ELEVELBAR  */
    HPANED = 472,                  /* HPANED  */
    PART_HPANED = 473,             /* PART_HPANED  */
    EHPANED = 474,                 /* EHPANED  */
    VPANED = 475,                  /* VPANED  */
    PART_VPANED = 476,             /* PART_VPANED  */
    EVPANED = 477,                 /* EVPANED  */
    ALIGNMENT = 478,               /* ALIGNMENT  */
    PART_ALIGNMENT = 479,          /* PART_ALIGNMENT  */
    EALIGNMENT = 480,              /* EALIGNMENT  */
    HBUTTONBOX = 481,              /* HBUTTONBOX  */
    PART_HBUTTONBOX = 482,         /* PART_HBUTTONBOX  */
    EHBUTTONBOX = 483,             /* EHBUTTONBOX  */
    VBUTTONBOX = 484,              /* VBUTTONBOX  */
    PART_VBUTTONBOX = 485,         /* PART_VBUTTONBOX  */
    EVBUTTONBOX = 486,             /* EVBUTTONBOX  */
    TOOLBAR = 487,                 /* TOOLBAR  */
    PART_TOOLBAR = 488,            /* PART_TOOLBAR  */
    ETOOLBAR = 489,                /* ETOOLBAR  */
    TOOLBUTTON = 490,              /* TOOLBUTTON  */
    PART_TOOLBUTTON = 491,         /* PART_TOOLBUTTON  */
    ETOOLBUTTON = 492,             /* ETOOLBUTTON  */
    TOGGLETOOLBUTTON = 493,        /* TOGGLETOOLBUTTON  */
    PART_TOGGLETOOLBUTTON = 494,   /* PART_TOGGLETOOLBUTTON  */
    ETOGGLETOOLBUTTON = 495,       /* ETOGGLETOOLBUTTON  */
    RADIOTOOLBUTTON = 496,         /* RADIOTOOLBUTTON  */
    PART_RADIOTOOLBUTTON = 497,    /* PART_RADIOTOOLBUTTON  */
    ERADIOTOOLBUTTON = 498,        /* ERADIOTOOLBUTTON  */
    MENUTOOLBUTTON = 499,          /* MENUTOOLBUTTON  */
    PART_MENUTOOLBUTTON = 500,     /* PART_MENUTOOLBUTTON  */
    EMENUTOOLBUTTON = 501,         /* EMENUTOOLBUTTON  */
    SEPARATORTOOLITEM = 502,       /* SEPARATORTOOLITEM  */
    PART_SEPARATORTOOLITEM = 503,  /* PART_SEPARATORTOOLITEM  */
    ESEPARATORTOOLITEM = 504,      /* ESEPARATORTOOLITEM  */
    CALENDAR = 505,                /* CALENDAR  */
    PART_CALENDAR = 506,           /* PART_CALENDAR  */
    ECALENDAR = 507,               /* ECALENDAR  */
    LINKBUTTON = 508,              /* LINKBUTTON  */
    PART_LINKBUTTON = 509,         /* PART_LINKBUTTON  */
    ELINKBUTTON = 510,             /* ELINKBUTTON  */
    SPINNER = 511,                 /* SPINNER  */
    PART_SPINNER = 512,            /* PART_SPINNER  */
    ESPINNER = 513,                /* ESPINNER  */
    GRID = 514,                    /* GRID  */
    PART_GRID = 515,               /* PART_GRID  */
    EGRID = 516,                   /* EGRID  */
    FILECHOOSERBUTTON = 517,       /* FILECHOOSERBUTTON  */
    PART_FILECHOOSERBUTTON = 518,  /* PART_FILECHOOSERBUTTON  */
    EFILECHOOSERBUTTON = 519,      /* EFILECHOOSERBUTTON  */
    INFOBAR = 520,                 /* INFOBAR  */
    PART_INFOBAR = 521,            /* PART_INFOBAR  */
    EINFOBAR = 522,                /* EINFOBAR  */
    ASSISTANT = 523,               /* ASSISTANT  */
    PART_ASSISTANT = 524,          /* PART_ASSISTANT  */
    EASSISTANT = 525,              /* EASSISTANT  */
    ICONVIEW = 526,                /* ICONVIEW  */
    PART_ICONVIEW = 527,           /* PART_ICONVIEW  */
    EICONVIEW = 528,               /* EICONVIEW  */
    SCALEBUTTON = 529,             /* SCALEBUTTON  */
    PART_SCALEBUTTON = 530,        /* PART_SCALEBUTTON  */
    ESCALEBUTTON = 531,            /* ESCALEBUTTON  */
    VOLUMEBUTTON = 532,            /* VOLUMEBUTTON  */
    PART_VOLUMEBUTTON = 533,       /* PART_VOLUMEBUTTON  */
    EVOLUMEBUTTON = 534,           /* EVOLUMEBUTTON  */
    ASPECTFRAME = 535,             /* ASPECTFRAME  */
    PART_ASPECTFRAME = 536,        /* PART_ASPECTFRAME  */
    EASPECTFRAME = 537,            /* EASPECTFRAME  */
    MESSAGEDIALOG = 538,           /* MESSAGEDIALOG  */
    PART_MESSAGEDIALOG = 539,      /* PART_MESSAGEDIALOG  */
    EMESSAGEDIALOG = 540,          /* EMESSAGEDIALOG  */
    DIALOG = 541,                  /* DIALOG  */
    PART_DIALOG = 542,             /* PART_DIALOG  */
    EDIALOG = 543,                 /* EDIALOG  */
    ABOUTDIALOG = 544,             /* ABOUTDIALOG  */
    PART_ABOUTDIALOG = 545,        /* PART_ABOUTDIALOG  */
    EABOUTDIALOG = 546,            /* EABOUTDIALOG  */
    RECENTCHOOSER = 547,           /* RECENTCHOOSER  */
    PART_RECENTCHOOSER = 548,      /* PART_RECENTCHOOSER  */
    ERECENTCHOOSER = 549,          /* ERECENTCHOOSER  */
    RECENTCHOOSERMENU = 550,       /* RECENTCHOOSERMENU  */
    PART_RECENTCHOOSERMENU = 551,  /* PART_RECENTCHOOSERMENU  */
    ERECENTCHOOSERMENU = 552,      /* ERECENTCHOOSERMENU  */
    STATUSICON = 553,              /* STATUSICON  */
    PART_STATUSICON = 554,         /* PART_STATUSICON  */
    ESTATUSICON = 555,             /* ESTATUSICON  */
    ACCELLABEL = 556,              /* ACCELLABEL  */
    PART_ACCELLABEL = 557,         /* PART_ACCELLABEL  */
    EACCELLABEL = 558,             /* EACCELLABEL  */
    ARROW = 559,                   /* ARROW  */
    PART_ARROW = 560,              /* PART_ARROW  */
    EARROW = 561,                  /* EARROW  */
    HSV = 562,                     /* HSV  */
    PART_HSV = 563,                /* PART_HSV  */
    EHSV = 564,                    /* EHSV  */
    PLUG = 565,                    /* PLUG  */
    PART_PLUG = 566,               /* PART_PLUG  */
    EPLUG = 567,                   /* EPLUG  */
    OFFSCREENWINDOW = 568,         /* OFFSCREENWINDOW  */
    PART_OFFSCREENWINDOW = 569,    /* PART_OFFSCREENWINDOW  */
    EOFFSCREENWINDOW = 570,        /* EOFFSCREENWINDOW  */
    FILECHOOSERDIALOG = 571,       /* FILECHOOSERDIALOG  */
    PART_FILECHOOSERDIALOG = 572,  /* PART_FILECHOOSERDIALOG  */
    EFILECHOOSERDIALOG = 573,      /* EFILECHOOSERDIALOG  */
    PAGESETUPDIALOG = 574,         /* PAGESETUPDIALOG  */
    PART_PAGESETUPDIALOG = 575,    /* PART_PAGESETUPDIALOG  */
    EPAGESETUPDIALOG = 576,        /* EPAGESETUPDIALOG  */
    PRINTDIALOG = 577,             /* PRINTDIALOG  */
    PART_PRINTDIALOG = 578,        /* PART_PRINTDIALOG  */
    EPRINTDIALOG = 579,            /* EPRINTDIALOG  */
    HRULER = 580,                  /* HRULER  */
    PART_HRULER = 581,             /* PART_HRULER  */
    EHRULER = 582,                 /* EHRULER  */
    VRULER = 583,                  /* VRULER  */
    PART_VRULER = 584,             /* PART_VRULER  */
    EVRULER = 585,                 /* EVRULER  */
    TOOLITEM = 586,                /* TOOLITEM  */
    PART_TOOLITEM = 587,           /* PART_TOOLITEM  */
    ETOOLITEM = 588,               /* ETOOLITEM  */
    FIXED = 589,                   /* FIXED  */
    PART_FIXED = 590,              /* PART_FIXED  */
    EFIXED = 591,                  /* EFIXED  */
    LAYOUT = 592,                  /* LAYOUT  */
    PART_LAYOUT = 593,             /* PART_LAYOUT  */
    ELAYOUT = 594,                 /* ELAYOUT  */
    SCROLLEDWINDOW = 595,          /* SCROLLEDWINDOW  */
    PART_SCROLLEDWINDOW = 596,     /* PART_SCROLLEDWINDOW  */
    ESCROLLEDWINDOW = 597,         /* ESCROLLEDWINDOW  */
    HANDLEBOX = 598,               /* HANDLEBOX  */
    PART_HANDLEBOX = 599,          /* PART_HANDLEBOX  */
    EHANDLEBOX = 600,              /* EHANDLEBOX  */
    TOOLPALETTE = 601,             /* TOOLPALETTE  */
    PART_TOOLPALETTE = 602,        /* PART_TOOLPALETTE  */
    ETOOLPALETTE = 603,            /* ETOOLPALETTE  */
    TOOLITEMGROUP = 604,           /* TOOLITEMGROUP  */
    PART_TOOLITEMGROUP = 605,      /* PART_TOOLITEMGROUP  */
    ETOOLITEMGROUP = 606,          /* ETOOLITEMGROUP  */
    CELLVIEW = 607,                /* CELLVIEW  */
    PART_CELLVIEW = 608,           /* PART_CELLVIEW  */
    ECELLVIEW = 609,               /* ECELLVIEW  */
    DRAWINGAREA = 610,             /* DRAWINGAREA  */
    PART_DRAWINGAREA = 611,        /* PART_DRAWINGAREA  */
    EDRAWINGAREA = 612,            /* EDRAWINGAREA  */
    IMAGEVIEW = 613,               /* IMAGEVIEW  */
    PART_IMAGEVIEW = 614,          /* PART_IMAGEVIEW  */
    EIMAGEVIEW = 615,              /* EIMAGEVIEW  */
    PART_OUTPUTFILE = 616,         /* PART_OUTPUTFILE  */
    SOCKET = 617,                  /* SOCKET  */
    PART_SOCKET = 618,             /* PART_SOCKET  */
    ESOCKET = 619,                 /* ESOCKET  */
    HSCROLLBAR = 620,              /* HSCROLLBAR  */
    PART_HSCROLLBAR = 621,         /* PART_HSCROLLBAR  */
    EHSCROLLBAR = 622,             /* EHSCROLLBAR  */
    VSCROLLBAR = 623,              /* VSCROLLBAR  */
    PART_VSCROLLBAR = 624,         /* PART_VSCROLLBAR  */
    EVSCROLLBAR = 625,             /* EVSCROLLBAR  */
    VIEWPORT = 626,                /* VIEWPORT  */
    PART_VIEWPORT = 627,           /* PART_VIEWPORT  */
    EVIEWPORT = 628,               /* EVIEWPORT  */
    COLORSELECTION = 629,          /* COLORSELECTION  */
    PART_COLORSELECTION = 630,     /* PART_COLORSELECTION  */
    ECOLORSELECTION = 631,         /* ECOLORSELECTION  */
    FONTSELECTION = 632,           /* FONTSELECTION  */
    PART_FONTSELECTION = 633,      /* PART_FONTSELECTION  */
    EFONTSELECTION = 634,          /* EFONTSELECTION  */
    APPCHOOSERBUTTON = 635,        /* APPCHOOSERBUTTON  */
    PART_APPCHOOSERBUTTON = 636,   /* PART_APPCHOOSERBUTTON  */
    EAPPCHOOSERBUTTON = 637,       /* EAPPCHOOSERBUTTON  */
    APPCHOOSER = 638,              /* APPCHOOSER  */
    PART_APPCHOOSER = 639,         /* PART_APPCHOOSER  */
    EAPPCHOOSER = 640,             /* EAPPCHOOSER  */
    APPCHOOSERDIALOG = 641,        /* APPCHOOSERDIALOG  */
    PART_APPCHOOSERDIALOG = 642,   /* PART_APPCHOOSERDIALOG  */
    EAPPCHOOSERDIALOG = 643,       /* EAPPCHOOSERDIALOG  */
    COLORCHOOSERDIALOG = 644,      /* COLORCHOOSERDIALOG  */
    PART_COLORCHOOSERDIALOG = 645, /* PART_COLORCHOOSERDIALOG  */
    ECOLORCHOOSERDIALOG = 646,     /* ECOLORCHOOSERDIALOG  */
    FONTCHOOSERDIALOG = 647,       /* FONTCHOOSERDIALOG  */
    PART_FONTCHOOSERDIALOG = 648,  /* PART_FONTCHOOSERDIALOG  */
    EFONTCHOOSERDIALOG = 649,      /* EFONTCHOOSERDIALOG  */
    RECENTCHOOSERDIALOG = 650,     /* RECENTCHOOSERDIALOG  */
    PART_RECENTCHOOSERDIALOG = 651, /* PART_RECENTCHOOSERDIALOG  */
    ERECENTCHOOSERDIALOG = 652,    /* ERECENTCHOOSERDIALOG  */
    SHORTCUTSWINDOW = 653,         /* SHORTCUTSWINDOW  */
    PART_SHORTCUTSWINDOW = 654,    /* PART_SHORTCUTSWINDOW  */
    ESHORTCUTSWINDOW = 655,        /* ESHORTCUTSWINDOW  */
    SHORTCUTSSECTION = 656,        /* SHORTCUTSSECTION  */
    PART_SHORTCUTSSECTION = 657,   /* PART_SHORTCUTSSECTION  */
    ESHORTCUTSSECTION = 658,       /* ESHORTCUTSSECTION  */
    SHORTCUTSGROUP = 659,          /* SHORTCUTSGROUP  */
    PART_SHORTCUTSGROUP = 660,     /* PART_SHORTCUTSGROUP  */
    ESHORTCUTSGROUP = 661,         /* ESHORTCUTSGROUP  */
    SHORTCUTSSHORTCUT = 662,       /* SHORTCUTSSHORTCUT  */
    PART_SHORTCUTSSHORTCUT = 663,  /* PART_SHORTCUTSSHORTCUT  */
    ESHORTCUTSSHORTCUT = 664       /* ESHORTCUTSSHORTCUT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 113 "gtkdialog_parser.y"

  double     dval;
  char      *cval;
  GList     *lval;
  tag_attr *nvval;
  gint       ival;

#line 481 "gtkdialog_parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE gtkdialog_lval;


int gtkdialog_parse (void);


#endif /* !YY_GTKDIALOG_GTKDIALOG_PARSER_H_INCLUDED  */
