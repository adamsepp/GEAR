// Src/Platform/Mac/MacTitlebar.mm
#import <Cocoa/Cocoa.h>
#import <GLFW/glfw3.h>
#import <GLFW/glfw3native.h>

extern "C" void MacSetupMenuAndTitlebar(GLFWwindow* win)
{
    NSWindow* nswin = glfwGetCocoaWindow(win);

    // Transparente, moderne Titlebar
    [nswin setTitleVisibility:NSWindowTitleHidden];
    [nswin setTitlebarAppearsTransparent:YES];
    [nswin setStyleMask:[nswin styleMask] | NSWindowStyleMaskFullSizeContentView];

    // --- Beispiel-Menüleiste ---
    NSMenu* menubar = [[NSMenu alloc] init];
    [NSApp setMainMenu:menubar];

    // App-Menü (automatisch App-Name)
    NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];
    [menubar addItem:appMenuItem];
    NSMenu* appMenu = [[NSMenu alloc] initWithTitle:@"App"];
    [appMenuItem setSubmenu:appMenu];

    // "Quit" hinzufügen
    NSString* quitTitle = [@"Quit " stringByAppendingString:[[NSProcessInfo processInfo] processName]];
    NSMenuItem* quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
                                                          action:@selector(terminate:)
                                                   keyEquivalent:@"q"];
    [appMenu addItem:quitMenuItem];

    // File-Menü
    NSMenuItem* fileMenuItem = [[NSMenuItem alloc] init];
    [menubar addItem:fileMenuItem];
    NSMenu* fileMenu = [[NSMenu alloc] initWithTitle:@"File"];
    [fileMenuItem setSubmenu:fileMenu];

    [fileMenu addItemWithTitle:@"New" action:nil keyEquivalent:@"n"];
    [fileMenu addItemWithTitle:@"Open…" action:nil keyEquivalent:@"o"];
    [fileMenu addItemWithTitle:@"Save" action:nil keyEquivalent:@"s"];

    // Edit-Menü
    NSMenuItem* editMenuItem = [[NSMenuItem alloc] init];
    [menubar addItem:editMenuItem];
    NSMenu* editMenu = [[NSMenu alloc] initWithTitle:@"Edit"];
    [editMenuItem setSubmenu:editMenu];

    [editMenu addItemWithTitle:@"Undo" action:@selector(undo:) keyEquivalent:@"z"];
    [editMenu addItemWithTitle:@"Redo" action:@selector(redo:) keyEquivalent:@"Z"];
    [editMenu addItem:[NSMenuItem separatorItem]];
    [editMenu addItemWithTitle:@"Cut" action:@selector(cut:) keyEquivalent:@"x"];
    [editMenu addItemWithTitle:@"Copy" action:@selector(copy:) keyEquivalent:@"c"];
    [editMenu addItemWithTitle:@"Paste" action:@selector(paste:) keyEquivalent:@"v"];

    // Optional: Toolbar hinzufügen → sorgt dafür, dass Titlebar hell/dunkel wie Safari wirkt
    NSToolbar* toolbar = [[NSToolbar alloc] initWithIdentifier:@"MainToolbar"];
    [toolbar setShowsBaselineSeparator:NO]; // keine Linie unten
    [nswin setToolbar:toolbar];
}
