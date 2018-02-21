
#include "node/include/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "gin/object_template_builder.h"
#include "gin/dictionary.h"
#include "base/strings/string_util.h"
#include <set>

namespace atom {

class Menu : public mate::EventEmitter<Menu> {
public:
    explicit Menu(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<Menu>::InitWith(isolate, wrapper);
        m_menuTemplate = nullptr;
        m_hMenu = NULL;
    }

    virtual ~Menu() override {
        OutputDebugStringA("~Menu\n");
        DebugBreak();
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        m_liveMenuItem = new std::set<Menu::MenuItem*>();
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Menu"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("_setApplicationMenu", &Menu::setApplicationMenuApi);
        builder.SetMethod("_sendActionToFirstResponder", &Menu::sendActionToFirstResponderApi);
        builder.SetMethod("_insert", &Menu::_insertApi);
        builder.SetMethod("_append", &Menu::_appendeApi);
        builder.SetMethod("_popup", &Menu::_popupApi);
        builder.SetMethod("getItemCount", &Menu::getItemCountApi);
        builder.SetMethod("quit", &Menu::nullFunction);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Menu"), prototype->GetFunction());
    }

    void nullFunction() {
        DebugBreak();
    }

    // Set the global menubar.
    void setApplicationMenuApi(/*Menu* menu*/int menuTemplateId) {
        DebugBreak();
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
                Menu* submenu = nullptr;
                if (!gin::Converter<Menu*>::FromV8(isolate, outValue, &submenu))
                    submenu = nullptr;
                if (submenu)
                    item->setSubMenu(submenu->m_hMenu);
            }

            if ("click" == keyNameStr && outValue->IsFunction()) {
                item->setClickCallback(outValue);
            }
        }

        if (label.empty())
            label = role;
        item->setLabel(label);

        if (!m_hMenu)
            m_hMenu = ::CreatePopupMenu();
        item->insertPlatformMenu(pos, m_hMenu);
        m_items.push_back(item);
    }
    
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {  
        v8::Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            new Menu(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

    void _popupApi() {
        if (!m_hHideParentWindow)
            createHideParentWindow();

        POINT pt;
        ::GetCursorPos(&pt);
        ::TrackPopupMenu(m_hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hHideParentWindow, NULL);
        ::DestroyMenu(m_hMenu);
        m_hMenu = nullptr;
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;

private:
    class MenuItem {
    public:
        enum MenuItemType {
            ActionType,
            CheckableActionType,
            SeparatorType,
            SubmenuType
        };

        MenuItem(v8::Isolate* isolate, Menu* menu) {
            m_isolate = isolate;
            m_type = ActionType;
            m_isEnabled = true;
            m_isChecked = false;
            m_action = (UINT)this;
            m_menu = menu;
            m_id = 0;
            Menu::m_liveMenuItem->insert(this);
        }

        ~MenuItem() {
            Menu::m_liveMenuItem->erase(this);
        }

        v8::Isolate* getIsolate() const {
            return m_isolate;
        }

        Menu* getMenu() const {
            return m_menu;;
        }

        void setType(MenuItemType type) {
            m_type = type;
        }

        void setSubMenu(HMENU hSubMenu) {
            m_hSubMenu = hSubMenu;
            m_type = SubmenuType;
        }

        void setLabel(const std::string label) {
            m_label = label;
        }

        void setEnabled(bool b) {
            m_isEnabled = b;
        }

        void setChecked(bool b) {
            m_isChecked = b;
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

        void insertPlatformMenu(size_t pos, HMENU hMenu) const {
            int count = ::GetMenuItemCount(hMenu);
            if (count < 0 && pos > count)
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
                info.hSubMenu = m_hSubMenu;
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

    private:
        MenuItemType m_type;
        HMENU m_hSubMenu;
        std::string m_label;
        bool m_isEnabled;
        bool m_isChecked;
        UINT m_action;
        v8::Persistent<v8::Value> m_clickCallbackValue;
        Menu* m_menu;
        int m_id;
        v8::Isolate* m_isolate;
    };

    void createHideParentWindow() {
        if (m_hHideParentWindow)
            return;

        WNDCLASS wc = { 0 };
        wc.style = 0;
        wc.lpfnWndProc = (WNDPROC)mainWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = NULL;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//         wc.hbrBackground = GetStockObject(WHITE_BRUSH);
//         wc.lpszMenuName = MAKEINTRESOURCE(IDM_MYMENURESOURCE);
        wc.lpszClassName = L"HideParentWindowClass";

        if (!RegisterClass(&wc))
            return;

        m_hHideParentWindow = ::CreateWindow(L"HideParentWindowClass", L"HideParentWindowClass",
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, NULL, NULL, NULL, NULL);
    }

    static LRESULT APIENTRY mainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        MenuItem* item = nullptr;
        switch (uMsg) {
        case WM_COMMAND: {
            item = (MenuItem*)wParam;
            if (!m_liveMenuItem || m_liveMenuItem->find(item) == m_liveMenuItem->end())
                return 0;

            //v8::Local<v8::Object> recv = item->getIsolate()->GetCurrentContext()->Global();
            //item->getClickCallback()->Call(recv, 0, nullptr);
            item->getMenu()->mate::EventEmitter<Menu>::emit("click", item->getClickCallbackValue());
            return 0;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }

    base::ListValue* m_menuTemplate;
    HMENU m_hMenu;
    std::vector<MenuItem*> m_items;

    static HWND m_hHideParentWindow;
    static std::set<MenuItem*>* m_liveMenuItem;
};

v8::Persistent<v8::Function> Menu::constructor;
gin::WrapperInfo Menu::kWrapperInfo = { gin::kEmbedderNativeGin };
std::set<Menu::MenuItem*>* Menu::m_liveMenuItem = nullptr;
HWND Menu::m_hHideParentWindow = nullptr;

static void initializeMenuApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Menu::init(env->isolate(), target);
}

static const char BrowserMenuNative[] =
"console.log('BrowserMenuNative');"
"exports = function {};";

static NodeNative nativeBrowserMenuNative{ "Menu", BrowserMenuNative, sizeof(BrowserMenuNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_menu, initializeMenuApi, &nativeBrowserMenuNative)

}