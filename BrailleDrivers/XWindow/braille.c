/*
 * BRLTTY - A background process providing access to the console screen (when in
 *          text mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 1995-2005 by The BRLTTY Team. All rights reserved.
 *
 * BRLTTY comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation.  Please see the file COPYING for details.
 *
 * Web Page: http://mielke.cc/brltty/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */
#define XW_VERSION "BRLTTY driver for X, version 0.1, 2004"
#define XW_COPYRIGHT "Copyright Samuel Thibault <samuel.thibault@ens-lyon.org>"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>

#include "Programs/misc.h"

#if defined(HAVE_PKG_XAW)
#define USE_XAW
#define USE_XT
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Repeater.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/SmeBSB.h>
#elif defined(HAVE_PKG_XAW3D)
#define USE_XAW
#define USE_XT
#include <X11/Xaw3d/Form.h>
#include <X11/Xaw3d/Paned.h>
#include <X11/Xaw3d/Label.h>
#include <X11/Xaw3d/Command.h>
#include <X11/Xaw3d/Repeater.h>
#include <X11/Xaw3d/SimpleMenu.h>
#include <X11/Xaw3d/SmeLine.h>
#include <X11/Xaw3d/SmeBSB.h>
#elif defined(HAVE_PKG_NEXTAW)
#define USE_XAW
#define USE_XT
#include <X11/neXtaw/Form.h>
#include <X11/neXtaw/Paned.h>
#include <X11/neXtaw/Label.h>
#include <X11/neXtaw/Command.h>
#include <X11/neXtaw/Repeater.h>
#include <X11/neXtaw/SimpleMenu.h>
#include <X11/neXtaw/SmeLine.h>
#include <X11/neXtaw/SmeBSB.h>
#elif defined(HAVE_PKG_XAWPLUS)
#define USE_XAW
#define USE_XT
#include <X11/XawPlus/Form.h>
#include <X11/XawPlus/Paned.h>
#include <X11/XawPlus/Label.h>
#include <X11/XawPlus/Command.h>
#include <X11/XawPlus/Repeater.h>
#include <X11/XawPlus/SimpleMenu.h>
#include <X11/XawPlus/SmeLine.h>
#include <X11/XawPlus/SmeBSB.h>
#elif defined(HAVE_PKG_XM)
#define USE_XM
#define USE_XT
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PanedW.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/MenuShell.h>
#elif defined(WINDOWS)
#define USE_WINDOWS
#else /* HAVE_PKG_ */
#error GUI toolkit either unspecified or unsupported
#endif /* HAVE_PKG_ */

#ifdef USE_XT
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#endif /* USE_XT */

#ifdef USE_WINDOWS
#include <commctrl.h>
#define XtNumber(t) (sizeof(t)/sizeof(*t))
#endif /* USE_WINDOWS */

#if defined(USE_XAW)
#define formWidgetClass       formWidgetClass
#define panedWidgetClass      panedWidgetClass
#define labelWidgetClass      labelWidgetClass
#define commandWidgetClass    commandWidgetClass
#define repeaterWidgetClass   repeaterWidgetClass
#define menuEntryWidgetClass  smeBSBObjectClass
#define Nlabel                XtNlabel
#define Ncallback             XtNcallback
#define NvalueChangedCallback XtNcallback
#define Ntop                  XtNtop
#define Nbottom               XtNbottom
#define Nleft                 XtNleft
#define Nright                XtNright
#define ChainTop              XtChainTop
#define ChainBottom           XtChainTop
#define ChainLeft             XtChainLeft
#define ChainRight            XtChainLeft
#define NvertDistance         XtNvertDistance
#define NhorizDistance        XtNhorizDistance
#define NtoggleState          XtNleftBitmap
#define toggleSet             check
#define toggleUnset           None
#elif defined(USE_XM)
#define formWidgetClass       xmFormWidgetClass
#define panedWidgetClass      xmPanedWindowWidgetClass
#define labelWidgetClass      xmLabelWidgetClass
#define commandWidgetClass    xmPushButtonWidgetClass
#define repeaterWidgetClass   xmPushButtonWidgetClass
#define menuEntryWidgetClass  xmToggleButtonWidgetClass
#define Nlabel                XmNlabelString
#define Ncallback             XmNactivateCallback
#define NvalueChangedCallback XmNvalueChangedCallback
#define Ntop                  XmNtopAttachment
#define Nbottom               XmNbottomAttachment
#define Nleft                 XmNleftAttachment
#define Nright                XmNrightAttachment
#define ChainTop              XmATTACH_FORM
#define ChainBottom           XmATTACH_NONE
#define ChainLeft             XmATTACH_FORM
#define ChainRight            XmATTACH_NONE
#define NvertDistance         XmNtopOffset
#define NhorizDistance        XmNleftOffset
#define NtoggleState          XmNset
#define toggleSet             XmSET
#define toggleUnset           XmUNSET
#elif defined(USE_WINDOWS)
#define Widget HWND
#define CHRX 16
#define CHRY 20
#else /* USE_ */
#error GUI toolkit paradigm either unspecified or unsupported
#endif /* USE_ */

