#include <bits/stdc++.h>
#include <libreoffice.hpp>

// note: css::uno::Reference should be allocated on stack
template <typename T>
using Ref = css::uno::Reference<T>;

template <typename T>
using Seq = css::uno::Sequence<T>;

int main()
{
    Ref<css::uno::XComponentContext> context = cppu::bootstrap();
    Ref<css::lang::XMultiComponentFactory> factory = context->getServiceManager();
    Seq<rtl::OUString> service_names = factory->getAvailableServiceNames();
    for (const auto& service_name : service_names)
        std::cout << service_name.toUtf8().getStr() << '\n';
}