
#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"
#include "browser/api/WindowInterface.h"
#include "browser/api/WindowList.h"
#include "browser/api/MenuEventNotif.h"
#include "common/NodeRegisterHelp.h"
#include "common/HideWndHelp.h"
#include "common/api/EventEmitter.h"
#include "gin/object_template_builder.h"
#include "gin/dictionary.h"
#include "base/strings/string_util.h"
#include <set>

namespace atom {

class Menu;

class MenuItem {
public:
    enum MenuItemType {
        ActionType,
        CheckableActionType,
        SeparatorType,
        SubmenuType
    };

    MenuItem(v8::Isolate* isolate, Menu* menu);

    ~MenuItem();

    v8::Isolate* getIsolate() const {
        return m_isolate;
    }

    Menu* getMenu() const {
        return m_menu;
    }

    void setType(MenuItemType type) {
        m_type = type;
    }

    MenuItemType getType() const {
        return m_type;
    }

    Menu* getSubMenu() const {
        return m_subMenu;
    }

    void setSubMenu(Menu* subMenu);

    void setLabel(const std::string label) {
        m_label = label;
    }

    void setEnabled(bool b) {
        m_isEnabled = b;
    }

    void setChecked(bool b) {
        m_isChecked = b;
    }

    bool getChecked() const {
        return m_isChecked;
    }

    void setClickCallback(v8::Local<v8::Value> callback) {
        m_clickCallbackValue.Reset(m_isolate, callback);
    }

    v8::Local<v8::Value> getClickCallbackValue() const {
        return m_clickCallbackValue.Get(m_isolate);
    }

    void setId(int id) {
        m_id = id;
    }

    int getId() const {
        return m_id;
    }

    void insertPlatformMenu(size_t pos, HMENU hMenu) const;

    void clear();

private:
    MenuItemType m_type;
    HMENU m_hSubMenu;
    Menu* m_subMenu;
    std::string m_label;
    bool m_isEnabled;
    bool m_isChecked;
    UINT m_action;
    v8::Persistent<v8::Value> m_clickCallbackValue;
    Menu* m_menu;
    int m_id;
    v8::Isolate* m_isolate;
};

//////////////////////////////////////////////////////////////////////////

class Menu : public mate::EventEmitter<Menu> {
public:
    explicit Menu(v8::Isolate* isolate, v8::Local<v8::Object> wrapper)
        : m_hideWndHelp(nullptr) {
        gin::Wrappable<Menu>::InitWith(isolate, wrapper);
        //m_menuTemplate = nullptr;
        m_hMenu = NULL;
        m_isItemNeedRebuilt = false;
        m_isAppOrPopupMenu = kNoInit;
    }

    virtual ~Menu() override {
        OutputDebugStringA("~Menu\n");
        ::DestroyMenu(m_hMenu);
        m_hMenu = nullptr;

        if (m_appMenu == this)
            m_appMenu = nullptr;
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        m_liveMenuItem = new std::set<MenuItem*>();
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Menu"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("_setApplicationMenu", &Menu::setApplicationMenuApi);
        builder.SetMethod("_sendActionToFirstResponder", &Menu::sendActionToFirstResponderApi);
        builder.SetMethod("_insert", &Menu::_insertApi);
        builder.SetMethod("_append", &Menu::_appendeApi);
        builder.SetMethod("_popup", &Menu::_popupApi);
        builder.SetMethod("_clear", &Menu::_clearApi);
        builder.SetMethod("getItemCount", &Menu::getItemCountApi);
        builder.SetMethod("quit", &Menu::nullFunction);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Menu"), prototype->GetFunction());
    }

    void nullFunction() {
        DebugBreak();
    }

    // Set the global menubar.
    void setApplicationMenuApi() {
        HMENU hmenuBar = buildMenus(true);
        
        WindowList::iterator winIt = WindowList::getInstance()->begin();
        for (; winIt != WindowList::getInstance()->end(); ++winIt) {
            WindowInterface* windowInterface = *winIt;
            HWND hParentWnd = windowInterface->getHWND();
            ::SetMenu(hParentWnd, hmenuBar);
        }
        m_appMenu = this;
    }

    void sendActionToFirstResponderApi(const std::string& action) {
        DebugBreak();
    }

    size_t getItemCountApi() const {
        return m_items.size();
    }

    void _appendeApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    }

    void _insertApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (2 != args.Length())
            return;

        v8::Isolate* isolate = args.GetIsolate();
        if (!args[0]->IsUint32())
            return;
        size_t pos = args[0]->ToUint32()->Value();

        v8::Object* v8Obj = v8::Object::Cast(*args[1]);
        v8::Local<v8::Array> v8ObjProps = v8Obj->GetOwnPropertyNames();
        size_t size = v8ObjProps->Length();

        std::string label;
        std::string role;

        MenuItem* item = new MenuItem(isolate, this);