typedef enum {
  PARM_TKPARMS,
  PARM_LINES,
  PARM_COLS,
  PARM_MODEL
} DriverParameter;
#define BRLPARMS "tkparms", "lines", "cols", "model"

#define BRL_HAVE_VISUAL_DISPLAY
#include "Programs/brl_driver.h"
#include "braille.h"

#define MAXLINES 3
#define MAXCOLS 80
#define WHOLESIZE (MAXLINES * MAXCOLS)
static int cols,lines;
static char *model = "simple";
static int xtArgc = 1;
static char *xtDefArgv[]= { "brltty", NULL };
static char **xtArgv = xtDefArgv;
static int regenerate;
static void generateToplevel(void);
static void destroyToplevel(void);
#ifdef USE_XAW
static unsigned char displayedWindow[WHOLESIZE];
#endif /* USE_XAW */
static unsigned char displayedVisual[WHOLESIZE];

#define BUTWIDTH 48
#define BUTHEIGHT 32

static Widget toplevel,hbox,display[WHOLESIZE];
#ifdef USE_XT
static Widget menu, bareMenuEntry;
#ifdef USE_XAW
static Pixmap check;
#endif /* USE_XAW */
#endif /* USE_XT */
static int lastcursor = -1;
#ifdef USE_XT
static Widget vbox,keybox;
static Pixel displayForeground,displayBackground;
#ifdef USE_XAW
static Widget displayb[WHOLESIZE];
static XFontSet fontset;
#endif /* USE_XAW */
static XtAppContext app_con;
#ifdef USE_XM
static XmString display_cs;
#endif /* USE_XAW */
#endif /* USE_XT */

#ifdef USE_WINDOWS
static int modelWidth,modelHeight;
#endif /* USE_WINDOWS */

static long keypressed;

#ifdef USE_XT
static void KeyPressCB(Widget w, XtPointer closure, XtPointer callData)
{
  LogPrint(LOG_DEBUG,"keypress(%p)",closure);
  keypressed=(long)closure;
}

static void route(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  int index = atoi(params[0]);
  LogPrint(LOG_DEBUG,"route(%u)", index);
  keypressed = BRL_BLK_ROUTE | (index&BRL_MSK_ARG);
}
#endif /* USE_XT */

