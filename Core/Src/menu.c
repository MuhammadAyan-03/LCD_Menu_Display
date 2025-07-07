#include "menu.h"

// ----------------------------------------------------------------------------
// Static NullItem
const MenuItem_t NullItem = { NULL, NULL, NULL, NULL, NULL, NULL };

// ----------------------------------------------------------------------------
// Initialize Engine
void Menu_Init(MenuEngine_t *engine) {
    engine->currentItem = &NullItem;
    engine->previousItem = &NullItem;
    engine->lastInvokedItem = &NullItem;
}

void Menu_InitWithItem(MenuEngine_t *engine, const MenuItem_t *initialItem) {
    engine->currentItem = initialItem;
    engine->previousItem = &NullItem;
    engine->lastInvokedItem = &NullItem;
}

// ----------------------------------------------------------------------------
// Get Item Properties
const char *Menu_GetLabel(MenuEngine_t *engine, const MenuItem_t *item) {
    const MenuItem_t *target = item ? item : engine->currentItem;
    return target->Label;
}

const MenuItem_t *Menu_GetPrev(MenuEngine_t *engine, const MenuItem_t *item) {
    const MenuItem_t *target = item ? item : engine->currentItem;
    return target->Previous;
}

const MenuItem_t *Menu_GetNext(MenuEngine_t *engine, const MenuItem_t *item) {
    const MenuItem_t *target = item ? item : engine->currentItem;
    return target->Next;
}

const MenuItem_t *Menu_GetParent(MenuEngine_t *engine, const MenuItem_t *item) {
    const MenuItem_t *target = item ? item : engine->currentItem;
    return target->Parent;
}

const MenuItem_t *Menu_GetChild(MenuEngine_t *engine, const MenuItem_t *item) {
    const MenuItem_t *target = item ? item : engine->currentItem;
    return target->Child;
}

// ----------------------------------------------------------------------------
// Execute Callback
bool Menu_ExecuteCallback(MenuEngine_t *engine, MenuAction_t action) {
    if (engine->currentItem) {
        MenuCallback_t callback = engine->currentItem->Callback;
        if (callback != NULL) {
            return callback(action);
        }
    }
    return true;
}

// ----------------------------------------------------------------------------
// Navigate to Another Item
void Menu_Navigate(MenuEngine_t *engine, const MenuItem_t *targetItem) {
    bool commit = true;
    if (targetItem && targetItem != &NullItem) {
        if (targetItem == Menu_GetParent(engine, engine->currentItem)) {
            commit = Menu_ExecuteCallback(engine, ACTION_PARENT);
            engine->lastInvokedItem = &NullItem;
        }
        if (commit) {
            engine->previousItem = engine->currentItem;
            engine->currentItem = targetItem;
            Menu_ExecuteCallback(engine, ACTION_LABEL);
        }
    }
}

// ----------------------------------------------------------------------------
// Invoke the Current Item
void Menu_Invoke(MenuEngine_t *engine) {
    bool preventTrigger = false;
    if (engine->lastInvokedItem != engine->currentItem) {
        engine->lastInvokedItem = engine->currentItem;
        preventTrigger = true;
        Menu_ExecuteCallback(engine, ACTION_DISPLAY);
    }

    const MenuItem_t *child = Menu_GetChild(engine, NULL);
    if (child && child != &NullItem) {
        Menu_Navigate(engine, child);
    } else {
        if (!preventTrigger) {
            Menu_ExecuteCallback(engine, ACTION_TRIGGER);
        }
    }
}

// ----------------------------------------------------------------------------
// Get Item Info
MenuInfo_t Menu_GetItemInfo(MenuEngine_t *engine, const MenuItem_t *item) {
    MenuInfo_t result = {0, 0};
    const MenuItem_t *i = Menu_GetChild(engine, Menu_GetParent(engine, NULL));
    while (i && i != &NullItem) {
        result.siblings++;
        if (i == item) {
            result.position = result.siblings;
        }
        i = Menu_GetNext(engine, i);
    }
    return result;
}

// ----------------------------------------------------------------------------
// Render Items
void Menu_Render(MenuEngine_t *engine, MenuRenderCallback_t render, uint8_t maxDisplayedMenuItems) {
    if (!engine->currentItem || engine->currentItem == &NullItem) {
        return;
    }

    uint8_t start = 0;
    uint8_t itemCount = 0;
    const uint8_t center = maxDisplayedMenuItems >> 1;
    MenuInfo_t mi = Menu_GetItemInfo(engine, engine->currentItem);

    if (mi.position >= (mi.siblings - center)) {
        start = mi.siblings > maxDisplayedMenuItems ? mi.siblings - maxDisplayedMenuItems : 0;
    } else {
        start = mi.position > center ? mi.position - center : 0;
        if (maxDisplayedMenuItems & 0x01) start--; // center if odd
    }

    // prevent underflow
    if ((int8_t)start < 0) start = 0;

    const MenuItem_t *i = Menu_GetChild(engine, Menu_GetParent(engine, NULL));
    for (; i && i != &NullItem; i = Menu_GetNext(engine, i)) {
        if (itemCount >= start && (itemCount - start) < maxDisplayedMenuItems) {
            render(i, itemCount - start);
        }
        itemCount++;
    }
}
