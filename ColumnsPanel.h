/* Do not edit this file. It was automatically generated. */

#ifndef HEADER_ColumnsPanel
#define HEADER_ColumnsPanel


#include "Panel.h"
#include "Settings.h"
#include "ScreenManager.h"

#include "debug.h"
#include <assert.h>


typedef struct ColumnsPanel_ {
   Panel super;

   Settings* settings;
   ScreenManager* scr;
} ColumnsPanel;


ColumnsPanel* ColumnsPanel_new(Settings* settings, ScreenManager* scr);

void ColumnsPanel_delete(Object* object);

void ColumnsPanel_update(Panel* super);

HandlerResult ColumnsPanel_eventHandler(Panel* super, int ch);

#endif