#ifdef USE_WINDOWS
static LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_COMMAND) {
    hwnd = (HWND) lParam;
    keypressed = GetWindowLong(hwnd, GWL_USERDATA);
    LogPrint(LOG_DEBUG,"keypress(%lx)", keypressed);
    if ((keypressed & BRL_MSK_BLK) == BRL_BLK_ROUTE)
      lastcursor = -1;
    return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif /* USE_WINDOWS */

static inline Widget crKeyBut(char *name, long keycode, int repeat,
    int horizDistance, int vertDistance)
{
  Widget button;
#if defined(USE_XT)
  button = XtVaCreateManagedWidget(name,
    repeat?repeaterWidgetClass:commandWidgetClass, keybox,
    XtNwidth, BUTWIDTH, XtNheight, BUTHEIGHT,
#ifdef USE_XAW
    XtNinitialDelay, 500, XtNminimumDelay, 100,
#endif /* USE_XAW */
    NhorizDistance, horizDistance,
    NvertDistance, vertDistance,
    Ntop, ChainTop,
    Nbottom, ChainBottom,
    Nleft, ChainLeft,
    Nright, ChainRight,
    NULL);
  XtAddCallback(button, Ncallback, KeyPressCB, (XtPointer) keycode);
#elif defined(USE_WINDOWS)
  button = CreateWindow(WC_BUTTON, name, WS_CHILD | WS_VISIBLE, horizDistance, lines*CHRY+1+vertDistance, BUTWIDTH, BUTHEIGHT, toplevel, NULL, NULL, NULL);
  SetWindowLong(button, GWL_USERDATA, (long) keycode);
#else /* USE_ */
#error Toolkit button creation unspecified
#endif /* USE_ */
  return button;
}

struct button {
  char *label;
  long keycode;
  int repeat;
  int x,y;
};

struct model {
  char *name;
  struct button *buttons;
  int width,height;
  Widget menuEntry;
};

static struct model *keyModel;

static struct button buttons_simple[] = {
  { "^",      BRL_CMD_LNUP,   1, 1, 0 },
  { "<",      BRL_CMD_FWINLT, 1, 0, 1 },
  { "Home",   BRL_CMD_HOME,   0, 1, 1 },
  { ">",      BRL_CMD_FWINRT, 1, 2, 1 },
  { "v",      BRL_CMD_LNDN,   1, 1, 2 },
  { "alt-a",  BRL_FLG_CHAR_META    | BRL_BLK_PASSCHAR | 'a', 0, 0, 3 },
  { "ctrl-a", BRL_FLG_CHAR_CONTROL | BRL_BLK_PASSCHAR | 'a', 0, 1, 3 },
  { "a",      BRL_BLK_PASSCHAR                        | 'a', 0, 2, 3 },
  { "A",      BRL_BLK_PASSCHAR                        | 'A', 0, 3, 3 },
  { NULL,     0,              0, 0, 0},
};

static struct button buttons_vs[] = {
	/*
  { "VT1",  BRL_BLK_SWITCHVT,   1, 0, 1 },
  { "VT2",  BRL_BLK_SWITCHVT+1, 1, 1, 1 },
  { "VT3",  BRL_BLK_SWITCHVT+2, 1, 2, 1 },
  { "VT4",  BRL_BLK_SWITCHVT+3, 1, 6, 1 },
  { "VT5",  BRL_BLK_SWITCHVT+4, 1, 7, 1 },
  { "VT6",  BRL_BLK_SWITCHVT+5, 1, 8, 1 },
	*/
  //{ "B5", EOF, /* cut */      1, 5, 2 },
  { "TOP",  BRL_CMD_TOP_LEFT,   1, 6, 2 },
  { "BOT",  BRL_CMD_BOT_LEFT,   1, 6, 4 },
  { "<=",   BRL_CMD_FWINLTSKIP, 1, 1, 0 },
  { "<=",   BRL_CMD_FWINLTSKIP, 1, 8, 2 },
  { "=>",   BRL_CMD_FWINRTSKIP, 1, 2, 0 },
  { "=>",   BRL_CMD_FWINRTSKIP, 1, 8, 4 },
  { "-^-",  BRL_CMD_LNUP,       1, 7, 2 },
  { "-v-",  BRL_CMD_LNDN,       1, 7, 4 },
  { "->",   BRL_CMD_FWINRT,     1, 8, 3 },
  { "<-",   BRL_CMD_FWINLT,     1, 6, 3 },
  { "HOME", BRL_CMD_HOME,       1, 7, 3 },
  { "^",    BRL_BLK_PASSKEY + BRL_KEY_CURSOR_UP,    1, 1, 2 },
  { "v",    BRL_BLK_PASSKEY + BRL_KEY_CURSOR_DOWN,  1, 1, 4 },
  { ">",    BRL_BLK_PASSKEY + BRL_KEY_CURSOR_RIGHT, 1, 2, 3 },
  { "<",    BRL_BLK_PASSKEY + BRL_KEY_CURSOR_LEFT,  1, 0, 3 },
  //{ "B3",   BRL_CMD_CSRVIS,     1, 2, 2 },
  { "DEL",  BRL_BLK_PASSKEY + BRL_KEY_DELETE,       1, 0, 4 },
  { "INS",  BRL_BLK_PASSKEY + BRL_KEY_INSERT,       1, 2, 4 },
  //{ "C5",   BRL_CMD_PASTE,      1, 5, 3 },
  //{ "D5",   EOF,                1, 5, 4 },
  //{ "B4",   EOF,                1, 3, 2 },

  //{ "B1",   EOF,                1, 0, 2 },
  //{ "C2",   EOF,                1, 1, 3 },
  //{ "C4",   EOF,                1, 3, 3 },
  //{ "D4",   EOF,                1, 3, 4 },
  { NULL,   0,                  0, 0, 0},
};

static struct model models[] = {
  { "normal",	buttons_simple,	4, 4, NULL },
  { "vs",	buttons_vs,	9, 5, NULL },
};

#ifdef USE_XT
static void setModel(Widget w, XtPointer closure, XtPointer data)
{
  char *newModel = (char *) closure;
  model = newModel;
  for (keyModel = models; keyModel < &models[XtNumber(models)] && strcmp(keyModel->name,model); keyModel++);
  if (keyModel == &models[XtNumber(models)]) keyModel = NULL;
  regenerate = 1;
}
#endif /* USE_XT */

static void createKeyButtons(struct button *buttons) {
  struct button *b;
  for (b=buttons; b->label; b++)
    crKeyBut(b->label, b->keycode, b->repeat, b->x*(BUTWIDTH+1), b->y*(BUTHEIGHT+1));
}

#ifdef USE_XT
struct radioInt {
  char *name;
  int value;
  Widget menuEntry;
};

static struct radioInt colsRadio [] = {
  { "80", 80, NULL },
  { "40", 40, NULL },
  { "20", 20, NULL },
  { "8",  8,  NULL },
};

static struct radioInt linesRadio [] = {
  { "3", 3, NULL },
  { "2", 2, NULL },
  { "1", 1, NULL },
};

static void setWidth(Widget w, XtPointer closure, XtPointer data)
{
  int newCols = (int) closure;
  cols = newCols;
  regenerate = 1;
}

static void setHeight(Widget w, XtPointer closure, XtPointer data)
{
  int newLines = (int) closure;
  lines = newLines;
  regenerate = 1;
}
#endif /* USE_XT */

static int brl_readCommand(BrailleDisplay *brl, BRL_DriverCommandContext context)
{
  int res=EOF;
#if defined(USE_XT)
  while (XtAppPending(app_con)) {
    XtAppProcessEvent(app_con,XtIMAll);
#elif defined(USE_WINDOWS)
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
	return BRL_CMD_RESTARTBRL;
      else {
	if (msg.message == WM_COMMAND)
	  fprintf(stderr,"message %p %d\n",msg.hwnd,msg.message);
	TranslateMessage(&msg);
	DispatchMessage(&msg);
      }
#else /* USE_ */
#error Toolkit loop unspecified
#endif /* USE_ */
    if (regenerate) {
      destroyToplevel();
      generateToplevel();
      regenerate = 0;
      brl->x = cols;
      brl->y = lines;
      brl->resizeRequired = 1;
    }
    if (keypressed) {
      res=keypressed;
      keypressed=BRL_CMD_NOOP;
      break;
    }
  }
  return res;
}

#ifdef USE_XT
static XrmOptionDescRec optionDescList[] = { };

static char *fallback_resources[] = {
  "*display.background: lightgreen",
#ifdef USE_XAW
  "*displayb.background: black",
  "*displayb.foreground: white",
#endif /* USE_XAW */
  "*keybox.background: lightgrey",
  "*menu.Label: Brltty",
  "*menu.background: lightgrey",
  NULL
};
#endif /* USE_XT */

static void brl_identify()
{
  LogPrint(LOG_NOTICE, XW_VERSION);
  LogPrint(LOG_INFO,   "   "XW_COPYRIGHT);
}

#ifdef USE_XM
static void popup(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  Widget shell = XtParent(menu);
  XmMenuPosition(menu, &event->xbutton);
  XtManageChild(menu);
  XtPopup(shell, XtGrabNone);
}
#endif /* USE_XM */

static void generateToplevel(void)
{
#ifdef USE_XT
  int argc;
  char **argv;
#ifdef USE_XAW
  char *def_string_return;
  char **missing_charset_list_return;
  int missing_charset_count_return;
#endif /* USE_XAW */
  XtActionsRec routeAction [] = { { "route", route } };
#ifdef USE_XM
  XtActionsRec popupAction [] = { { "popup", popup } };
#endif /* USE_XM */
  Widget tmp_vbox;
  char *disp;
#ifdef USE_XAW
  char *dispb;
#endif /* USE_XAW */
  XtCallbackRec cb[2] = { { NULL, NULL }, { NULL, NULL } };
  struct radioInt *radioInt;
  struct model *radioModel;
#endif /* USE_XT */
  int y,x;

#if defined(USE_XT)
  argc = xtArgc;
  argv = mallocWrapper((xtArgc + 1) * sizeof(*xtArgv));
  memcpy(argv, xtArgv, (xtArgc + 1) * sizeof(*xtArgv));

  /* toplevel */
  toplevel = XtVaOpenApplication(&app_con, "Brltty",
    optionDescList, XtNumber(optionDescList),
    &argc, argv, fallback_resources,
    sessionShellWidgetClass,
    XtNallowShellResize, True,
    NULL);
#elif defined(USE_WINDOWS)
  {
    WNDCLASS wndclass = {
      .style = 0,
      .lpfnWndProc = wndProc,
      .cbClsExtra = 0,
      .cbWndExtra = 0,
      .hInstance = NULL,
      .hIcon = LoadIcon(NULL, IDI_APPLICATION), /* TODO: nice icon */
      .hCursor = LoadCursor(NULL, IDC_ARROW),
      .hbrBackground = NULL,
      .lpszMenuName = NULL,
      .lpszClassName = "BRLTTYWClass",
    };
    if (!(RegisterClass(&wndclass))) {
      LogWindowsError("RegisterClass");
      exit(1);
    }
    modelWidth = cols*CHRX;
    if (keyModel) {
      if (keyModel->width*(BUTWIDTH+1)+1 > modelWidth)
	modelWidth = keyModel->width *(BUTWIDTH +1)-1;
      modelHeight = keyModel->height*(BUTHEIGHT+1);
    } else {
      modelHeight = 0;
    }
    if (!(toplevel = CreateWindow("BRLTTYWClass", "BRLTTY", WS_POPUP,
	    GetSystemMetrics(SM_CXSCREEN)-modelWidth-60, 0,
	    modelWidth, lines*CHRY+modelHeight, NULL, NULL, NULL, NULL))) {
      LogWindowsError("CreateWindow");
      exit(1);
    }
  }
#else /* USE_ */
#error Toolkit toplevel creation unspecified
#endif /* USE_ */

#ifdef USE_XT
  XtAppAddActions(app_con,routeAction,XtNumber(routeAction));
#ifdef USE_XM
  XtAppAddActions(app_con,popupAction,XtNumber(popupAction));
#endif /* USE_XM */
#endif /* USE_XT */

  /* vertical separation */
#ifdef USE_XT
  vbox = XtVaCreateManagedWidget("vbox",panedWidgetClass,toplevel,
#ifdef USE_XM
    XmNmarginHeight, 0,
    XmNmarginWidth, 0,
    XmNspacing, 1,
#endif /* USE_XM */
    XtNresize, True,
    NULL);
#endif /* USE_XT */

#ifdef USE_XAW
  if (!(fontset = XCreateFontSet(XtDisplay(toplevel),"-*-clearlyu-*-*-*-*-17-*-*-*-*-*-iso10646-1", &missing_charset_list_return, &missing_charset_count_return, &def_string_return)))
    LogPrint(LOG_ERR,"Error while loading braille font");
#endif /* USE_XAW */
  
#ifdef USE_XT
  /* horizontal separation */
  hbox = XtVaCreateManagedWidget("hbox",panedWidgetClass,vbox,
    XtNorientation, XtEhorizontal,
#ifdef USE_XM
    XmNmarginHeight, 0,
    XmNmarginWidth, 0,
    XmNspacing, 0,
#endif /* USE_XM */
#ifdef USE_XAW
    XtNshowGrip,False,
#else /* USE_XAW */
    XmNpaneMaximum,20*lines,
    XmNpaneMinimum,20*lines,
    XmNskipAdjust, True,
#endif /* USE_XAW */
    XtNresize, True,
    NULL);

  /* display Label */
  disp=XtMalloc(2);
  disp[0]=' ';
  disp[1]=0;

#ifdef USE_XAW
  dispb=XtMalloc(4);
  dispb[0]=0xe0|((0x28>>4)&0x0f);
  dispb[1]=0x80|((0x28<<2)&0x3f);
  dispb[2]=0x80;
  dispb[3]=0;
#endif /* USE_XAW */

#ifdef USE_XM
  display_cs = XmStringCreateLocalized(disp);
#endif /* USE_XM */
#endif /* USE_XT */

#ifdef USE_WINDOWS
  hbox = CreateWindow(WC_STATIC, "", WS_CHILD | WS_VISIBLE, 0, 0,
       modelWidth, lines*CHRY+modelHeight, toplevel, NULL, NULL, NULL);
#endif /* USE_WINDOWS */

  for (x=0;x<cols;x++) {
#ifdef USE_XT
    /* vertical separation */
    tmp_vbox = XtVaCreateManagedWidget("tmp_vbox",panedWidgetClass,hbox,
#ifdef USE_XAW
      XtNshowGrip,False,
#else /* USE_XAW */
      XmNpaneMaximum,20,
      XmNpaneMinimum,20,
      XmNskipAdjust, True,
#endif /* USE_XAW */
#ifdef USE_XM
      XmNmarginHeight, 0,
      XmNmarginWidth, 0,
      XmNspacing, 0,
#endif /* USE_XM */
      XtNresize, True,
      NULL);
#endif /* USE_XT */

    for (y=0;y<lines;y++) {
#if defined(USE_XT)
      char action[] = "<Btn1Up>: route(100)";
      XtTranslations transl;

      snprintf(action,sizeof(action),"<Btn1Up>: route(%u)",y*cols+x);
      transl = XtParseTranslationTable(action);

      display[y*cols+x] = XtVaCreateManagedWidget("display",labelWidgetClass,tmp_vbox,
	XtNtranslations, transl,
#ifdef USE_XAW
	XtNshowGrip,False,
#else /* USE_XAW */
	XmNpaneMaximum,20,
	XmNpaneMinimum,20,
	XmNskipAdjust, True,
#endif /* USE_XAW */
#ifdef USE_XAW
	XtNlabel, disp,
#else /* USE_XAW */
	XmNlabelString, display_cs,
#endif /* USE_XAW */
	NULL);

#ifdef USE_XAW
      if (fontset) {
	displayb[y*cols+x] = XtVaCreateManagedWidget("displayb",labelWidgetClass,tmp_vbox,
	  XtNtranslations, transl,
	  XtNinternational, True,
	  XNFontSet, fontset,
	  XtNshowGrip,False,
	  XtNlabel, dispb,
	  NULL);
      }
#endif /* USE_XAW */
#elif defined(USE_WINDOWS)
      display[y*cols+x] = CreateWindow(WC_BUTTON, " ", WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_PUSHLIKE, x*CHRX, y*CHRY, CHRX, CHRY, toplevel, NULL, NULL, NULL);
      SetWindowLong(display[y*cols+x], GWL_USERDATA, (long) (BRL_BLK_ROUTE | ((y*cols+x)&BRL_MSK_ARG)));
#else /* USE_ */
#error Toolkit display unspecified
#endif /* USE_ */
    }
  }
#ifdef USE_XT
#ifdef USE_XM
  XmStringFree(display_cs);
#endif /* USE_XM */
  XtFree(disp);
#endif /* USE_XT */
#ifdef USE_XT
  XtVaGetValues(display[0],
    XtNforeground, &displayForeground,
    XtNbackground, &displayBackground,
    NULL);
#endif /* USE_XT */

  if (keyModel) {
    /* key box */
#ifdef USE_XT
    keybox = XtVaCreateManagedWidget("keybox",formWidgetClass,vbox,
#ifdef USE_XAW
      XtNdefaultDistance,0,
#endif /* USE_XAW */
      NULL);
#endif /* USE_XT */
    createKeyButtons(keyModel->buttons);
  }

#ifdef USE_XT
#if defined(USE_XAW)
  menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, toplevel, NULL, 0);
#elif defined(USE_XM)
  menu = XmCreatePopupMenu(toplevel, "menu", NULL, 0);
#else
#error Toolkit popup menu creation unspecified
#endif /* USE_ */

#ifdef USE_XAW
  if (!check) {
    static unsigned char checkimg [] = {
      0x00, 0x00, 0xc0, 0x60, 0x33, 0x1e, 0x0c, 0x00
    };
    check = XCreateBitmapFromData(XtDisplay(toplevel),
	RootWindowOfScreen(XtScreen(toplevel)), (char *) checkimg, 8, 8);
  }
#endif /* USE_XAW */
#ifdef USE_XAW
  XtVaCreateManagedWidget("WidthLine", smeLineObjectClass, menu, NULL);
#endif /* USE_XAW */
  XtVaCreateManagedWidget("Width", menuEntryWidgetClass, menu, NULL);
  cb[0].callback = (XtCallbackProc) setWidth;
  for (radioInt = colsRadio; radioInt < &colsRadio[XtNumber(colsRadio)]; radioInt++) {
    cb[0].closure = (caddr_t) radioInt->value;
    radioInt->menuEntry = XtVaCreateManagedWidget(radioInt->name, menuEntryWidgetClass, menu,
	NvalueChangedCallback, cb,
	NtoggleState, radioInt->value == cols ? toggleSet : toggleUnset,
#ifdef USE_XAW
	XtNleftMargin, 9,
#endif /* USE_XAW */
	NULL);
  }
#ifdef USE_XAW
  XtVaCreateManagedWidget("HeightLine", smeLineObjectClass, menu, NULL);
#endif /* USE_XAW */
  XtVaCreateManagedWidget("Height", menuEntryWidgetClass, menu, NULL);
  cb[0].callback = (XtCallbackProc) setHeight;
  for (radioInt = linesRadio; radioInt < &linesRadio[XtNumber(linesRadio)]; radioInt++) {
    cb[0].closure = (caddr_t) radioInt->value;
    radioInt->menuEntry = XtVaCreateManagedWidget(radioInt->name, menuEntryWidgetClass, menu,
	NvalueChangedCallback, cb,
	NtoggleState, radioInt->value == lines ? toggleSet : toggleUnset,
#ifdef USE_XAW
	XtNleftMargin, 9,
#endif /* USE_XAW */
	NULL);
  }
#ifdef USE_XAW
  XtVaCreateManagedWidget("ModelLine", smeLineObjectClass, menu, NULL);
#endif /* USE_XAW */
  XtVaCreateManagedWidget("Model", menuEntryWidgetClass, menu, NULL);
  cb[0].callback = (XtCallbackProc) setModel;
  for (radioModel = models; radioModel < &models[XtNumber(models)]; radioModel++) {
    cb[0].closure = (caddr_t) radioModel->name;
    radioModel->menuEntry = XtVaCreateManagedWidget(radioModel->name, menuEntryWidgetClass, menu,
	NvalueChangedCallback, cb,
	NtoggleState, radioModel == keyModel ? toggleSet : toggleUnset,
#ifdef USE_XAW
	XtNleftMargin, 9,
#endif /* USE_XAW */
	NULL);
  }
  cb[0].closure = (caddr_t) "bare";
  bareMenuEntry = XtVaCreateManagedWidget("bare", menuEntryWidgetClass, menu,
      NvalueChangedCallback, cb,
      NtoggleState, keyModel ? toggleUnset : toggleSet,
#ifdef USE_XAW
      XtNleftMargin, 9,
#endif /* USE_XAW */
      NULL);
  XtVaSetValues(toplevel, XtNtranslations, XtParseTranslationTable(
	"None<Btn3Down>: "
#if defined(USE_XAW)
	"XawPositionSimpleMenu(menu) MenuPopup(menu)"
#elif defined(USE_XM)
	"popup()"
#else /* USE_ */
#error Toolkit menu popup unspecified
#endif /* USE_ */
	), NULL);
#endif /* USE_XT */

  /* go go go */
#if defined(USE_XT)
  XtRealizeWidget(toplevel);
#elif defined(USE_WINDOWS)
  ShowWindow(toplevel, SW_SHOWDEFAULT);
  UpdateWindow(toplevel);
#else /* USE_ */
#error Toolkit toplevel realization unspecified
#endif /* USE_ */
#ifdef USE_XAW
  memset(displayedWindow,0,sizeof(displayedWindow));
#endif /* USE_XAW */
  memset(displayedVisual,0,sizeof(displayedVisual));
  lastcursor = -1;
}

