#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstddef>
#include <iostream>
#include <thread>

#include <com/sun/star/awt/Rectangle.hpp>
// #include <com/sun/star/awt/XToolkit.hpp>
// #include <com/sun/star/awt/XToolkit2.hpp>
// #include <com/sun/star/awt/XToolkit3.hpp>
#include <com/sun/star/awt/XToolkitExperimental.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/XSystemChildFactory.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XFrames.hpp>
#include <com/sun/star/frame/XFramesSupplier.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/uno/XInterface.hpp>
#include <osl/file.hxx>
#include <rtl/ustring.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/lang/SystemDependent.hpp>

using css::awt::Rectangle;
using css::awt::WindowDescriptor;
using css::awt::XSystemChildFactory;
// using css::awt::XToolkit;
// using css::awt::XToolkit2;
// using css::awt::XToolkit3;
using css::awt::XToolkitExperimental;
using css::awt::XWindow;
using css::awt::XWindowPeer;
using css::awt::WindowClass::WindowClass_TOP;
using css::beans::PropertyValue;
using css::frame::XComponentLoader;
using css::frame::XFrame;
using css::frame::XFrames;
using css::frame::XFramesSupplier;
using css::lang::XComponent;
using css::lang::XMultiComponentFactory;
using css::uno::UNO_QUERY;
using css::uno::XComponentContext;
using css::uno::XInterface;
using osl::FileBase;
using rtl::OUString;

template <typename T>
using Ref = css::uno::Reference<T>;

template <typename T>
using Seq = css::uno::Sequence<T>;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "usage: ./Desk <full filename>\n";
        return 1;
    }

    OUString path, url;
    path = OUString::createFromAscii(argv[1]);
    FileBase::getFileURLFromSystemPath(path, url);

    Ref<XComponentContext> ctx = cppu::bootstrap();

    // Create the view frame for showing the office documents on demand.

    // Create an empty office frame first
    Ref<XFrame> frame(ctx->getServiceManager()->createInstanceWithContext("com.sun.star.frame.Frame", ctx), UNO_QUERY);
    if (frame != nullptr)
        std::cout << "an empty office frame created..\n";

    // Create an office window then

    // Get access to toolkit of remote office to create the container window of new target frame
    Ref<XToolkitExperimental> toolkit(ctx->getServiceManager()->createInstanceWithContext("com.sun.star.awt.Toolkit", ctx), UNO_QUERY);
    if (toolkit != nullptr)
        std::cout << "got toolkit..\n";

    // Create an external system window
    WindowDescriptor wd;
    wd.Type = WindowClass_TOP;
    wd.WindowServiceName = "window";
    wd.ParentIndex = -1;
    wd.Parent = nullptr;
    wd.Bounds = Rectangle(0, 0, 0, 0);
    wd.WindowAttributes = css::awt::WindowAttribute::BORDER |
                          css::awt::WindowAttribute::MOVEABLE |
                          css::awt::WindowAttribute::SIZEABLE |
                          css::awt::WindowAttribute::CLOSEABLE;
    Ref<XWindowPeer> peer = toolkit->createWindow(wd);
    Ref<XWindow> window(peer, UNO_QUERY);
    if (window != nullptr)
        std::cout << "window created..\n";

    // Pass the window the frame as his new container window.
    frame->initialize(window);

    // Insert the new frame in desktop hierarchy.
    Ref<XFramesSupplier> tree_root(ctx->getServiceManager()->createInstanceWithContext("com.sun.star.frame.Desktop", ctx), UNO_QUERY);
    Ref<XFrames> child_container = tree_root->getFrames();
    child_container->append(frame);

    // Make some further initializations on frame and window.
    OUString frame_name("odk_officedev_desk_old_frame_name");
    window->setVisible(true);
    frame->setName(frame_name);

    // Load document specified by a URL into given frame synchronously.
    OUString target("odk_officedev_desk");
    frame->setName(target);
    Seq<PropertyValue> load_props(0);
    Ref<XComponentLoader> loader(ctx->getServiceManager()->createInstanceWithContext("com.sun.star.frame.Desktop", ctx), UNO_QUERY);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    Ref<XComponent> doc = loader->loadComponentFromURL(url, target, css::frame::FrameSearchFlag::CHILDREN, load_props);
    frame->setName(frame_name);

    // Ref<XSystemChildFactory> child_factory(toolkit, UNO_QUERY);
    // if (child_factory != nullptr)
    //     std::cout << "child_factory created..\n";

    // Display *display = XOpenDisplay(NULL);
    // int screen = DefaultScreen(display);
    // sal_Int64 xid = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 100, 100, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    // try {
    //     child_factory->createSystemChild(css::uno::Any(xid), Seq<sal_Int8>(), css::lang::SystemDependent::SYSTEM_XWINDOW);
    // } catch (css::uno::Exception &e) {
    //     std::cout << e.Message << '\n';
    // }
}
