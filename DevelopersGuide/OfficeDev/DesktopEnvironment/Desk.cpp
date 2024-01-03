#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>

#include <osl/file.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/awt/Toolkit.hpp>
#include <com/sun/star/awt/XSystemChildFactory.hpp>
#include <com/sun/star/frame/XFramesSupplier.hpp>

#define SYSTEM_WIN32 1
#define SYSTEM_WIN16 2
#define SYSTEM_JAVA 3
#define SYSTEM_OS2 4
#define SYSTEM_MAC 5
#define SYSTEM_XWINDOW 6

template <typename T>
using Ref = css::uno::Reference<T>;

template <typename T>
using Seq = css::uno::Sequence<T>;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "usage: ./SlideShow <full filename>\n";
        return 1;
    }

    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display.\n");
        return 1;
    }

    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 1920, 1080, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    try {
        rtl::OUString file_path, file_url;
        file_path = rtl::OUString::createFromAscii(argv[1]);
        osl::FileBase::getFileURLFromSystemPath(file_path, file_url);

        Ref<css::uno::XComponentContext> context = cppu::bootstrap();
        Ref<css::lang::XMultiComponentFactory> comp_factory = context->getServiceManager();

        Ref<css::frame::XFrame> frame(comp_factory->createInstanceWithContext("com.sun.star.frame.Frame", context), css::uno::UNO_QUERY);

        Ref<css::awt::XToolkit> toolkit(comp_factory->createInstanceWithContext("com.sun.star.awt.Toolkit", context), css::uno::UNO_QUERY);

        Ref<css::awt::XSystemChildFactory> child_factory(toolkit, css::uno::UNO_QUERY);
        auto peer = child_factory->createSystemChild(css::uno::Any(window), Seq<sal_Int8>(), SYSTEM_XWINDOW);
        if (peer == nullptr)
            printf("failed!!!!\n");
        Ref<css::awt::XWindow> xwindow(peer, css::uno::UNO_QUERY);
        frame->initialize(xwindow);
        Ref<css::frame::XFramesSupplier> tree_root(comp_factory->createInstanceWithContext("com.sun.star.frame.Desktop", context), css::uno::UNO_QUERY);
        xwindow->setVisible(true);
        printf("...\n");

        // Ref<css::uno::XInterface> desktop = comp_factory->createInstanceWithContext("com.sun.star.frame.Desktop", context);
        // Ref<css::frame::XComponentLoader> loader(desktop, css::uno::UNO_QUERY);

        // Seq<css::beans::PropertyValue> load_props(0);
        // Ref<css::lang::XComponent> doc = loader->loadComponentFromURL(file_url, "_blank", 0, load_props);

    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }

    XEvent event;
    while (1) {
        XNextEvent(display, &event);
    }

    XCloseDisplay(display);

    return 0;
}