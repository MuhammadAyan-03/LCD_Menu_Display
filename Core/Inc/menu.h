#ifndef __MENU_H__
#define __MENU_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// Action Types
typedef enum {
    ACTION_NONE    = 0,
    ACTION_LABEL   = (1 << 0),
    ACTION_DISPLAY = (1 << 1),
    ACTION_TRIGGER = (1 << 2),
    ACTION_PARENT  = (1 << 3),
    ACTION_CUSTOM  = (1 << 7)
} MenuAction_t;

// ----------------------------------------------------------------------------
// Callback Type
typedef bool (*MenuCallback_t)(MenuAction_t);

// ----------------------------------------------------------------------------
// Item Info
typedef struct {
    uint8_t siblings;
    uint8_t position;
} MenuInfo_t;

// ----------------------------------------------------------------------------
// Menu Item Definition
typedef struct MenuItem {
    const struct MenuItem *Next;
    const struct MenuItem *Previous;
    const struct MenuItem *Parent;
    const struct MenuItem *Child;
    MenuCallback_t Callback;
    const char *Label;
} MenuItem_t;

// ----------------------------------------------------------------------------
// Render Callback
typedef void (*MenuRenderCallback_t)(const MenuItem_t *, uint8_t);

// ----------------------------------------------------------------------------
// Menu Engine
typedef struct {
    const MenuItem_t *currentItem;
    const MenuItem_t *previousItem;
    const MenuItem_t *lastInvokedItem;
} MenuEngine_t;

// ----------------------------------------------------------------------------
// Public API
extern const MenuItem_t NullItem;

void Menu_Init(MenuEngine_t *engine);
void Menu_InitWithItem(MenuEngine_t *engine, const MenuItem_t *initialItem);

void Menu_Navigate(MenuEngine_t *engine, const MenuItem_t *targetItem);
void Menu_Invoke(MenuEngine_t *engine);
bool Menu_ExecuteCallback(MenuEngine_t *engine, MenuAction_t action);
void Menu_Render(MenuEngine_t *engine, MenuRenderCallback_t render, uint8_t maxDisplayedMenuItems);

MenuInfo_t Menu_GetItemInfo(MenuEngine_t *engine, const MenuItem_t *item);
const char *Menu_GetLabel(MenuEngine_t *engine, const MenuItem_t *item);
const MenuItem_t *Menu_GetPrev(MenuEngine_t *engine, const MenuItem_t *item);
const MenuItem_t *Menu_GetNext(MenuEngine_t *engine, const MenuItem_t *item);
const MenuItem_t *Menu_GetParent(MenuEngine_t *engine, const MenuItem_t *item);
const MenuItem_t *Menu_GetChild(MenuEngine_t *engine, const MenuItem_t *item);

#ifdef __cplusplus
}
#endif

#endif // __MENU_H__