static int brl_open(BrailleDisplay *brl, char **parameters, const char *device)
{
  lines=1;
  if (*parameters[PARM_LINES]) {
    static const int minimum = 1;
    static const int maximum = MAXLINES;
    int value;
    if (validateInteger(&value, "lines", parameters[PARM_LINES], &minimum, &maximum))
      lines=value;
  }

  cols=40;
  if (*parameters[PARM_COLS]) {
    static const int minimum = 1;
    static const int maximum = MAXCOLS;
    int value;
    if (validateInteger(&value, "cols", parameters[PARM_COLS], &minimum, &maximum))
      cols=value;
  }

  if (*parameters[PARM_TKPARMS]) {
    if (xtArgv != xtDefArgv)
      deallocateStrings(xtArgv);
    xtArgv = splitString(parameters[PARM_TKPARMS],' ',&xtArgc);
    xtArgv = reallocWrapper(xtArgv, (xtArgc+2) * sizeof(char *));
    memmove(xtArgv+1,xtArgv,(xtArgc+1) * sizeof(char *));
    xtArgv[0] = strdupWrapper(xtDefArgv[0]);
    xtArgc++;
  }

  if (*parameters[PARM_MODEL]) {
    model = parameters[PARM_MODEL];
    for (keyModel = models; keyModel < &models[XtNumber(models)] && strcmp(keyModel->name,model); keyModel++);
    if (keyModel == &models[XtNumber(models)]) keyModel = NULL;
  }
  
#if defined(USE_XT)
  XtToolkitThreadInitialize();
  XtSetLanguageProc(NULL, NULL, NULL);
#endif /* USE_XT */ 

  brl->x=cols;
  brl->y=lines;

  generateToplevel();

  return 1;
}
static void destroyToplevel(void)
{
#if defined(USE_XT)
#ifdef USE_XAW
  if (fontset) {
    XFreeFontSet(XtDisplay(toplevel),fontset);
    fontset = NULL;
  }
  check = None;
#endif /* USE_XAW */
  XtDestroyApplicationContext(app_con);
  app_con = NULL;
#elif defined(USE_WINDOWS)
  if (!DestroyWindow(toplevel))
    LogWindowsError("DestroyWindow");
#else /* USE_ */
#error Toolkit toplevel destruction unspecified
#endif /* USE_ */
}