        for (size_t i = 0; i < size; ++i) {
            v8::Local<v8::Value> keyNameValue = v8ObjProps->Get(i);
            v8::Local<v8::Value> outValue = v8Obj->Get(keyNameValue);

            std::string keyNameStr;
            if (!gin::Converter<std::string>::FromV8(isolate, keyNameValue, &keyNameStr))
                return;

            std::string type;
            if ("type" == keyNameStr && outValue->IsString()) {
                v8::String::Utf8Value utf8(outValue->ToString(isolate));
                type = *utf8;
            }
            if ("separator" == type)
                item->setType(MenuItem::SeparatorType);
            if ("checkbox" == type || "radio" == type)
                item->setType(MenuItem::CheckableActionType);

            if ("label" == keyNameStr && outValue->IsString()) {
                v8::String::Utf8Value utf8(outValue->ToString(isolate));
                label = *utf8;
            }
           
            if ("role" == keyNameStr && outValue->IsString()) {
                v8::String::Utf8Value utf8(outValue->ToString(isolate));
                role = *utf8;
            }

            if ("enabled" == keyNameStr && outValue->IsBoolean()) {
                item->setEnabled(outValue->ToBoolean()->Value());
            }

            if ("checked" == keyNameStr && outValue->IsBoolean()) {
                item->setChecked(outValue->ToBoolean()->Value());
            }
            
            if ("submenu" == keyNameStr) {
                Menu* subMenu = nullptr;
                if (!gin::Converter<Menu*>::FromV8(isolate, outValue, &subMenu))
                    subMenu = nullptr;
                if (subMenu)
                    item->setSubMenu(subMenu);
            }

            if ("click" == keyNameStr && outValue->IsFunction()) {
                item->setClickCallback(outValue);
            }
        }

        if (label.empty())
            label = role;
        item->setLabel(label);

