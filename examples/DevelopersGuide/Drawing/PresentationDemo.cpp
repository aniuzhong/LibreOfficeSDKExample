#include <bits/stdc++.h>
#include <libreoffice.hpp>

template <typename T>
using Ref = css::uno::Reference<T>; // note: css::uno::Reference should be allocated on stack

template <typename T>
using Seq = css::uno::Sequence<T>;

static Ref<css::drawing::XShape>
create_shape(Ref<css::lang::XComponent> draw_doc, css::awt::Point pos, css::awt::Size size, rtl::OUString shape_type)
{
    Ref<css::lang::XMultiServiceFactory> factory(draw_doc, css::uno::UNO_QUERY);
    Ref<css::drawing::XShape> shape(factory->createInstance(shape_type), css::uno::UNO_QUERY);
    shape->setPosition(pos);
    shape->setSize(size);
    return shape;
}

static Ref<css::beans::XPropertySet>
create_and_insert_shape(Ref<css::lang::XComponent> draw_doc, Ref<css::drawing::XShapes> shapes, css::awt::Point pos, css::awt::Size size, rtl::OUString shape_type)
{
    Ref<css::drawing::XShape> shape = create_shape(draw_doc, pos, size, shape_type);
    shapes->add(shape);
    return Ref<css::beans::XPropertySet>(shape, css::uno::UNO_QUERY);
}

static void
set_slide_transition(Ref<css::drawing::XDrawPage> page, css::presentation::FadeEffect effect, css::presentation::AnimationSpeed speed, int change, int duration)
{
    Ref<css::lang::XServiceInfo> service_info(page, css::uno::UNO_QUERY);
    if (service_info->supportsService("com.sun.star.presentation.DrawPage")) {
        Ref<css::beans::XPropertySet> page_prop_set(page, css::uno::UNO_QUERY);
        page_prop_set->setPropertyValue("Effect", css::uno::Any(effect));
        page_prop_set->setPropertyValue("Speed", css::uno::Any(speed));
        page_prop_set->setPropertyValue("Change", css::uno::Any(change));
        page_prop_set->setPropertyValue("Duration", css::uno::Any(duration));
    }
}

static Ref<css::beans::XPropertySet>
add_portion(Ref<css::drawing::XShape> shape, rtl::OUString text_str, bool new_paragraph)
{
    Ref<css::text::XText> text(shape, css::uno::UNO_QUERY);
    Ref<css::text::XTextCursor> text_cursor = text->createTextCursor();
    text_cursor->gotoEnd(false);
    Ref<css::text::XTextRange> text_range(text_cursor, css::uno::UNO_QUERY);
    if (new_paragraph) {
        text->insertControlCharacter(text_range, css::text::ControlCharacter::PARAGRAPH_BREAK, false);
        text_cursor->gotoEnd(false);
    }
    text_range->setString(text_str);
    text_cursor->gotoEnd(true);
    return Ref<css::beans::XPropertySet>(text_range, css::uno::UNO_QUERY);
}

