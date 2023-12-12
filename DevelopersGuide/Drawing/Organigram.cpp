#include <iostream>

#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/text/XText.hpp>

template <typename T>
using Ref = css::uno::Reference<T>; // note: css::uno::Reference should be allocated on stack

template <typename T>
using Seq = css::uno::Sequence<T>;

int main()
{
    try {
        // Get the remote office context of a running office
        Ref<css::uno::XComponentContext> ctx = cppu::bootstrap();
        Ref<css::lang::XMultiComponentFactory> factory = ctx->getServiceManager();

        Ref<css::uno::XInterface> desktop = factory->createInstanceWithContext("com.sun.star.frame.Desktop", ctx);
        Ref<css::frame::XComponentLoader> loader(desktop, css::uno::UNO_QUERY);

        Seq<css::beans::PropertyValue> load_props(0);
        Ref<css::lang::XComponent> draw_doc = loader->loadComponentFromURL("private:factory/sdraw", "_blank", 0, load_props);

        // Get draw page by index
        Ref<css::drawing::XDrawPagesSupplier> pages_supplier(draw_doc, css::uno::UNO_QUERY);
        Ref<css::drawing::XDrawPages> pages = pages_supplier->getDrawPages();
        Ref<css::drawing::XDrawPage> page(pages->getByIndex(0), css::uno::UNO_QUERY);

        // Find out page dimensions
        Ref<css::beans::XPropertySet> page_props(page, css::uno::UNO_QUERY);

        int page_width, page_height;
        int page_border_top, page_border_left, page_border_right;
        page_props->getPropertyValue("Width") >>= page_width;
        page_props->getPropertyValue("Height") >>= page_height;
        page_props->getPropertyValue("BorderTop") >>= page_border_top;
        page_props->getPropertyValue("BorderLeft") >>= page_border_left;
        page_props->getPropertyValue("BorderRight") >>= page_border_right;

        int draw_width = page_width - page_border_left - page_border_right;
        int hor_center = page_border_left + draw_width / 2;

        // Data for organigram
        rtl::OUString org_units[2][4];
        org_units[0][0] = "Management";  // level 0
        org_units[1][0] = "Production";  // level 1
        org_units[1][1] = "Purchasing";  // level 1
        org_units[1][2] = "IT Services"; // level 1
        org_units[1][3] = "Sales";       // level 1
        int level_count[2] = {1, 4};

        // Calculate shape sizes and positions
        int hor_space = 300;
        int ver_space = 3000;
        int shape_width = (draw_width - (level_count[1] - 1) * hor_space) / level_count[1];
        int shape_height = page_height / 20;
        int shape_x = page_width / 2 - shape_width / 2;
        int level_y = 0;
        Ref<css::drawing::XShape> start_shape = nullptr;

        // Get document factory
        Ref<css::lang::XMultiServiceFactory> doc_factory(draw_doc, css::uno::UNO_QUERY);

        // Creating and adding RecangleShapes and Connectors
        for (int level = 0; level <= 1; ++level) {
            level_y = page_border_top + 2000 + level * (shape_height + ver_space);
            for (int i = level_count[level] - 1; i > -1; --i) {
                shape_x = hor_center - (level_count[level] * shape_width + (level_count[level] - 1) * hor_space) / 2 + i * shape_width + i * hor_space;
                Ref<css::drawing::XShape> shape(doc_factory->createInstance("com.sun.star.drawing.RectangleShape"), css::uno::UNO_QUERY);
                shape->setPosition({shape_x, level_y});
                shape->setSize({shape_width, shape_height});
                page->add(shape);

                // Set the text
                Ref<css::text::XText> text(shape, css::uno::UNO_QUERY);
                text->setString(org_units[level][i]);

                // memorize the root shape, for connectors
                if (level == 0 && i == 0)
                    start_shape = shape;

                // add connectors for level 1
                if (level == 1) {
                    doc_factory->createInstance("com.sun.star.drawing.ConnectorShape");
                    Ref<css::drawing::XShape> connector(doc_factory->createInstance("com.sun.star.drawing.ConnectorShape"), css::uno::UNO_QUERY);
                    page->add(connector);
                    Ref<css::beans::XPropertySet> connector_props(connector, css::uno::UNO_QUERY);
                    connector_props->setPropertyValue("StartShape", css::uno::Any(start_shape));
                    connector_props->setPropertyValue("EndShape", css::uno::Any(shape));
                    // Glue point positions: 0=top 1=left 2=bottom 3=right
                    connector_props->setPropertyValue("StartGluePointIndex", css::uno::Any(2));
                    connector_props->setPropertyValue("EndGluePointIndex", css::uno::Any(0));
                }
            }
        }
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }

    return 0;
}
