// Src/Platform/Mac/MacTitlebar.mm
#import <Cocoa/Cocoa.h>
#import <GLFW/glfw3.h>
#import <GLFW/glfw3native.h>

#include <functional>
#include <string>
#include <vector>

// Deine C++-Typen verfügbar machen:
#include "GUI/GuiLayer.h"   // enthält MenuDef/MenuItem/GuiLayer

// Retain-Ziele, damit Funktionsobjekte leben
static NSMutableArray<id>* sMenuTargets;

@interface MenuActionTarget : NSObject
@property(nonatomic, copy) void (^block)(void);
- (instancetype)initWithFunction:(const std::function<void()>&)fn;
- (void)invoke:(id)sender;
@end

@implementation MenuActionTarget
- (instancetype)initWithFunction:(const std::function<void()>&)fn {
    if ((self = [super init])) {
        // Wrap std::function in Block
        std::function<void()> f = fn;
        self.block = ^{ if (f) f(); };
    }
    return self;
}
- (void)invoke:(id)sender { if (self.block) self.block(); }
@end

// --- Shortcut-Parsing: "Cmd+Shift+N", "Alt+F4", "Ctrl+S" ---
static void ParseShortcut(const std::string& s, NSString** keyOut, NSEventModifierFlags* modsOut) {
    NSString* key = @"";
    NSEventModifierFlags mods = 0;

    // naive Split an '+'
    std::string token;
    size_t start = 0;
    auto upper = [](std::string t){ for (auto& c: t) c = (char)toupper((unsigned char)c); return t; };

    while (start <= s.size()) {
        size_t plus = s.find('+', start);
        std::string t = s.substr(start, plus==std::string::npos ? std::string::npos : plus-start);
        // trim
        size_t a = t.find_first_not_of(" \t"); if (a==std::string::npos) t.clear(); else t = t.substr(a, t.find_last_not_of(" \t")-a+1);
        std::string up = upper(t);

        if (up=="CMD" || up=="COMMAND" || up=="META" || up=="SUPER") mods |= NSEventModifierFlagCommand;
        else if (up=="CTRL" || up=="CONTROL") mods |= NSEventModifierFlagControl;
        else if (up=="ALT" || up=="OPTION") mods |= NSEventModifierFlagOption;
        else if (up=="SHIFT") mods |= NSEventModifierFlagShift;
        else {
            // Taste
            if (up.size()==1) {
                key = [NSString stringWithFormat:@"%c", (char)tolower((unsigned char)up[0])];
            } else if (up.rfind("F",0)==0) {
                // F1..F12: macOS erwartet z.B. @"\uF704" für F1, @"\uF705" F2, ...
                // Minimal: kein KeyEquivalent setzen (nur Menüeintrag), oder mappe ein paar häufige:
                static NSDictionary<NSString*, NSString*>* fx = @{
                    @"F1":  @"\uF704", @"F2":  @"\uF705", @"F3":  @"\uF706", @"F4":  @"\uF707",
                    @"F5":  @"\uF708", @"F6":  @"\uF709", @"F7":  @"\uF70A", @"F8":  @"\uF70B",
                    @"F9":  @"\uF70C", @"F10": @"\uF70D", @"F11": @"\uF70E", @"F12": @"\uF70F"
                };
                key = fx[ [NSString stringWithUTF8String:up.c_str()] ] ?: @"";
            } else if (up=="ENTER"||up=="RETURN") key = @"\r";
            else if (up=="ESC"||up=="ESCAPE")     key = @"\e";
            else if (up=="SPACE")                 key = @" ";
            else if (up=="TAB")                   key = @"\t";
            // sonst leer lassen
        }

        if (plus == std::string::npos) break;
        start = plus + 1;
    }

    if (keyOut) *keyOut = key;
    if (modsOut) *modsOut = mods;
}

// Hilfsfunktion: NSMenuItem bauen (Separator oder Action)
static NSMenuItem* MakeItem(const gear::MenuItem& mi) {
    if (mi.isSeparator) return [NSMenuItem separatorItem];

    NSString* title = [NSString stringWithUTF8String:mi.label.c_str()];
    NSString* key = @"";
    NSEventModifierFlags mods = 0;
    if (mi.shortcut && !mi.shortcut->empty()) {
        ParseShortcut(*mi.shortcut, &key, &mods);
    }

    SEL sel = mi.action ? @selector(invoke:) : nil;
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:title action:sel keyEquivalent:key];
    [item setKeyEquivalentModifierMask:mods];

    if (mi.action) {
        if (!sMenuTargets) sMenuTargets = [NSMutableArray array];
        MenuActionTarget* tgt = [[MenuActionTarget alloc] initWithFunction:mi.action];
        [item setTarget:tgt];
        [sMenuTargets addObject:tgt]; // retain
    }
    return item;
}

extern "C" void MacSetupMenuAndTitlebar(GLFWwindow* win)
{
    NSWindow* nswin = glfwGetCocoaWindow(win);

    [nswin setTitleVisibility:NSWindowTitleHidden];
    [nswin setTitlebarAppearsTransparent:YES];
    [nswin setStyleMask:[nswin styleMask] | NSWindowStyleMaskFullSizeContentView];

    NSToolbar* toolbar = [[NSToolbar alloc] initWithIdentifier:@"MainToolbar"];
    [toolbar setShowsBaselineSeparator:NO];
    [nswin setToolbar:toolbar];
}

// SPIEGELT die GuiLayer-Menüs in die native Menüleiste
extern "C" void MacSyncMenusFromGuiLayer(gear::GuiLayer* layer)
{
    if (!layer) return;

    NSMenu* menubar = [[NSMenu alloc] init];
    [NSApp setMainMenu:menubar];

    // --- App-Menü (Pflicht auf macOS): About/Hide/Quit etc. ---
    NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];
    [menubar addItem:appMenuItem];
    NSMenu* appMenu = [[NSMenu alloc] initWithTitle:@"App"];
    [appMenuItem setSubmenu:appMenu];

    NSString* appName = [[NSProcessInfo processInfo] processName];
    [appMenu addItemWithTitle:[@"About " stringByAppendingString:appName] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];

    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:[@"Hide " stringByAppendingString:appName] action:@selector(hide:) keyEquivalent:@"h"];
    NSMenuItem* hideOthers = [[NSMenuItem alloc] initWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
    [hideOthers setKeyEquivalentModifierMask:NSEventModifierFlagOption|NSEventModifierFlagCommand];
    [appMenu addItem:hideOthers];
    [appMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

    [appMenu addItem:[NSMenuItem separatorItem]];
    // Cmd+Q: System-weit üblich – nimm Cocoa-terminate:, nicht deine eigene Action
    NSString* quitTitle = [@"Quit " stringByAppendingString:appName];
    [appMenu addItemWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];

    // --- Aus deiner Registry weitere Menüs anhängen ---
    const auto& menus = layer->Menus();
    for (const auto& m : menus) {
        if (m.name == "File") {
            // Meist redundant mit App-Menü (Quit), aber wir können es trotzdem zeigen:
        }
        NSString* title = [NSString stringWithUTF8String:m.name.c_str()];
        NSMenuItem* top = [[NSMenuItem alloc] initWithTitle:title action:nil keyEquivalent:@""];
        [menubar addItem:top];

        NSMenu* submenu = [[NSMenu alloc] initWithTitle:title];
        [top setSubmenu:submenu];

        for (const auto& it : m.items) {
            NSMenuItem* item = MakeItem(it);
            [submenu addItem:item];
        }
    }
}
