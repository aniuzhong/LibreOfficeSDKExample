#include <iostream>

#include <osl/file.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/presentation/XPresentationSupplier.hpp>
#include <com/sun/star/presentation/XPresentation2.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

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

    rtl::OUString file_path, file_url;
    file_path = rtl::OUString::createFromAscii(argv[1]);
    osl::FileBase::getFileURLFromSystemPath(file_path, file_url);

    Ref<css::uno::XComponentContext> context = cppu::bootstrap();
    Ref<css::lang::XMultiComponentFactory> factory = context->getServiceManager();

    Ref<css::uno::XInterface> desktop = factory->createInstanceWithContext("com.sun.star.frame.Desktop", context);
    Ref<css::frame::XComponentLoader> loader(desktop, css::uno::UNO_QUERY);

    Seq<css::beans::PropertyValue> load_props(0);
    Ref<css::lang::XComponent> doc = loader->loadComponentFromURL(file_url, "_blank", 0, load_props);

    Ref<css::presentation::XPresentationSupplier> presentation_supplier(doc, css::uno::UNO_QUERY);
    Ref<css::presentation::XPresentation> presentation = presentation_supplier->getPresentation();

    Ref<css::beans::XPropertySet> presentation_prop_set(presentation, css::uno::UNO_QUERY);
    // presentation_prop_set->setPropertyValue("AllowAnimations", css::uno::Any(true));
    // presentation_prop_set->setPropertyValue("CustomShow", css::uno::Any(rtl::OUString()));
    // presentation_prop_set->setPropertyValue("FirstPage", css::uno::Any(rtl::OUString()));
    presentation_prop_set->setPropertyValue("IsAlwaysOnTop", css::uno::Any(true));
    presentation_prop_set->setPropertyValue("IsAutomatic", css::uno::Any(false));
    presentation_prop_set->setPropertyValue("IsEndless", css::uno::Any(true));
    presentation_prop_set->setPropertyValue("IsFullScreen", css::uno::Any(true));
    // presentation_prop_set->setPropertyValue("IsLivePresentation", css::uno::Any(true));
    presentation_prop_set->setPropertyValue("IsMouseVisible", css::uno::Any(true));
    presentation_prop_set->setPropertyValue("Pause", css::uno::Any(0));
    presentation_prop_set->setPropertyValue("StartWithNavigator", css::uno::Any(true));
    presentation_prop_set->setPropertyValue("UsePen", css::uno::Any(true));

    Ref<css::presentation::XPresentation2> presentation2(presentation, css::uno::UNO_QUERY);
    // presentation2->getPropertySetInfo();
    // presentation2->addPropertyChangeListener()
    // presentation2->removePropertyChangeListener()
    // presentation2->addVetoableChangeListener()
    // presentation2->removeVetoableChangeListener()
    presentation2->start();
    presentation2->rehearseTimings();

    Ref<css::presentation::XSlideShowController> controller = presentation2->getController();
    std::cout << "getSlideCount:" << controller->getSlideCount() << '\n';
    std::cout << "isEndless:" << controller->isEndless() << '\n';
    std::cout << "isFullScreen:" << controller->isFullScreen() << '\n';
    // getSlideByIndex
    // addSlideShowListener
    // removeSlideShowListener

    while (true) {
        std::string input;
        std::cin >> input;
        if (input == "gotoNextSlide") {
            controller->gotoNextSlide();
        } else if (input == "gotoPreviousSlide") {
            controller->gotoPreviousSlide();
        } else if (input == "gotoFirstSlide") {
            controller->gotoFirstSlide();
        } else if (input == "gotoLastSlide") {
            controller->gotoLastSlide();
        } else if (input == "gotoSlideIndex") { // gotoSlide (com::sun::star::drawing::XDrawPage)
            sal_Int32 index;
            std::cin >> index;
            controller->gotoSlideIndex(index);
        } else if (input == "stopSound") {
            controller->stopSound();
        } else if (input == "pause") {
            if (!controller->isPaused())
                controller->pause();
        } else if (input == "resume") {
            if (controller->isPaused())
                controller->resume();
        } else if (input == "gotoNextEffect") {
            controller->gotoNextEffect();
        } else if (input == "gotoPreviousEffect") {
            controller->gotoPreviousEffect();
        } else if (input == "blankScreen") {
            sal_Int32 color;
            std::cin >> color;
            controller->gotoSlideIndex(color);
        } else if (input == "active") {
            if (!controller->isActive())
                controller->activate();
        } else if (input == "deactivate") {
            if (controller->isActive())
                controller->deactivate();
        } else if (input == "setEraseAllInk") {
            int erase_all_ink;
            std::cin >> erase_all_ink;
            controller->setEraseAllInk(erase_all_ink);
        } else if (input == "Q") {
            break;
        } else {
            continue;
        }
    }

    // getSlideShow()
    
    std::cout << std::boolalpha << "Presentation is running: " << presentation2->isRunning() << '\n';
    std::cout << "Presentation is running: " << controller->isRunning() << '\n';

    // presentation->end();

    return 1;
}