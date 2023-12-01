#include <string>
#include <thread>
#include <iostream>

#include <sal/main.h>
#include <osl/file.hxx>
#include <osl/process.h>
#include <rtl/process.h>
#include <rtl/ustring.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv)
{
    (void)argc;
    (void)argv;

    rtl::OUString file_name, file_url;

    if (rtl_getAppCommandArgCount() < 1) {
        std::cout << "using: load <file_name>\n";
    } else {
        rtl_getAppCommandArg(0, &file_name.pData);
        osl::FileBase::getFileURLFromSystemPath(file_name, file_url);
    }

    try {
        std::cout << "connecting to a running office..\n";
        css::uno::Reference<css::uno::XComponentContext> ctx(cppu::bootstrap());
        css::uno::Reference<css::lang::XMultiComponentFactory> mcf = ctx->getServiceManager();
        css::uno::Reference<css::uno::XInterface> desk = mcf->createInstanceWithContext("com.sun.star.frame.Desktop", ctx);
        
        std::cout << "loading file..\n";
        css::uno::Reference<css::frame::XComponentLoader> loader(desk, css::uno::UNO_QUERY);
        css::uno::Sequence<css::beans::PropertyValue> args(0);
        loader->loadComponentFromURL(file_url, "_blank", 0, args);

        std::cout << "waiting 1 second.." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        css::uno::Reference<css::util::XCloseable> closeable(desk, css::uno::UNO_QUERY);
        if (closeable != nullptr) {
            closeable->close(false);
        } else {
            css::uno::Reference<css::lang::XComponent> cmp(desk, css::uno::UNO_QUERY);
            cmp->dispose();
        }
        std::cout << "closing file..\n";

    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }
    return 0;
}