static void brl_close(BrailleDisplay *brl)
{
  destroyToplevel();
}

static void brl_writeWindow(BrailleDisplay *brl)
{
#ifdef USE_XAW
  int i;
  unsigned char data[4];
  char c;

  if (!displayb[0] || !memcmp(brl->buffer,displayedWindow,brl->y*brl->x)) return;
  memcpy(displayedWindow,brl->buffer,brl->y*brl->x);

  for (i=0;i<brl->y*brl->x;i++) {
    c = brl->buffer[i];
    brl->buffer[i] =
       (!!(c&BRL_DOT1))<<0
      |(!!(c&BRL_DOT2))<<1
      |(!!(c&BRL_DOT3))<<2
      |(!!(c&BRL_DOT4))<<3
      |(!!(c&BRL_DOT5))<<4
      |(!!(c&BRL_DOT6))<<5
      |(!!(c&BRL_DOT7))<<6
      |(!!(c&BRL_DOT8))<<7;
    data[0]=0xe0|((0x28>>4)&0x0f);
    data[1]=0x80|((0x28<<2)&0x3f)|(brl->buffer[i]>>6);
    data[2]=0x80                 |(brl->buffer[i]&0x3f);
    data[3]=0;

    XtVaSetValues(displayb[i],
      XtNlabel, data,
      NULL);
  }
#endif /* USE_XAW */
}