int main()
{
    try {
        Ref<css::uno::XComponentContext> context = cppu::bootstrap();
        Ref<css::lang::XMultiComponentFactory> factory = context->getServiceManager();

        Ref<css::uno::XInterface> desktop = factory->createInstanceWithContext("com.sun.star.frame.Desktop", context);
        Ref<css::frame::XComponentLoader> loader(desktop, css::uno::UNO_QUERY);

        Seq<css::beans::PropertyValue> load_props(1);
        load_props[0].Name = "Silent";
        load_props[0].Value = css::uno::Any(true);
        Ref<css::lang::XComponent> draw_doc = loader->loadComponentFromURL("private:factory/simpress", "_blank", 0, load_props);

        // Create pages, so that three are available
        Ref<css::drawing::XDrawPagesSupplier> pages_supplier(draw_doc, css::uno::UNO_QUERY);
        Ref<css::drawing::XDrawPages> pages = pages_supplier->getDrawPages();
        while (pages->getCount() < 3)
            pages->insertNewByIndex(0);

        // Set the slide transition for the first page
        Ref<css::drawing::XDrawPage> page(pages->getByIndex(0), css::uno::UNO_QUERY);
        Ref<css::drawing::XShapes> shapes(page, css::uno::UNO_QUERY);

        // Set slide transition effect
        set_slide_transition(page, css::presentation::FadeEffect::FadeEffect_FADE_FROM_RIGHT, css::presentation::AnimationSpeed::AnimationSpeed_FAST, 1, 0);
        // Create a rectangle that is placed on the top left of the page
        Ref<css::beans::XPropertySet> shape_prop_set = create_and_insert_shape(draw_doc, shapes, {1000, 1000}, {5000, 5000}, "com.sun.star.drawing.RectangleShape");
        shape_prop_set->setPropertyValue("Effect", css::uno::Any(css::presentation::AnimationEffect::AnimationEffect_WAVYLINE_FROM_BOTTOM));

        // The following three properties provokes that the shape is dimmed to red
        // after the animation has been finished
        shape_prop_set->setPropertyValue("DimHide", css::uno::Any(false));
        shape_prop_set->setPropertyValue("DimPrevious", css::uno::Any(true));
        shape_prop_set->setPropertyValue("DimColor", css::uno::Any(0xff0000));

        // Set the slide transition for the second page
        page = Ref<css::drawing::XDrawPage>(pages->getByIndex(1), css::uno::UNO_QUERY);
        shapes = Ref<css::drawing::XShapes>(page, css::uno::UNO_QUERY);
        set_slide_transition(page, css::presentation::FadeEffect::FadeEffect_FADE_FROM_RIGHT, css::presentation::AnimationSpeed::AnimationSpeed_SLOW, 1, 0);

        // Create an ellipse that is placed on the bottom right of second page
        shape_prop_set = create_and_insert_shape(draw_doc, shapes, {21000, 15000}, {5000, 5000}, "com.sun.star.drawing.EllipseShape");

        // Create two objects for the third page
        // clicking the first object lets the presentation jump
        // to page one by using ClickAction.FIRSTPAGE,
        // the second object lets the presentation jump to page two
        // by using a ClickAction.BOOKMARK;
        page = Ref<css::drawing::XDrawPage>(pages->getByIndex(2), css::uno::UNO_QUERY);
        shapes = Ref<css::drawing::XShapes>(page, css::uno::UNO_QUERY);
        set_slide_transition(page, css::presentation::FadeEffect::FadeEffect_ROLL_FROM_LEFT, css::presentation::AnimationSpeed::AnimationSpeed_MEDIUM, 2, 0);
        Ref<css::drawing::XShape> shape = create_shape(draw_doc, {1000, 8000}, {5000, 5000}, "com.sun.star.drawing.EllipseShape");
        shapes->add(shape);
        add_portion(shape, "click to go", false);
        add_portion(shape, "to the first page", true);
        shape_prop_set = Ref<css::beans::XPropertySet>(shape, css::uno::UNO_QUERY);
        shape_prop_set->setPropertyValue("Effect", css::uno::Any(css::presentation::AnimationEffect::AnimationEffect_FADE_FROM_BOTTOM));
        shape_prop_set->setPropertyValue("OnClick", css::uno::Any(css::presentation::ClickAction::ClickAction_FIRSTPAGE));

        shape = create_shape(draw_doc, {22000, 8000}, {5000, 5000}, "com.sun.star.drawing.RectangleShape");
        shapes->add(shape);
        add_portion(shape, "click to go", false);
        add_portion(shape, "to the second page", true);
        shape_prop_set = Ref<css::beans::XPropertySet>(shape, css::uno::UNO_QUERY);
        shape_prop_set->setPropertyValue("Effect", css::uno::Any(css::presentation::AnimationEffect::AnimationEffect_FADE_FROM_BOTTOM));
        shape_prop_set->setPropertyValue("OnClick", css::uno::Any(css::presentation::ClickAction::ClickAction_BOOKMARK));

        // Set the name of page two, and use it with bookmark action
        Ref<css::container::XNamed> page_name(pages->getByIndex(1), css::uno::UNO_QUERY);
        page_name->setName("page two");
        shape_prop_set->setPropertyValue("Bookmark", css::uno::Any(page_name->getName()));

        // Start an endless presentation which is displayed in full-screen mode and placed on top
        Ref<css::presentation::XPresentationSupplier> presentation_supplier(draw_doc, css::uno::UNO_QUERY);
        Ref<css::presentation::XPresentation> presentation = presentation_supplier->getPresentation();
        Ref<css::beans::XPropertySet> presentation_prop_set(presentation, css::uno::UNO_QUERY);
        presentation_prop_set->setPropertyValue("IsEndless", css::uno::Any(true));
        presentation_prop_set->setPropertyValue("IsAlwaysOnTop", css::uno::Any(true));
        presentation_prop_set->setPropertyValue("Pause", css::uno::Any(0));
        presentation->start();
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }

    return 0;
}