        m_items.insert(m_items.begin() + pos, item);
        m_isItemNeedRebuilt = true;
    }
    
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {  
        v8::Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            new Menu(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

    void _popupApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        Menu* self = this;
        if (!m_hideWndHelp) {
            m_hideWndHelp = new HideWndHelp(L"HideParentWindowClass", [self] (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ->LRESULT {
                return hideWndProc(hWnd, uMsg, wParam, lParam);
            });
        }

        buildMenus(false);

        ::SetForegroundWindow(m_hideWndHelp->getWnd());

        POINT pt;
        if (args.Length() == 2 && args[0]->IsInt32() && args[1]->IsInt32()) {
            pt.x = args[0]->ToInt32()->Value();
            pt.y = args[0]->ToInt32()->Value();
        } else {
            ::GetCursorPos(&pt);
        }
        ::TrackPopupMenu(m_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hideWndHelp->getWnd(), NULL);
    }

    void _clearApi() {
        clear();
    }
    void clear() {
        if (m_hMenu)
            ::DestroyMenu(m_hMenu);
        m_hMenu = nullptr;
        for (size_t i = 0; i < m_items.size(); ++i) {
            MenuItem* it = m_items[i];
            it->clear();
            delete it;
        }
        m_items.clear();
    }

    void onCommon(MenuItem* item, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        MENUITEMINFOW info = { 0 };
        info.cbSize = sizeof(MENUITEMINFOW);
        info.fMask = MIIM_STATE;     // information to get 
        int index = findItemIndex(item);
        BOOL b = ::GetMenuItemInfo(m_hMenu, (UINT)index, TRUE, &info);

        if (MenuItem::CheckableActionType == item->getType()) {
            if (item->getChecked()) {
                info.fState = MFS_UNCHECKED;
                b = ::SetMenuItemInfo(m_hMenu, (UINT)index, TRUE, &info);
                item->setChecked(false);
            } else {
                info.fState |= MFS_CHECKED;
                b = ::SetMenuItemInfo(m_hMenu, (UINT)index, TRUE, &info);
                item->setChecked(true);
            }
        }

        v8::Local<v8::Value> focusedWindow = WindowInterface::getFocusedWindow(isolate());
        item->getMenu()->mate::EventEmitter<Menu>::emit("click", 
            item->getClickCallbackValue(), focusedWindow /*, focusedWebContents*/);
    }

    static Menu* getAppMenu() {
        return m_appMenu;
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
    static std::set<MenuItem*>* m_liveMenuItem;

    HideWndHelp* m_hideWndHelp;

    friend class MenuItem;

    HMENU buildMenus(bool isAppOrPopupMenu) {
        if (!m_isItemNeedRebuilt)
            return m_hMenu;
        m_isItemNeedRebuilt = false;

        AppOrPopupType appOrPopupMenuType = isAppOrPopupMenu ? kIsApp : kIsPopup;
        if (kNoInit == m_isAppOrPopupMenu)
            m_isAppOrPopupMenu = appOrPopupMenuType;
        else if (m_isAppOrPopupMenu != appOrPopupMenuType)
            return nullptr;

        if (m_hMenu)
            ::DestroyMenu(m_hMenu);
        m_hMenu = nullptr;

        return buildMenu(this, isAppOrPopupMenu);
    }

private:
    static HMENU buildMenu(Menu* menu, bool isAppOrPopupMenu) {
        size_t size = menu->m_items.size();
        if (0 == size)
            return nullptr;

        menu->m_hMenu = ((isAppOrPopupMenu) ? ::CreateMenu() : ::CreatePopupMenu());

        for (size_t i = 0; i < size; ++i) {
            MenuItem* item = menu->m_items[i];
            item->insertPlatformMenu(i, menu->m_hMenu);
        }
        return menu->m_hMenu;
    }

    int findItemIndex(MenuItem* item) {
        for (size_t i = 0; i < m_items.size(); ++i) {
            MenuItem* it = m_items[i];
            if (it == item)
                return i;
        }
        return -1;
    }   

    static LRESULT APIENTRY hideWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        MenuItem* item = nullptr;

        switch (uMsg) {
        case WM_COMMAND: {
            MenuEventNotif::onMenuCommon(uMsg, wParam, lParam);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }

    //base::ListValue* m_menuTemplate;
    HMENU m_hMenu;
    std::vector<MenuItem*> m_items;
    bool m_isItemNeedRebuilt;

    static Menu* m_appMenu;

    enum AppOrPopupType {
        kNoInit,
        kIsApp,
        kIsPopup,
    };
    AppOrPopupType m_isAppOrPopupMenu;
};

//////////////////////////////////////////////////////////////////////////

MenuItem::MenuItem(v8::Isolate* isolate, Menu* menu) {
    m_isolate = isolate;
    m_type = ActionType;
    m_isEnabled = true;
    m_isChecked = false;
    m_action = (UINT)this;
    m_menu = menu;
    m_subMenu = nullptr;
    m_id = 0;
    Menu::m_liveMenuItem->insert(this);
}

MenuItem::~MenuItem() {
    Menu::m_liveMenuItem->erase(this);
}

void MenuItem::setSubMenu(Menu* subMenu)
{
    HMENU hSubMenu = subMenu->m_hMenu;
    m_hSubMenu = hSubMenu;
    m_subMenu = subMenu;
    m_type = SubmenuType;
}

void MenuItem::insertPlatformMenu(size_t pos, HMENU hMenu) const {
    int count = ::GetMenuItemCount(hMenu);
    if (count < 0 && (int)pos > count)
        return;

    MENUITEMINFO info = { 0 };
    info.cbSize = sizeof(MENUITEMINFO);

    if (m_type == SeparatorType) {
        info.fMask = MIIM_FTYPE;
        info.fType = MFT_SEPARATOR;
        ::InsertMenuItem(hMenu, count, TRUE, &info);
        return;
    }

    info.fMask = MIIM_FTYPE | MIIM_ID | MIIM_STATE;
    info.fType = MFT_STRING;
    info.wID = m_action;

    if (m_type == SubmenuType) {
        info.fMask |= MIIM_SUBMENU;
        info.hSubMenu = Menu::buildMenu(m_subMenu, false); // m_hSubMenu;
    }

    std::wstring labelW = base::UTF8ToWide(m_label);
    if (!labelW.empty()) {
        info.fMask |= MIIM_STRING;
        info.cch = labelW.size();
        info.dwTypeData = const_cast<LPWSTR>(labelW.c_str());
    }

    info.fState |= m_isEnabled ? MFS_ENABLED : MFS_DISABLED;
    if (CheckableActionType == m_type)
        info.fState |= m_isChecked ? MFS_CHECKED : MFS_UNCHECKED;
    ::InsertMenuItem(hMenu, count, TRUE, &info);
}

void MenuItem::clear() {
    if (m_hSubMenu)
        ::DestroyMenu(m_hSubMenu);
    m_hSubMenu = nullptr;
    if (m_subMenu)
        m_subMenu->clear();
    m_subMenu = nullptr;
}

void MenuEventNotif::onWindowDidCreated(WindowInterface* window) {
    HWND hParentWnd = window->getHWND();
    if (Menu::getAppMenu()) {
        HMENU hmenuBar = Menu::getAppMenu()->buildMenus(true);
        ::SetMenu(hParentWnd, hmenuBar);
    }
}

void MenuEventNotif::onMenuCommon(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    MenuItem* item = (MenuItem*)wParam;
    if (!Menu::m_liveMenuItem || Menu::m_liveMenuItem->find(item) == Menu::m_liveMenuItem->end())
        return;

    item->getMenu()->onCommon(item, uMsg, wParam, lParam);
}

v8::Persistent<v8::Function> Menu::constructor;
gin::WrapperInfo Menu::kWrapperInfo = { gin::kEmbedderNativeGin };
std::set<MenuItem*>* Menu::m_liveMenuItem = nullptr;

Menu* Menu::m_appMenu = nullptr;

static void initializeMenuApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Menu::init(env->isolate(), target);
}

static const char BrowserMenuNative[] = "exports = function {};";

static NodeNative nativeBrowserMenuNative{ "Menu", BrowserMenuNative, sizeof(BrowserMenuNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_menu, initializeMenuApi, &nativeBrowserMenuNative)

}