#include <bits/stdc++.h>
#include <libreoffice.hpp>

using css::uno::XInterface;

template <typename T>
using Ref = css::uno::Reference<T>; // note: css::uno::Reference should be allocated on stack

template <typename T>
using Seq = css::uno::Sequence<T>;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "usage: UrlResolver uno-url\n"
                  << "       uno-url  The uno-url identifying the object to\n"
                  << "                be imported, for instance\n"
                  << "                uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager"
                  << "                (use \" on unix shells to avoid ;-problems)\n";
        return 1;
    }

    try {
        // Create default local componenet context
        Ref<css::uno::XComponentContext> context = cppu::bootstrap();

        // Initial service mananger
        Ref<css::lang::XMultiComponentFactory> factory = context->getServiceManager();

        // Create a UnoUrlResolver
        Ref<css::bridge::XUnoUrlResolver> url_resolver(factory->createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", context), css::uno::UNO_QUERY);

        // Import the object
        Ref<XInterface> object = url_resolver->resolve(rtl::OUString::createFromAscii(argv[1]));

        if (object != nullptr) {
            std::cout << "initial object successfully retrieved\n";
        } else {
            std::cout << "given initial-object name unknown at server side\n";
        }
    } catch (css::connection::NoConnectException &e) {
        std::cout << "Couldn't connect to remote server\n"
                  << e.Message << '\n';
    } catch (css::connection::ConnectionSetupException &e) {
        std::cout << "Couldn't access necessary local resource to establish the interprocess connection\n"
                  << e.Message << '\n';
    } catch (css::lang::IllegalAccessException &e) {
        std::cout << "uno-url is syntactical illegal ( " << argv[1] << " )\n"
                  << e.Message << '\n';
    } catch (css::uno::RuntimeException &e) {
        std::cout << "RuntimeException\n"
                  << e.Message << '\n';
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }

    return 0;
}