static void brl_writeVisual(BrailleDisplay *brl)
{
  int i;
  unsigned char data[2];

  if (lastcursor != brl->cursor) {
    if (lastcursor>=0) {
#if defined(USE_XT)
      XtVaSetValues(display[lastcursor],
	XtNforeground, displayForeground,
	XtNbackground, displayBackground,
	NULL);
#elif defined(USE_WINDOWS)
      SendMessage(display[lastcursor],BM_SETSTATE,FALSE,0);
#else /* USE_ */
#error Toolkit cursor not specified
#endif /* USE_ */
    }
    lastcursor = brl->cursor;
    if (lastcursor>=0) {
#if defined(USE_XT)
      XtVaSetValues(display[lastcursor],
	XtNforeground, displayBackground,
	XtNbackground, displayForeground,
	NULL);
#elif defined(USE_WINDOWS)
      SendMessage(display[lastcursor],BM_SETSTATE,TRUE,0);
#else /* USE_ */
#error Toolkit cursor not specified
#endif /* USE_ */
    }
  }

  if (!memcmp(brl->buffer,displayedVisual,brl->y*brl->x)) return;
  memcpy(displayedVisual,brl->buffer,brl->y*brl->x);

  for (i=0;i<brl->y*brl->x;i++) {
    data[0]=brl->buffer[i];
    if (data[0]==0) data[0]=' ';
    data[1]=0;

#if defined(USE_XT)
#ifdef USE_XM
    display_cs = XmStringCreateLocalized(data);
#endif /* USE_XM */
    XtVaSetValues(display[i],
#ifdef USE_XAW
      XtNlabel, data,
#else /* USE_XAW */
      XmNlabelString, display_cs,
#endif /* USE_XAW */
      NULL);
#ifdef USE_XM
    XmStringFree(display_cs);
#endif /* USE_XM */
#elif defined(USE_WINDOWS)
    SetWindowText(display[i],data);
#else /* USE_ */
#error Toolkit display refresh unspecified
#endif /* USE_ */
  }
}

static void brl_writeStatus(BrailleDisplay *brl, const unsigned char *s)
{
  /* TODO: another label */
}
